#include "AutoscalePair.h"
template <typename T> CAutoScalePair<T>::CAutoScalePair(){
    targetMax = 150;
    targetMin = 50;
}

template <typename T> CAutoScalePair<T>::CAutoScalePair(T targMin, T targMax){
    targetMax = targMax;
    targetMin = targMin;
}

template <typename T> bool CAutoScalePair<T>::update(T *inChA, T *inChB, T *outA, T *outB){
    m__chA.actVal = *inChA;
    m__chB.actVal = *inChB;
    searchMinMax(&m__chA, &m__chB);
    searchMinMax(&m__chB, &m__chA);
    updateMinMaxAverage(&m__chA, &m__chB);   
    updateMinMaxAverage(&m__chB, &m__chA);


    scaleCH(&m__chA, outA);
    scaleCH(&m__chB, outB);
    if (m__chA.mode == 1 && m__chB.mode == 1){
        return true;
    }
    return false;
} 

template <typename T> void CAutoScalePair<T>::searchMinMax(Ch *ChToChk, Ch *ChRef){
    if (ChToChk->actVal > ChToChk->lastVal && ChToChk->risingIndex < 5){
        if (ChToChk->risingIndex >=0){ChToChk->risingIndex++;}
        else{ChToChk->risingIndex = 0;}
    }
    else if (ChToChk->actVal < ChToChk->lastVal&& ChToChk->risingIndex > -5){
        if (ChToChk->risingIndex <=0){ChToChk->risingIndex--;}
        else{ChToChk->risingIndex = 0;}
    }
    else if (ChToChk->actVal == ChToChk->lastVal){
        ChToChk->risingIndex = 0;
    }
    if (ChToChk->actVal > ChToChk->tempMax){ChToChk->tempMax = ChToChk->actVal;}
    if (ChToChk->actVal < ChToChk->tempMin){ChToChk->tempMin = ChToChk->actVal;}
    ChToChk->lastVal = ChToChk->actVal;
}

template <typename T> void CAutoScalePair<T>::updateMinMaxAverage(Ch *ChToChk, Ch *ChRef){
   if (ChRef->risingIndex > 2 || ChRef->risingIndex < -2){
        if (ChToChk->risingIndex == 3 ){
            ChToChk->avMin = ChToChk->FiltAverMin.measurement(ChToChk->tempMin);
        }
        if (ChToChk->risingIndex == 3){
            ChToChk->avMax = ChToChk->FiltAverMax.measurement(ChToChk->tempMax); 
        }
    }
}

template <typename T> void CAutoScalePair<T>::updateScalingFactors(Ch *ChToChk){
    if (ChToChk->FiltAverMax.getNbrMeas() > 5 && ChToChk->FiltAverMin.getNbrMeas()){
        ChToChk->mode = 1;
        ChToChk->minLevel = ChToChk->avMin;
        ChToChk->factor = targetMax/(ChToChk->avMax - ChToChk->minLevel);
    }
}

template <typename T> void CAutoScalePair<T>::scaleCH(Ch *ChToScale, T *ChOut){
    //amplifying channels
    *ChOut = (ChToScale->actVal - ChToScale->minLevel) * ChToScale->factor + ChToScale->minLevel;
} 

template <typename T> void CAutoScalePair<T>::setScalingFactors(T Ch1_fact, T Ch2_fact, T Ch1_minLev, T Ch2_minLev){
    m__chA.factor = Ch1_fact;
    m__chB.factor = Ch2_fact;
    m__chA.minLevel = Ch1_minLev;
    m__chB.minLevel = Ch2_minLev;
}

template class CAutoScalePair<int>;
template class CAutoScalePair<float>;
template class CAutoScalePair<double>;

