#pragma once

#include <analog_filter.h>
#include "TeethMemory.h"


#define INTPOLRES 100 //resulution of interpolation between halfteeth
#define DEFAULT_MAX -9999
#define DEFAULT_MIN 9999

class CSinIncCntr{
    private: 
        //used for basic counting the theeth (counts twice per tooth on each crossing point of the 2 channel curves):
        int m__sum;  //summary of both channels
        int m__sub;   //CH1 - CH2
        int m__sumMidLine = 0; //approx middle line of summary
        short m__actStatusSUM = 0;  //wich half of the summary curve we're now (-1=MIN, 0 undefined(at beginning), 1=MAX)
        short m__actStatusSUB = 0;  //wich half of the difference curve we're now (-1=negative, 0 undefined(at beginning), 1=positive)

        int m__offset = 0; //offset for end result
        
        // used to find the average peek and valley to be used for interplation
        ANFLTR::CFilterAnalogOverMeasures<int> m__SumCurveLastMaxs{10,10};
        ANFLTR::CFilterAnalogOverMeasures<int> m__SumCurveLastMins{10,10}; 

        //individual peeks to to be used for interplation
        int m__tempMax = DEFAULT_MAX; //max on actual curve section 
        int m__tempMin = DEFAULT_MIN; //min on actual curve section
        int m__directionIndex = 0; //shows the actual direction of counting and how many counts since direction change
        CTeethMemory<int> m__IndividualMaxs{-10,10};
        CTeethMemory<int> m__IndividualMins{-10,10};

        int m__intpolMax;  //actual Max used for interpolation
        int m__intpolMin;    //actual Min used for interpolation

        int m__actHalfTooth;  
        int m__actPos; //endresult
        int m__SinInterpolMinMax(int min, int max, int actval, int resolution);
        
    public:
        CSinIncCntr();
        int calc(int actCh1, int actCh2);
        int read();
        int setTo(int value);
        int setSumMidLine(int midl); //set line to be used with m__sum
        bool debug(float *result_arr, int size_arr11);
};