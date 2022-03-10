////////////////////////////////////////////////////////////////////////////
//
//  ���������ļ�������Ϸ���촫˵��һ���֣�ȫ����Ȩ���� COKUCO ����������
//        ������Ȩ��COKUCO �����ұ�����������Ȩ��
//
//  ������CTArray ��C - Class  T - Template��)
//  ������ģ�廯��һά�����࣬ʵ���ǰ�ȫ��װ��һ�����ϵ�һά���顣
//        ����ǿ���ģ�壬���ڲ������κ����ܵ�����µĿɸ��á�
//        ����������
//
//  ���ߣ�Kudoxinyiji
//  ���ڣ�2005-10-06
//
////////////////////////////////////////////////////////////////////////////



#include "TArray.h"


//	���������
#define ARRAY_SIZE_MAX      32768

//	Ĭ�Ϸ������
#define ARRAY_SIZE_DEFAULT  256


//////////////////////////////////////////////////////////////////
//
//  ���ܣ�Ĭ�Ϲ��캯��
//  ���ڣ�2005-10-06
//
template<class T> CTArray<T>::CTArray()
{
	m_pData       = 0;
	m_nCount      = 0;
	m_nAllocCount = 0;
	m_nCurrentPos = -1;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�Ĭ����������
//  ���ڣ�2005-10-06
//
template<class T> CTArray<T>::~CTArray()
{
	Delete();
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ��������ݴ�С
//  ���ڣ�2005-10-06
//  ������const long nCount   [in] ����
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template<class T> long CTArray<T>::SetSize(const long& nCount)
{
	if(0 >= nCount || ARRAY_SIZE_MAX < nCount)
	{
		//	����ƫС��ƫ��
		return 0L;
	}

    if(nCount <= m_nAllocCount)
    {
        //  �µĴ�С���ѷ���Ŀռ�ҪС��Ϊ������Ч��
        //  ֱ�����������������αֱ꣬�ӷ���
		m_nCount       = nCount;
        m_nCurrentPos  = 0;
        return 1L;
    }

    //  ��ԭ�����ѷ����ڴ���л�������
    Delete();

    long nAllocCount   = (nCount > ARRAY_SIZE_DEFAULT) ? nCount : ARRAY_SIZE_DEFAULT;

    try
    {
        //  ��̬����һ����ά����
        m_pData = new T[nAllocCount];
    }
    catch(...)
    {
        //  ������new �����ڴ�ʧ�ܴ���
        //  ���ѷ�������٣����� 0 ��ʾʧ��
        Delete();
        return 0L;
    }

	//	�����С
	m_nCount      = nCount;
	m_nAllocCount = nAllocCount;

    //  �����α궨����һ�е�һ��λ��
    m_nCurrentPos       = 0;

	return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ��������
//  ���ڣ�2005-10-06
//  ������const T* pData [in]  �µ�һ�����������
//  ����ֵ������Ƿ�ɹ�     1 - �ɹ�    0 - ʧ��
//
template<class T> long CTArray<T>::Fill(const T* pData)
{
    //
    //  �������Ķ���ָ���� NULL
    //  ���ߵ�ǰλ���Ѿ�������Χ���򷵻� 0 ��ʾʧ��
    //  ����ڷ�Χ�ڣ�����ӽ�ȥ�����ҷ��� 1 ��ʾ�ɹ�
    //
    if(pData && 0 <= m_nCurrentPos && m_nCurrentPos < m_nCount)
    {
        m_pData[m_nCurrentPos] = *pData;
        m_nCurrentPos ++ ;

        return 1L;
    }

    return 0L;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�����ָ���±������
//  ���ڣ�2005-10-06
//  ������const T* pData     [in] ���ݵ�����
//        const long nLine   [in] ����
//        const long nColumn [in] ����
//  ����ֵ��1 - �ɹ�    0 - ʧ��
//
template<class T> long CTArray<T>::SetData(const T* pData, const long& nPos) const
{
	if(0 == pData || 0 > nPos || m_nCount >= nPos)
	{
		return 0L;
	}

	m_pData[nPos] = *pData;

	return 1L;
}




//	��������
template<class T> long CTArray<T>::Insert(const long& nPos, const long& nCount)
{
}




//	�Ƴ�����
template<class T> long CTArray<T>::Remove(const long& nPos, const long& nCount)
{
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�ɾ��ȫ��������
//  ���ڣ�2005-10-06
//
template<class T> void CTArray<T>::Delete()
{
	if(m_pData)
	{
		delete[] m_pData;
		m_pData = 0;
	}

	//	���λ����
	m_nCount      = 0;
	m_nAllocCount = 0;
	m_nCurrentPos = -1;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ�������ݵ�����
//  ���ڣ�2005-10-06
//  ����ֵ��T** ȫ���������ݵ����ã�һά���飩
//
template<class T> inline T* CTArray<T>::GetDataRef() const
{
	return m_pData;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ����һά����ĸ���
//  ���ڣ�2005-10-06
//  ����ֵ��һά�������
//
template<class T> inline long CTArray<T>::GetCount() const
{
	return m_nCount;
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ������Ƿ�Ϸ�
//  ���ڣ�2005-10-06
//  ����ֵ��1-�Ϸ� 0-���Ϸ�
//
template<class T> inline long CTArray<T>::IsValid() const
{
	return (m_nCurrentPos == m_nCount);
}




//////////////////////////////////////////////////////////////////
//
//  ���ܣ���������Ϊ�Ϸ�
//  ���ڣ�2005-10-06
//
//  ע��������ô˷���������Ҫ���д������ݵĺϷ���
//
template<class T> inline void CTArray<T>::SetValid()
{
	m_nCurrentPos = m_nCount;
}



//////////////////////////////////////////////////////////////////
//
//  ���ܣ����������ŷ��������ض����±�Ķ���
//  ���ڣ�2005-10-06
//  ������const long& nPos [in]  �����±�
//  ����ֵ����Ӧ�кŵ�һά����
//
template<class T> inline T CTArray<T>::operator[] (const long& nPos) const
{
	return m_pData[nPos];
}



/*
//	�����ô���
#define ARRAY_TEST_SIZE 10
#include <stdio.h>
int main()
{
	CTArray<long> al;

	if(! al.SetSize(ARRAY_TEST_SIZE))
	{
		printf("SetSize Failed.\n");
		return 0;
	}

	long i;
	for(i = 0; i < ARRAY_TEST_SIZE; ++i)
	{
		al.Fill(&i);
	}

	if(! al.IsValid())
	{
		printf("IsValid return false.\n");
		return 0;
	}

	for(i = 0; i < ARRAY_TEST_SIZE; ++i)
	{
		printf(" %d - %ld\n", i, al[i]);
	}

	return 0;
}
*/


