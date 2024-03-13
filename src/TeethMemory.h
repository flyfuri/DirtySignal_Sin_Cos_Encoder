#pragma once
#include <analog_filter.h>

template <typename T>
class CTeethMemory{
    protected:
        ANFLTR::CFilterAnalogOverMeasures<T> **m__ValuesInd0Pos; //array of filters for indexes 0 and positive
        ANFLTR::CFilterAnalogOverMeasures<T> **m__ValuesIndNeg; //array of filters for negative indexes
        
        ANFLTR::CFilterAnalogOverMeasures<T> *m__actPtrValues;  //pointer to actual halfteeth
        int m__ind0Pos;
        int m__indNeg;
        
        void m_init(int indNeg, int indPos);
    public:
        CTeethMemory();
        CTeethMemory(int indNeg, int indPos);
        virtual ~CTeethMemory();

        int addValueByIndex(T value, int index);
        T getAvByIndex(int index); 
        double getAvByIndexDbl(int index); 
        int getNbrMeasByIndex(int index);  
};
       