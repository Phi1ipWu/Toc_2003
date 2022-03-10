////////////////////////////////////////////////////////////////////////////
//
//  声明：此文件属于游戏创造传说的一部分，全部版权归属 COKUCO 工作室所有
//        如有侵权，COKUCO 工作室保留法律起诉权利
//
//  类名：CTArray （C - Class  T - Template）)
//  描述：模板化的一维数组类，实际是安全包装了一个堆上的一维数组。
//        利用强大的模板，是在不牺牲任何性能的情况下的可复用。
//        挖咔咔咔咔
//
//  作者：Kudoxinyiji
//  日期：2005-10-06
//
////////////////////////////////////////////////////////////////////////////



#include "TArray.h"


//	最大分配个数
#define ARRAY_SIZE_MAX      32768

//	默认分配个数
#define ARRAY_SIZE_DEFAULT  256


//////////////////////////////////////////////////////////////////
//
//  功能：默认构造函数
//  日期：2005-10-06
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
//  功能：默认析构函数
//  日期：2005-10-06
//
template<class T> CTArray<T>::~CTArray()
{
	Delete();
}




//////////////////////////////////////////////////////////////////
//
//  功能：设置数据大小
//  日期：2005-10-06
//  参数：const long nCount   [in] 行数
//  返回值：1 - 成功    0 - 失败
//
template<class T> long CTArray<T>::SetSize(const long& nCount)
{
	if(0 >= nCount || ARRAY_SIZE_MAX < nCount)
	{
		//	个数偏小过偏大
		return 0L;
	}

    if(nCount <= m_nAllocCount)
    {
        //  新的大小比已分配的空间要小，为了提升效率
        //  直接设置行数列数和游标，直接返回
		m_nCount       = nCount;
        m_nCurrentPos  = 0;
        return 1L;
    }

    //  将原来的已分配内存进行回收销毁
    Delete();

    long nAllocCount   = (nCount > ARRAY_SIZE_DEFAULT) ? nCount : ARRAY_SIZE_DEFAULT;

    try
    {
        //  动态创建一个二维数组
        m_pData = new T[nAllocCount];
    }
    catch(...)
    {
        //  出错处理，new 分配内存失败处理
        //  将已分配的销毁，返回 0 表示失败
        Delete();
        return 0L;
    }

	//	保存大小
	m_nCount      = nCount;
	m_nAllocCount = nAllocCount;

    //  数据游标定到第一行第一列位置
    m_nCurrentPos       = 0;

	return 1L;
}




//////////////////////////////////////////////////////////////////
//
//  功能：填充数据
//  日期：2005-10-06
//  参数：const T* pData [in]  新的一块区域的引用
//  返回值：添加是否成功     1 - 成功    0 - 失败
//
template<class T> long CTArray<T>::Fill(const T* pData)
{
    //
    //  如果传入的对象指针是 NULL
    //  或者当前位置已经超出范围，则返回 0 表示失败
    //  如果在范围内，则添加进去，并且返回 1 表示成功
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
//  功能：设置指定下标的数据
//  日期：2005-10-06
//  参数：const T* pData     [in] 数据的引用
//        const long nLine   [in] 行数
//        const long nColumn [in] 列数
//  返回值：1 - 成功    0 - 失败
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




//	插入数据
template<class T> long CTArray<T>::Insert(const long& nPos, const long& nCount)
{
}




//	移除数据
template<class T> long CTArray<T>::Remove(const long& nPos, const long& nCount)
{
}




//////////////////////////////////////////////////////////////////
//
//  功能：删除全部的数据
//  日期：2005-10-06
//
template<class T> void CTArray<T>::Delete()
{
	if(m_pData)
	{
		delete[] m_pData;
		m_pData = 0;
	}

	//	标记位置零
	m_nCount      = 0;
	m_nAllocCount = 0;
	m_nCurrentPos = -1;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得数据的引用
//  日期：2005-10-06
//  返回值：T** 全部区域数据的引用（一维数组）
//
template<class T> inline T* CTArray<T>::GetDataRef() const
{
	return m_pData;
}




//////////////////////////////////////////////////////////////////
//
//  功能：获得一维数组的个数
//  日期：2005-10-06
//  返回值：一维数组个数
//
template<class T> inline long CTArray<T>::GetCount() const
{
	return m_nCount;
}




//////////////////////////////////////////////////////////////////
//
//  功能：数据是否合法
//  日期：2005-10-06
//  返回值：1-合法 0-不合法
//
template<class T> inline long CTArray<T>::IsValid() const
{
	return (m_nCurrentPos == m_nCount);
}




//////////////////////////////////////////////////////////////////
//
//  功能：设置数据为合法
//  日期：2005-10-06
//
//  注：如果调用此方法，则需要自行处理数据的合法性
//
template<class T> inline void CTArray<T>::SetValid()
{
	m_nCurrentPos = m_nCount;
}



//////////////////////////////////////////////////////////////////
//
//  功能：重载中括号方法，返回对象下标的对象
//  日期：2005-10-06
//  参数：const long& nPos [in]  对象下标
//  返回值：对应行号的一维数组
//
template<class T> inline T CTArray<T>::operator[] (const long& nPos) const
{
	return m_pData[nPos];
}



/*
//	测试用代码
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


