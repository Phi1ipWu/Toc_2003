////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CTMatrix （C - Class  T - Template）
//  描述：模板化的矩阵类，实际是安全包装了一个二维数组。
//        利用强大的模板，是在不牺牲任何性能的情况下的可复用。
//        挖咔咔咔咔
//
//  作者：Kudoxinyiji
//  日期：2005-11-09
//
////////////////////////////////////////////////////////////////////////////



#include "TMatrix.h"
#include <memory.h>


//  最大行数
#define LINE_COUNT_MAX        32768

//  最大列数
#define COLUMN_COUNT_MAX      32768

//  默认行数
#define LINE_COUNT_DEFAULT    256

//  默认列数
#define COLUMN_COUNT_DEFAULT  256




//////////////////////////////////////////////////////////////////
//
//  功能：默认构造函数
//  日期：2005-09-04
//
template <typename T> CTMatrix<T>::CTMatrix()
{
    //  数据引用设 NULL，当前位置 -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  行数和列数均先设为 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  已分配内存的行数和列数也为 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;
}




//////////////////////////////////////////////////////////////////
//
//  功能：拷贝构造函数
//  日期：2005-11-09
//  参数：const CTMatrix<T>& oT [in]  另一对象
//
template <typename T> CTMatrix<T>::CTMatrix(const CTMatrix<T>& oT)
{
    //  数据引用设 NULL，当前位置 -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  行数和列数均先设为 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  已分配内存的行数和列数也为 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    this->operator =(oT);
}




//////////////////////////////////////////////////////////////////
//
//  功能：构造一个已经确定大小的对象的构造函数
//  日期：2005-11-09
//  参数：const long nLineCount    [in] 行数
//        const long nColumnCount  [in] 列数
//
template <typename T> CTMatrix<T>::CTMatrix(const long& nLineCount, const long& nColumnCount)
{
    //  数据引用设 NULL，当前位置 -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  行数和列数均先设为 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  已分配内存的行数和列数也为 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    this->SetSize(nLineCount, nColumnCount);
}




//////////////////////////////////////////////////////////////////
//
//  功能：默认析构函数
//  日期：2005-09-04
//
template <typename T> CTMatrix<T>::~CTMatrix()
{
    //  对已经动态分配的内存进行销毁
    this->Delete();
}




//////////////////////////////////////////////////////////////////
//
//  功能：设置数据大小
//  日期：2005-09-04
//  参数：const long nLineCount   [in] 行数
//        const long nColumnCount [in] 列数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::SetSize(const long& nLineCount, const long& nColumnCount)
{
    if(0 >= nLineCount   || LINE_COUNT_MAX   < nLineCount ||
       0 >= nColumnCount || COLUMN_COUNT_MAX < nColumnCount)
    {
        //  行数和列数不正常，或是偏小或是偏大
        return 0L;
    }

    if(nLineCount <= m_nAllocLineCount && nColumnCount <= m_nAllocColumnCount)
    {
        //  新的场景大小比已分配的空间要小，为了保证效率
        //  直接设置行数列数和游标，直接返回
        m_nColumnCount = nColumnCount;
        m_nLineCount   = nLineCount;
        m_nCurrentPos  = 0;
        return 1L;
    }

    //  将原来的已分配内存进行回收销毁
    Delete();

    const long nAllocLineCount   = (nLineCount   > LINE_COUNT_DEFAULT)   ? nLineCount   : LINE_COUNT_DEFAULT;
    const long nAllocColumnCount = (nColumnCount > COLUMN_COUNT_DEFAULT) ? nColumnCount : COLUMN_COUNT_DEFAULT;

    try
    {
        //  动态创建一个二维数组
        m_pData = new T*[nAllocLineCount];
        memset(m_pData, 0, nAllocLineCount * sizeof(T*));

        for(long i = 0; i < nAllocLineCount; ++i)
        {
            m_pData[i] = new T[nAllocColumnCount];
        }
    }
    catch(...)
    {
        //  出错处理，new 分配内存失败处理
        //  将已分配的销毁，返回 0 表示失败
        Delete();
        return 0L;
    }

    //  保存数据的行数和列数
    m_nLineCount        = nLineCount;
    m_nColumnCount      = nColumnCount;

    //  保存已分配空间的行数和列数
    m_nAllocLineCount   = nAllocLineCount;
    m_nAllocColumnCount = nAllocColumnCount;

    //  数据游标定到第一行第一列位置
    m_nCurrentPos       = 0;

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：设置指定行指定列的数据
//  日期：2005-09-14
//  参数：const T* pData     [in] 数据的引用
//        const long nLine   [in] 行数
//        const long nColumn [in] 列数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::SetData(const T* pData, const long& nLine, const long& nColumn) const
{
    //
    //  如果传入的对象指针是 NULL
    //  或者当前位置已经超出范围，则返回 0 表示失败
    //  如果在范围内，则直接修改，返回 1
    //
    //  注：因为已经直接设置过了，则游标已不再具有可比性
    //  设置游标为数据已满的位置。
    //
    if(0 == m_pData || 0 == pData || nLine   >= m_nLineCount || 0 > nLine ||
                                     nColumn >= nColumnCount || 0 > nColumn)
    {
        return 0L;
    }

    pData[nLine][nColumn] = *pData;
    m_nCurrentPos = m_nLineCount * m_nColumnCount;

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：在指定行的位置插入指定行数
//  日期：2005-11-08
//  参数：const long nPos    [in] 插入行的位置
//        const long nCount  [in] 插入行的个数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::InsertLine(const long& nPos, const long& nCount)
{
    //
    //  如果数据还未初始化或者插入个数小于零或者过大
    //  返回 0 表示插入失败，不会影响原来的数据
    //
    if(0 == m_pData || 0 >= nCount || LINE_COUNT_MAX < (nCount + m_nLineCount))
    {
        return 0L;
    }

    long i;

    //
    //  如果新增添加若干个数后会超过原来已经分配的个数
    //  则重新分配新的内存空间来存放一行数据的指针
    //  并且在最后部分新申请内存来存储
    //
    if(m_nAllocLineCount < nCount + m_nLineCount)
    {
        T** pData;

        try
        {
            //  重新分配内存
            pData = new T*[nCount + m_nLineCount];

            //  先对刚分配的内存清零，再保留原先的全部指针
            memset(pData, 0, (nCount + m_nLineCount) * sizeof(T*));
            memcpy(pData, m_pData, m_nLineCount * sizeof(T*));

            //  对超出范围内的未分配空行进行内存分配
            for(i = 0; i < nCount; ++i)
            {
                pData[i + m_nLineCount] = new T[m_nAllocColumnCount];
            }
        }
        catch(...)
        {
            //  在分配内存中出现错误
            //  将可能已经分配了一半的内存释放

            if(pData)
            {
                for(i = 0; i < nCount; ++i)
                {
                    if(pData[i + m_nLineCount])
                    {
                        delete[] pData[i + m_nLineCount];
                        pData[i + m_nLineCount] = 0;
                    }
                }

                delete[] pData;
                pData = 0;
            }

            return 0L;
        }

        //  将原来过小的指针数组释放，再设置为刚分配的指针数组
        if(0 != pData)
        {
            delete[] m_pData;
            m_pData = pData;
            pData = 0;
        }
        else
        {
            //  新分配的指针丢失
            return 0L;
        }

        //  保存插入后的行数和已分配的行数
        m_nLineCount      = nCount + m_nLineCount;
        m_nAllocLineCount = m_nLineCount;
    }
    else
    {
        //  已分配存放指针的数组比添加后还大
        //  只需要对行数进行一下添加即可
        m_nLineCount += nCount;
    }

    //
    //  进行插入操作，如果插入位置不在有效范围内
    //  则会视为在最下面的位置进行添加，无需进入下面的 if 块内
    //
    //  插入方式，将原来数据均向下挪位。从下向上地挪
    //
    if(0 <= nPos && nPos < m_nLineCount)
    {
        T* pTemp;
        const long nLoop = (nPos + nCount > m_nLineCount) ? (m_nLineCount - nPos) : (m_nLineCount - nPos - nCount);

        for(i = 1; i <= nLoop; ++i)
        {
            //  将后面新增的与插入位置之后的进行移位
            pTemp = m_pData[m_nLineCount - i];
            m_pData[m_nLineCount - i] = m_pData[m_nLineCount - nCount - i];
            m_pData[m_nLineCount - nCount - i] = pTemp;
        }
    }

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：在指定列的位置插入指定列数
//  日期：2005-11-08
//  参数：const long nPos    [in] 插入列的位置
//        const long nCount  [in] 插入列的个数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::InsertColumn(const long& nPos, const long& nCount)
{
    //
    //  如果数据还未初始化或者插入个数小于零或者过大
    //  返回 0 表示插入失败，不会影响原来的数据
    //
    if(0 == m_pData || 0 > nCount || COLUMN_COUNT_MAX < (nCount + m_nColumnCount))
    {
        return 0L;
    }

    long i, j;

    //  超过已经分配的范围
    if(m_nAllocColumnCount < nCount + m_nColumnCount)
    {
        T* pTemp;

        try
        {
            for(i = 0; i < m_nAllocLineCount; ++i)
            {
                pTemp = new T[nCount + m_nColumnCount];
                memcpy(pTemp, m_pData[i], m_nColumnCount * sizeof(T));
                delete[] m_pData[i];
                m_pData[i] = pTemp;
            }
        }
        catch(...)
        {
            //  分配内存中出现错误
            return 0L;
        }

        //  保存下已经分配的列数的大小
        m_nColumnCount      = nCount + m_nColumnCount;
        m_nAllocColumnCount = m_nColumnCount;
    }
    else
    {
        //  已分配存放一行数据的数组比添加后还大
        //  只需要对列数进行一下添加即可
        m_nColumnCount += nCount;
    }

    //
    //  进行插入操作，如果插入位置不在有效范围内
    //  则会视为在最右的位置进行添加，无需进入下面的 if 块内
    //
    //  插入方式，将原来数据均向右挪位。从右向左地挪
    //
    if(0 <= nPos && nPos < m_nColumnCount)
    {
        T objTemp;
        const long nLoop = m_nColumnCount - nPos - nCount;

        for(j = 0; j < m_nColumnCount; ++j)
        {
            for(i = 1; i <= nLoop; ++i)
            {
                objTemp = m_pData[j][m_nColumnCount - i];
                m_pData[j][m_nColumnCount - i] = m_pData[j][m_nColumnCount - nCount - i];
                m_pData[j][m_nColumnCount - nCount - i] = objTemp;
            }
        }
    }

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：在指定行的位置移除指定行数
//  日期：2005-11-09
//  参数：const long nPos    [in] 移除行的位置
//        const long nCount  [in] 移除行的个数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::RemoveLine(const long& nPos, const long& nCount)
{
    //
    //  如果数据未设置过大小，则直接返回 0 表示失败
    //  或者 nPos 不在有效范围内，或者要移除的已经超过已经分配的范围了
    //
    if(0 == m_pData || 0 > nPos || nPos >= m_nLineCount || 0 >= nCount)
    {
        return 0L;
    }

    long i;
    T* pTemp;

    //
    //  防止越界，对向后移动的行数进行判定并分开处理
    //  同时对二维数组的行数进行调整
    //
    long nInterval;
    if(nPos + nCount > m_nLineCount)
    {
        //  移除个数越界
        nInterval    = m_nLineCount - nPos;
        m_nLineCount = nPos;
    }
    else
    {
        nInterval     = nCount;
        m_nLineCount -= nCount;
    }

    //
    //  最后一步：数据行移位，从上向下依次向后移
    //  将移除的行存放至最后一行元素的下面
    //
    for(i = 0; i < m_nLineCount - nPos; ++i)
    {
        pTemp = m_pData[nPos + i];
        m_pData[nPos + i] = m_pData[nPos + nInterval + i];
        m_pData[nPos + nInterval + i] = pTemp;
    }

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：在指定列的位置移除指定列数
//  日期：2005-11-09
//  参数：const long nPos    [in] 移除列的位置
//        const long nCount  [in] 移除列的个数
//  返回值：1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::RemoveColumn(const long& nPos, const long& nCount)
{
    //
    //  如果数据未设置过大小，则直接返回 0 表示失败
    //  或者 nPos 不在有效范围内
    //
    if(0 == m_pData || 0 > nPos || nPos >= m_nColumnCount || 0 >= nCount)
    {
        return 0L;
    }

    long i, j;
    T objTemp;

    //
    //  防止越界，对向后移动的行数进行判定并分开处理
    //  同时对二维数组的列数进行调整
    //
    long nInterval;
    if(nPos + nCount > m_nColumnCount)
    {
        //  移除个数越界
        nInterval      = m_nColumnCount - nPos;
        m_nColumnCount = nPos;
    }
    else
    {
        nInterval       = nCount;
        m_nColumnCount -= nCount;
    }

    //
    //  数据列移位，从左至右依次向右移
    //  将每一行移除的元素存放至最后一个元素的右面
    //
    for(j = 0; j < m_nLineCount; ++j)
    {
        for(i = 0; i < m_nColumnCount - nPos; ++i)
        {
            objTemp = m_pData[j][nPos + i];
            m_pData[j][nPos + i] = m_pData[j][nPos + nInterval + i];
            m_pData[j][nPos + nInterval + i] = objTemp;
        }
    }

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得数据的引用
//  日期：2005-09-04
//  返回值：T** 全部区域数据的引用（二维数组）
//
template <typename T> inline T** CTMatrix<T>::GetDataRef() const
{
    return m_pData;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得当前数据位置（内联函式）
//  日期：2005-09-04
//  返回值：当前数据位置
//
template <typename T> inline long CTMatrix<T>::GetCurrentPos() const
{
    return m_nCurrentPos;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得行数（内联函式）
//  日期：2005-09-11
//  返回值：行数
//
template <typename T> inline long CTMatrix<T>::GetLineCount() const
{
    return m_nLineCount;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得列数（内联函式）
//  日期：2005-09-11
//  返回值：列数
//
template <typename T> inline long CTMatrix<T>::GetColumnCount() const
{
    return m_nColumnCount;
}




//////////////////////////////////////////////////////////////////
//
//  功能：添加数据
//  日期：2005-09-04
//  参数：const T* pData  [in]  新的一块区域的引用
//  返回值：添加是否成功     1 - 成功    0 - 失败
//
template <typename T> long CTMatrix<T>::Fill(const T* pData)
{
    //
    //  如果传入的对象指针是 NULL
    //  或者当前位置已经超出范围，则返回 0 表示失败
    //  如果在范围内，则添加进去，并且返回 1 表示成功
    //
    if(pData && 0 <= m_nCurrentPos && (m_nCurrentPos < m_nColumnCount * m_nLineCount) && m_nColumnCount && m_nLineCount)
    {
        m_pData[m_nCurrentPos / m_nColumnCount][m_nCurrentPos % m_nColumnCount] = *pData;
        m_nCurrentPos ++ ;

        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：删除全部的数据
//  日期：2005-09-04
//
template <typename T> void CTMatrix<T>::Delete()
{
    if(m_pData)
    {
        for(long i = 0L; i < m_nAllocLineCount; ++i)
        {
            if(m_pData[i])
            {
                delete[] m_pData[i];
                m_pData[i] = 0;
            }
        }

        delete[] m_pData;
        m_pData = 0;
    }

    //  全部清 0
    m_nLineCount        = 0;
    m_nColumnCount      = 0;
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    m_nCurrentPos       = -1;
}




//////////////////////////////////////////////////////////////////
//
//  功能：数据是否合法
//  日期：2005-09-06
//  返回值：1-合法 0-不合法
//
template <typename T> inline long CTMatrix<T>::IsValid() const
{
    //  如果位置游标已经移到最后，则表示数据都已经填充
    //  是合法的，如果未移到最后，则数据是不完整的。
    return (m_nLineCount * m_nColumnCount == m_nCurrentPos);
}




//////////////////////////////////////////////////////////////////
//
//  功能：设置数据为合法
//  日期：2005-09-15
//
//  注：如果调用此方法，则需要自行处理数据的合法性
//
template <typename T> inline void CTMatrix<T>::SetValid()
{
    m_nCurrentPos = m_nLineCount * m_nColumnCount;
}




//////////////////////////////////////////////////////////////////
//
//  功能：重载中括号方法，返回一维数组
//  日期：2005-09-04
//  参数：const T* pData  [in]  新的一块区域的引用
//  返回值：对应行号的一维数组
//
template <typename T> inline T* CTMatrix<T>::operator[] (const long& nLine) const
{
    return m_pData[nLine];
}




//////////////////////////////////////////////////////////////////
//
//  功能：重载等号
//  日期：2005-11-09
//  参数：const CTMatrix<T>& oT [in]  另一对象
//  返回值：T& 当前对象自己
//
template <typename T> const CTMatrix<T>& CTMatrix<T>::operator=(const CTMatrix<T>& oT)
{
    this->Delete();

    const long nLineCount   = oT.GetLineCount();
    const long nColumnCount = oT.GetColumnCount();

    if(! this->SetSize(nLineCount, nColumnCount))
    {
        return (*this);
    }

    //  矩阵中每个对象按位拷贝
    for(long i = 0; i < nLineCount; ++i)
    {
        for(long j = 0; j < nColumnCount; ++j)
        {
            this->m_pData[i][j] = oT[i][j];
        }
    }

    return (*this);
}




//////////////////////////////////////////////////////////////////
//
//  功能：重载二维数组类型
//  日期：2005-11-09
//  返回值：二维数组的地址
//
template <typename T> inline CTMatrix<T>::operator T**() const
{
    return m_pData;
}



/*
//  CTMatrix 测试用演示
//  平时要放到注释块里

#include <stdio.h>
#pragma warning(disable:4313)

int main()
{
    long line = 10, col = 10;
    long i, j;

    CTMatrix<long> longMatrix;//, longMatrixBak;
    if(! longMatrix.SetSize(line, col))
    {
        printf("  SetSize Ln=%d, Col=%d Failed!\n", line, col);
    }

    for(i = 0; i < line * col; ++i)
    {
        longMatrix.Fill(&i);
    }

    CTMatrix<long> longMatrixBak(longMatrix);

    longMatrix.RemoveColumn(4, 3);
    longMatrixBak.InsertLine(8, 2);

    line = longMatrix.GetLineCount();
    col  = longMatrix.GetColumnCount();

    long** pTemp = longMatrix;
    printf("-------------------------------------------------------------\n");
    printf("  longMatrix       %ld x %ld  -  %ld\n\n", line, col, pTemp);

    for(i = 0; i < line; ++i)
    {
        printf("  %d", pTemp[i]);

        for(j = 0; j < col; ++j)
        {
            printf(" %4d", pTemp[i][j] < 0 ? -1 : pTemp[i][j]);
        }

        printf("\n");
    }

    line = longMatrixBak.GetLineCount();
    col  = longMatrixBak.GetColumnCount();

    printf("-------------------------------------------------------------\n");
    printf("  longMatrixBak    %ld x %ld  -  %ld\n\n", line, col, longMatrixBak.GetDataRef());

    for(i = 0; i < line; ++i)
    {
        printf("  %d", longMatrixBak[i]);

        for(j = 0; j < col; ++j)
        {
            printf(" %4d", longMatrixBak[i][j] < 0 ? -1 : longMatrixBak[i][j]);
        }

        printf("\n");
    }

    printf("-------------------------------------------------------------\n");

}
*/
