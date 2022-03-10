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



#ifndef _TARRAY_H
#define _TARRAY_H



template <class T> class CTArray
{

public:

	//	Ĭ�Ϲ��캯��
	CTArray();

	//	������������������
	virtual ~CTArray();

	//	���ö������
	long SetSize(const long& nCount);

    //  �������
    long Fill(const T* pData);

    //  ��������
    long SetData(const T* pData, const long& nPos) const;

    //	��������
    long Insert(const long& nPos, const long& nCount = 1);

    //	�Ƴ�����
    long Remove(const long& nPos, const long& nCount = 1);

	//  ɾ��ȫ��������
    void Delete();

    //  ���ȫ�����ݵ�����
    T* GetDataRef() const;

    //  ��õ�ǰ����λ��
    //long GetCurrentPos() const;

    //  ��ø���
    long GetCount() const;

    //  �����Ƿ�Ϸ�
    long IsValid() const;

    //  �������ݺϷ���������⣩
    void SetValid();

    //  ���������ţ�����һ��һά����
    T operator[] (const long& nPos) const;

protected:

	//  �������ݣ�һά����ָ��
    T* m_pData;

    //  ��ǰ���������������еı��
    //  ȡֵ��Χ [ 0 , m_nCount - 1 ]
    long m_nCurrentPos;

	//	�������
	long m_nCount;

	//	�Ѿ��������
	long m_nAllocCount;

};


#endif

