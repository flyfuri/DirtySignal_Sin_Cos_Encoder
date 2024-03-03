#pragma once

#define DEFAULT_MAX -9999
#define DEFAULT_MIN 9999

#include <analog_filter.h>


template <typename T> 
class CAutoScalePair{
private: 
    T targetMax = 100, targetMin = 0; //target size of sinoide defined my valley(min) and peaks(max)
    struct Ch{
        T tempMax = DEFAULT_MAX, tempMin = DEFAULT_MIN; //min max on actual curve section
        T actVal= 0; //actual value 
        T lastVal = 0;  // prevoius value
        T avMin = DEFAULT_MIN, avMax = DEFAULT_MAX; //actual average min/max 
        T minLevelOffs = 0;  //level of minimum(above 0) 
        float factor = 1; //factor to scale the channel
        int mode = 0; //0= init, 1 = scaling
        int risingIndex; //value from -3 to 3 meaning how many consecutive times the value raised or fell
        T SumAbsValChangeLastX, SumValChangeLastX;  //
        ANFLTR::CFilterAnalogOverMeasures<T> FiltAverMin{10,10}, FiltAverMax{10,10}; //filter to calculate rolling average min and max of signal
        ANFLTR::CFilterAnalogOverMeasures<T> FiltValueChangesAbsolut{5,5}, FiltValueChanges{5,5};
    }m__chA, m__chB;
    void scaleCH(Ch *ChToScale, T *ChOut);          
    void searchMinMax (Ch *ChToChk);
    void updateMinMaxAverage(Ch *ChToChk, Ch *ChRef);
    void updateScalingFactors(Ch *ChtoChk);
    bool debugChX(Ch *ChToDbg, float *result_arr, int size_arr12);
public: 
    CAutoScalePair();
    CAutoScalePair(T targMin, T targMax);
    void setScalingFactors(T Ch1_fact, T Ch2_fact, T Ch1_minLev, T Ch2_minLev);
    bool update(T *inChA, T *inChB, T *outA, T *outB); 
    bool debugChA(float *result_arr, int size_arr12);
    bool debugChB(float *result_arr, int size_arr12);
};

