#include <cmath>
#include "GaussianBlurRB.h"

GaussianBlurRB::GaussianBlurRB(sc_module_name n) : sc_module(n) {
  SC_THREAD(row_buffer);
  sensitive << i_clk.pos(); 
  dont_initialize(); // do not run the tread when the thread is being construct
  reset_signal_is(i_rst, false);
}

void GaussianBlurRB::row_buffer()
{
  int count = 1 ;
  for(unsigned int i = 0 ; i < 3 ; ++i)
  {
    for(unsigned int j = 0 ; j < 258 ; ++j)
    {
      pixel[i][j] = 0 ;
    } 
  }
  while(true)
  {
    /* push rows up */
    for(unsigned int a = 1 ; a  < 3 ; ++a) 
    {
      for(unsigned int b = 1 ; b < 257 ; ++b)
      {
        pixel[a-1][b] = pixel[a][b];
        //printf("%d  ",pixel[a-1][b]);
      }
        //printf("\n");
    }
    //printf("\n");
    
    //printf("%d\n",count);
    /* read the next row data */
    for(unsigned int c = 1 ; c < 257 ; ++c)
    {
      pixel[2][c] = (i_r[c-1].read() + i_g[c-1].read() +i_b[c-1].read()) / 3;
      //printf("%d  ",pixel[2][c]);
    }
    //printf("\n");
    count++;
    /* do computation with calling a function */
    if(count > 2)
    {
      do_filter(pixel,257);
    }
    wait(2);  
  }
  
}


void GaussianBlurRB::do_filter(unsigned char array[3][258],int len) 
{
    const double mask[MASK_N][MASK_X][MASK_Y] = {{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}}}; 
    
    
    printf("call\n");
    printf("%d\n",len);
    /*for(unsigned int a = 0 ; a  < 3 ; ++a) 
    {
      for(unsigned int b = 0 ; b < 258 ; ++b)
      {
        printf("%d  ",array[a][b]);
      }
        printf("\n");
    }*/
    for(int x = 1 ; x < len ; ++x)
    {
      for (unsigned int i = 0; i < MASK_N; ++i) { 
        val[i] = 0; // val[0]
      }
      //printf("%d ",x);
      for (unsigned int u = 0; u < MASK_X; ++u) 
      { //u,v are the filter size for row and col
        for (unsigned int v = 0; v < MASK_Y; ++v) 
        { // line 22 23 go through the 9 pixel
          for (unsigned int i = 0; i != MASK_N; ++i) 
          { // we only have one filter (mask) so can be ignore
            val[i] += array[u][x+v-1] * mask[i][u][v]; 
            printf("%lf  ",val[i]);
          }
        }
      }
      double total = 0;
      for (unsigned int i = 0; i != MASK_N; ++i) {
        total += val[i] * val[i]; // squre
      }
      int result = (int)(std::sqrt(total)); // root
      o_result[x-1].write(result);
    }
    printf("\n");
    
    
    //wait(2); //emulate module delay
}
