#include "AutoscalePair.h"
#include <arduino.h>


template <typename T> CAutoScalePair<T>::CAutoScalePair(){
    targetMax = 150;
    targetMin = 50;
}

template <typename T> CAutoScalePair<T>::CAutoScalePair(T targMin, T targMax){
    targetMax = targMax;
    targetMin = targMin;
}

template <typename T> bool CAutoScalePair<T>::update(T *inChA, T *inChB, T *outA, T *outB){
    m__chA.lastVal = m__chA.actVal;
    m__chB.lastVal = m__chB.actVal;
    m__chA.actVal = *inChA;
    m__chB.actVal = *inChB;
    searchMinMax(&m__chA);
    searchMinMax(&m__chB);
    updateMinMaxAverage(&m__chA, &m__chB);   
    updateMinMaxAverage(&m__chB, &m__chA);
    updateScalingFactors(&m__chA);
    updateScalingFactors(&m__chB);
    scaleCH(&m__chA, outA);
    scaleCH(&m__chB, outB);
    if (m__chA.mode == 1 && m__chB.mode == 1){
        return true;
    }
    return false;
} 

template <typename T> void CAutoScalePair<T>::searchMinMax(Ch *ChToChk){
    if (ChToChk->actVal > ChToChk->lastVal){
        if (ChToChk->risingIndex >= 0){ChToChk->risingIndex++;}
        else{ChToChk->risingIndex = 1;}
    }
    else if (ChToChk->actVal < ChToChk->lastVal){
        if (ChToChk->risingIndex <= 0){ChToChk->risingIndex--;}
        else{ChToChk->risingIndex = -1;}
    }

    if (ChToChk->actVal > ChToChk->tempMax){ChToChk->tempMax = ChToChk->actVal;}
    if (ChToChk->actVal < ChToChk->tempMin){ChToChk->tempMin = ChToChk->actVal;}
    T valueChange = ChToChk->actVal - ChToChk->lastVal;
    ChToChk->FiltValueChanges.measurement(valueChange);
    ChToChk->SumValChangeLastX = ChToChk->FiltValueChanges.getSum();
    valueChange = abs(valueChange);
    ChToChk->FiltValueChangesAbsolut.measurement(valueChange);
    ChToChk->SumAbsValChangeLastX = ChToChk->FiltValueChangesAbsolut.getSum();
    
}

template <typename T> void CAutoScalePair<T>::updateMinMaxAverage(Ch *ChToChk, Ch *ChRef){
   if (ChRef->SumAbsValChangeLastX > ChToChk->lowerTresChangeRateLastXRef && ChRef->SumAbsValChangeLastX < ChToChk->upperTreshChangRateLastXRef 
       && (ChRef->risingIndex > ChToChk->treshRisIndxRef || ChRef->risingIndex <  ChToChk->treshRisIndxRef * (-1))){
        if (ChToChk->risingIndex == ChToChk->treshRisIndx ){
            ChToChk->risingIndex++; //avoid taking same valley twice
            ChToChk->avMin = ChToChk->FiltAverMin.measurement(ChToChk->tempMin);
            ChToChk->tempMin = DEFAULT_MIN;
            ChToChk->tempMax = DEFAULT_MAX;
        }
        if (ChToChk->risingIndex == ChToChk->treshRisIndx * (-1)){
            ChToChk->avMax = ChToChk->FiltAverMax.measurement(ChToChk->tempMax);
            ChToChk->risingIndex--; //avoid taking same peak twice
            ChToChk->tempMin = DEFAULT_MIN;
            ChToChk->tempMax = DEFAULT_MAX; 
        }
    }
}

template <typename T> void CAutoScalePair<T>::updateScalingFactors(Ch *ChToChk){
    if (ChToChk->FiltAverMax.getNbrMeas() > 5 && ChToChk->FiltAverMin.getNbrMeas()){
        ChToChk->mode = 1;
        ChToChk->minLevelOffs = targetMin - ChToChk->avMin;
        ChToChk->factor = (float)targetMax/(float)(ChToChk->avMax - ChToChk->avMin);
    }
}

template <typename T> void CAutoScalePair<T>::scaleCH(Ch *ChToScale, T *ChOut){
    //amplifying channels
    *ChOut = (ChToScale->actVal - ChToScale->avMin) * ChToScale->factor + ChToScale->avMin + ChToScale->minLevelOffs;
} 



template <typename T> void CAutoScalePair<T>::setScalingFactors(T Ch1_fact, T Ch2_fact, T Ch1_minLevOffs, T Ch2_minLevOffs){
    m__chA.factor = (float)Ch1_fact;
    m__chB.factor = (float)Ch2_fact;
    m__chA.minLevelOffs = Ch1_minLevOffs;
    m__chB.minLevelOffs = Ch2_minLevOffs;
}

template <typename T> void CAutoScalePair<T>::setPeekDetectThresholdsChX(Ch *ChToSet,int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef){
    if(treshRisIndx >= 0){ChToSet->treshRisIndx = treshRisIndx;}
    if(treshRisIndxRef >= 0){ChToSet->treshRisIndxRef = treshRisIndxRef;}
    if(lowerTresChangeRateLastXRef >= 0){ChToSet->lowerTresChangeRateLastXRef = lowerTresChangeRateLastXRef;}
    if(upperTreshChangRateLastXRef >=0){ChToSet->upperTreshChangRateLastXRef = upperTreshChangRateLastXRef;}
}

template <typename T> void CAutoScalePair<T>::setPeekDetectThresholdsChA(int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef){
    setPeekDetectThresholdsChX(&m__chA, treshRisIndx, treshRisIndxRef, lowerTresChangeRateLastXRef, upperTreshChangRateLastXRef);
}

template <typename T> void CAutoScalePair<T>::setPeekDetectThresholdsChB(int treshRisIndx, int treshRisIndxRef, int lowerTresChangeRateLastXRef, int upperTreshChangRateLastXRef){
    setPeekDetectThresholdsChX(&m__chB, treshRisIndx, treshRisIndxRef, lowerTresChangeRateLastXRef, upperTreshChangRateLastXRef);
}

template <typename T> bool CAutoScalePair<T>::debugChX(Ch *ChToDbg, float *result_arr, int size_arr){
    if (size_arr >= 16){
        *result_arr = ChToDbg->tempMin;
        *(result_arr + 1) = ChToDbg->tempMax;
        *(result_arr + 2) = ChToDbg->actVal;
        *(result_arr + 3) = ChToDbg->lastVal;
        *(result_arr + 4) = ChToDbg->avMin;
        *(result_arr + 5) = ChToDbg->avMax;
        *(result_arr + 6) = ChToDbg->minLevelOffs;
        *(result_arr + 7) = ChToDbg->factor;
        *(result_arr + 8) = ChToDbg->mode;
        *(result_arr + 9) = ChToDbg->risingIndex;
        *(result_arr + 10) = ChToDbg->SumAbsValChangeLastX;
        *(result_arr + 11) = ChToDbg->SumValChangeLastX;
        *(result_arr + 12) = ChToDbg->treshRisIndx;
        *(result_arr + 13) = ChToDbg->treshRisIndxRef;
        *(result_arr + 14) = ChToDbg->lowerTresChangeRateLastXRef;
        *(result_arr + 15) = ChToDbg->upperTreshChangRateLastXRef;
        return true;
    }
    return false;
}

template <typename T> bool CAutoScalePair<T>::debugChA(float *result_arr, int size_arr16){
    return debugChX(&m__chA, result_arr, size_arr16);
}

template <typename T> bool CAutoScalePair<T>::debugChB(float *result_arr, int size_arr16){
    return debugChX(&m__chB, result_arr, size_arr16);
}

template class CAutoScalePair<int>;
template class CAutoScalePair<float>;
template class CAutoScalePair<double>;

