# ESL Homework 1

## Information

---

### 110061637  林佳瑩

### Github link : [https://github.com/nthu110061637/ESL_HomeWork1](https://github.com/nthu110061637/ESL_HomeWork1)

## Implementation : Part 1 - hw1_1

---

Implement Gaussian Blur filter based on the example code of the sobel filter.

```cpp
/* GaussianBlur.cpp */
const double mask[MASK_N][MASK_X][MASK_Y] = {{{0.077847, 0.123317, 0.077847}, {0.123317, 0.195346, 0.123317}, {0.077847, 0.123317, 0.077847}}};
/* setting the filter with the content of the reference pge */
```

```cpp
/* filter_def.h */
const int MASK_N = 1; 
/* replace 2 with 1 because we only need one mask */

```

```cpp
/* Testbench.cpp */
if(i_result.num_available()==0) wait(i_result.data_written_event());
      total = i_result.read();
      //cout << "Now at " << sc_time_stamp() << endl; //print current sc_time
/* Let the picture show up without truncated by threshold so that the picture will be grey scale */
      //if (total - THRESHOLD >= 0) {
        // black
        *(target_bitmap + bytes_per_pixel * (width * y + x) + 2) = total;
        *(target_bitmap + bytes_per_pixel * (width * y + x) + 1) = total;
        *(target_bitmap + bytes_per_pixel * (width * y + x) + 0) = total;
      //} else {
        // white
        /**(target_bitmap + bytes_per_pixel * (width * y + x) + 2) = WHITE;
        *(target_bitmap + bytes_per_pixel * (width * y + x) + 1) = WHITE;
        *(target_bitmap + bytes_per_pixel * (width * y + x) + 0) = WHITE;*/
      //}
			
	
```

```cpp
/* GaussianBlur.cpp */
const double mask[MASK_N][MASK_X][MASK_Y] = {{{0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 0.0}}};
/* I use this to get the original picture which read from testbench in grey scale*/
/* This aims to do comparison with the picture after convolution with filter to see the effect of blur */
```

## Result : Part 1 - hw1_1

---

![ **Original**  ](https://raw.githubusercontent.com/nthu110061637/ESL_HomeWork1/main/hw1_1/build/origin.bmp)

 **Original**  

![**After**](https://raw.githubusercontent.com/nthu110061637/ESL_HomeWork1/main/hw1_1/build/GaussianBlur.bmp)

**After**

## Implementation : Part 2 - hw1_2

---

Implement Gaussian Blur filter with row buffer method to reduce the pixel transmitted.

```cpp
/* GaussianBlurRB.h */
sc_fifo_in<unsigned char> i_r [256];
sc_fifo_in<unsigned char> i_g [256];
sc_fifo_in<unsigned char> i_b [256];
sc_fifo_out<int> o_result [256];

/* use the wider fifo to send a whole row of pixel*/
```

**In GaussianBlurRB.cpp (too long to show up the entire code)**

- I use **“pixel”** array with the size of **[3][258]**. Using the number of 258 is to do padding.
- And I transmit the **entire row** through the fifo from testbnech to row buffer.
- And I push the second row to the first row and the third row to the second row then I read the next row of data to the third row.
```cpp
/* GaussianBlurRB.cpp */
/* the push row part */
for(unsigned int a = 1 ; a  < 3 ; ++a) 
{
  for(unsigned int b = 1 ; b < 257 ; ++b)
  {
    pixel[a-1][b] = pixel[a][b];
    //printf("%d  ",pixel[a-1][b]);
  }
    //printf("\n");
}
/* the read new row part*/
for(unsigned int c = 1 ; c < 257 ; ++c)
{
  pixel[2][c] = (i_r[c-1].read() + i_g[c-1].read() +i_b[c-1].read()) / 3;
  //printf("%d  ",pixel[2][c]);
}
```
- If there’s two rows have been read into the “pixel” array (i.e. row buffer), the thread “row_buffer” will call the function “do_filter” to do the computation then return the result to testbench.
- The data structure of **do_filter** is the picture below to multiply and add 9 pixels from row buffer and the Gaussian Blur mask.
```cpp
/* GaussianBlurRB.cpp */
/* the convolution part */
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

```
![ **Concept**  ](https://raw.githubusercontent.com/nthu110061637/ESL_HomeWork1/main/concept.jpg)
## Result : Part 2 - hw1_2

---
![ **Original**  ](https://raw.githubusercontent.com/nthu110061637/ESL_HomeWork1/main/hw1_2/build/originrb.bmp)

 **Original**  

![**After**](https://raw.githubusercontent.com/nthu110061637/ESL_HomeWork1/main/hw1_2/build/rowbuffer.bmp)

**After**
## Conclusion

---

The row buffer based can reduced the pixel transmitted from **256*256*9 to 256*256**.

Then the **data** stored in the row buffer can be **reused** to do computation.
