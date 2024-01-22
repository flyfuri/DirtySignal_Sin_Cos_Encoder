#pragma once

#define NBR_TEETH_ON_RACK 10 //number of theeth on rail
#define NBR_TO_AVR_IND_TOOTH 5 //number to average individual tooth 

template <typename T>
class CTeethMemory{
    private:
        struct HalfTooth {
                    T halft_MinMax[NBR_TO_AVR_IND_TOOTH]; //point on summary curve above crossing of both channels is taken as max of that theeth
                    int halftMinMax_subindex; // index of subarray (second dimension) containing the last x (NBR_TO_AVR_IND_TOOTH) measurement for a specific halfteeth
                    T MinMaxAv;
                }toothedrack[NBR_TEETH_ON_RACK  * 2]; //learned behaviour of teethrack. Array double as long and start in middle of array since we don't know where we start at init

    public:
        CTeethMemory();
        bool calcMemIndexForMin(int halftooth, int &calculatedIndex); //calculate actual index to find learned halfteeth min max (returns false when out of range)  
        bool calcMemIndexForMax(int halftooth, int &calculatedIndex); //calculate actual index to find learned halfteeth min max (returns false when out of range)        
        T addCalcMinAv(int halftooth, T valueToAdd); //add and calc average Min for given half-tooth
        T addCalcMaxAv(int halftooth, T valueToAdd); //add and calc average Min for given half-tooth  
        T getMinMaxAv_withArrIndex(int arrIndex); //get min max average straight from memory when index is known (only to use with index created by "calcMemIndexFor.."!)
        T getMinAv(int halftooth); //get min average
        T getMaxAv(int halftooth); //get max average

};
       