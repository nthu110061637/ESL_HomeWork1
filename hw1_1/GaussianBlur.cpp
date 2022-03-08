#include <cmath>

#include "GaussianBlur.h"

GaussianBlur::GaussianBlur(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos(); 
  dont_initialize(); // do not run the tread when the thread is being construct
  reset_signal_is(i_rst, false);
}

// Gaussian Blur mask
const double mask[MASK_N][MASK_X][MASK_Y] = {{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}}};
                  /*[1][3][3]*/            
//const double mask[MASK_N][MASK_X][MASK_Y] = {{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}}};

void GaussianBlur::do_filter() {
  while (true) {
    for (unsigned int i = 0; i < MASK_N; ++i) { 
      val[i] = 0; // val[0]
    }
    for (unsigned int v = 0; v < MASK_Y; ++v) { //u,v are the filter size for row and col
      for (unsigned int u = 0; u < MASK_X; ++u) { // line 22 23 go through the 9 pixel
        unsigned char grey = (i_r.read() + i_g.read() + i_b.read()) / 3 ; //avg the RGB
        for (unsigned int i = 0; i != MASK_N; ++i) { // we only have one filter (mask) so can be ignore
          val[i] += grey * mask[i][u][v]; 
        }
      }
    }
    double total = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total += val[i] * val[i]; // squre
    }
    int result = (int)(std::sqrt(total)); // root
    o_result.write(result);
    wait(10); //emulate module delay
  }
}
