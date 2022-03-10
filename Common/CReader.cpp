//---------------------------------
//  ������  CReader
//  ������  ���ű���
//---------------------------------


#include "CReader.h"

//-----------------------------------------------------
//  ������ ���ű��๹�캯ʽ
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strFilePath  ��Ϸ�ű���INI����·��
//         strGroupName �ű��е�������[] �������
//-----------------------------------------------------
CReader::CReader(const string& strFilePath, const string& strGroupName)
{
    m_strFilePath   = strFilePath;
    m_strGroupName  = strGroupName;
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  ������ ���ű��๹�캯ʽ
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strFilePath  ��Ϸ�ű���INI����·��
//-----------------------------------------------------
CReader::CReader(const string& strFilePath)
{
    m_strFilePath   = strFilePath;
    m_strGroupName  = "";
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  ������ ���ű���Ĭ�Ϲ��캯ʽ
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ ��
//-----------------------------------------------------
CReader::CReader()
{
    m_strFilePath   = "";
    m_strGroupName  = "";
    m_strSplit      = " = ";
}


//-----------------------------------------------------
//  ������ ���ű���������ʽ
//  ���ܣ� ����ļ�δ�ر����Զ��ر�
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ ��
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
//  ������ ���ű���һ����Ч�У���ò������Ͷ�Ӧֵ
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strParam �ַ���������strValue �ַ���ֵ����Ϊ���ã�
//  ����ֵ����ȡһ�гɹ����� true, ʧ�ܷ��� false
//--------------------------------------------------------------
bool CReader::GetLine(string& strParam, string& strValue)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  ���ļ��л���
    string strLine;                 //  �����ݽ�����
    size_t pos;                     //  ������λ�ö�λ��

    strParam = "";
    strValue = "";

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  ע�ͻ���У�����
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  ������һ���ͷ��־���رն��ļ����󡣷���ʧ��
            m_fsObj.close();
            return false;
        }
        else
        {
            strLine = buffer;
            pos = strLine.find(m_strSplit);
            if(pos == 0)
            {
                //  δ�ҵ����ڣ���������һ��
                memset(buffer, 0, 255);
                continue;
            }

            strParam = strLine.substr(0, pos);  //  ����Ч���л�ò���
            strValue = (&buffer[pos + 3]);      //  ����Ч���л��ֵ
            memset(buffer, 0, 255);

            return true;
        }
    }

    m_fsObj.close();
    return false;
}


//--------------------------------------------------------
//  ������ ���ű���һ����Ч�У���ò������Ͷ�Ӧֵ
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strParam �ַ���������nValue ��ֵ����Ϊ���ã�
//  ����ֵ����ȡһ�гɹ����� true, ʧ�ܷ��� false
//  ע�� �����ȡ�ɹ�����ֵת��ʧ���� nValue Ϊ -1
//--------------------------------------------------------
bool CReader::GetLine(string& strParam, long& nValue)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  ���ļ��л���
    string strLine;                 //  �����ݽ�����
    size_t pos;                     //  ������λ�ö�λ��

    strParam = "";
    nValue = 0;

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  ע�ͻ���У�����
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  ������һ���ͷ��־���رն��ļ����󡣷���ʧ��
            m_fsObj.close();
            return false;
        }
        else
        {
            strLine = buffer;
            pos = strLine.find(m_strSplit);
            if(pos == 0)
            {
                //  δ�ҵ����ڣ���������һ��
                continue;
            }

            strParam = strLine.substr(0, pos);      //  ����Ч���л�ò���
            nValue = Str2long(&buffer[pos + 3]);    //  ����Ч���л��ֵ���Զ�ת��Ϊ������
            memset(buffer, 0, 255);

            return true;
        }
    }

    m_fsObj.close();
    return false;
}



//--------------------------------------------------------
//  ������ ��ȡ��һ��Ч�У���������
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/05/15
//  ������ ��
//  ����ֵ����ȡ�򿪳ɹ����� true, ʧ�ܷ��� false
//--------------------------------------------------------
bool CReader::GetLine(string& strLine)
{
    if(! m_fsObj.is_open()) return false;

    char buffer[255] = {0};         //  ���ļ��л���

    while(m_fsObj.getline(buffer, 255))
    {
        if(buffer[0] == '\0' || buffer[0] == ';' || (buffer[0] == '/' && buffer[1] == '/'))
        {
            //  ע�ͻ���У�����
            memset(buffer, 0, 255);
            continue;
        }

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            //  ������һ���ͷ��־���رն��ļ����󡣷���ʧ��
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
//  ������ �򿪽ű��ļ�
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ ��
//  ����ֵ���ļ��򿪳ɹ����� true, ʧ�ܷ��� false
//--------------------------------------------------------
bool CReader::Open()
{
    //  �ļ��Ѿ��� �� ����Ϊ�� �򷵻� false
    if(m_fsObj.is_open() || m_strGroupName == "") return false;

    //  �򿪲��ж��Ƿ�ɹ�
    m_fsObj.open(m_strFilePath.c_str());
    if(! m_fsObj.is_open()) return false;

    string strLine;     //  ��Ŵӻ����ж�����һ��
    char buffer[255];   //  ���壬���ڴ��ļ��ж�һ��

    while(m_fsObj.getline(buffer, 255))
    {
        strLine = buffer;

        if(buffer[0] == '[' && buffer[strlen(buffer) - 1] == ']')
        {
            memset(buffer, 0, 255);

            if(m_strGroupName == strLine.substr(1, strLine.length() - 2))
            {
                //  ����Ϊ��Ӧ���ͷ
                return true;
            }
        }
    }

    m_fsObj.close();
    return false;
}


//-------------------------------------------------
//  ������ �Ƿ��Ѿ����ļ�
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ ��
//  ����ֵ���ļ��Ѵ򿪷��� true, δ�򿪷��� false
//-------------------------------------------------
bool CReader::IsOpen()
{
    return m_fsObj.is_open();
}


//--------------------------------------
//  ������ �ر�һ���Ѿ��򿪵� INI �ļ�
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ ��
//  ����ֵ���ر�һ���Ѿ��򿪵��ļ�
//--------------------------------------
void CReader::Close()
{
    if(m_fsObj.is_open())
    {
        m_fsObj.close();
    }
}


//--------------------------------------
//  ������ ���� INI �е�������������[]
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strGroupName Ϊ��������
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
//  ������ ���� INI �ļ�·��
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ strFilePath Ϊ�����ļ�·��
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
//  ������ ����һ���зָ��ַ���
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/04/29
//  ������ strSplit Ϊ����ָ��ַ���
//--------------------------------------
void CReader::SetSplit(const string& strSplit)
{
    if(strSplit != "")
    {
        m_strSplit = strSplit;  
    }
}


//--------------------------------------
//  ������ ��������ַ���ת���ɳ�����
//  ���ߣ� KudoXinyiji
//  ���ڣ� 2005/02/10
//  ������ pStr Ϊ������ַ���
//  ����ֵ���ַ���ת���������
//  ע�� ת��ʧ�ܷ��� -1
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

//  ���´���Ϊ�൥�������õĴ��롣��������Ҫ�����ע�Ϳ��С�

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
