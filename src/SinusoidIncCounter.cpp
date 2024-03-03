//sinusoid increment counter (encoder).cpp

#include "SinusoidIncCounter.h"
#include "Teethmemory.h"
#include <math.h>
#include <Arduino.h>


#define _USE_MATH_DEFINES

#ifndef DEBGOUT
    #define DEBGOUT 0  //if this is 99 dbugprints will be active
#endif

CSinIncCntr::CSinIncCntr(){
    m__sumOnLastCrossing = -9999; 
    m__sumMidLine = 0;
    m__actStatusSUB = 0;  
    m__actStatusSUM = 0; 
    m__sumAtPowerON = -9999; 
    m__offset = -9999;
}


int CSinIncCntr::m__calcSumMid(){
    
    int tempCrsDiff = m__sum - m__sumOnLastCrossing;
    if(m__sumOnLastCrossing == -9999){  //only the case at very first crossing measured
        m__sumOnLastCrossing = m__sum;
    }
    else if (tempCrsDiff <= INIT_MIN_DIST_SUM_MINMAX * -1){ //act sum (this crossing) is MIN of curve
       if(m__sumMidLine == 0){ //initial search for middle line of summary curve
            m__offset = 0;//m__offset= (-2 * INTPOLRES) + m__SinInterpolMinMax(m__sum, m__sumOnLastCrossing, m__sumAtPowerON, INTPOLRES);//TODO not working correctly yet
            m__sumHighestMin = m__sum;
            m__sumLowestMax = m__sumOnLastCrossing;  
        }  
        if(m__sum > m__sumHighestMin)
            {m__sumHighestMin = m__sum;}
        if(m__sumOnLastCrossing > m__sumLowestMax)
            {m__sumLowestMax = m__sumOnLastCrossing;}  
        
        //return line which decides whether sum curve is MIN or MAX
        m__sumMidLine = (m__sumHighestMin + m__sumLowestMax) / 2;  
    }
    else if (tempCrsDiff >= INIT_MIN_DIST_SUM_MINMAX){ //act sum (this crossing) is MAX of curve
       if(m__sumMidLine == 0){ //initial search for middle line of summary curve
             m__offset = 0;//m__offset = (m__SinInterpolMinMax(m__sumOnLastCrossing, m__sum, m__sumAtPowerON, INTPOLRES) * -1) - INTPOLRES; //TODO not working correctly yet
            m__sumHighestMin = m__sumOnLastCrossing;
            m__sumLowestMax = m__sum;
        }  
        
        if(m__sumOnLastCrossing > m__sumHighestMin)
            {m__sumHighestMin = m__sumOnLastCrossing;}
        if(m__sum < m__sumLowestMax)
            {m__sumLowestMax = m__sum;}

        //return line which decides whether sum curve is MIN or MAX
        m__sumMidLine = (m__sumHighestMin + m__sumLowestMax) / 2; 
    }
    if(m__sumMidLine > 0){
        if (m__sum >= m__sumMidLine){
            m__actStatusSUM = 1; 
        }
        else{
            m__actStatusSUM = -1;
        }
        return m__sumMidLine;
        }
    else{
        return 0;
    }
}

int CSinIncCntr::m__SinInterpolMinMax(int min, int max, int actval, int resolution){
    if(max > 0 && min > 0){ //do only interpolate if some min and max is memorised already
        if(actval < min){
            return 0;
        }
        else if(actval >= max){
            return resolution;
        }
        else{
            double tmpresult = (resolution / PI) * ((asin(((2 / ((float)max - (float)min)) * ((float)actval - (float)min)) - 1)) + PI / 2);

            /*double tmpmax = max -min;  
            double tmpactval = actval - min;
            double tmpresult = (resolution / PI) * ((asin(((2 / tmpmax) * tmpactval)-1)) + PI/2);
            //double tmpresult = (resolution/(PI/2)) * sin(((PI/2)/tmpmax) * tmpactval);*/
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
        m__sumAtPowerON = m__sum;  //memorize actual sum to do interpolation of very first position later
    }

    if(m__actStatusSUB > 0 && m__sub < 0){ //CROSSING: when difference is crossing Null-line from positive to negative
        if (m__calcSumMid() != 0){
            if(m__sum >= m__sumMidLine) //sub FALLING sum at MAX  (channel lines are crossing: sub-curve crossing nullpoint FALLING with summary at MAX)
            {   
                if(m__sumOnLastCrossing < m__sumMidLine){    
                    TeethMem_Sum.addCalcMinAv(m__actHalfTooth, m__sumOnLastCrossing);
                }
                m__actHalfTooth--;
                if(TeethMem_Sum.calcMemIndexForMax(m__actHalfTooth, m__actIndexTeethrack)){
                    m__intpolMax = TeethMem_Sum.addCalcMaxAv(m__actHalfTooth, m__sum);
                    m__intpolMin = TeethMem_Sum.getMinAv(m__actHalfTooth);
                }
            }
            else //sub FALLING sum at MIN (channel lines are crossing: sub-curve crossing nullpoint FALLING with summary at MIN)
            {
                if(m__sumOnLastCrossing >= m__sumMidLine){    
                        TeethMem_Sum.addCalcMaxAv(m__actHalfTooth, m__sumOnLastCrossing);
                }
                m__actHalfTooth++;
                if(TeethMem_Sum.calcMemIndexForMin(m__actHalfTooth, m__actIndexTeethrack)){
                    m__intpolMin = TeethMem_Sum.addCalcMinAv(m__actHalfTooth, m__sum);
                    m__intpolMax = TeethMem_Sum.getMaxAv(m__actHalfTooth);
                }
            } 
        }
        m__sumOnLastCrossing = m__sum;
        m__actStatusSUB = -1;  //always to do when sub crossing zero line!
    }  
    else if(m__actStatusSUB < 0 && m__sub >= 0){////CROSSING: when difference is crossing Null-line from negative to positive
        if (m__calcSumMid() != 0){
            if(m__sum >= m__sumMidLine)//sub RISING sum at MAX  (channel lines are crossing: sub-curve crossing nullpoint RISING with summary at MAX)
            {   
                if(m__sumOnLastCrossing < m__sumMidLine){    
                    TeethMem_Sum.addCalcMaxAv(m__actHalfTooth, m__sumOnLastCrossing);
                }
                m__actHalfTooth++;
                m__intpolMax = TeethMem_Sum.addCalcMaxAv(m__actHalfTooth, m__sum);
                m__intpolMin = TeethMem_Sum.getMinAv(m__actHalfTooth);
            }
            else //sub RISING sum at MIN  (channel lines are crossing: sub-curve crossing nullpoint RISING with summary at MIN)
            {
                if(m__sumOnLastCrossing >= m__sumMidLine){    
                        TeethMem_Sum.addCalcMinAv(m__actHalfTooth, m__sumOnLastCrossing);
                }
                m__actHalfTooth--;
                m__intpolMin = TeethMem_Sum.addCalcMinAv(m__actHalfTooth, m__sum);
                m__intpolMax = TeethMem_Sum.getMaxAv(m__actHalfTooth);
            }
        }
        m__sumOnLastCrossing = m__sum;
        m__actStatusSUB = 1; //always to do when sub crossing zero line!
    }
    /*else{
        m__intpolMax = toothedrack[NBR_TEETH_ON_RACK + m__actHalfTooth].maxAv;
        m__intpolMin = toothedrack[NBR_TEETH_ON_RACK + m__actHalfTooth].minAv;
    } */
    if (m__sumMidLine != 0){
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
