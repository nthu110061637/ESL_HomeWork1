#ifndef GAUSSIAN_BLUR_RB_H_
#define GAUSSIAN_BLUR_RB_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class GaussianBlurRB : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned char> i_r [256];
  sc_fifo_in<unsigned char> i_g [256];
  sc_fifo_in<unsigned char> i_b [256];
  sc_fifo_out<int> o_result [256];

  SC_HAS_PROCESS(GaussianBlurRB);
  GaussianBlurRB(sc_module_name n);
  ~GaussianBlurRB() = default; // destructor design and tb will be removed from the simulation stack

private:
  unsigned char pixel[3][258];
  void do_filter(unsigned char [3][258],int);
  void row_buffer();
  double val[MASK_N];
  
};
#endif
