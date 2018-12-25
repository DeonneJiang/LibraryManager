#include "common.h"

//计算字符串之间的距离之间的递归函数
int CalculateStringDistance_Recursive( std::string strA , int pABegin  , int pAEnd , std::string strB , int pBBegin , int pBEnd )
{
    if( pABegin > pAEnd )
    {
        if( pBBegin > pBEnd )
        {
            return 0;
        }
        else
        {
            return pBEnd - pBBegin + 1;
        }
    }

    if( pBBegin > pBEnd )
    {
        if( pABegin > pAEnd )
        {
            return 0;
        }
        else
        {
            return pAEnd - pBBegin + 1;
        }
    }

    if( strA[pABegin] == strB[pBBegin] )
    {
        return CalculateStringDistance_Recursive( strA , pABegin+1 , pAEnd , strB , pBBegin+1 , pBEnd );
    }
    else
    {
        int t1 = CalculateStringDistance_Recursive( strA , pABegin+1 , pAEnd , strB , pBBegin , pBEnd  );
        int t2 = CalculateStringDistance_Recursive( strA , pABegin , pAEnd , strB , pBBegin+1 , pBEnd  );
        if( t1>=t2)
        {
            return t2+1;
        }
        else
        {
            return t1+1;
        }
    }

}

//计算字符串相似度函数（计算公式：【 1/（距离+1）】），建议匹配距离：1/7
float CalculateStringSimilarity( std::string strA , std::string strB )
{
    int pABegin = 0;
    int pAEnd   = strA.length()-1;
    int pBBegin = 0;
    int pBEnd   = strB.length()-1;
    //int a = CalculateStringDistance_Recursive( strA , pABegin , pAEnd , strB , pBBegin , pBEnd )+1;
    return 1/((float)CalculateStringDistance_Recursive( strA , pABegin , pAEnd , strB , pBBegin , pBEnd )+1);
}

