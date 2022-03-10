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



#ifndef _CTMATRIX_H
#define _CTMATRIX_H



template <typename T> class CTMatrix
{
public:

    //  Ĭ�Ϲ��캯��
    CTMatrix();

    //  �������캯��
    CTMatrix(const CTMatrix<T>& oT);

    //  �������Ĺ��캯��
    CTMatrix(const long& nLineCount, const long& nColumnCount);

    //  ��������
    virtual ~CTMatrix();

    //  ���ó������ݴ�С
    long SetSize(const long& nLineCount, const long& nColumnCount);

    //  �������
    long Fill(const T* pData);

    //  ��������
    long SetData(const T* pData, const long& nLine, const long& nColumn) const;

    //  ������
    long InsertLine(const long& nPos, const long& nCount = 1);

    //  ������
    long InsertColumn(const long& nPos, const long& nCount = 1);

    //  �Ƴ���
    long RemoveLine(const long& nPos, const long& nCount = 1);

    //  �Ƴ���
    long RemoveColumn(const long& nPos, const long& nCount = 1);

    //  ɾ��ȫ��������
    void Delete();

    //  ���ȫ�����ݵ�����
    T** GetDataRef() const;

    //  ��õ�ǰ����λ��
    long GetCurrentPos() const;

    //  ����и���
    long GetLineCount() const;

    //  ����и���
    long GetColumnCount() const;

    //  �����Ƿ�Ϸ�
    long IsValid() const;

    //  �������ݺϷ���������⣩
    void SetValid();

    //  ���������ţ�����һ��һά����
    T* operator[] (const long& nLine) const;

    //  ���صȺţ������Կ�
    const CTMatrix<T>& operator=(const CTMatrix<T>& oT);

    //  ��ά����ָ����������
    operator T**() const;

protected:

    //  �������ݣ���ά����ָ��
    T** m_pData;

    //  ��ǰ���������������еı��
    //  ȡֵ��Χ [ 0 , Line * Column - 1 ]
    long m_nCurrentPos;

    //  ������������� T* �ĸ�����
    long m_nLineCount;

    //  ������������� T �ĸ�����
    long m_nColumnCount;

    //  �ѷ����ڴ������
    long m_nAllocLineCount;

    //  �ѷ����ڴ������
    long m_nAllocColumnCount;

};


#endif
