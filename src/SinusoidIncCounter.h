#pragma once

#include <analog_filter.h>
#include "TeethMemory.h"


#define INTPOLRES 100 //resulution of interpolation between halfteeth
#define INIT_MIN_DIST_SUM_MINMAX 80 //initial minimal dist
#define SUM_MIDLINE_ABOVE_MIN 23 //initial minimal dist


class CSinIncCntr{
    private: 
        //used for basic counting the theeth (counts twice per tooth on each crossing point of the 2 channel curves):
        int m__sum;  //summary of both channels
        int m__sub;   //CH1 - CH2
        int m__sumOnLastCrossing = -9999; //point on sum curve when channel curves crosse last time
        int m__sumLowestMax;  //defines middle line of summary 
        int m__sumHighestMin; //defines middle line of summary 
        int m__sumMidLine = 0; //approx middle line of summary
        short m__actStatusSUB = 0;  //wich half of the difference curve we're now (-1=negative, 0 undefined(at beginning), 1=positive)
        short m__actStatusSUM = 0;  //wich half of the summary curve we're now (-1=MIN, 0 undefined(at beginning), 1=MAX)
        int m__sumAtPowerON = -9999; //take 
        int m__offset = -9999; //offset calculated after very first travel beetween to opposite crossings of the channel curve
        
        // used to find the approx. middle line of summary curve
        //ANFLTR::CFilterAnalogOverMeasures SumCurveLastMaxs{10,10};
        //ANFLTR::CFilterAnalogOverMeasures SumCurveLastMins{10,10};
        //bool m__initialSumMidFound; //initial sum middle line has been found

        //used for interpolation beetween flank counts to increse resolution 
        CTeethMemory<int> TeethMem_Sum;   

        int m__actIndexTeethrack; 
        int m__intpolMax;  //actual Max used for interpolation
        int m__intpolMin;    //actual Min used for interpolation


        double m_Ch1_scaled, m_Ch2_scaled; //scaled channels

        int m__actHalfTooth;  
        int m__actPos; //endresult

        
        //int m__addCalcMinAv(int halftooth, int valueToAdd); //add and calc average Min for given half-tooth
        //int m__addCalcMaxAv(int halftooth, int valueToAdd); //add and calc average Min for given half-tooth  
        int m__calcSumMid(); //add actual m_sum and calculate initial mid between min and max (if they differ a minimum amount and if min 10 measures were taken)
        int m__SinInterpolMinMax(int min, int max, int actval, int resolution);
        
    public:
        CSinIncCntr();
        int calc(int actCh1, int actCh2);
        int read();
        int setTo(int value);
        void debug(HardwareSerial* PtrToSerial);
};