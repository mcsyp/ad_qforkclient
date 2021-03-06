#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

namespace qri_neuron_lib{
  /**
   * @date   2017-01-03
   * @author Song Yunpeng
   * This DataFrame is the basic processing format of the pipeline
   **/
  class DataFrame
  {
  public:
    //static DataFrame* Create(int rows, int cols);

    DataFrame(int rows, int cols);
    virtual ~DataFrame();

    void clear();//clear the dataList memeory, but just empty the dataArray.

    /*purpose: push one row data to the frame
     * @row_data, one row of data buffer
     * @len, the length of the data buffer
     *return:
     * @return true if success
     */
    bool push(const float * row_data,int len);

    //popout the first row in the frame
    void pop(int offset);

    /*purpose:dump all memory into a 1D buffer.
     *input:
     * @buffer, the output buffer
     * @len, the length of the buffer
     *return:
     * @the output buffer length.
     * @-1, fail to output buffer.
     */
    int serialize(float* buffer, int len) const;

    float  read(int rowIndex,int colIndex) const;//return the data

    /*Purpose: read entire column data
     *input:
     * @col_index, the column index
     *return:
     * @the column data buffer.d
     */
    const float * readColumnData(int col_index) const;

    /*Purpose: output one row data
     *input:
     * @row_index, the row index
     * @out_row_size, the output buffer size
     *output:
     * @out_row, the output row buffer
     *return:
     * @the output buffer length.
     */
    int readRowData(int row_index, float* out_row,int out_row_size) const;

    const float* reference()const;//return the whole raw buffer of this frame.
    int framesize()const;//the frame size

    //return the rows and columns of this data frame
    int rows() const{return rows_;}
    int columns() const{return cols_;}

    //return how many rows in this frame
    int rowLength() const{ return row_counter_;}

    inline bool full() const{ return (row_counter_==rows_);}
    inline bool empty() const{ return (row_counter_==0);}

    float sum(int col_index) const;
    float sum()const;
    float mean(int col_index) const;
    float mean() const;

    DataFrame* duplicate();
  protected:
    bool setup(int rows,int columns);
    //clear frame data
    void disposeMatrix();

  private:
    float computeSum(float * buffer, int len) const;//calc the sum
  protected:
    uint16_t rows_; //rows of this dataframe
    uint16_t cols_;//total columns of this dataframe
    uint16_t row_counter_;//how many rows of valid data now.

  private:
    float *data_buffer_;//the unified data buffer
    uint16_t data_size_;//the size of data buffer

    int * col_offset_array_;//this list is used to save the offset for each column
    float * col_sum_array_;//the sum value for each column data
  public:
    int category_;
    int offset_;//gloabl counter for multiple purpose
  };


} //end of namespace
#endif // DATAFRAME_H
