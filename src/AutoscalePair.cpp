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
   if (ChRef->SumAbsValChangeLastX > 10 && ChRef->SumAbsValChangeLastX < 25 && (ChRef->risingIndex > 6 || ChRef->risingIndex < -6)){
        if (ChToChk->risingIndex == 3 ){
            ChToChk->risingIndex++; //avoid taking same valley twice
            ChToChk->avMin = ChToChk->FiltAverMin.measurement(ChToChk->tempMin);
            ChToChk->tempMin = DEFAULT_MIN;
            ChToChk->tempMax = DEFAULT_MAX;
        }
        if (ChToChk->risingIndex == -3){
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

template <typename T> bool CAutoScalePair<T>::debugCh(bool ChB, float *result_arr, int size_arr){
    Ch *ChToDbg;
    if (ChB){
        ChToDbg = &m__chB;
    }
    else {
        ChToDbg = &m__chA;
    }

    if (size_arr >= 12){
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
        return true;
    }
    return false;
}

template class CAutoScalePair<int>;
template class CAutoScalePair<float>;
template class CAutoScalePair<double>;

