//sinusoid increment counter (encoder).cpp

#include "SinusoidIncCounter.h"
#include "Teethmemory.h"
#include <math.h>
#include <Arduino.h>


#define _USE_MATH_DEFINES

CSinIncCntr::CSinIncCntr(){
    m__sumMidLine = 80;
    m__actStatusSUB = 0;  
    m__actStatusSUM = 0; 
    m__offset = 0;
}

int CSinIncCntr::setSumMidLine(int midl){
    return m__sumMidLine = midl;
}

int CSinIncCntr::m__SinInterpolMinMax(int min, int max, int actval, int resolution){
    if(max - min > 20){ 
        if(actval < min){
            return 0;
        }
        else if(actval >= max){
            return resolution;
        }
        else{
            double tmpresult = (resolution / PI) * ((asin(((2 / ((float)max - (float)min)) * ((float)actval - (float)min)) - 1)) + PI / 2);
            return (int)(tmpresult * 1000) % 1000 >= 500 ? (int)tmpresult + 1 : (int)tmpresult;  //take 3 digits after period to round
        }
    }
    else{
        return 0;
    }
}

int CSinIncCntr::calc(int actCh1, int actCh2){

    //calculate sum of both channels the help determine counting direction
    m__sum = actCh1 + actCh2;   

    //calculate difference (subtraction) of the channels to see where they cross (nullpoint of difference)
    m__sub = actCh1 - actCh2;


    //init difference curve half status at beginning
    if(m__actStatusSUB == 0){
        if(m__sub >=0){
            m__actStatusSUB = 1;
        }
        else{
            m__actStatusSUB = -1;
        } 
    }

    if(m__actStatusSUB > 0 && m__sub < 0){ //CROSSING: when difference is crossing Null-line from positive to negative
        if(m__sum >= m__sumMidLine) //sub FALLING sum at MAX  (channel lines are crossing: sub-curve crossing nullpoint FALLING with summary at MAX)
        {   
            m__intpolMax = SumCurveLastMaxs.measurement(m__sum);
            m__actHalfTooth--;
        }
        else //sub FALLING sum at MIN (channel lines are crossing: sub-curve crossing nullpoint FALLING with summary at MIN)
        {
            m__intpolMin = SumCurveLastMins.measurement(m__sum);
            m__actHalfTooth++;
        } 
        m__actStatusSUB = -1;  //always to do when sub crossing zero line!
    }  
    else if(m__actStatusSUB < 0 && m__sub >= 0){////CROSSING: when difference is crossing Null-line from negative to positive
       if(m__sum >= m__sumMidLine)//sub RISING sum at MAX  (channel lines are crossing: sub-curve crossing nullpoint RISING with summary at MAX)
        {   
            m__intpolMax = SumCurveLastMaxs.measurement(m__sum);
            m__actHalfTooth++;
        }
        else //sub RISING sum at MIN  (channel lines are crossing: sub-curve crossing nullpoint RISING with summary at MIN)
        {
            m__intpolMin = SumCurveLastMins.measurement(m__sum);
            m__actHalfTooth--;
        }
        m__actStatusSUB = 1; //always to do when sub crossing zero line!
    }
    if (m__intpolMax - m__intpolMin > 20){  //only interplate when min and max differ
        int tempIntpol = m__SinInterpolMinMax(m__intpolMin, m__intpolMax, m__sum, INTPOLRES);

        int tmpActPos;
        if (m__actStatusSUB < 0){
            tmpActPos = m__actHalfTooth * INTPOLRES + tempIntpol;
        }
        else if (m__actStatusSUB > 0){
            tmpActPos = m__actHalfTooth * INTPOLRES + (INTPOLRES - tempIntpol);
        }
        else{
            tmpActPos = m__actHalfTooth * INTPOLRES;
        }

        m__actPos = tmpActPos + m__offset;
    }

    return m__actPos;
} 

int CSinIncCntr::read(){
    return m__actPos;
} 

int CSinIncCntr::setTo(int value){
    return m__offset = value - (m__actPos - m__offset);
} 

bool CSinIncCntr::debug(float *result_arr, int size_arr9){
    if (size_arr9 >= 9){
        *result_arr = m__sum;
        *(result_arr + 1) = m__sub;
        *(result_arr + 2) = m__actStatusSUM;
        *(result_arr + 3) = m__actStatusSUB;
        *(result_arr + 4) = m__offset;
        *(result_arr + 5) = m__intpolMax;
        *(result_arr + 6) = m__intpolMin;
        *(result_arr + 7) = m__actHalfTooth;
        *(result_arr + 8) = m__actPos;
        return true;
    }
    return false;
}
