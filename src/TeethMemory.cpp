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

#include "TeethMemory.h"
#include <analog_filter.h>
#include <math.h>

template <typename T> CTeethMemory<T>::CTeethMemory(){ //Constructor
    m_init(-10,10);
}

template <typename T> CTeethMemory<T>::CTeethMemory(int indNeg, int indPos){ //Constructor
    m_init(indNeg, indPos);
}

template <typename T> CTeethMemory<T>::~CTeethMemory(){
    for (int i = 0; i < m__indNeg; i++){
        delete m__ValuesIndNeg[i];
    }
    for (int i = 0; i < m__ind0Pos; i++){
        delete m__ValuesInd0Pos[i];
    }
    delete [] m__ValuesInd0Pos;
    delete [] m__ValuesIndNeg;
}

template <typename T> void CTeethMemory<T>::m_init(int indNeg, int indPos){ //Constructor
  if (indNeg > -2)
    indNeg = 2;
  else if (indNeg < -10000)
    indNeg = 10000;
  else
    m__indNeg = abs(indNeg);

  m__ValuesIndNeg = new ANFLTR::CFilterAnalogOverMeasures<T>*[m__indNeg];
  for (int i = 0; i < m__indNeg; i++){
    m__ValuesIndNeg[i] = new ANFLTR::CFilterAnalogOverMeasures<T>(3,3);
    m__ValuesIndNeg[i]->setErrDefault(-99999);
  }

  if (indPos < 2)
    indPos = 2;
  else if (indNeg > 10000)
    indPos = 10000;
  else
    m__ind0Pos = indPos;

  m__ValuesInd0Pos = new ANFLTR::CFilterAnalogOverMeasures<T>*[m__ind0Pos];
  for (int i = 0; i < m__ind0Pos; i++){
    m__ValuesInd0Pos[i] = new ANFLTR::CFilterAnalogOverMeasures<T>(3,3);  
    m__ValuesInd0Pos[i]->setErrDefault(-99999);
  }
}

template <typename T> int CTeethMemory<T>::addValueByIndex(T value, int index){
    T tmpvalue = value;
    if (index >= 0 && index < m__ind0Pos || index < 0 && index < m__indNeg){
        if(index >= 0){
            m__actPtrValues = m__ValuesInd0Pos[index];
        }
        else{
            m__actPtrValues = m__ValuesIndNeg[abs(index)];
        }
        m__actPtrValues->measurement(tmpvalue);
        return m__actPtrValues->getNbrMeas();
    }
    else{
        return 0;
    }
}

template <typename T> T CTeethMemory<T>::getAvByIndex(int index){
    if (index >= 0 && index < m__ind0Pos || index < 0 && index < m__indNeg){
        if(index >= 0){
            m__actPtrValues = m__ValuesInd0Pos[index];
        }
        else{
            m__actPtrValues = m__ValuesIndNeg[abs(index)];
        }
        return m__actPtrValues->getAverage();
    }
    else{
        return -999999;
    }
}

template <typename T> double CTeethMemory<T>::getAvByIndexDbl(int index){
    if (index >= 0 && index < m__ind0Pos || index < 0 && index < m__indNeg){
        if(index >= 0){
            m__actPtrValues = m__ValuesInd0Pos[index];
        }
        else{
            m__actPtrValues = m__ValuesIndNeg[abs(index)];
        }
        return m__actPtrValues->getAverageDbl();
    }
    else{
        return -999999;
    }
}

template <typename T> int CTeethMemory<T>::getNbrMeasByIndex(int index){
    if (index >= 0 && index < m__ind0Pos || index < 0 && index < m__indNeg){
        if(index >= 0){
            m__actPtrValues = m__ValuesInd0Pos[index];
        }
        else{
            m__actPtrValues = m__ValuesIndNeg[abs(index)];
        }
        return m__actPtrValues->getNbrMeas();
    }
    else{
        return 0;
    }
}


template class CTeethMemory<int>;
template class CTeethMemory<float>;
template class CTeethMemory<double>;