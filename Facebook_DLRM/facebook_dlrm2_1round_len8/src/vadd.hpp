#include <hls_stream.h>

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
    const t_hbm_wide* table_HBM0, const t_hbm_wide* table_HBM1,
    const t_hbm_wide* table_HBM2, const t_hbm_wide* table_HBM3,
    const t_hbm_wide* table_HBM4, const t_hbm_wide* table_HBM5,
    const t_hbm_wide* table_HBM6, const t_hbm_wide* table_HBM7,
    const t_hbm_wide* table_HBM8, const t_hbm_wide* table_HBM9,
    const t_hbm_wide* table_HBM10, const t_hbm_wide* table_HBM11,
    const t_hbm_wide* table_HBM12, const t_hbm_wide* table_HBM13,
    const t_hbm_wide* table_HBM14, const t_hbm_wide* table_HBM15,
    const t_hbm_wide* table_HBM16, const t_hbm_wide* table_HBM17,
    const t_hbm_wide* table_HBM18, const t_hbm_wide* table_HBM19,
    const t_hbm_wide* table_HBM20, const t_hbm_wide* table_HBM21,
    const t_hbm_wide* table_HBM22, const t_hbm_wide* table_HBM23,
    const t_hbm_wide* table_HBM24, const t_hbm_wide* table_HBM25,
    const t_hbm_wide* table_HBM26, const t_hbm_wide* table_HBM27,
    const t_hbm_wide* table_HBM28, const t_hbm_wide* table_HBM29,
    const t_hbm_wide* table_HBM30, const t_hbm_wide* table_HBM31,
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
