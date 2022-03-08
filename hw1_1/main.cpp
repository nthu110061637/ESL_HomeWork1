#include <iostream>
#include <string>
using namespace std;

// Wall Clock Time Measurement
#include <sys/time.h>

#include "GaussianBlur.h"
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
  GaussianBlur gaussian_blur("gaussian_blur");
  //Create signals
  sc_clock clk("clk", CLOCK_PERIOD, SC_NS); // which is defined in the CMakeList
  sc_signal<bool> rst("rst");

  //Create FIFO channels
  sc_fifo<unsigned char> r;
  sc_fifo<unsigned char> g;
  sc_fifo<unsigned char> b;
  sc_fifo<int> result;

  //Connect FIFO channels with modules (reference the architecture diagram)
  tb.i_clk(clk);
  tb.o_rst(rst);
  gaussian_blur.i_clk(clk); // we will need it when we do synthesis
  gaussian_blur.i_rst(rst);
  tb.o_r(r); // tb will output RGB data
  tb.o_g(g);
  tb.o_b(b);
  tb.i_result(result); 
  gaussian_blur.i_r(r); //  filter will receive RGB data
  gaussian_blur.i_g(g);
  gaussian_blur.i_b(b);
  gaussian_blur.o_result(result);

  tb.read_bmp(argv[1]);
  sc_start();
  std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
  tb.write_bmp(argv[2]);

  return 0;
}
