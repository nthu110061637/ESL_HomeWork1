#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "GaussianBlurRB.h"
#include "Testbench.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {
  if ((argc < 3) || (argc > 4)) {
    cout << "No arguments for the executable : " << argv[0] << endl;
    cout << "Usage : >" << argv[0] << " in_image_file_name out_image_file_name"
         << endl;
    return 0;
  }

  //Create modules 
  Testbench tb("tb");
  GaussianBlurRB gaussian_blur_rb("gaussian_blur_rb");
  //Create signals
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS); // which is defined in the CMakeList
  sc_signal<bool> rst("rst");

  //Create FIFO channels
  sc_fifo<unsigned char> r [256];
  sc_fifo<unsigned char> g [256];
  sc_fifo<unsigned char> b [256];
  sc_fifo<int> result [256];

  //Connect FIFO channels with modules (reference the architecture diagram)
  tb.i_clk(clk);
  tb.o_rst(rst);
  gaussian_blur_rb.i_clk(clk); // we will need it when we do synthesis
  gaussian_blur_rb.i_rst(rst);

  for(int i = 0 ; i < 256 ; ++i)
  {
    tb.o_r[i](r[i]); // tb will output RGB data
    tb.o_g[i](g[i]);
    tb.o_b[i](b[i]);
    tb.i_result[i](result[i]); 
    gaussian_blur_rb.i_r[i](r[i]); //  filter will receive RGB data
    gaussian_blur_rb.i_g[i](g[i]);
    gaussian_blur_rb.i_b[i](b[i]);
    gaussian_blur_rb.o_result[i](result[i]);
  }
  

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}
