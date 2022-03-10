////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CTMatrix ��C - Class  T - Template��
//  ������ģ�廯�ľ����࣬ʵ���ǰ�ȫ��װ��һ����ά���顣
//        ����ǿ���ģ�壬���ڲ������κ����ܵ�����µĿɸ��á�
//        ����������
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-11-09
//
////////////////////////////////////////////////////////////////////////////



#include "TMatrix.h"
#include <memory.h>


//  �������
#define LINE_COUNT_MAX        32768

//  �������
#define COLUMN_COUNT_MAX      32768

//  Ĭ������
#define LINE_COUNT_DEFAULT    256

//  Ĭ������
#define COLUMN_COUNT_DEFAULT  256




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�Ĭ�Ϲ��캯��
//  ���ڣ�2005-09-04
//
template <typename T> CTMatrix<T>::CTMatrix()
{
    //  ���������� NULL����ǰλ�� -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  ����������������Ϊ 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  �ѷ����ڴ������������ҲΪ 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ��������캯��
//  ���ڣ�2005-11-09
//  ������const CTMatrix<T>& oT [in]  ��һ����
//
template <typename T> CTMatrix<T>::CTMatrix(const CTMatrix<T>& oT)
{
    //  ���������� NULL����ǰλ�� -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  ����������������Ϊ 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  �ѷ����ڴ������������ҲΪ 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    this->operator =(oT);
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�����һ���Ѿ�ȷ����С�Ķ���Ĺ��캯��
//  ���ڣ�2005-11-09
//  ������const long nLineCount    [in] ����
//        const long nColumnCount  [in] ����
//
template <typename T> CTMatrix<T>::CTMatrix(const long& nLineCount, const long& nColumnCount)
{
    //  ���������� NULL����ǰλ�� -1
    m_pData        = 0;
    m_nCurrentPos  = -1;

    //  ����������������Ϊ 0
    m_nLineCount   = 0;
    m_nColumnCount = 0;

    //  �ѷ����ڴ������������ҲΪ 0
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    this->SetSize(nLineCount, nColumnCount);
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�Ĭ����������
//  ���ڣ�2005-09-04
//
template <typename T> CTMatrix<T>::~CTMatrix()
{
    //  ���Ѿ���̬������ڴ��������
    this->Delete();
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ��������ݴ�С
//  ���ڣ�2005-09-04
//  ������const long nLineCount   [in] ����
//        const long nColumnCount [in] ����
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::SetSize(const long& nLineCount, const long& nColumnCount)
{
    if(0 >= nLineCount   || LINE_COUNT_MAX   < nLineCount ||
       0 >= nColumnCount || COLUMN_COUNT_MAX < nColumnCount)
    {
        //  ����������������������ƫС����ƫ��
        return 0L;
    }

    if(nLineCount <= m_nAllocLineCount && nColumnCount <= m_nAllocColumnCount)
    {
        //  �µĳ�����С���ѷ���Ŀռ�ҪС��Ϊ�˱�֤Ч��
        //  ֱ�����������������αֱ꣬�ӷ���
        m_nColumnCount = nColumnCount;
        m_nLineCount   = nLineCount;
        m_nCurrentPos  = 0;
        return 1L;
    }

    //  ��ԭ�����ѷ����ڴ���л�������
    Delete();

    const long nAllocLineCount   = (nLineCount   > LINE_COUNT_DEFAULT)   ? nLineCount   : LINE_COUNT_DEFAULT;
    const long nAllocColumnCount = (nColumnCount > COLUMN_COUNT_DEFAULT) ? nColumnCount : COLUMN_COUNT_DEFAULT;

    try
    {
        //  ��̬����һ����ά����
        m_pData = new T*[nAllocLineCount];
        memset(m_pData, 0, nAllocLineCount * sizeof(T*));

        for(long i = 0; i < nAllocLineCount; ++i)
        {
            m_pData[i] = new T[nAllocColumnCount];
        }
    }
    catch(...)
    {
        //  ������new �����ڴ�ʧ�ܴ���
        //  ���ѷ�������٣����� 0 ��ʾʧ��
        Delete();
        return 0L;
    }

    //  �������ݵ�����������
    m_nLineCount        = nLineCount;
    m_nColumnCount      = nColumnCount;

    //  �����ѷ���ռ������������
    m_nAllocLineCount   = nAllocLineCount;
    m_nAllocColumnCount = nAllocColumnCount;

    //  �����α궨����һ�е�һ��λ��
    m_nCurrentPos       = 0;

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�����ָ����ָ���е�����
//  ���ڣ�2005-09-14
//  ������const T* pData     [in] ���ݵ�����
//        const long nLine   [in] ����
//        const long nColumn [in] ����
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::SetData(const T* pData, const long& nLine, const long& nColumn) const
{
    //
    //  �������Ķ���ָ���� NULL
    //  ���ߵ�ǰλ���Ѿ�������Χ���򷵻� 0 ��ʾʧ��
    //  ����ڷ�Χ�ڣ���ֱ���޸ģ����� 1
    //
    //  ע����Ϊ�Ѿ�ֱ�����ù��ˣ����α��Ѳ��پ��пɱ���
    //  �����α�Ϊ����������λ�á�
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
//  ���ܣ���ָ���е�λ�ò���ָ������
//  ���ڣ�2005-11-08
//  ������const long nPos    [in] �����е�λ��
//        const long nCount  [in] �����еĸ���
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::InsertLine(const long& nPos, const long& nCount)
{
    //
    //  ������ݻ�δ��ʼ�����߲������С������߹���
    //  ���� 0 ��ʾ����ʧ�ܣ�����Ӱ��ԭ��������
    //
    if(0 == m_pData || 0 >= nCount || LINE_COUNT_MAX < (nCount + m_nLineCount))
    {
        return 0L;
    }

    long i;

    //
    //  �������������ɸ�����ᳬ��ԭ���Ѿ�����ĸ���
    //  �����·����µ��ڴ�ռ������һ�����ݵ�ָ��
    //  ��������󲿷��������ڴ����洢
    //
    if(m_nAllocLineCount < nCount + m_nLineCount)
    {
        T** pData;

        try
        {
            //  ���·����ڴ�
            pData = new T*[nCount + m_nLineCount];

            //  �ȶԸշ�����ڴ����㣬�ٱ���ԭ�ȵ�ȫ��ָ��
            memset(pData, 0, (nCount + m_nLineCount) * sizeof(T*));
            memcpy(pData, m_pData, m_nLineCount * sizeof(T*));

            //  �Գ�����Χ�ڵ�δ������н����ڴ����
            for(i = 0; i < nCount; ++i)
            {
                pData[i + m_nLineCount] = new T[m_nAllocColumnCount];
            }
        }
        catch(...)
        {
            //  �ڷ����ڴ��г��ִ���
            //  �������Ѿ�������һ����ڴ��ͷ�

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

        //  ��ԭ����С��ָ�������ͷţ�������Ϊ�շ����ָ������
        if(0 != pData)
        {
            delete[] m_pData;
            m_pData = pData;
            pData = 0;
        }
        else
        {
            //  �·����ָ�붪ʧ
            return 0L;
        }

        //  ����������������ѷ��������
        m_nLineCount      = nCount + m_nLineCount;
        m_nAllocLineCount = m_nLineCount;
    }
    else
    {
        //  �ѷ�����ָ����������Ӻ󻹴�
        //  ֻ��Ҫ����������һ����Ӽ���
        m_nLineCount += nCount;
    }

    //
    //  ���в���������������λ�ò�����Ч��Χ��
    //  �����Ϊ���������λ�ý�����ӣ������������� if ����
    //
    //  ���뷽ʽ����ԭ�����ݾ�����Ųλ���������ϵ�Ų
    //
    if(0 <= nPos && nPos < m_nLineCount)
    {
        T* pTemp;
        const long nLoop = (nPos + nCount > m_nLineCount) ? (m_nLineCount - nPos) : (m_nLineCount - nPos - nCount);

        for(i = 1; i <= nLoop; ++i)
        {
            //  �����������������λ��֮��Ľ�����λ
            pTemp = m_pData[m_nLineCount - i];
            m_pData[m_nLineCount - i] = m_pData[m_nLineCount - nCount - i];
            m_pData[m_nLineCount - nCount - i] = pTemp;
        }
    }

    return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ���ָ���е�λ�ò���ָ������
//  ���ڣ�2005-11-08
//  ������const long nPos    [in] �����е�λ��
//        const long nCount  [in] �����еĸ���
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::InsertColumn(const long& nPos, const long& nCount)
{
    //
    //  ������ݻ�δ��ʼ�����߲������С������߹���
    //  ���� 0 ��ʾ����ʧ�ܣ�����Ӱ��ԭ��������
    //
    if(0 == m_pData || 0 > nCount || COLUMN_COUNT_MAX < (nCount + m_nColumnCount))
    {
        return 0L;
    }

    long i, j;

    //  �����Ѿ�����ķ�Χ
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
            //  �����ڴ��г��ִ���
            return 0L;
        }

        //  �������Ѿ�����������Ĵ�С
        m_nColumnCount      = nCount + m_nColumnCount;
        m_nAllocColumnCount = m_nColumnCount;
    }
    else
    {
        //  �ѷ�����һ�����ݵ��������Ӻ󻹴�
        //  ֻ��Ҫ����������һ����Ӽ���
        m_nColumnCount += nCount;
    }

    //
    //  ���в���������������λ�ò�����Ч��Χ��
    //  �����Ϊ�����ҵ�λ�ý�����ӣ������������� if ����
    //
    //  ���뷽ʽ����ԭ�����ݾ�����Ųλ�����������Ų
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
//  ���ܣ���ָ���е�λ���Ƴ�ָ������
//  ���ڣ�2005-11-09
//  ������const long nPos    [in] �Ƴ��е�λ��
//        const long nCount  [in] �Ƴ��еĸ���
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::RemoveLine(const long& nPos, const long& nCount)
{
    //
    //  �������δ���ù���С����ֱ�ӷ��� 0 ��ʾʧ��
    //  ���� nPos ������Ч��Χ�ڣ�����Ҫ�Ƴ����Ѿ������Ѿ�����ķ�Χ��
    //
    if(0 == m_pData || 0 > nPos || nPos >= m_nLineCount || 0 >= nCount)
    {
        return 0L;
    }

    long i;
    T* pTemp;

    //
    //  ��ֹԽ�磬������ƶ������������ж����ֿ�����
    //  ͬʱ�Զ�ά������������е���
    //
    long nInterval;
    if(nPos + nCount > m_nLineCount)
    {
        //  �Ƴ�����Խ��
        nInterval    = m_nLineCount - nPos;
        m_nLineCount = nPos;
    }
    else
    {
        nInterval     = nCount;
        m_nLineCount -= nCount;
    }

    //
    //  ���һ������������λ�������������������
    //  ���Ƴ����д�������һ��Ԫ�ص�����
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
//  ���ܣ���ָ���е�λ���Ƴ�ָ������
//  ���ڣ�2005-11-09
//  ������const long nPos    [in] �Ƴ��е�λ��
//        const long nCount  [in] �Ƴ��еĸ���
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::RemoveColumn(const long& nPos, const long& nCount)
{
    //
    //  �������δ���ù���С����ֱ�ӷ��� 0 ��ʾʧ��
    //  ���� nPos ������Ч��Χ��
    //
    if(0 == m_pData || 0 > nPos || nPos >= m_nColumnCount || 0 >= nCount)
    {
        return 0L;
    }

    long i, j;
    T objTemp;

    //
    //  ��ֹԽ�磬������ƶ������������ж����ֿ�����
    //  ͬʱ�Զ�ά������������е���
    //
    long nInterval;
    if(nPos + nCount > m_nColumnCount)
    {
        //  �Ƴ�����Խ��
        nInterval      = m_nColumnCount - nPos;
        m_nColumnCount = nPos;
    }
    else
    {
        nInterval       = nCount;
        m_nColumnCount -= nCount;
    }

    //
    //  ��������λ��������������������
    //  ��ÿһ���Ƴ���Ԫ�ش�������һ��Ԫ�ص�����
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
//  ���ܣ�������ݵ�����
//  ���ڣ�2005-09-04
//  ����ֵ��T** ȫ���������ݵ����ã���ά���飩
//
template <typename T> inline T** CTMatrix<T>::GetDataRef() const
{
    return m_pData;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ���õ�ǰ����λ�ã�������ʽ��
//  ���ڣ�2005-09-04
//  ����ֵ����ǰ����λ��
//
template <typename T> inline long CTMatrix<T>::GetCurrentPos() const
{
    return m_nCurrentPos;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ����������������ʽ��
//  ���ڣ�2005-09-11
//  ����ֵ������
//
template <typename T> inline long CTMatrix<T>::GetLineCount() const
{
    return m_nLineCount;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ����������������ʽ��
//  ���ڣ�2005-09-11
//  ����ֵ������
//
template <typename T> inline long CTMatrix<T>::GetColumnCount() const
{
    return m_nColumnCount;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ��������
//  ���ڣ�2005-09-04
//  ������const T* pData  [in]  �µ�һ�����������
//  ����ֵ������Ƿ�ɹ�     1 - �ɹ�    0 - ʧ��
//
template <typename T> long CTMatrix<T>::Fill(const T* pData)
{
    //
    //  �������Ķ���ָ���� NULL
    //  ���ߵ�ǰλ���Ѿ�������Χ���򷵻� 0 ��ʾʧ��
    //  ����ڷ�Χ�ڣ�����ӽ�ȥ�����ҷ��� 1 ��ʾ�ɹ�
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
//  ���ܣ�ɾ��ȫ��������
//  ���ڣ�2005-09-04
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

    //  ȫ���� 0
    m_nLineCount        = 0;
    m_nColumnCount      = 0;
    m_nAllocLineCount   = 0;
    m_nAllocColumnCount = 0;

    m_nCurrentPos       = -1;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ������Ƿ�Ϸ�
//  ���ڣ�2005-09-06
//  ����ֵ��1-�Ϸ� 0-���Ϸ�
//
template <typename T> inline long CTMatrix<T>::IsValid() const
{
    //  ���λ���α��Ѿ��Ƶ�������ʾ���ݶ��Ѿ����
    //  �ǺϷ��ģ����δ�Ƶ�����������ǲ������ġ�
    return (m_nLineCount * m_nColumnCount == m_nCurrentPos);
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ���������Ϊ�Ϸ�
//  ���ڣ�2005-09-15
//
//  ע��������ô˷���������Ҫ���д������ݵĺϷ���
//
template <typename T> inline void CTMatrix<T>::SetValid()
{
    m_nCurrentPos = m_nLineCount * m_nColumnCount;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ����������ŷ���������һά����
//  ���ڣ�2005-09-04
//  ������const T* pData  [in]  �µ�һ�����������
//  ����ֵ����Ӧ�кŵ�һά����
//
template <typename T> inline T* CTMatrix<T>::operator[] (const long& nLine) const
{
    return m_pData[nLine];
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ����صȺ�
//  ���ڣ�2005-11-09
//  ������const CTMatrix<T>& oT [in]  ��һ����
//  ����ֵ��T& ��ǰ�����Լ�
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

    //  ������ÿ������λ����
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
//  ���ܣ����ض�ά��������
//  ���ڣ�2005-11-09
//  ����ֵ����ά����ĵ�ַ
//
template <typename T> inline CTMatrix<T>::operator T**() const
{
    return m_pData;
}



/*
//  CTMatrix ��������ʾ
//  ƽʱҪ�ŵ�ע�Ϳ���

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
