//---------------------------------
//  类名：  CReader
//  描述：  读脚本类
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

    bool GetLine(string& strParam, string& strValue);   //  获得 INI 文件中一行的参数和值
    bool GetLine(string& strParam, long& nValue);       //  获得 INI 文件中一行的参数和值
    bool GetLine(string& strLine);      //  获取 INI 文件中下一个有效行的内容（不作解析）

    bool Open();        //  打开读取对象，如果打开失败则返回 false;
    bool IsOpen();      //  检测是否打开
    void Close();       //  关闭读取对象。

    void SetSplit(const string& strSplit);              //  设置分隔字符串
    void SetGroupName(const string& strGroupName);      //  设置组名，不包括 []
    void SetFilePath(const string& strFilePath);        //  设置 INI 文件路径
    static long Str2long(const char* const pStr);       //  字符串转数值

private:

    ifstream m_fsObj;           //  读文件对象
    string m_strFilePath;       //  文件路径
    string m_strGroupName;      //  INI 中的组名
    string m_strSplit;          //  INI 中参数名和值的分隔符
};


#endif
