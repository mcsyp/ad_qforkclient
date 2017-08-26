#ifndef FEATURESTANDARDDEVIATION_H
#define FEATURESTANDARDDEVIATION_H

#include "featuremse.h"
namespace qri_neuron_lib {
  /**
   * @date   2016-12-12
   * @author Song Yunpeng
   * @details
   * This class computes the MSE value of the input float buffer
   **/
  class FeatureStandardDeviation{
  public:
    /*purpose: compute the mse of the input buffer
     *input:
     * @src, the source float buffer.
     * @len, the length of the source buffer
     * @mean, the mean of the input src buffer
     *return:
     * return standard deviation
     */
    float Process(const float *src_buffer,int len,float mean);
    float Process(const float *src_buffer,int len);
  };

}//end of pipeline
#endif // FEATURESTANDARDDEVIATION_H
