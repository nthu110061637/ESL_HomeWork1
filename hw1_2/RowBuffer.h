#ifndef ROW_BUFFER_H_
#define ROW_BUFFER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class RowBuffer : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r [256];
  sc_fifo_in<unsigned char> i_g [256];
  sc_fifo_in<unsigned char> i_b [256];
  sc_fifo_out<int> o_result [256];

  SC_HAS_PROCESS(RowBuffer);
  RowBuffer(sc_module_name n);
  ~RowBuffer() = default; // destructor design and tb will be removed from the simulation stack

private:
  void do_filter();
  double val[MASK_N];
};
#endif