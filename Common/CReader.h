//---------------------------------
//  ������  CReader
//  ������  ���ű���
//---------------------------------


#ifndef _CREADER_H_
#define _CREADER_H_


#include <string>
#include <fstream>
#include <windows.h>
using namespace std;


class CReader
{
public:
    CReader(const string& strFilePath, const string& strGroupName);
    CReader(const string& strFilePath);
    CReader();
    ~CReader();

    bool GetLine(string& strParam, string& strValue);   //  ��� INI �ļ���һ�еĲ�����ֵ
    bool GetLine(string& strParam, long& nValue);       //  ��� INI �ļ���һ�еĲ�����ֵ
    bool GetLine(string& strLine);      //  ��ȡ INI �ļ�����һ����Ч�е����ݣ�����������

    bool Open();        //  �򿪶�ȡ���������ʧ���򷵻� false;
    bool IsOpen();      //  ����Ƿ��
    void Close();       //  �رն�ȡ����

    void SetSplit(const string& strSplit);              //  ���÷ָ��ַ���
    void SetGroupName(const string& strGroupName);      //  ���������������� []
    void SetFilePath(const string& strFilePath);        //  ���� INI �ļ�·��
    static long Str2long(const char* const pStr);       //  �ַ���ת��ֵ

private:

    ifstream m_fsObj;           //  ���ļ�����
    string m_strFilePath;       //  �ļ�·��
    string m_strGroupName;      //  INI �е�����
    string m_strSplit;          //  INI �в�������ֵ�ķָ���
};


#endif
