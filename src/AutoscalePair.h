#pragma once
#define DEFAULT_MAX -9999
#define DEFAULT_MIN 9999

#include <analog_filter.h>


template <typename T> 
class CAutoScalePair{
private: 
    T targetMax = 150, targetMin = 50; //target size of sinoide defined my valley(min) and peaks(max)
    struct Ch{
        T tempMax = DEFAULT_MAX, tempMin = DEFAULT_MIN; //min max on actual curve section
        T actVal= 0; //actual value 
        T lastVal = 0;  // prevoius value
        int mode = 0; //0= init, 1 = scaling
        int risingIndex; //value from -3 to 3 meaning how many consecutive times the value raised or fell
        ANFLTR::CFilterAnalogOverMeasures<T> FiltAverMin{10,10}, FiltAverMax{10,10}; //filter to calculate rolling average min and max of signal
        T avMin = DEFAULT_MIN, avMax = DEFAULT_MAX; //actual average min/max 
        T factor = 1; //factor to scale the channel
        T minLevel = 0;  //level of minimum(above 0) 
    }m__chA, m__chB;
    CAutoScalePair();
    CAutoScalePair(T targMin, T targMax);
    void scaleCH(Ch *ChToScale, T *ChOut);          
    void searchMinMax (Ch *ChToChk, Ch *ChRef);
    void updateMinMaxAverage(Ch *ChToChk, Ch *ChRef);
    void updateScalingFactors(Ch *ChtoChk);
public:
    
    void setScalingFactors(T Ch1_fact, T Ch2_fact, T Ch1_minLev, T Ch2_minLev);
    bool update(T *inChA, T *inChB, T *outA, T *outB);  

};

