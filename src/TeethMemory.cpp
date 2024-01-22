/* this class serves to memorize mins and maxs of the curve along the toothed rail
    how the memory is organized
                   *
                 *   *
               *      *
    *         *        *
     *       *          *
       *   *              *   *
         *                  *
    -1 ->|<- 0   ->|<- 1  ->|<-  2      half-tooth enumeration
         |         |        |
      min(-1)    max(0)   min(1)        odd min, even max, 
      min(0)     max(1)   min(2)        even min, odd max,
       **+0      **+1     **+2          index in memory array, ** = NBR_TEETH_ON_RACK (array is double the size)
*/

#include <TeethMemory.h>

template <typename T> CTeethMemory<T>::CTeethMemory(){
    for(int i = 0; i < NBR_TEETH_ON_RACK  * 2; i++){
        for(int ii = 0; ii < NBR_TO_AVR_IND_TOOTH; ii++){
            toothedrack[i].halft_MinMax[ii]=0;
        }
        toothedrack[i].MinMaxAv=0;
        toothedrack[i].halftMinMax_subindex=0;
    }
}

template <typename T> bool CTeethMemory<T>::calcMemIndexForMin(int halftooth, int &calculatedIndex){
    int tempTeethindex = (NBR_TEETH_ON_RACK + halftooth);
    tempTeethindex = (halftooth % 2 == 0) ? tempTeethindex : tempTeethindex + 1; 
    if(tempTeethindex < 0){
        calculatedIndex = 0;
        return false;
    } 
    else if (tempTeethindex >= NBR_TEETH_ON_RACK * 2){
        calculatedIndex = (NBR_TEETH_ON_RACK * 2) -1;
        return false;
    }
    else{
        calculatedIndex = tempTeethindex;
        return true;
    }
}

template <typename T> bool CTeethMemory<T>::calcMemIndexForMax(int halftooth, int &calculatedIndex){
    int tempTeethindex = (NBR_TEETH_ON_RACK + halftooth);
    tempTeethindex = (halftooth % 2 == 0) ? tempTeethindex + 1 : tempTeethindex; 
    if(tempTeethindex < 0){
        calculatedIndex = 0;
        return false;
    } 
    else if (tempTeethindex >= NBR_TEETH_ON_RACK * 2){
        calculatedIndex = (NBR_TEETH_ON_RACK * 2) -1;
        return false;
    }
    else{
        calculatedIndex = tempTeethindex;
        return true;
    }
}

template <typename T> T CTeethMemory<T>::getMinMaxAv_withArrIndex(int arrIndex){
    return toothedrack[arrIndex].MinMaxAv;
}

template <typename T> T CTeethMemory<T>::getMinAv(int halftooth){
    int tempToothIndex;
    if(calcMemIndexForMin(halftooth, tempToothIndex)){ 
        return getMinMaxAv_withArrIndex(tempToothIndex);
    }
    return 0; //TODO
}

template <typename T> T CTeethMemory<T>::getMaxAv(int halftooth){
    int tempToothIndex;
    if(calcMemIndexForMax(halftooth, tempToothIndex)){ 
        return getMinMaxAv_withArrIndex(tempToothIndex);
    }
    return 0; //TODO
}

template <typename T> T CTeethMemory<T>::addCalcMinAv(int halftooth, T valueToAdd){ //add and calc average Min for given half-tooth
    int tempToothIndex;
    if(calcMemIndexForMin(halftooth, tempToothIndex)){ 
        toothedrack[tempToothIndex].halft_MinMax[toothedrack[tempToothIndex].halftMinMax_subindex] = valueToAdd;
        if (++(toothedrack[tempToothIndex].halftMinMax_subindex) >= NBR_TO_AVR_IND_TOOTH ) //move index for next measure to replace
            toothedrack[tempToothIndex].halftMinMax_subindex = 0;
    }
    
    T temptotal = 0;
    int tempNbrsToAv = NBR_TO_AVR_IND_TOOTH;
    for(int i = 0; i < NBR_TO_AVR_IND_TOOTH; i++){  //calc average
        if(toothedrack[tempToothIndex].halft_MinMax[i] > 0){
            temptotal += toothedrack[tempToothIndex].halft_MinMax[i];
        }
        else if (tempNbrsToAv > 1){ //don't go lower than 1 to avoid later DIV0
            tempNbrsToAv--;
        }
    }
    if (temptotal > 0){
        return toothedrack[tempToothIndex].MinMaxAv = temptotal / tempNbrsToAv;
    }
    else{
        return toothedrack[tempToothIndex].MinMaxAv;
    }
}

template <typename T> T CTeethMemory<T>::addCalcMaxAv(int halftooth, T valueToAdd){ //add and calc average Max for given half-tooth 
    int tempToothIndex;
     if(calcMemIndexForMax(halftooth, tempToothIndex)){ 
        toothedrack[tempToothIndex].halft_MinMax[toothedrack[tempToothIndex].halftMinMax_subindex] = valueToAdd;
        if (++(toothedrack[tempToothIndex].halftMinMax_subindex) >= NBR_TO_AVR_IND_TOOTH ) //move index for next measure to replace
            toothedrack[tempToothIndex].halftMinMax_subindex = 0;
    }
    
    T temptotal = 0;
    int tempNbrsToAv = NBR_TO_AVR_IND_TOOTH;
    for(int i = 0; i < NBR_TO_AVR_IND_TOOTH; i++){  //calc average
        if(toothedrack[tempToothIndex].halft_MinMax[i] > 0){
            temptotal += toothedrack[tempToothIndex].halft_MinMax[i];
        }
        else if (tempNbrsToAv > 1){ //don't go lower than 1 to avoid later DIV0
            tempNbrsToAv--;
        }
    }
    if (temptotal > 0){
        return toothedrack[tempToothIndex].MinMaxAv = temptotal / tempNbrsToAv;
    }
    else{
        return toothedrack[tempToothIndex].MinMaxAv;
    }
}

template class CTeethMemory<int>;
template class CTeethMemory<float>;
template class CTeethMemory<double>;