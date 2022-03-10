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



#ifndef _TARRAY_H
#define _TARRAY_H



template <class T> class CTArray
{

public:

	//	默认构造函数
	CTArray();

	//	析构函数（虚析构）
	virtual ~CTArray();

	//	设置对象个数
	long SetSize(const long& nCount);

    //  填充数据
    long Fill(const T* pData);

    //  设置数据
    long SetData(const T* pData, const long& nPos) const;

    //	插入数据
    long Insert(const long& nPos, const long& nCount = 1);

    //	移除数据
    long Remove(const long& nPos, const long& nCount = 1);

	//  删除全部的数据
    void Delete();

    //  获得全部数据的引用
    T* GetDataRef() const;

    //  获得当前数据位置
    //long GetCurrentPos() const;

    //  获得个数
    long GetCount() const;

    //  数据是否合法
    long IsValid() const;

    //  设置数据合法（不作检测）
    void SetValid();

    //  重载中括号，返回一个一维数组
    T operator[] (const long& nPos) const;

protected:

	//  核心数据，一维数组指针
    T* m_pData;

    //  当前操作对象在序列中的编号
    //  取值范围 [ 0 , m_nCount - 1 ]
    long m_nCurrentPos;

	//	对象个数
	long m_nCount;

	//	已经分配个数
	long m_nAllocCount;

};


#endif

