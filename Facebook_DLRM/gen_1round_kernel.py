#!/usr/bin/env python3
"""Generate the concatenating, pipelined MicroRec kernel (vadd.hpp/vadd.cpp)
for the facebook_dlrm2_1round_len* models. The sources only depend on the
model's constants.hpp, so the same files go into every 1round directory."""
import sys
from pathlib import Path

N = 32  # HBM channels = tables

HPP = r'''#include <hls_stream.h>

#include "constants.hpp"

// One request: the index of the entry to look up in each table.
struct t_idx_pack {
    int indices[32];
};

// Kernel geometry, from the model's constants (every table has the same
// embedding dimension in the 1round models).
#define EMB_DIM VECTOR_SIZE_HBM_BANK_0
#define EMB_BITS (EMB_DIM * 32)
// Response stream beat: 512 bits (one DCP flit / one 64-byte RAM word).
#define OUT_BITS 512
typedef ap_uint<OUT_BITS> t_out;
// What a load stage hands the concatenation stage: a whole embedding when it
// fits in a response beat (len4: 128 bits, len8: 256), else 512-bit pieces.
#define PART_BITS (EMB_BITS < OUT_BITS ? EMB_BITS : OUT_BITS)
#define PART_WORDS (PART_BITS / 32)
typedef ap_uint<PART_BITS> t_part;
#define PARTS_PER_LOOKUP (EMB_BITS / PART_BITS)
#define PARTS_PER_OUT (OUT_BITS / PART_BITS)
// 256-bit HBM beats read per lookup.
#define BEATS_PER_LOOKUP (EMB_BITS < 256 ? 1 : EMB_BITS / 256)
// Response beats per inference: all tables' embeddings, table 0 first.
#define OUT_BEATS_PER_INFERENCE (HBM_BANK_NUM * PARTS_PER_LOOKUP / PARTS_PER_OUT)

extern "C" {

void vadd(
@TABLE_ARGS@
    hls::stream<t_idx_pack>& idx_in,
    hls::stream<t_part> emb_out[HBM_BANK_NUM]
    );
}

// Requests are distributed to the channels in two levels (4 groups of 8), so
// that no stage's stall depends on all 32 channel FIFOs (spread over the die).
struct t_idx_group {
    int indices[8];
};

void load_access_idx(
    hls::stream<t_idx_pack>& idx_in, hls::stream<t_idx_group> s_idx_group[4]);

void distribute_idx(
    hls::stream<t_idx_group>& s_idx_group,
    hls::stream<int>& o0, hls::stream<int>& o1, hls::stream<int>& o2,
    hls::stream<int>& o3, hls::stream<int>& o4, hls::stream<int>& o5,
    hls::stream<int>& o6, hls::stream<int>& o7);

template<const long START_ADDR_0, const long AXI_PADDED_SIZE_0>
void load_single_embedding_1_tables(
    hls::stream<int>& s_idx_buffer, const t_hbm_wide* table_RAM,
    hls::stream<t_part>& s_embedding_buffer);

void forward_embeddings(
    hls::stream<t_part>& s_embedding_buffer, hls::stream<t_part>& emb_out);

const int fifo_batch_size = FIFO_BATCH_SIZE;
// Embedding FIFO per channel, in parts: room for a few lookups while the
// concatenation stage drains the channels in table order.
const int depth_s_embedding_buffer = 32;
'''

CPP = r'''#include "vadd.hpp"

// Concatenating MicroRec kernel (facebook_dlrm2_1round_len*).
//
// For each request (one index per table), every HBM channel looks up its
// table's entry. Channel c streams its embedding on emb_out[c], in
// PARTS_PER_LOOKUP parts of PART_BITS; the RTL wrapper (rec_accel_hls_sv_top)
// concatenates the 32 embeddings, table 0 first, into OUT_BEATS_PER_INFERENCE
// beats of 512 bits (128 x EMB_DIM bytes). (A concatenation stage in HLS
// missed timing on the AU55C by up to 1.1 ns: its stall logic chained the
// output's back-pressure, the channel selection and the 32 FIFO reads.) The
// upstream kernels read the same entries but reduced them to a single 32-bit
// sum of each embedding's first word; returning the embeddings makes the
// response grow with the embedding dimension, as an embedding layer that
// feeds a host-side MLP would.
//
// Every stage is a free-running II=1 pipeline (ap_ctrl_none): requests are
// pipelined across the channels, many lookups in flight per channel.

static_assert(HBM_BANK_NUM == 32 && TABLE_NUM == 32, "one table per channel");
static_assert(EMB_BITS % 128 == 0, "embeddings are multiples of 16 bytes");
static_assert(PARTS_PER_OUT == 1 || PARTS_PER_LOOKUP == 1, "part geometry");
static_assert(OUT_BEATS_PER_INFERENCE * OUT_BITS == HBM_BANK_NUM * EMB_BITS,
              "response size");
@STATIC_ASSERTS@

void vadd(
@TABLE_ARGS@
    hls::stream<t_idx_pack>& idx_in,
    hls::stream<t_part> emb_out[HBM_BANK_NUM]
    )
{
#pragma HLS DATAFLOW disable_start_propagation

#pragma HLS INTERFACE ap_ctrl_none port=return

@MAXI_PRAGMAS@

#pragma HLS INTERFACE axis port=idx_in
#pragma HLS INTERFACE axis port=emb_out

@IDX_STREAMS@

    hls::stream<t_part> s_embedding_buffer[HBM_BANK_NUM];
#pragma HLS stream variable=s_embedding_buffer depth=depth_s_embedding_buffer

    hls::stream<t_idx_group> s_idx_group[4];
#pragma HLS stream variable=s_idx_group depth=2

    load_access_idx(idx_in, s_idx_group);
@IDX_DIST_CALLS@

@LOAD_CALLS@

    for (int c = 0; c < HBM_BANK_NUM; c++) {
        #pragma HLS unroll
        forward_embeddings(s_embedding_buffer[c], emb_out[c]);
    }
}

void load_access_idx(
    hls::stream<t_idx_pack>& idx_in, hls::stream<t_idx_group> s_idx_group[4]) {

    while (true) {
        #pragma HLS pipeline II=1
        t_idx_pack current_idx = idx_in.read();
        for (int g = 0; g < 4; g++) {
            #pragma HLS unroll
            t_idx_group group;
            for (int i = 0; i < 8; i++) {
                #pragma HLS unroll
                group.indices[i] = current_idx.indices[8 * g + i];
            }
            s_idx_group[g].write(group);
        }
    }
}

void distribute_idx(
    hls::stream<t_idx_group>& s_idx_group,
    hls::stream<int>& o0, hls::stream<int>& o1, hls::stream<int>& o2,
    hls::stream<int>& o3, hls::stream<int>& o4, hls::stream<int>& o5,
    hls::stream<int>& o6, hls::stream<int>& o7) {
#pragma HLS INLINE off

    while (true) {
        #pragma HLS pipeline II=1
        t_idx_group group = s_idx_group.read();
        o0.write(group.indices[0]);
        o1.write(group.indices[1]);
        o2.write(group.indices[2]);
        o3.write(group.indices[3]);
        o4.write(group.indices[4]);
        o5.write(group.indices[5]);
        o6.write(group.indices[6]);
        o7.write(group.indices[7]);
    }
}

template<const long START_ADDR_0, const long AXI_PADDED_SIZE_0>
void load_single_embedding_1_tables(
    hls::stream<int>& s_idx_buffer, const t_hbm_wide* table_RAM,
    hls::stream<t_part>& s_embedding_buffer) {
#pragma HLS INLINE off

    // One 256-bit HBM beat per cycle, pipelined across lookups so that many
    // reads are in flight on the port. A lookup reads BEATS_PER_LOOKUP beats
    // (len4: the half beat holding the 16-byte entry). Parts wider than a beat
    // (512 bits) are assembled from two consecutive beats.
    //
    // The index is read without blocking: this pipeline is deep (the m_axi
    // latency) and HLS implements it as a stall pipeline, where a blocking
    // read of an empty stream would freeze every stage, holding the lookups
    // already in flight until more requests arrived.
    int beat = 0;          // next beat of the current lookup
    long base_word = 0;    // first 32-bit word of the current entry
#if PART_BITS > 256
    ap_uint<256> low_half = 0;
#endif
    while (true) {
        #pragma HLS pipeline II=1
        bool active = false;
        long entry_word = base_word;
        if (beat == 0) {
            int idx;
            if (s_idx_buffer.read_nb(idx)) {
                entry_word = START_ADDR_0 + (long)idx * AXI_PADDED_SIZE_0;
                base_word = entry_word;
                active = true;
            }
        } else {
            active = true;
        }
        if (active) {
            const long word = entry_word + (long)beat * HBM_WORDS_PER_WIDE;
            const t_hbm_wide data = table_RAM[word / HBM_WORDS_PER_WIDE];
#if PART_BITS < 256
            const int lane = (word % HBM_WORDS_PER_WIDE) / PART_WORDS;
            t_part value = data.range(PART_BITS * lane + PART_BITS - 1,
                                      PART_BITS * lane);
            s_embedding_buffer.write(value);
#elif PART_BITS == 256
            s_embedding_buffer.write(data);
#else
            if ((beat & 1) == 0) {
                low_half = data;
            } else {
                t_part value = (data, low_half);
                s_embedding_buffer.write(value);
            }
#endif
            beat = (beat == BEATS_PER_LOOKUP - 1) ? 0 : beat + 1;
        }
    }
}

void forward_embeddings(
    hls::stream<t_part>& s_embedding_buffer, hls::stream<t_part>& emb_out) {
#pragma HLS INLINE off

    // Moves a channel's parts from its FIFO to its output port. The read does
    // not block (see load_single_embedding_1_tables).
    while (true) {
        #pragma HLS pipeline II=1
        t_part value;
        if (s_embedding_buffer.read_nb(value)) {
            emb_out.write(value);
        }
    }
}
'''


def table_args(type_='const t_hbm_wide*'):
    lines = []
    for i in range(0, N, 2):
        lines.append(f'    {type_} table_HBM{i}, {type_} table_HBM{i+1},')
    return '\n'.join(lines)


def idx_args():
    lines = []
    for i in range(0, N, 2):
        sep = ',' if i + 2 < N else ''
        lines.append(f'    hls::stream<int>& s_idx_buffer_HBM{i}, '
                     f'hls::stream<int>& s_idx_buffer_HBM{i+1}{sep}')
    return '\n'.join(lines)


def main():
    out_dirs = [Path(p) for p in sys.argv[1:]]
    hpp = HPP.replace('@TABLE_ARGS@', table_args()).replace(
        '@IDX_ARGS@', idx_args())
    maxi = '\n'.join(
        f'#pragma HLS INTERFACE m_axi port=table_HBM{i:<2d} offset=off '
        f'bundle=gmem{i:<2d} max_widen_bitwidth=256 '
        f'num_read_outstanding=64 latency=32'.replace('  ', ' ')
        for i in range(N))
    idx_streams = '\n'.join(
        f'    hls::stream<int> s_idx_buffer_HBM{i};\n'
        f'#pragma HLS stream variable=s_idx_buffer_HBM{i} depth=fifo_batch_size'
        for i in range(N))
    idx_call = '\n'.join(
        f'        s_idx_buffer_HBM{i}, s_idx_buffer_HBM{i+1}'
        + (',' if i + 2 < N else '') for i in range(0, N, 2))
    loads = '\n'.join(
        f'    load_single_embedding_1_tables<ADDR_AXI_HBM_{i}, '
        f'AXI_PADDED_SIZE_HBM_{i}>(\n'
        f'        s_idx_buffer_HBM{i}, table_HBM{i}, s_embedding_buffer[{i}]);'
        for i in range(N))
    dist_calls = '\n'.join(
        f'    distribute_idx(s_idx_group[{g}], '
        + ', '.join(f's_idx_buffer_HBM{8 * g + i}' for i in range(8)) + ');'
        for g in range(4))
    idx_writes = '\n'.join(
        f'        s_idx_buffer_HBM{i}.write(current_idx.indices[{i}]);'
        for i in range(N))
    asserts = '\n'.join(
        f'static_assert(VECTOR_SIZE_HBM_BANK_{i} == EMB_DIM && '
        f'AXI_PADDED_SIZE_HBM_{i} == EMB_DIM && ADDR_AXI_HBM_{i} == 0, '
        f'"table {i} geometry");' for i in range(N))
    cpp = (CPP.replace('@TABLE_ARGS@', table_args())
           .replace('@MAXI_PRAGMAS@', maxi)
           .replace('@IDX_STREAMS@', idx_streams)
           .replace('@IDX_DIST_CALLS@', dist_calls)
           .replace('@LOAD_CALLS@', loads)
           .replace('@IDX_ARGS@', idx_args())
           .replace('@IDX_WRITES@', idx_writes)
           .replace('@STATIC_ASSERTS@', asserts))
    for d in out_dirs:
        (d / 'vadd.hpp').write_text(hpp)
        (d / 'vadd.cpp').write_text(cpp)
        print('wrote', d)


if __name__ == '__main__':
    main()
