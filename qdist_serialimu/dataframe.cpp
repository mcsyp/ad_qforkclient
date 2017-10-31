/***************************************************************************
**
** Autohr : Yunpeng Song
** Contact: 413740951@qq.com
** Date:    2016-11-14
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "dataframe.h"

using namespace qri_neuron_lib;
using namespace std;

#if 0
DataFrame::DataFrame():category_(0){
  cols_=0;
  rows_=0;
  row_counter_=0;
  col_offset_array_=NULL;//this list is used to save the offset for each column
  data_buffer_=NULL;//the unified data buffer
  data_size_=0;//the size of data buffer

  col_sum_array_=NULL;
}
DataFrame* DataFrame::Create(int rows, int cols){
  if(rows<=0 || cols<=0)return NULL;
  DataFrame* frame = new DataFrame();
  if(!frame->Setup(rows,cols)){
    delete frame;
    return NULL;
  }
  return frame;
}

#endif

DataFrame::DataFrame(int rows, int cols){
  data_buffer_=NULL;
  col_sum_array_=NULL;
  col_offset_array_=NULL;

  setup(rows,cols);
}

DataFrame::~DataFrame(){
  disposeMatrix();
}

bool DataFrame::setup(int rows,int columns){
  if(rows<=0 || columns<=0){
    return false;
  }

  disposeMatrix();//release buffer

  //step1.save the rows and columns
  rows_ = rows;
  cols_ = columns;
  data_size_ = rows_*cols_;//calc the size of data buffer
  row_counter_ = 0;//set the row counter to 0;

  bool ret=true;
  do{
    //step2.malloc the buffer
    data_buffer_ =(float*)malloc(data_size_*sizeof(float));
    if(data_buffer_==NULL){
      ret = false;
      break;
    }

    //clear the data
    memset(data_buffer_,0,data_size_*sizeof(float));

    //step3.calc the offset for each row
    col_offset_array_ = (int*)malloc(cols_*sizeof(int));
    if(col_offset_array_==NULL){
      ret = false;
      break;
    }
    for(int i=0;i<cols_;++i){
      col_offset_array_[i] = i*rows;
    }

    //step4.create col_sum_array_ buffer
    col_sum_array_ = (float*)malloc(sizeof(float)*cols_);
    if(col_sum_array_==NULL){
      ret = false;
      break;
    }
    memset(col_sum_array_,0,sizeof(float)*cols_);

  }while(0);

  if(ret==false){
    disposeMatrix();
  }

  return ret;
}
void DataFrame::disposeMatrix(){
  cols_=0;
  rows_=0;
  row_counter_ = 0;
  data_size_ = 0;
  if(data_buffer_)free(data_buffer_);
  if(col_sum_array_)free(col_sum_array_);
  if(col_offset_array_)free(col_offset_array_);
}

const float* DataFrame::readColumnData(int index) const{
  if(index<0 ||
     index>=cols_ ||
     data_buffer_==NULL ||
     col_offset_array_==NULL){
    return NULL;
  }
  return (data_buffer_+col_offset_array_[index]);
}

int DataFrame::readRowData(int row_index, float *out_row, int out_row_size) const{
  if(row_index<0 || row_counter_>=rows_) return 0;
  if(out_row==NULL || out_row_size<cols_) return 0;

  for(int i=0;i<cols_;++i){
    out_row[i] = read(row_index,i);
  }
  return cols_;
}
float DataFrame::read(int row, int col) const{
  if(col >= cols_ ||  row>=row_counter_){
    return 0.0f;
  }
  const float * col_data = readColumnData(col);
  if(col_data==NULL)return 0.0f;

  return col_data[row];
}

const float * DataFrame::reference() const{
  return data_buffer_;
}
int DataFrame::framesize() const{
  return data_size_;
}

bool DataFrame::push(const float * row_data,int len){
  if(cols_==0||
   row_data==NULL ||
   len<=0 || len>cols_||
   full()){
   return false;
  }

  for(int i=0;i<len;++i){
    int offset = col_offset_array_[i];
    float * row_buffer = data_buffer_+offset;
    //save the data to the queue
    row_buffer[row_counter_]=row_data[i];

    //sum the value to the sum
    col_sum_array_[i] += row_data[i];
  }
  ++row_counter_;

  return true;
}


void DataFrame::pop(int pop_size){
  if(empty() || pop_size<=0 || pop_size>row_counter_){
    return;
  }


  for(int i=0;i<cols_;i++){
    int offset = col_offset_array_[i];
    float * row_buffer = data_buffer_+offset;

    //update stastistic related value
    for(int k=0;k<pop_size;++k){
      col_sum_array_[i] -= static_cast<float>(row_buffer[k]);
    }

    //roll back part front.
    if(pop_size!=row_counter_){
      memcpy(row_buffer,row_buffer+pop_size,(row_counter_-pop_size)*sizeof(float));
    }
  }

  //update length counter;
  row_counter_=row_counter_-pop_size;
}

void DataFrame::clear(){
  row_counter_=0;
  if(data_buffer_){//clear the buffer
    memset(data_buffer_,0,data_size_*sizeof(float));
  }
  if(col_sum_array_){//clear sum array
    memset(col_sum_array_,0,cols_*sizeof(float));
  }
}


int DataFrame::serialize(float* buffer, int len) const {
  unsigned int size = rowLength()*cols_;
  if(buffer==NULL || (unsigned int)len<size){
    return 0;
  }

  //serialzie the outptut
  if(full()){//if the frame is full. all in.
    memcpy(buffer,data_buffer_,size*sizeof(float));
  }else{
    float* ptr_buffer=buffer;
    size=0;
    for(int i=0;i<cols_;++i){
      const float * col_buffer = readColumnData(i);
      ptr_buffer = buffer+i*rowLength();
      size+=rowLength();
      memcpy(ptr_buffer,col_buffer,rowLength()*sizeof(float));
    }
  }

  return size;
}
float DataFrame::computeSum(float *buffer, int len) const{
  float sum=0.0f;
  for(int i=0;i<len;++i){
    sum += buffer[i];
  }
  return sum;
}

float DataFrame::sum(int col_index) const{
  if(col_index<0 || col_index>=cols_){
    return 0.0f;
  }
  return col_sum_array_[col_index];
}
float DataFrame::sum() const{
  float sum = 0.0f;
  for(int i=0;i<cols_;++i){
    sum += col_sum_array_[i];
  }
  return sum;
}
float DataFrame::mean(int col_index) const{
  return sum(col_index)/this->row_counter_;
}
float DataFrame::mean() const{
  return sum()/(this->row_counter_*this->cols_);
}

DataFrame* DataFrame::duplicate(){
  DataFrame* dst_frame = new DataFrame(this->rows_,this->cols_);
  if(dst_frame==NULL)return NULL;

  //step2.duplicate the data
  memcpy(dst_frame->data_buffer_,this->data_buffer_,this->data_size_*sizeof(float));
  dst_frame->row_counter_ = this->row_counter_;

  //step3.duplicate the col sum array for each column
  memcpy(dst_frame->col_sum_array_,this->col_sum_array_,this->cols_*sizeof(float));
  //step4. cat
  dst_frame->category_ = category_;

  return dst_frame;
}
