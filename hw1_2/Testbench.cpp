#include <cassert>
#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"

unsigned char header[54] = {
    0x42,          // identity : B  0
    0x4d,          // identity : M  1
    0,    0, 0, 0, // file size     2 3 4 5
    0,    0,       // reserved1     6 7
    0,    0,       // reserved2     8 9
    54,   0, 0, 0, // RGB data offset  10 11 12 13
    40,   0, 0, 0, // struct BITMAPINFOHEADER size  14 15 16 17
    0,    0, 0, 0, // bmp width : int 會有 4個char 18 19 20 21 
    0,    0, 0, 0, // bmp height  22 23 24 25
    1,    0,       // planes     26 27
    24,   0,       // bit per pixel   28 29
    0,    0, 0, 0, // compression   30 31 32 33
    0,    0, 0, 0, // data size     34 35 36 37
    0,    0, 0, 0, // h resolution  38 39 40 41
    0,    0, 0, 0, // v resolution  42 43 44 45
    0,    0, 0, 0, // used colors   46 47 48 49
    0,    0, 0, 0  // important colors  50 51 52 53
};

Testbench::Testbench(sc_module_name n)
    : sc_module(n), output_rgb_raw_data_offset(54) { //檔讀進來後前面54個是header到第54個才是 raw data
  SC_THREAD(do_GaussianBlurRB);
  sensitive << i_clk.pos();
  dont_initialize();
}

int Testbench::read_bmp(string infile_name) {
  FILE *fp_s = NULL; // source file handler
  fp_s = fopen(infile_name.c_str(), "rb");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }
  // move offset to 10 to find rgb raw data offset
  fseek(fp_s, 10, SEEK_SET);
  assert(fread(&input_rgb_raw_data_offset, sizeof(unsigned int), 1, fp_s));

  // move offset to 18 to get width & height;
  fseek(fp_s, 18, SEEK_SET);
  assert(fread(&width, sizeof(unsigned int), 1, fp_s));
  assert(fread(&height, sizeof(unsigned int), 1, fp_s));

  // get bit per pixel
  fseek(fp_s, 28, SEEK_SET);
  assert(fread(&bits_per_pixel, sizeof(unsigned short), 1, fp_s));
  bytes_per_pixel = bits_per_pixel / 8;

  // move offset to input_rgb_raw_data_offset to get RGB raw data
  fseek(fp_s, input_rgb_raw_data_offset, SEEK_SET); //移到54

  source_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel); //size_t is unsigned int type
  if (source_bitmap == NULL) {
    printf("malloc images_s error\n");
    return -1;
  }

  target_bitmap =
      (unsigned char *)malloc((size_t)width * height * bytes_per_pixel);
  if (target_bitmap == NULL) {
    printf("malloc target_bitmap error\n");
    return -1;
  }

  printf("Image width=%d, height=%d\n", width, height);
  assert(fread(source_bitmap, sizeof(unsigned char),
               (size_t)(long)width * height * bytes_per_pixel, fp_s));
  fclose(fp_s);
  return 0;
}

int Testbench::write_bmp(string outfile_name) {
  FILE *fp_t = NULL;      // target file handler
  unsigned int file_size; // file size

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }
  /* 因為這些變數都是int 所以要變成char需用mask 取值 */
  // file size
  file_size = width * height * bytes_per_pixel + output_rgb_raw_data_offset;
  header[2] = (unsigned char)(file_size & 0x000000ff);
  header[3] = (file_size >> 8) & 0x000000ff;
  header[4] = (file_size >> 16) & 0x000000ff;
  header[5] = (file_size >> 24) & 0x000000ff;

  // width
  header[18] = width & 0x000000ff;
  header[19] = (width >> 8) & 0x000000ff;
  header[20] = (width >> 16) & 0x000000ff;
  header[21] = (width >> 24) & 0x000000ff;

  // height
  header[22] = height & 0x000000ff;
  header[23] = (height >> 8) & 0x000000ff;
  header[24] = (height >> 16) & 0x000000ff;
  header[25] = (height >> 24) & 0x000000ff;

  // bit per pixel
  header[28] = bits_per_pixel;

  // write header
  fwrite(header, sizeof(unsigned char), output_rgb_raw_data_offset, fp_t);

  // write image
  fwrite(target_bitmap, sizeof(unsigned char),
         (size_t)(long)width * height * bytes_per_pixel, fp_t);

  fclose(fp_t);
  return 0;
}

void Testbench::do_GaussianBlurRB() {
  int x, y;        // for loop counter
  unsigned char R, G, B; // color of R, G, B
  int total;
  int count = 1;
  o_rst.write(false);
  o_rst.write(true);

  for (y = 0; y <= height; ++y) 
  {
    for (x = 0; x < width; ++x) 
    {
      if( y != height )
      {
        R = *(source_bitmap +
            bytes_per_pixel * (width * (y) + (x)) + 2);
        G = *(source_bitmap +
            bytes_per_pixel * (width * (y) + (x)) + 1);
        B = *(source_bitmap +
            bytes_per_pixel * (width * (y) + (x)) + 0);
      }
      else
      {
        R = 0;
        G = 0;
        B = 0;
      }
      o_r[x].write(R);
      o_g[x].write(G);
      o_b[x].write(B);
      //printf("%d  ",x);
      //printf("%d\n",y);
       //emulate channel delay
    }
    count++;
    wait(1);
    if(count>2){
    for (unsigned int q = 0 ; q < width ; ++q)
    {
      if(i_result[q].num_available()==0) wait(i_result[q].data_written_event());
      total = i_result[q].read();
      *(target_bitmap + bytes_per_pixel * (width * y + q) + 2) = total;
      *(target_bitmap + bytes_per_pixel * (width * y + q) + 1) = total;
      *(target_bitmap + bytes_per_pixel * (width * y + q) + 0) = total;
    }
    }
  }
  sc_stop();
}
