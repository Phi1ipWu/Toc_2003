//---------------------------------
//  类名：  CReader
//  描述：  读脚本类
//---------------------------------


#include "CReader.h"

//-----------------------------------------------------
//  描述： 读脚本类构造函式
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strFilePath  游戏脚本（INI）的路径
//         strGroupName 脚本中的组名，[] 里的内容
//-----------------------------------------------------
CReader::CReader(const string& strFilePath, const string& strGroupName)
{
    m_strFilePath   = strFilePath;
    m_strGroupName  = strGroupName;
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  描述： 读脚本类构造函式
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strFilePath  游戏脚本（INI）的路径
//-----------------------------------------------------
CReader::CReader(const string& strFilePath)
{
    m_strFilePath   = strFilePath;
    m_strGroupName  = "";
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  描述： 读脚本类默认构造函式
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： 无
//-----------------------------------------------------
CReader::CReader()
{
    m_strFilePath   = "";
    m_strGroupName  = "";
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  描述： 读脚本类析构函式
//  功能： 如果文件未关闭则自动关闭
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： 无
//-----------------------------------------------------
CReader::~CReader()
{
    if(m_fsObj.is_open())
    {
        m_fsObj.close();
    }

    m_strFilePath.clear();
    m_strGroupName.clear();
    m_strSplit.clear();
}


//--------------------------------------------------------------
//  描述： 读脚本下一个有效行，获得参数名和对应值
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strParam 字符串参数，strValue 字符串值（均为引用）
//  返回值：获取一行成功返回 true, 失败返回 false
//--------------------------------------------------------------
bool CReader::GetLine(string& strParam, string& strValue)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  读文件行缓冲
    string strLine;                 //  行内容解析用
    size_t pos;                     //  行内容位置定位用

    strParam = "";
    strValue = "";

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  注释或空行，跳过
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  遇到下一组的头标志，关闭读文件对象。返回失败
            m_fsObj.close();
            return false;
        }
        else
        {
            strLine = buffer;
            pos = strLine.find(m_strSplit);
            if(pos == 0)
            {
                //  未找到等于，继续读下一行
                memset(buffer, 0, 255);
                continue;
            }

            strParam = strLine.substr(0, pos);  //  从有效行中获得参数
            strValue = (&buffer[pos + 3]);      //  从有效行中获得值
            memset(buffer, 0, 255);

            return true;
        }
    }

    m_fsObj.close();
    return false;
}


//--------------------------------------------------------
//  描述： 读脚本下一个有效行，获得参数名和对应值
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strParam 字符串参数，nValue 数值（均为引用）
//  返回值：获取一行成功返回 true, 失败返回 false
//  注： 如果读取成功而数值转换失败则 nValue 为 -1
//--------------------------------------------------------
bool CReader::GetLine(string& strParam, long& nValue)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  读文件行缓冲
    string strLine;                 //  行内容解析用
    size_t pos;                     //  行内容位置定位用

    strParam = "";
    nValue = 0;

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  注释或空行，跳过
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  遇到下一组的头标志，关闭读文件对象。返回失败
            m_fsObj.close();
            return false;
        }
        else
        {
            strLine = buffer;
            pos = strLine.find(m_strSplit);
            if(pos == 0)
            {
                //  未找到等于，继续读下一行
                continue;
            }

            strParam = strLine.substr(0, pos);      //  从有效行中获得参数
            nValue = Str2long(&buffer[pos + 3]);    //  从有效行中获得值并自动转换为长整型
            memset(buffer, 0, 255);

            return true;
        }
    }

    m_fsObj.close();
    return false;
}



//--------------------------------------------------------
//  描述： 读取下一有效行，不作解析
//  作者： KudoXinyiji
//  日期： 2005/05/15
//  参数： 无
//  返回值：读取打开成功返回 true, 失败返回 false
//--------------------------------------------------------
bool CReader::GetLine(string& strLine)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  读文件行缓冲

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  注释或空行，跳过
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  遇到下一组的头标志，关闭读文件对象。返回失败
            m_fsObj.close();
            return false;
        }
        else
        {
            strLine = buffer;
            return true;
        }
    }

    m_fsObj.close();
    return false;
}



//--------------------------------------------------------
//  描述： 打开脚本文件
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： 无
//  返回值：文件打开成功返回 true, 失败返回 false
//--------------------------------------------------------
bool CReader::Open()
{
    //  文件已经打开 或 组名为空 则返回 false
    if(m_fsObj.is_open() || m_strGroupName == "") return false;

    //  打开并判定是否成功
    m_fsObj.open(m_strFilePath.c_str());
    if(! m_fsObj.is_open()) return false;

    string strLine;     //  存放从缓冲中读出的一行
    char buffer[255];   //  缓冲，用于从文件中读一行

    while(m_fsObj.getline(buffer, 255))
    {
        strLine = buffer;

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            memset(buffer, 0, 255);

            if(m_strGroupName == strLine.substr(1, strLine.length() - 2))
            {
                //  发现为对应标记头
                return true;
            }
        }
    }

    m_fsObj.close();
    return false;
}


//-------------------------------------------------
//  描述： 是否已经打开文件
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： 无
//  返回值：文件已打开返回 true, 未打开返回 false
//-------------------------------------------------
bool CReader::IsOpen()
{
    return m_fsObj.is_open();
}


//--------------------------------------
//  描述： 关闭一个已经打开的 INI 文件
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： 无
//  返回值：关闭一个已经打开的文件
//--------------------------------------
void CReader::Close()
{
    if(m_fsObj.is_open())
    {
        m_fsObj.close();
    }
}


//--------------------------------------
//  描述： 设置 INI 中的组名，不包括[]
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strGroupName 为传入组名
//--------------------------------------
void CReader::SetGroupName(const string& strGroupName)
{
    if(m_fsObj.is_open())
    {
        m_fsObj.close();
    }

    m_strGroupName = strGroupName;
}


//--------------------------------------
//  描述： 设置 INI 文件路径
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： strFilePath 为传入文件路径
//--------------------------------------
void CReader::SetFilePath(const string& strFilePath)
{
    if(m_fsObj.is_open())
    {   
        m_fsObj.close();
    }

    m_strFilePath = strFilePath;
}


//--------------------------------------
//  描述： 设置一行中分隔字符串
//  作者： KudoXinyiji
//  日期： 2005/04/29
//  参数： strSplit 为传入分隔字符串
//--------------------------------------
void CReader::SetSplit(const string& strSplit)
{
    if(strSplit != "")
    {
        m_strSplit = strSplit;  
    }
}


//--------------------------------------
//  描述： 将传入的字符串转换成长整型
//  作者： KudoXinyiji
//  日期： 2005/02/10
//  参数： pStr 为传入的字符串
//  返回值：字符串转换后的整数
//  注： 转换失败返回 -1
//--------------------------------------
long CReader::Str2long(const char* const pStr)
{
    long nRet = 0;
    long j = (long)strlen(pStr);

    for(long i = 0; i < j; ++ i)
    {
        if(pStr[i] == ' ') continue;

        if(pStr[i] <= '9' && pStr[i] >= '0')
        {
            nRet = nRet * 10 + (pStr[i] - '0');
        }
        else
        {
            if(nRet == 0 && i == 0)
            {
                return -1;
            }
            else
            {
                return nRet;
            }
        }
    }

    return nRet;
}


/*

//  以下代码为类单独调试用的代码。无特殊需要请放在注释块中。

int main()
{
    string strParam;
    long nValue;

    CReader readObj("D:\\Program\\Temp\\CGameSet\\Release\\Common.ini", "KEY");

    if(readObj.Open())
    {
        while(readObj.GetLine(strParam, nValue))
        {
            cout << strParam << " = " << nValue << std::endl;
        }
    }

    system("pause");
}

*/
