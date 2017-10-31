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
#include "featuremse.h"
#include  <stdlib.h>
#include  <math.h>
using namespace qri_neuron_lib;

float FeatureMSE::process(const float *src_buffer, int len){
  float mean = computeMean(src_buffer,len);
  return process(src_buffer,len,mean);
}
float FeatureMSE::process(const float *src_buffer, int len, float mean){
  if(src_buffer==NULL || len<=0)return 0.0f;

  float sum=0.0f;
  for(int i=0;i<len;++i){
    float temp=0.0f;
    temp = fabs(src_buffer[i]-mean);
    sum+=temp;
  }

  return (sum/len);
}

float FeatureMSE::computeMean(const float *src_buffer, int len){
  if(src_buffer==NULL || len<=0)return 0.0f;

  float sum=0.0f;
  for(int i=0;i<len;++i){
    sum += src_buffer[i];
  }
  return (sum/len);
}
float FeatureMSE::computeAbsoluteMean(const float *src_buffer, int len){
  if(src_buffer==NULL || len<=0)return 0.0f;

  float sum=0.0f;
  for(int i=0;i<len;++i){
    sum += (src_buffer[i]>=0)?(src_buffer[i]):(-src_buffer[i]);
  }
  return (sum/len);
}
