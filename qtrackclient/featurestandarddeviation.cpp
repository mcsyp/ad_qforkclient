#include "featurestandarddeviation.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
using namespace std;
using namespace qri_neuron_lib;
float FeatureStandardDeviation::Process(const float *src_buffer, int len){
  float mean = FeatureMSE::ComputeMean(src_buffer,len);
  return Process(src_buffer,len,mean);
}

float FeatureStandardDeviation::Process(const float *src_buffer, int src_len, float mean){
  if(src_buffer==NULL || src_len<=1) return 0.0f;

  float sum=0.0f;
  for(int i=0;i<src_len;++i){
    sum = sum +(src_buffer[i]-mean)*(src_buffer[i]-mean);
  }
  return sqrt((float)(sum/(src_len-1)));
}


