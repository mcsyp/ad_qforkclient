#ifndef FILTERMSE_H
#define FILTERMSE_H

namespace qri_neuron_lib {
  /**
   * @date   2016-12-12
   * @author Song Yunpeng
   * @details
   * This class computes the MSE value of the input float buffer
   **/
  class FeatureMSE{
  public:
    /*purpose: compute the mse of the input buffer
     *input:
     * @src, the source float buffer.
     * @len, the length of the source buffer
     *return:
     * MSE value
     */
    virtual float Process(const float *src_buffer,int len);

    /*purpose: scan the peaks of the input float buffer.
     *input:
     * @src, the source float buffer.
     * @len, the length of the source buffer
     * @mean, the pre-computed mean of the input buffer
     *return:
     * the total number of peaks and valleys
     */
    virtual float Process(const float *src_buffer,int len,float mean);

    static float ComputeMean(const float * src_buffer, int len);
    static float ComputeAbsoluteMean(const float * src_buffer, int len);
  };

}//end of pipeline
#endif // FILTERTHRESHOLD_H
