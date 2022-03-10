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



#ifndef _CTMATRIX_H
#define _CTMATRIX_H



template <typename T> class CTMatrix
{
public:

    //  默认构造函数
    CTMatrix();

    //  拷贝构造函数
    CTMatrix(const CTMatrix<T>& oT);

    //  带参数的构造函数
    CTMatrix(const long& nLineCount, const long& nColumnCount);

    //  析构函数
    virtual ~CTMatrix();

    //  设置场景数据大小
    long SetSize(const long& nLineCount, const long& nColumnCount);

    //  填充数据
    long Fill(const T* pData);

    //  设置数据
    long SetData(const T* pData, const long& nLine, const long& nColumn) const;

    //  插入行
    long InsertLine(const long& nPos, const long& nCount = 1);

    //  插入列
    long InsertColumn(const long& nPos, const long& nCount = 1);

    //  移除行
    long RemoveLine(const long& nPos, const long& nCount = 1);

    //  移除列
    long RemoveColumn(const long& nPos, const long& nCount = 1);

    //  删除全部的数据
    void Delete();

    //  获得全部数据的引用
    T** GetDataRef() const;

    //  获得当前数据位置
    long GetCurrentPos() const;

    //  获得行个数
    long GetLineCount() const;

    //  获得列个数
    long GetColumnCount() const;

    //  数据是否合法
    long IsValid() const;

    //  设置数据合法（不作检测）
    void SetValid();

    //  重载中括号，返回一个一维数组
    T* operator[] (const long& nLine) const;

    //  重载等号，两个对拷
    const CTMatrix<T>& operator=(const CTMatrix<T>& oT);

    //  二维数组指针类型重载
    operator T**() const;

protected:

    //  核心数据，二维数组指针
    T** m_pData;

    //  当前操作对象在序列中的编号
    //  取值范围 [ 0 , Line * Column - 1 ]
    long m_nCurrentPos;

    //  行数（决定存放 T* 的个数）
    long m_nLineCount;

    //  列数（决定存放 T 的个数）
    long m_nColumnCount;

    //  已分配内存的行数
    long m_nAllocLineCount;

    //  已分配内存的列数
    long m_nAllocColumnCount;

};


#endif
