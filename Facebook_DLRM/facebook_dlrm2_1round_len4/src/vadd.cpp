#include "vadd.hpp"

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
static_assert(VECTOR_SIZE_HBM_BANK_0 == EMB_DIM && AXI_PADDED_SIZE_HBM_0 == EMB_DIM && ADDR_AXI_HBM_0 == 0, "table 0 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_1 == EMB_DIM && AXI_PADDED_SIZE_HBM_1 == EMB_DIM && ADDR_AXI_HBM_1 == 0, "table 1 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_2 == EMB_DIM && AXI_PADDED_SIZE_HBM_2 == EMB_DIM && ADDR_AXI_HBM_2 == 0, "table 2 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_3 == EMB_DIM && AXI_PADDED_SIZE_HBM_3 == EMB_DIM && ADDR_AXI_HBM_3 == 0, "table 3 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_4 == EMB_DIM && AXI_PADDED_SIZE_HBM_4 == EMB_DIM && ADDR_AXI_HBM_4 == 0, "table 4 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_5 == EMB_DIM && AXI_PADDED_SIZE_HBM_5 == EMB_DIM && ADDR_AXI_HBM_5 == 0, "table 5 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_6 == EMB_DIM && AXI_PADDED_SIZE_HBM_6 == EMB_DIM && ADDR_AXI_HBM_6 == 0, "table 6 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_7 == EMB_DIM && AXI_PADDED_SIZE_HBM_7 == EMB_DIM && ADDR_AXI_HBM_7 == 0, "table 7 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_8 == EMB_DIM && AXI_PADDED_SIZE_HBM_8 == EMB_DIM && ADDR_AXI_HBM_8 == 0, "table 8 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_9 == EMB_DIM && AXI_PADDED_SIZE_HBM_9 == EMB_DIM && ADDR_AXI_HBM_9 == 0, "table 9 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_10 == EMB_DIM && AXI_PADDED_SIZE_HBM_10 == EMB_DIM && ADDR_AXI_HBM_10 == 0, "table 10 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_11 == EMB_DIM && AXI_PADDED_SIZE_HBM_11 == EMB_DIM && ADDR_AXI_HBM_11 == 0, "table 11 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_12 == EMB_DIM && AXI_PADDED_SIZE_HBM_12 == EMB_DIM && ADDR_AXI_HBM_12 == 0, "table 12 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_13 == EMB_DIM && AXI_PADDED_SIZE_HBM_13 == EMB_DIM && ADDR_AXI_HBM_13 == 0, "table 13 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_14 == EMB_DIM && AXI_PADDED_SIZE_HBM_14 == EMB_DIM && ADDR_AXI_HBM_14 == 0, "table 14 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_15 == EMB_DIM && AXI_PADDED_SIZE_HBM_15 == EMB_DIM && ADDR_AXI_HBM_15 == 0, "table 15 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_16 == EMB_DIM && AXI_PADDED_SIZE_HBM_16 == EMB_DIM && ADDR_AXI_HBM_16 == 0, "table 16 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_17 == EMB_DIM && AXI_PADDED_SIZE_HBM_17 == EMB_DIM && ADDR_AXI_HBM_17 == 0, "table 17 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_18 == EMB_DIM && AXI_PADDED_SIZE_HBM_18 == EMB_DIM && ADDR_AXI_HBM_18 == 0, "table 18 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_19 == EMB_DIM && AXI_PADDED_SIZE_HBM_19 == EMB_DIM && ADDR_AXI_HBM_19 == 0, "table 19 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_20 == EMB_DIM && AXI_PADDED_SIZE_HBM_20 == EMB_DIM && ADDR_AXI_HBM_20 == 0, "table 20 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_21 == EMB_DIM && AXI_PADDED_SIZE_HBM_21 == EMB_DIM && ADDR_AXI_HBM_21 == 0, "table 21 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_22 == EMB_DIM && AXI_PADDED_SIZE_HBM_22 == EMB_DIM && ADDR_AXI_HBM_22 == 0, "table 22 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_23 == EMB_DIM && AXI_PADDED_SIZE_HBM_23 == EMB_DIM && ADDR_AXI_HBM_23 == 0, "table 23 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_24 == EMB_DIM && AXI_PADDED_SIZE_HBM_24 == EMB_DIM && ADDR_AXI_HBM_24 == 0, "table 24 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_25 == EMB_DIM && AXI_PADDED_SIZE_HBM_25 == EMB_DIM && ADDR_AXI_HBM_25 == 0, "table 25 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_26 == EMB_DIM && AXI_PADDED_SIZE_HBM_26 == EMB_DIM && ADDR_AXI_HBM_26 == 0, "table 26 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_27 == EMB_DIM && AXI_PADDED_SIZE_HBM_27 == EMB_DIM && ADDR_AXI_HBM_27 == 0, "table 27 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_28 == EMB_DIM && AXI_PADDED_SIZE_HBM_28 == EMB_DIM && ADDR_AXI_HBM_28 == 0, "table 28 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_29 == EMB_DIM && AXI_PADDED_SIZE_HBM_29 == EMB_DIM && ADDR_AXI_HBM_29 == 0, "table 29 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_30 == EMB_DIM && AXI_PADDED_SIZE_HBM_30 == EMB_DIM && ADDR_AXI_HBM_30 == 0, "table 30 geometry");
static_assert(VECTOR_SIZE_HBM_BANK_31 == EMB_DIM && AXI_PADDED_SIZE_HBM_31 == EMB_DIM && ADDR_AXI_HBM_31 == 0, "table 31 geometry");

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
    )
{
#pragma HLS DATAFLOW disable_start_propagation

#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS INTERFACE m_axi port=table_HBM0 offset=off bundle=gmem0 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM1 offset=off bundle=gmem1 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM2 offset=off bundle=gmem2 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM3 offset=off bundle=gmem3 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM4 offset=off bundle=gmem4 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM5 offset=off bundle=gmem5 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM6 offset=off bundle=gmem6 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM7 offset=off bundle=gmem7 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM8 offset=off bundle=gmem8 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM9 offset=off bundle=gmem9 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM10 offset=off bundle=gmem10 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM11 offset=off bundle=gmem11 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM12 offset=off bundle=gmem12 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM13 offset=off bundle=gmem13 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM14 offset=off bundle=gmem14 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM15 offset=off bundle=gmem15 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM16 offset=off bundle=gmem16 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM17 offset=off bundle=gmem17 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM18 offset=off bundle=gmem18 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM19 offset=off bundle=gmem19 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM20 offset=off bundle=gmem20 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM21 offset=off bundle=gmem21 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM22 offset=off bundle=gmem22 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM23 offset=off bundle=gmem23 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM24 offset=off bundle=gmem24 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM25 offset=off bundle=gmem25 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM26 offset=off bundle=gmem26 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM27 offset=off bundle=gmem27 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM28 offset=off bundle=gmem28 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM29 offset=off bundle=gmem29 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM30 offset=off bundle=gmem30 max_widen_bitwidth=256 num_read_outstanding=64 latency=32
#pragma HLS INTERFACE m_axi port=table_HBM31 offset=off bundle=gmem31 max_widen_bitwidth=256 num_read_outstanding=64 latency=32

#pragma HLS INTERFACE axis port=idx_in
#pragma HLS INTERFACE axis port=emb_out

    hls::stream<int> s_idx_buffer_HBM0;
#pragma HLS stream variable=s_idx_buffer_HBM0 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM1;
#pragma HLS stream variable=s_idx_buffer_HBM1 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM2;
#pragma HLS stream variable=s_idx_buffer_HBM2 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM3;
#pragma HLS stream variable=s_idx_buffer_HBM3 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM4;
#pragma HLS stream variable=s_idx_buffer_HBM4 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM5;
#pragma HLS stream variable=s_idx_buffer_HBM5 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM6;
#pragma HLS stream variable=s_idx_buffer_HBM6 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM7;
#pragma HLS stream variable=s_idx_buffer_HBM7 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM8;
#pragma HLS stream variable=s_idx_buffer_HBM8 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM9;
#pragma HLS stream variable=s_idx_buffer_HBM9 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM10;
#pragma HLS stream variable=s_idx_buffer_HBM10 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM11;
#pragma HLS stream variable=s_idx_buffer_HBM11 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM12;
#pragma HLS stream variable=s_idx_buffer_HBM12 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM13;
#pragma HLS stream variable=s_idx_buffer_HBM13 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM14;
#pragma HLS stream variable=s_idx_buffer_HBM14 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM15;
#pragma HLS stream variable=s_idx_buffer_HBM15 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM16;
#pragma HLS stream variable=s_idx_buffer_HBM16 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM17;
#pragma HLS stream variable=s_idx_buffer_HBM17 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM18;
#pragma HLS stream variable=s_idx_buffer_HBM18 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM19;
#pragma HLS stream variable=s_idx_buffer_HBM19 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM20;
#pragma HLS stream variable=s_idx_buffer_HBM20 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM21;
#pragma HLS stream variable=s_idx_buffer_HBM21 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM22;
#pragma HLS stream variable=s_idx_buffer_HBM22 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM23;
#pragma HLS stream variable=s_idx_buffer_HBM23 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM24;
#pragma HLS stream variable=s_idx_buffer_HBM24 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM25;
#pragma HLS stream variable=s_idx_buffer_HBM25 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM26;
#pragma HLS stream variable=s_idx_buffer_HBM26 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM27;
#pragma HLS stream variable=s_idx_buffer_HBM27 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM28;
#pragma HLS stream variable=s_idx_buffer_HBM28 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM29;
#pragma HLS stream variable=s_idx_buffer_HBM29 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM30;
#pragma HLS stream variable=s_idx_buffer_HBM30 depth=fifo_batch_size
    hls::stream<int> s_idx_buffer_HBM31;
#pragma HLS stream variable=s_idx_buffer_HBM31 depth=fifo_batch_size

    hls::stream<t_part> s_embedding_buffer[HBM_BANK_NUM];
#pragma HLS stream variable=s_embedding_buffer depth=depth_s_embedding_buffer

    hls::stream<t_idx_group> s_idx_group[4];
#pragma HLS stream variable=s_idx_group depth=2

    load_access_idx(idx_in, s_idx_group);
    distribute_idx(s_idx_group[0], s_idx_buffer_HBM0, s_idx_buffer_HBM1, s_idx_buffer_HBM2, s_idx_buffer_HBM3, s_idx_buffer_HBM4, s_idx_buffer_HBM5, s_idx_buffer_HBM6, s_idx_buffer_HBM7);
    distribute_idx(s_idx_group[1], s_idx_buffer_HBM8, s_idx_buffer_HBM9, s_idx_buffer_HBM10, s_idx_buffer_HBM11, s_idx_buffer_HBM12, s_idx_buffer_HBM13, s_idx_buffer_HBM14, s_idx_buffer_HBM15);
    distribute_idx(s_idx_group[2], s_idx_buffer_HBM16, s_idx_buffer_HBM17, s_idx_buffer_HBM18, s_idx_buffer_HBM19, s_idx_buffer_HBM20, s_idx_buffer_HBM21, s_idx_buffer_HBM22, s_idx_buffer_HBM23);
    distribute_idx(s_idx_group[3], s_idx_buffer_HBM24, s_idx_buffer_HBM25, s_idx_buffer_HBM26, s_idx_buffer_HBM27, s_idx_buffer_HBM28, s_idx_buffer_HBM29, s_idx_buffer_HBM30, s_idx_buffer_HBM31);

    load_single_embedding_1_tables<ADDR_AXI_HBM_0, AXI_PADDED_SIZE_HBM_0>(
        s_idx_buffer_HBM0, table_HBM0, s_embedding_buffer[0]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_1, AXI_PADDED_SIZE_HBM_1>(
        s_idx_buffer_HBM1, table_HBM1, s_embedding_buffer[1]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_2, AXI_PADDED_SIZE_HBM_2>(
        s_idx_buffer_HBM2, table_HBM2, s_embedding_buffer[2]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_3, AXI_PADDED_SIZE_HBM_3>(
        s_idx_buffer_HBM3, table_HBM3, s_embedding_buffer[3]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_4, AXI_PADDED_SIZE_HBM_4>(
        s_idx_buffer_HBM4, table_HBM4, s_embedding_buffer[4]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_5, AXI_PADDED_SIZE_HBM_5>(
        s_idx_buffer_HBM5, table_HBM5, s_embedding_buffer[5]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_6, AXI_PADDED_SIZE_HBM_6>(
        s_idx_buffer_HBM6, table_HBM6, s_embedding_buffer[6]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_7, AXI_PADDED_SIZE_HBM_7>(
        s_idx_buffer_HBM7, table_HBM7, s_embedding_buffer[7]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_8, AXI_PADDED_SIZE_HBM_8>(
        s_idx_buffer_HBM8, table_HBM8, s_embedding_buffer[8]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_9, AXI_PADDED_SIZE_HBM_9>(
        s_idx_buffer_HBM9, table_HBM9, s_embedding_buffer[9]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_10, AXI_PADDED_SIZE_HBM_10>(
        s_idx_buffer_HBM10, table_HBM10, s_embedding_buffer[10]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_11, AXI_PADDED_SIZE_HBM_11>(
        s_idx_buffer_HBM11, table_HBM11, s_embedding_buffer[11]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_12, AXI_PADDED_SIZE_HBM_12>(
        s_idx_buffer_HBM12, table_HBM12, s_embedding_buffer[12]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_13, AXI_PADDED_SIZE_HBM_13>(
        s_idx_buffer_HBM13, table_HBM13, s_embedding_buffer[13]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_14, AXI_PADDED_SIZE_HBM_14>(
        s_idx_buffer_HBM14, table_HBM14, s_embedding_buffer[14]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_15, AXI_PADDED_SIZE_HBM_15>(
        s_idx_buffer_HBM15, table_HBM15, s_embedding_buffer[15]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_16, AXI_PADDED_SIZE_HBM_16>(
        s_idx_buffer_HBM16, table_HBM16, s_embedding_buffer[16]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_17, AXI_PADDED_SIZE_HBM_17>(
        s_idx_buffer_HBM17, table_HBM17, s_embedding_buffer[17]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_18, AXI_PADDED_SIZE_HBM_18>(
        s_idx_buffer_HBM18, table_HBM18, s_embedding_buffer[18]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_19, AXI_PADDED_SIZE_HBM_19>(
        s_idx_buffer_HBM19, table_HBM19, s_embedding_buffer[19]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_20, AXI_PADDED_SIZE_HBM_20>(
        s_idx_buffer_HBM20, table_HBM20, s_embedding_buffer[20]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_21, AXI_PADDED_SIZE_HBM_21>(
        s_idx_buffer_HBM21, table_HBM21, s_embedding_buffer[21]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_22, AXI_PADDED_SIZE_HBM_22>(
        s_idx_buffer_HBM22, table_HBM22, s_embedding_buffer[22]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_23, AXI_PADDED_SIZE_HBM_23>(
        s_idx_buffer_HBM23, table_HBM23, s_embedding_buffer[23]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_24, AXI_PADDED_SIZE_HBM_24>(
        s_idx_buffer_HBM24, table_HBM24, s_embedding_buffer[24]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_25, AXI_PADDED_SIZE_HBM_25>(
        s_idx_buffer_HBM25, table_HBM25, s_embedding_buffer[25]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_26, AXI_PADDED_SIZE_HBM_26>(
        s_idx_buffer_HBM26, table_HBM26, s_embedding_buffer[26]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_27, AXI_PADDED_SIZE_HBM_27>(
        s_idx_buffer_HBM27, table_HBM27, s_embedding_buffer[27]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_28, AXI_PADDED_SIZE_HBM_28>(
        s_idx_buffer_HBM28, table_HBM28, s_embedding_buffer[28]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_29, AXI_PADDED_SIZE_HBM_29>(
        s_idx_buffer_HBM29, table_HBM29, s_embedding_buffer[29]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_30, AXI_PADDED_SIZE_HBM_30>(
        s_idx_buffer_HBM30, table_HBM30, s_embedding_buffer[30]);
    load_single_embedding_1_tables<ADDR_AXI_HBM_31, AXI_PADDED_SIZE_HBM_31>(
        s_idx_buffer_HBM31, table_HBM31, s_embedding_buffer[31]);

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
