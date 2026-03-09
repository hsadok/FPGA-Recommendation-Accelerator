#include "vadd.hpp"

static void reduction_add_pair(
    hls::stream<int>& lhs,
    hls::stream<int>& rhs,
    hls::stream<int>& out);

void vadd(  
    const ap_int<AXI_WIDTH_HBM>* table_HBM0, const ap_int<AXI_WIDTH_HBM>* table_HBM1, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM2, const ap_int<AXI_WIDTH_HBM>* table_HBM3, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM4, const ap_int<AXI_WIDTH_HBM>* table_HBM5, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM6, const ap_int<AXI_WIDTH_HBM>* table_HBM7, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM8, const ap_int<AXI_WIDTH_HBM>* table_HBM9, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM10, const ap_int<AXI_WIDTH_HBM>* table_HBM11, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM12, const ap_int<AXI_WIDTH_HBM>* table_HBM13, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM14, const ap_int<AXI_WIDTH_HBM>* table_HBM15, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM16, const ap_int<AXI_WIDTH_HBM>* table_HBM17, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM18, const ap_int<AXI_WIDTH_HBM>* table_HBM19, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM20, const ap_int<AXI_WIDTH_HBM>* table_HBM21, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM22, const ap_int<AXI_WIDTH_HBM>* table_HBM23, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM24, const ap_int<AXI_WIDTH_HBM>* table_HBM25, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM26, const ap_int<AXI_WIDTH_HBM>* table_HBM27, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM28, const ap_int<AXI_WIDTH_HBM>* table_HBM29, 
    const ap_int<AXI_WIDTH_HBM>* table_HBM30, const ap_int<AXI_WIDTH_HBM>* table_HBM31,
    hls::stream<t_idx_pack>& idx_in,
    hls::stream<int>& out
    )
{
#pragma HLS DATAFLOW disable_start_propagation

#pragma HLS INTERFACE ap_ctrl_none port=return

#pragma HLS INTERFACE m_axi port=table_HBM0  offset=off bundle=gmem0  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM1  offset=off bundle=gmem1  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM2  offset=off bundle=gmem2  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM3  offset=off bundle=gmem3  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM4  offset=off bundle=gmem4  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM5  offset=off bundle=gmem5  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM6  offset=off bundle=gmem6  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM7  offset=off bundle=gmem7  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM8  offset=off bundle=gmem8  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM9  offset=off bundle=gmem9  max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM10 offset=off bundle=gmem10 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM11 offset=off bundle=gmem11 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM12 offset=off bundle=gmem12 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM13 offset=off bundle=gmem13 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM14 offset=off bundle=gmem14 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM15 offset=off bundle=gmem15 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM16 offset=off bundle=gmem16 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM17 offset=off bundle=gmem17 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM18 offset=off bundle=gmem18 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM19 offset=off bundle=gmem19 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM20 offset=off bundle=gmem20 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM21 offset=off bundle=gmem21 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM22 offset=off bundle=gmem22 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM23 offset=off bundle=gmem23 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM24 offset=off bundle=gmem24 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM25 offset=off bundle=gmem25 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM26 offset=off bundle=gmem26 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM27 offset=off bundle=gmem27 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM28 offset=off bundle=gmem28 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM29 offset=off bundle=gmem29 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM30 offset=off bundle=gmem30 max_widen_bitwidth=256
#pragma HLS INTERFACE m_axi port=table_HBM31 offset=off bundle=gmem31 max_widen_bitwidth=256

#pragma HLS INTERFACE axis port=idx_in
#pragma HLS INTERFACE axis port=out


    hls::stream<t_hbm> s_embedding_buffer_HBM0;
    hls::stream<t_hbm> s_embedding_buffer_HBM1;
    hls::stream<t_hbm> s_embedding_buffer_HBM2;
    hls::stream<t_hbm> s_embedding_buffer_HBM3;
    hls::stream<t_hbm> s_embedding_buffer_HBM4;
    hls::stream<t_hbm> s_embedding_buffer_HBM5;
    hls::stream<t_hbm> s_embedding_buffer_HBM6;
    hls::stream<t_hbm> s_embedding_buffer_HBM7;
    hls::stream<t_hbm> s_embedding_buffer_HBM8;
    hls::stream<t_hbm> s_embedding_buffer_HBM9;
    hls::stream<t_hbm> s_embedding_buffer_HBM10;
    hls::stream<t_hbm> s_embedding_buffer_HBM11;
    hls::stream<t_hbm> s_embedding_buffer_HBM12;
    hls::stream<t_hbm> s_embedding_buffer_HBM13;
    hls::stream<t_hbm> s_embedding_buffer_HBM14;
    hls::stream<t_hbm> s_embedding_buffer_HBM15;
    hls::stream<t_hbm> s_embedding_buffer_HBM16;
    hls::stream<t_hbm> s_embedding_buffer_HBM17;
    hls::stream<t_hbm> s_embedding_buffer_HBM18;
    hls::stream<t_hbm> s_embedding_buffer_HBM19;
    hls::stream<t_hbm> s_embedding_buffer_HBM20;
    hls::stream<t_hbm> s_embedding_buffer_HBM21;
    hls::stream<t_hbm> s_embedding_buffer_HBM22;
    hls::stream<t_hbm> s_embedding_buffer_HBM23;
    hls::stream<t_hbm> s_embedding_buffer_HBM24;
    hls::stream<t_hbm> s_embedding_buffer_HBM25;
    hls::stream<t_hbm> s_embedding_buffer_HBM26;
    hls::stream<t_hbm> s_embedding_buffer_HBM27;
    hls::stream<t_hbm> s_embedding_buffer_HBM28;
    hls::stream<t_hbm> s_embedding_buffer_HBM29;
    hls::stream<t_hbm> s_embedding_buffer_HBM30;
    hls::stream<t_hbm> s_embedding_buffer_HBM31;
#pragma HLS stream variable=s_embedding_buffer_HBM0 depth=depth_s_embedding_buffer_HBM0
#pragma HLS stream variable=s_embedding_buffer_HBM1 depth=depth_s_embedding_buffer_HBM1
#pragma HLS stream variable=s_embedding_buffer_HBM2 depth=depth_s_embedding_buffer_HBM2
#pragma HLS stream variable=s_embedding_buffer_HBM3 depth=depth_s_embedding_buffer_HBM3
#pragma HLS stream variable=s_embedding_buffer_HBM4 depth=depth_s_embedding_buffer_HBM4
#pragma HLS stream variable=s_embedding_buffer_HBM5 depth=depth_s_embedding_buffer_HBM5
#pragma HLS stream variable=s_embedding_buffer_HBM6 depth=depth_s_embedding_buffer_HBM6
#pragma HLS stream variable=s_embedding_buffer_HBM7 depth=depth_s_embedding_buffer_HBM7
#pragma HLS stream variable=s_embedding_buffer_HBM8 depth=depth_s_embedding_buffer_HBM8
#pragma HLS stream variable=s_embedding_buffer_HBM9 depth=depth_s_embedding_buffer_HBM9
#pragma HLS stream variable=s_embedding_buffer_HBM10 depth=depth_s_embedding_buffer_HBM10
#pragma HLS stream variable=s_embedding_buffer_HBM11 depth=depth_s_embedding_buffer_HBM11
#pragma HLS stream variable=s_embedding_buffer_HBM12 depth=depth_s_embedding_buffer_HBM12
#pragma HLS stream variable=s_embedding_buffer_HBM13 depth=depth_s_embedding_buffer_HBM13
#pragma HLS stream variable=s_embedding_buffer_HBM14 depth=depth_s_embedding_buffer_HBM14
#pragma HLS stream variable=s_embedding_buffer_HBM15 depth=depth_s_embedding_buffer_HBM15
#pragma HLS stream variable=s_embedding_buffer_HBM16 depth=depth_s_embedding_buffer_HBM16
#pragma HLS stream variable=s_embedding_buffer_HBM17 depth=depth_s_embedding_buffer_HBM17
#pragma HLS stream variable=s_embedding_buffer_HBM18 depth=depth_s_embedding_buffer_HBM18
#pragma HLS stream variable=s_embedding_buffer_HBM19 depth=depth_s_embedding_buffer_HBM19
#pragma HLS stream variable=s_embedding_buffer_HBM20 depth=depth_s_embedding_buffer_HBM20
#pragma HLS stream variable=s_embedding_buffer_HBM21 depth=depth_s_embedding_buffer_HBM21
#pragma HLS stream variable=s_embedding_buffer_HBM22 depth=depth_s_embedding_buffer_HBM22
#pragma HLS stream variable=s_embedding_buffer_HBM23 depth=depth_s_embedding_buffer_HBM23
#pragma HLS stream variable=s_embedding_buffer_HBM24 depth=depth_s_embedding_buffer_HBM24
#pragma HLS stream variable=s_embedding_buffer_HBM25 depth=depth_s_embedding_buffer_HBM25
#pragma HLS stream variable=s_embedding_buffer_HBM26 depth=depth_s_embedding_buffer_HBM26
#pragma HLS stream variable=s_embedding_buffer_HBM27 depth=depth_s_embedding_buffer_HBM27
#pragma HLS stream variable=s_embedding_buffer_HBM28 depth=depth_s_embedding_buffer_HBM28
#pragma HLS stream variable=s_embedding_buffer_HBM29 depth=depth_s_embedding_buffer_HBM29
#pragma HLS stream variable=s_embedding_buffer_HBM30 depth=depth_s_embedding_buffer_HBM30
#pragma HLS stream variable=s_embedding_buffer_HBM31 depth=depth_s_embedding_buffer_HBM31

    hls::stream<int> s_idx_buffer_HBM0;
    hls::stream<int> s_idx_buffer_HBM1;
    hls::stream<int> s_idx_buffer_HBM2;
    hls::stream<int> s_idx_buffer_HBM3;
    hls::stream<int> s_idx_buffer_HBM4;
    hls::stream<int> s_idx_buffer_HBM5;
    hls::stream<int> s_idx_buffer_HBM6;
    hls::stream<int> s_idx_buffer_HBM7;
    hls::stream<int> s_idx_buffer_HBM8;
    hls::stream<int> s_idx_buffer_HBM9;
    hls::stream<int> s_idx_buffer_HBM10;
    hls::stream<int> s_idx_buffer_HBM11;
    hls::stream<int> s_idx_buffer_HBM12;
    hls::stream<int> s_idx_buffer_HBM13;
    hls::stream<int> s_idx_buffer_HBM14;
    hls::stream<int> s_idx_buffer_HBM15;
    hls::stream<int> s_idx_buffer_HBM16;
    hls::stream<int> s_idx_buffer_HBM17;
    hls::stream<int> s_idx_buffer_HBM18;
    hls::stream<int> s_idx_buffer_HBM19;
    hls::stream<int> s_idx_buffer_HBM20;
    hls::stream<int> s_idx_buffer_HBM21;
    hls::stream<int> s_idx_buffer_HBM22;
    hls::stream<int> s_idx_buffer_HBM23;
    hls::stream<int> s_idx_buffer_HBM24;
    hls::stream<int> s_idx_buffer_HBM25;
    hls::stream<int> s_idx_buffer_HBM26;
    hls::stream<int> s_idx_buffer_HBM27;
    hls::stream<int> s_idx_buffer_HBM28;
    hls::stream<int> s_idx_buffer_HBM29;
    hls::stream<int> s_idx_buffer_HBM30;
    hls::stream<int> s_idx_buffer_HBM31;
#pragma HLS stream variable=s_idx_buffer_HBM0 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM1 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM2 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM3 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM4 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM5 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM6 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM7 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM8 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM9 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM10 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM11 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM12 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM13 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM14 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM15 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM16 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM17 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM18 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM19 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM20 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM21 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM22 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM23 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM24 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM25 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM26 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM27 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM28 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM29 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM30 depth=fifo_batch_size
#pragma HLS stream variable=s_idx_buffer_HBM31 depth=fifo_batch_size

    hls::stream<int> s_result_buffer_HBM0;
    hls::stream<int> s_result_buffer_HBM1;
    hls::stream<int> s_result_buffer_HBM2;
    hls::stream<int> s_result_buffer_HBM3;
    hls::stream<int> s_result_buffer_HBM4;
    hls::stream<int> s_result_buffer_HBM5;
    hls::stream<int> s_result_buffer_HBM6;
    hls::stream<int> s_result_buffer_HBM7;
    hls::stream<int> s_result_buffer_HBM8;
    hls::stream<int> s_result_buffer_HBM9;
    hls::stream<int> s_result_buffer_HBM10;
    hls::stream<int> s_result_buffer_HBM11;
    hls::stream<int> s_result_buffer_HBM12;
    hls::stream<int> s_result_buffer_HBM13;
    hls::stream<int> s_result_buffer_HBM14;
    hls::stream<int> s_result_buffer_HBM15;
    hls::stream<int> s_result_buffer_HBM16;
    hls::stream<int> s_result_buffer_HBM17;
    hls::stream<int> s_result_buffer_HBM18;
    hls::stream<int> s_result_buffer_HBM19;
    hls::stream<int> s_result_buffer_HBM20;
    hls::stream<int> s_result_buffer_HBM21;
    hls::stream<int> s_result_buffer_HBM22;
    hls::stream<int> s_result_buffer_HBM23;
    hls::stream<int> s_result_buffer_HBM24;
    hls::stream<int> s_result_buffer_HBM25;
    hls::stream<int> s_result_buffer_HBM26;
    hls::stream<int> s_result_buffer_HBM27;
    hls::stream<int> s_result_buffer_HBM28;
    hls::stream<int> s_result_buffer_HBM29;
    hls::stream<int> s_result_buffer_HBM30;
    hls::stream<int> s_result_buffer_HBM31;
#pragma HLS stream variable=s_result_buffer_HBM0 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM1 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM2 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM3 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM4 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM5 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM6 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM7 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM8 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM9 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM10 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM11 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM12 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM13 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM14 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM15 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM16 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM17 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM18 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM19 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM20 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM21 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM22 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM23 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM24 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM25 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM26 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM27 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM28 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM29 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM30 depth=fifo_batch_size
#pragma HLS stream variable=s_result_buffer_HBM31 depth=fifo_batch_size

    hls::stream<int> s_vout_buffer; 
#pragma HLS stream variable=s_vout_buffer depth=fifo_batch_size

    hls::stream<int> s_reduce_stage1_0;
    hls::stream<int> s_reduce_stage1_1;
    hls::stream<int> s_reduce_stage1_2;
    hls::stream<int> s_reduce_stage1_3;
    hls::stream<int> s_reduce_stage1_4;
    hls::stream<int> s_reduce_stage1_5;
    hls::stream<int> s_reduce_stage1_6;
    hls::stream<int> s_reduce_stage1_7;
    hls::stream<int> s_reduce_stage1_8;
    hls::stream<int> s_reduce_stage1_9;
    hls::stream<int> s_reduce_stage1_10;
    hls::stream<int> s_reduce_stage1_11;
    hls::stream<int> s_reduce_stage1_12;
    hls::stream<int> s_reduce_stage1_13;
    hls::stream<int> s_reduce_stage1_14;
    hls::stream<int> s_reduce_stage1_15;
    hls::stream<int> s_reduce_stage2_0;
    hls::stream<int> s_reduce_stage2_1;
    hls::stream<int> s_reduce_stage2_2;
    hls::stream<int> s_reduce_stage2_3;
    hls::stream<int> s_reduce_stage2_4;
    hls::stream<int> s_reduce_stage2_5;
    hls::stream<int> s_reduce_stage2_6;
    hls::stream<int> s_reduce_stage2_7;
    hls::stream<int> s_reduce_stage3_0;
    hls::stream<int> s_reduce_stage3_1;
    hls::stream<int> s_reduce_stage3_2;
    hls::stream<int> s_reduce_stage3_3;
    hls::stream<int> s_reduce_stage4_0;
    hls::stream<int> s_reduce_stage4_1;
#pragma HLS stream variable=s_reduce_stage1_0 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_1 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_2 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_3 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_4 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_5 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_6 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_7 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_8 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_9 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_10 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_11 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_12 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_13 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_14 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage1_15 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_0 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_1 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_2 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_3 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_4 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_5 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_6 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage2_7 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage3_0 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage3_1 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage3_2 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage3_3 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage4_0 depth=fifo_batch_size
#pragma HLS stream variable=s_reduce_stage4_1 depth=fifo_batch_size

    load_access_idx(
        idx_in,
        s_idx_buffer_HBM0, s_idx_buffer_HBM1, s_idx_buffer_HBM2, s_idx_buffer_HBM3, 
        s_idx_buffer_HBM4, s_idx_buffer_HBM5, s_idx_buffer_HBM6, s_idx_buffer_HBM7, 
        s_idx_buffer_HBM8, s_idx_buffer_HBM9, s_idx_buffer_HBM10, s_idx_buffer_HBM11, 
        s_idx_buffer_HBM12, s_idx_buffer_HBM13, s_idx_buffer_HBM14, s_idx_buffer_HBM15, 
        s_idx_buffer_HBM16, s_idx_buffer_HBM17, s_idx_buffer_HBM18, s_idx_buffer_HBM19, 
        s_idx_buffer_HBM20, s_idx_buffer_HBM21, s_idx_buffer_HBM22, s_idx_buffer_HBM23, 
        s_idx_buffer_HBM24, s_idx_buffer_HBM25, s_idx_buffer_HBM26, s_idx_buffer_HBM27, 
        s_idx_buffer_HBM28, s_idx_buffer_HBM29, s_idx_buffer_HBM30, s_idx_buffer_HBM31);

//////////////////////////////     Load Vectors & Concatenate     ////////////////////////////// 

    // 8 ~ 12 table x 4 vector / table = 32 ~ 48 loads
    // maximum: 48 = 32 + 16, half of the channels has the second round
    load_single_embedding_1_tables<ADDR_AXI_HBM_0, AXI_PADDED_SIZE_HBM_0> (
        s_idx_buffer_HBM0, table_HBM0, s_embedding_buffer_HBM0);
    load_single_embedding_1_tables<ADDR_AXI_HBM_1, AXI_PADDED_SIZE_HBM_1> (
        s_idx_buffer_HBM1, table_HBM1, s_embedding_buffer_HBM1);
    load_single_embedding_1_tables<ADDR_AXI_HBM_2, AXI_PADDED_SIZE_HBM_2> (
        s_idx_buffer_HBM2, table_HBM2, s_embedding_buffer_HBM2);
    load_single_embedding_1_tables<ADDR_AXI_HBM_3, AXI_PADDED_SIZE_HBM_3> (
        s_idx_buffer_HBM3, table_HBM3, s_embedding_buffer_HBM3);
    load_single_embedding_1_tables<ADDR_AXI_HBM_4, AXI_PADDED_SIZE_HBM_4> (
        s_idx_buffer_HBM4, table_HBM4, s_embedding_buffer_HBM4);
    load_single_embedding_1_tables<ADDR_AXI_HBM_5, AXI_PADDED_SIZE_HBM_5> (
        s_idx_buffer_HBM5, table_HBM5, s_embedding_buffer_HBM5);
    load_single_embedding_1_tables<ADDR_AXI_HBM_6, AXI_PADDED_SIZE_HBM_6> (
        s_idx_buffer_HBM6, table_HBM6, s_embedding_buffer_HBM6);
    load_single_embedding_1_tables<ADDR_AXI_HBM_7, AXI_PADDED_SIZE_HBM_7> (
        s_idx_buffer_HBM7, table_HBM7, s_embedding_buffer_HBM7);
    load_single_embedding_1_tables<ADDR_AXI_HBM_8, AXI_PADDED_SIZE_HBM_8> (
        s_idx_buffer_HBM8, table_HBM8, s_embedding_buffer_HBM8);
    load_single_embedding_1_tables<ADDR_AXI_HBM_9, AXI_PADDED_SIZE_HBM_9> (
        s_idx_buffer_HBM9, table_HBM9, s_embedding_buffer_HBM9);
    load_single_embedding_1_tables<ADDR_AXI_HBM_10, AXI_PADDED_SIZE_HBM_10> (
        s_idx_buffer_HBM10, table_HBM10, s_embedding_buffer_HBM10);
    load_single_embedding_1_tables<ADDR_AXI_HBM_11, AXI_PADDED_SIZE_HBM_11> (
        s_idx_buffer_HBM11, table_HBM11, s_embedding_buffer_HBM11);
    load_single_embedding_1_tables<ADDR_AXI_HBM_12, AXI_PADDED_SIZE_HBM_12> (
        s_idx_buffer_HBM12, table_HBM12, s_embedding_buffer_HBM12);
    load_single_embedding_1_tables<ADDR_AXI_HBM_13, AXI_PADDED_SIZE_HBM_13> (
        s_idx_buffer_HBM13, table_HBM13, s_embedding_buffer_HBM13);
    load_single_embedding_1_tables<ADDR_AXI_HBM_14, AXI_PADDED_SIZE_HBM_14> (
        s_idx_buffer_HBM14, table_HBM14, s_embedding_buffer_HBM14);
    load_single_embedding_1_tables<ADDR_AXI_HBM_15, AXI_PADDED_SIZE_HBM_15> (
        s_idx_buffer_HBM15, table_HBM15, s_embedding_buffer_HBM15);
    load_single_embedding_1_tables<ADDR_AXI_HBM_16, AXI_PADDED_SIZE_HBM_16> (
        s_idx_buffer_HBM16, table_HBM16, s_embedding_buffer_HBM16);
    load_single_embedding_1_tables<ADDR_AXI_HBM_17, AXI_PADDED_SIZE_HBM_17> (
        s_idx_buffer_HBM17, table_HBM17, s_embedding_buffer_HBM17);
    load_single_embedding_1_tables<ADDR_AXI_HBM_18, AXI_PADDED_SIZE_HBM_18> (
        s_idx_buffer_HBM18, table_HBM18, s_embedding_buffer_HBM18);
    load_single_embedding_1_tables<ADDR_AXI_HBM_19, AXI_PADDED_SIZE_HBM_19> (
        s_idx_buffer_HBM19, table_HBM19, s_embedding_buffer_HBM19);
    load_single_embedding_1_tables<ADDR_AXI_HBM_20, AXI_PADDED_SIZE_HBM_20> (
        s_idx_buffer_HBM20, table_HBM20, s_embedding_buffer_HBM20);
    load_single_embedding_1_tables<ADDR_AXI_HBM_21, AXI_PADDED_SIZE_HBM_21> (
        s_idx_buffer_HBM21, table_HBM21, s_embedding_buffer_HBM21);
    load_single_embedding_1_tables<ADDR_AXI_HBM_22, AXI_PADDED_SIZE_HBM_22> (
        s_idx_buffer_HBM22, table_HBM22, s_embedding_buffer_HBM22);
    load_single_embedding_1_tables<ADDR_AXI_HBM_23, AXI_PADDED_SIZE_HBM_23> (
        s_idx_buffer_HBM23, table_HBM23, s_embedding_buffer_HBM23);
    load_single_embedding_1_tables<ADDR_AXI_HBM_24, AXI_PADDED_SIZE_HBM_24> (
        s_idx_buffer_HBM24, table_HBM24, s_embedding_buffer_HBM24);
    load_single_embedding_1_tables<ADDR_AXI_HBM_25, AXI_PADDED_SIZE_HBM_25> (
        s_idx_buffer_HBM25, table_HBM25, s_embedding_buffer_HBM25);
    load_single_embedding_1_tables<ADDR_AXI_HBM_26, AXI_PADDED_SIZE_HBM_26> (
        s_idx_buffer_HBM26, table_HBM26, s_embedding_buffer_HBM26);
    load_single_embedding_1_tables<ADDR_AXI_HBM_27, AXI_PADDED_SIZE_HBM_27> (
        s_idx_buffer_HBM27, table_HBM27, s_embedding_buffer_HBM27);
    load_single_embedding_1_tables<ADDR_AXI_HBM_28, AXI_PADDED_SIZE_HBM_28> (
        s_idx_buffer_HBM28, table_HBM28, s_embedding_buffer_HBM28);
    load_single_embedding_1_tables<ADDR_AXI_HBM_29, AXI_PADDED_SIZE_HBM_29> (
        s_idx_buffer_HBM29, table_HBM29, s_embedding_buffer_HBM29);
    load_single_embedding_1_tables<ADDR_AXI_HBM_30, AXI_PADDED_SIZE_HBM_30> (
        s_idx_buffer_HBM30, table_HBM30, s_embedding_buffer_HBM30);
    load_single_embedding_1_tables<ADDR_AXI_HBM_31, AXI_PADDED_SIZE_HBM_31> (
        s_idx_buffer_HBM31, table_HBM31, s_embedding_buffer_HBM31);

    reduction_add_single<VECTOR_SIZE_HBM_BANK_0>(s_embedding_buffer_HBM0, s_result_buffer_HBM0);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_1>(s_embedding_buffer_HBM1, s_result_buffer_HBM1);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_2>(s_embedding_buffer_HBM2, s_result_buffer_HBM2);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_3>(s_embedding_buffer_HBM3, s_result_buffer_HBM3);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_4>(s_embedding_buffer_HBM4, s_result_buffer_HBM4);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_5>(s_embedding_buffer_HBM5, s_result_buffer_HBM5);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_6>(s_embedding_buffer_HBM6, s_result_buffer_HBM6);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_7>(s_embedding_buffer_HBM7, s_result_buffer_HBM7);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_8>(s_embedding_buffer_HBM8, s_result_buffer_HBM8);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_9>(s_embedding_buffer_HBM9, s_result_buffer_HBM9);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_10>(s_embedding_buffer_HBM10, s_result_buffer_HBM10);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_11>(s_embedding_buffer_HBM11, s_result_buffer_HBM11);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_12>(s_embedding_buffer_HBM12, s_result_buffer_HBM12);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_13>(s_embedding_buffer_HBM13, s_result_buffer_HBM13);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_14>(s_embedding_buffer_HBM14, s_result_buffer_HBM14);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_15>(s_embedding_buffer_HBM15, s_result_buffer_HBM15);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_16>(s_embedding_buffer_HBM16, s_result_buffer_HBM16);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_17>(s_embedding_buffer_HBM17, s_result_buffer_HBM17);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_18>(s_embedding_buffer_HBM18, s_result_buffer_HBM18);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_19>(s_embedding_buffer_HBM19, s_result_buffer_HBM19);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_20>(s_embedding_buffer_HBM20, s_result_buffer_HBM20);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_21>(s_embedding_buffer_HBM21, s_result_buffer_HBM21);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_22>(s_embedding_buffer_HBM22, s_result_buffer_HBM22);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_23>(s_embedding_buffer_HBM23, s_result_buffer_HBM23);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_24>(s_embedding_buffer_HBM24, s_result_buffer_HBM24);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_25>(s_embedding_buffer_HBM25, s_result_buffer_HBM25);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_26>(s_embedding_buffer_HBM26, s_result_buffer_HBM26);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_27>(s_embedding_buffer_HBM27, s_result_buffer_HBM27);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_28>(s_embedding_buffer_HBM28, s_result_buffer_HBM28);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_29>(s_embedding_buffer_HBM29, s_result_buffer_HBM29);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_30>(s_embedding_buffer_HBM30, s_result_buffer_HBM30);
    reduction_add_single<VECTOR_SIZE_HBM_BANK_31>(s_embedding_buffer_HBM31, s_result_buffer_HBM31);

    reduction_add_pair(s_result_buffer_HBM0, s_result_buffer_HBM1, s_reduce_stage1_0);
    reduction_add_pair(s_result_buffer_HBM2, s_result_buffer_HBM3, s_reduce_stage1_1);
    reduction_add_pair(s_result_buffer_HBM4, s_result_buffer_HBM5, s_reduce_stage1_2);
    reduction_add_pair(s_result_buffer_HBM6, s_result_buffer_HBM7, s_reduce_stage1_3);
    reduction_add_pair(s_result_buffer_HBM8, s_result_buffer_HBM9, s_reduce_stage1_4);
    reduction_add_pair(s_result_buffer_HBM10, s_result_buffer_HBM11, s_reduce_stage1_5);
    reduction_add_pair(s_result_buffer_HBM12, s_result_buffer_HBM13, s_reduce_stage1_6);
    reduction_add_pair(s_result_buffer_HBM14, s_result_buffer_HBM15, s_reduce_stage1_7);
    reduction_add_pair(s_result_buffer_HBM16, s_result_buffer_HBM17, s_reduce_stage1_8);
    reduction_add_pair(s_result_buffer_HBM18, s_result_buffer_HBM19, s_reduce_stage1_9);
    reduction_add_pair(s_result_buffer_HBM20, s_result_buffer_HBM21, s_reduce_stage1_10);
    reduction_add_pair(s_result_buffer_HBM22, s_result_buffer_HBM23, s_reduce_stage1_11);
    reduction_add_pair(s_result_buffer_HBM24, s_result_buffer_HBM25, s_reduce_stage1_12);
    reduction_add_pair(s_result_buffer_HBM26, s_result_buffer_HBM27, s_reduce_stage1_13);
    reduction_add_pair(s_result_buffer_HBM28, s_result_buffer_HBM29, s_reduce_stage1_14);
    reduction_add_pair(s_result_buffer_HBM30, s_result_buffer_HBM31, s_reduce_stage1_15);

    reduction_add_pair(s_reduce_stage1_0, s_reduce_stage1_1, s_reduce_stage2_0);
    reduction_add_pair(s_reduce_stage1_2, s_reduce_stage1_3, s_reduce_stage2_1);
    reduction_add_pair(s_reduce_stage1_4, s_reduce_stage1_5, s_reduce_stage2_2);
    reduction_add_pair(s_reduce_stage1_6, s_reduce_stage1_7, s_reduce_stage2_3);
    reduction_add_pair(s_reduce_stage1_8, s_reduce_stage1_9, s_reduce_stage2_4);
    reduction_add_pair(s_reduce_stage1_10, s_reduce_stage1_11, s_reduce_stage2_5);
    reduction_add_pair(s_reduce_stage1_12, s_reduce_stage1_13, s_reduce_stage2_6);
    reduction_add_pair(s_reduce_stage1_14, s_reduce_stage1_15, s_reduce_stage2_7);

    reduction_add_pair(s_reduce_stage2_0, s_reduce_stage2_1, s_reduce_stage3_0);
    reduction_add_pair(s_reduce_stage2_2, s_reduce_stage2_3, s_reduce_stage3_1);
    reduction_add_pair(s_reduce_stage2_4, s_reduce_stage2_5, s_reduce_stage3_2);
    reduction_add_pair(s_reduce_stage2_6, s_reduce_stage2_7, s_reduce_stage3_3);

    reduction_add_pair(s_reduce_stage3_0, s_reduce_stage3_1, s_reduce_stage4_0);
    reduction_add_pair(s_reduce_stage3_2, s_reduce_stage3_3, s_reduce_stage4_1);

    reduction_add_pair(s_reduce_stage4_0, s_reduce_stage4_1, s_vout_buffer);
        
    write_results(s_vout_buffer, out); 
}

void load_access_idx(
    hls::stream<t_idx_pack>& idx_in,
    hls::stream<int>& s_idx_buffer_HBM0, hls::stream<int>& s_idx_buffer_HBM1, 
    hls::stream<int>& s_idx_buffer_HBM2, hls::stream<int>& s_idx_buffer_HBM3, 
    hls::stream<int>& s_idx_buffer_HBM4, hls::stream<int>& s_idx_buffer_HBM5, 
    hls::stream<int>& s_idx_buffer_HBM6, hls::stream<int>& s_idx_buffer_HBM7, 
    hls::stream<int>& s_idx_buffer_HBM8, hls::stream<int>& s_idx_buffer_HBM9, 
    hls::stream<int>& s_idx_buffer_HBM10, hls::stream<int>& s_idx_buffer_HBM11, 
    hls::stream<int>& s_idx_buffer_HBM12, hls::stream<int>& s_idx_buffer_HBM13, 
    hls::stream<int>& s_idx_buffer_HBM14, hls::stream<int>& s_idx_buffer_HBM15, 
    hls::stream<int>& s_idx_buffer_HBM16, hls::stream<int>& s_idx_buffer_HBM17, 
    hls::stream<int>& s_idx_buffer_HBM18, hls::stream<int>& s_idx_buffer_HBM19, 
    hls::stream<int>& s_idx_buffer_HBM20, hls::stream<int>& s_idx_buffer_HBM21, 
    hls::stream<int>& s_idx_buffer_HBM22, hls::stream<int>& s_idx_buffer_HBM23, 
    hls::stream<int>& s_idx_buffer_HBM24, hls::stream<int>& s_idx_buffer_HBM25, 
    hls::stream<int>& s_idx_buffer_HBM26, hls::stream<int>& s_idx_buffer_HBM27, 
    hls::stream<int>& s_idx_buffer_HBM28, hls::stream<int>& s_idx_buffer_HBM29, 
    hls::stream<int>& s_idx_buffer_HBM30, hls::stream<int>& s_idx_buffer_HBM31) { 


    while (true) {
        #pragma HLS pipeline II=1
        t_idx_pack current_idx = idx_in.read();

        s_idx_buffer_HBM0.write(current_idx.indices[0]);
        s_idx_buffer_HBM1.write(current_idx.indices[1]);
        s_idx_buffer_HBM2.write(current_idx.indices[2]);
        s_idx_buffer_HBM3.write(current_idx.indices[3]);
        s_idx_buffer_HBM4.write(current_idx.indices[4]);
        s_idx_buffer_HBM5.write(current_idx.indices[5]);
        s_idx_buffer_HBM6.write(current_idx.indices[6]);
        s_idx_buffer_HBM7.write(current_idx.indices[7]);
        s_idx_buffer_HBM8.write(current_idx.indices[8]);
        s_idx_buffer_HBM9.write(current_idx.indices[9]);
        s_idx_buffer_HBM10.write(current_idx.indices[10]);
        s_idx_buffer_HBM11.write(current_idx.indices[11]);
        s_idx_buffer_HBM12.write(current_idx.indices[12]);
        s_idx_buffer_HBM13.write(current_idx.indices[13]);
        s_idx_buffer_HBM14.write(current_idx.indices[14]);
        s_idx_buffer_HBM15.write(current_idx.indices[15]);
        s_idx_buffer_HBM16.write(current_idx.indices[16]);
        s_idx_buffer_HBM17.write(current_idx.indices[17]);
        s_idx_buffer_HBM18.write(current_idx.indices[18]);
        s_idx_buffer_HBM19.write(current_idx.indices[19]);
        s_idx_buffer_HBM20.write(current_idx.indices[20]);
        s_idx_buffer_HBM21.write(current_idx.indices[21]);
        s_idx_buffer_HBM22.write(current_idx.indices[22]);
        s_idx_buffer_HBM23.write(current_idx.indices[23]);
        s_idx_buffer_HBM24.write(current_idx.indices[24]);
        s_idx_buffer_HBM25.write(current_idx.indices[25]);
        s_idx_buffer_HBM26.write(current_idx.indices[26]);
        s_idx_buffer_HBM27.write(current_idx.indices[27]);
        s_idx_buffer_HBM28.write(current_idx.indices[28]);
        s_idx_buffer_HBM29.write(current_idx.indices[29]);
        s_idx_buffer_HBM30.write(current_idx.indices[30]);
        s_idx_buffer_HBM31.write(current_idx.indices[31]);
    }
}

template<const long START_ADDR_0, const long AXI_PADDED_SIZE_0>
void load_single_embedding_1_tables(
    hls::stream<int>& s_idx_buffer, const t_hbm* table_RAM, 
    hls::stream<t_hbm>& s_embedding_buffer) {
#pragma HLS INLINE off

    while (true) {
        long idx = s_idx_buffer.read();

        long base_addr_0 = START_ADDR_0 + idx * AXI_PADDED_SIZE_0;
        for (int j = 0; j < AXI_PADDED_SIZE_0; j++) {
            #pragma HLS pipeline II=1
            s_embedding_buffer.write(table_RAM[base_addr_0 + j]);
        }
    }
}

template<const int TOTAL_VECTOR_LENGTH>
void reduction_add_single(
    hls::stream<t_hbm>& s_embedding_buffer, 
    hls::stream<int>& s_result_buffer)  {
#pragma HLS INLINE off

    while (true) {
        // load the first and consume the rest
        int result = s_embedding_buffer.read();
        for (int j = 0; j < TOTAL_VECTOR_LENGTH - 1; j++) {
            s_embedding_buffer.read();
        }
        s_result_buffer.write(result);
    }
}

static void reduction_add_pair(
    hls::stream<int>& lhs,
    hls::stream<int>& rhs,
    hls::stream<int>& out) {
#pragma HLS INLINE off

    bool have_lhs = false;
    bool have_rhs = false;
    bool out_valid = false;
    int lhs_val = 0;
    int rhs_val = 0;
    int out_val = 0;

    while (true) {
        #pragma HLS pipeline II=1

        if (out_valid && !out.full()) {
            out.write(out_val);
            out_valid = false;
        }

        if (!have_lhs && !lhs.empty()) {
            lhs_val = lhs.read();
            have_lhs = true;
        }

        if (!have_rhs && !rhs.empty()) {
            rhs_val = rhs.read();
            have_rhs = true;
        }

        if (!out_valid && have_lhs && have_rhs) {
            out_val = lhs_val + rhs_val;
            out_valid = true;
            have_lhs = false;
            have_rhs = false;
        }
    }
}

void write_results(
    hls::stream<int>& s_vout_buffer, hls::stream<int>& out) {

    while (true) {
        #pragma HLS pipeline II=1
        out.write(s_vout_buffer.read());
    }
}
