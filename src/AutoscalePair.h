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
        int treshRisIndx = 3; //threshold of rising index to check if peek/valley
        int treshRisIndxRef = 4; //threshold of rising index of ref channel for recognizing peek/valley
        int lowerTresChangeRateLastXRef = 8; //lower threshold of change rate of last couple values (abs) for recognizing peek/valley
        int upperTreshChangRateLastXRef = 30; //upper threshold of change rate of last couple values (abs) for recognizing peek/valley
        ANFLTR::CFilterAnalogOverMeasures<T> FiltAverMin{10,10}, FiltAverMax{10,10}; //filter to calculate rolling average min and max of signal
        ANFLTR::CFilterAnalogOverMeasures<T> FiltValueChangesAbsolut{5,5}, FiltValueChanges{5,5};
    }m__chA, m__chB;

    void scaleCH(Ch *ChToScale, T *ChOut);          
    void searchMinMax (Ch *ChToChk);
    void updateMinMaxAverage(Ch *ChToChk, Ch *ChRef);
    void updateScalingFactors(Ch *ChtoChk);
    void setPeekDetectThresholdsChX(Ch *ChToSet, int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef);
    bool debugChX(Ch *ChToDbg, float *result_arr, int size_arr12);
public: 
    CAutoScalePair();
    CAutoScalePair(T targMin, T targMax);
    void setPeekDetectThresholdsChA(int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef);
    void setPeekDetectThresholdsChB(int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef);
    void setScalingFactors(T Ch1_fact, T Ch2_fact, T Ch1_minLev, T Ch2_minLev);
    bool update(T *inChA, T *inChB, T *outA, T *outB); 
    bool debugChA(float *result_arr, int size_arr16);
    bool debugChB(float *result_arr, int size_arr16);
};

