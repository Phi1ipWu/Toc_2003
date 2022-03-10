//---------------------------------
//	������	CSetting
//	������	��Ϸ������
//	ģʽ��	����
//---------------------------------

#include <dinput.h>
#include "CSetting.h"

#include "..\..\Common\CReader.h"
using namespace TocCore;

//	��Ϸ����Ψһʵ���� NULL
CSetting* CSetting::m_pGsObj = 0;


//-----------------------------------------------------
//	������ ���ö����ʼ��
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� bool - true ��ʼ���ɹ���false ��ʼ��ʧ��
//-----------------------------------------------------
bool CSetting::InitInstance()
{
	if(0 != m_pGsObj) return false;
	m_pGsObj = new CSetting();

	if(!m_pGsObj->InitGamePath())
	{
		delete m_pGsObj;
		m_pGsObj = 0;
		return false;
	}

	string strParam;
	string strValue;
	long nValue;

	CReader reader(m_pGsObj->m_strPath + "Config.cfg");
	reader.SetGroupName("SCREEN");
	if(reader.Open())
	{
		while(reader.GetLine(strParam, nValue))
		{
			if(strParam == "SHOWFPS" && nValue > 0)
			{	
				m_pGsObj->m_dwShowFps = nValue;
			}

			if(strParam == "DRAWMODE" && nValue >= 1 && nValue <= 6)
			{
				m_pGsObj->m_dwShowMode = nValue;
			}
		}
	}
	else
	{
		delete m_pGsObj;
		m_pGsObj = 0;
		return false;
	}

	reader.SetGroupName("KEY");
	if(reader.Open())
	{
		while(reader.GetLine(strParam, nValue))
		{
			if(nValue > 0)
			{
				if(strParam == "UP")	 m_pGsObj->m_dwUp	  = nValue;
				if(strParam == "DOWN")	 m_pGsObj->m_dwDown	  = nValue;
				if(strParam == "LEFT")	 m_pGsObj->m_dwLeft	  = nValue;
				if(strParam == "RIGHT")	 m_pGsObj->m_dwRight  = nValue;
				if(strParam == "CHECK")	 m_pGsObj->m_dwCheck  = nValue;
				if(strParam == "ESCAPE") m_pGsObj->m_dwEscape = nValue;
			}
		}

		m_pGsObj->m_dwDIUp     = VK2DI(m_pGsObj->m_dwUp);
		m_pGsObj->m_dwDIDown   = VK2DI(m_pGsObj->m_dwDown);
		m_pGsObj->m_dwDILeft   = VK2DI(m_pGsObj->m_dwLeft);
		m_pGsObj->m_dwDIRight  = VK2DI(m_pGsObj->m_dwRight);
		m_pGsObj->m_dwDICheck  = VK2DI(m_pGsObj->m_dwCheck);
		m_pGsObj->m_dwDIEscape = VK2DI(m_pGsObj->m_dwEscape);

		if( m_pGsObj->m_dwUp		== 0 ||
			m_pGsObj->m_dwDown		== 0 ||
			m_pGsObj->m_dwLeft		== 0 ||
			m_pGsObj->m_dwRight		== 0 ||
			m_pGsObj->m_dwCheck		== 0 ||
			m_pGsObj->m_dwEscape	== 0 ||
			m_pGsObj->m_dwDIUp      == 0 ||
			m_pGsObj->m_dwDIDown    == 0 ||
			m_pGsObj->m_dwDILeft    == 0 ||
			m_pGsObj->m_dwDIRight   == 0 ||
			m_pGsObj->m_dwDICheck   == 0 ||
			m_pGsObj->m_dwDIEscape  == 0 ||
			m_pGsObj->m_dwShowMode	== 0 )
		{
			delete m_pGsObj;
			m_pGsObj = 0;
			return false;
		}
	}
	else
	{
		delete m_pGsObj;
		m_pGsObj = 0;
		return false;
	}

	reader.SetGroupName("SOUND");
	if(reader.Open())
	{
		while(reader.GetLine(strParam, nValue))
		{
			if(strParam == "MUSIC" && nValue >= 1)
				m_pGsObj->m_bMusic = true;
		}
	}
	else
	{
		delete m_pGsObj;
		m_pGsObj = 0;
		return false;
	}
	return true;
}

//---------------------------------------------
//	������ ��ʼ����Ϸ���е�·����string������
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� bool ��ʼ����Ϸ·���Ƿ�ɹ�
//---------------------------------------------
bool CSetting::InitGamePath()
{
	char szBuffer[255];
	memset(szBuffer, 0, 255);

	if(0 == GetModuleFileName(0, szBuffer, 255))
	{
		return false;
	}

	long nPos = 0;
	for(long i = 0; i < 254; ++ i)
	{
		if(szBuffer[i] == '\\')
			nPos = i;
	}

	if(nPos > 0 && nPos < 254)
	{	
		szBuffer[nPos + 1] = '\0';
		SetCurrentDirectory(szBuffer);
	}
	else
	{
		return false;
	}

	m_pGsObj->m_strPath = szBuffer;

	return true;
}

//-----------------------------------
//	������ ������Ϸ����ʽ����Ŀ¼
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� string
//-----------------------------------
string CSetting::GetRootPath()
{
	return m_strPath;
}


//-----------------------------------
//	������ ������Ϸ��������Ŀ¼
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� string
//-----------------------------------
string CSetting::GetLangPath()
{
	return (m_strPath + "Data\\Chs\\");
}

//-----------------------------------
//	������ ������Ϸ��������Ŀ¼
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� string
//-----------------------------------
string CSetting::GetDataPath()
{
	return (m_strPath + "Data\\");
}

//-----------------------------------
//	������ �������Ψһ���󣨾�̬��
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� Ψһ���ö��������
//-----------------------------------
CSetting* CSetting::Instance()
{
	return m_pGsObj;
}


//---------------------------------
//	������ �˳�ʱ���ٶ��󣨾�̬��
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
void CSetting::ExitInstance()
{
	if(m_pGsObj)
	{
		delete m_pGsObj;
		m_pGsObj = 0;
	}
}



//---------------------------------
//	������ �����Ϸ���ϡ�����
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyUp(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIUp : m_dwUp);
}


//---------------------------------
//	������ �����Ϸ���¡�����
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyDown(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIDown : m_dwDown);
}


//---------------------------------
//	������ �����Ϸ���󡱰���
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyLeft(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDILeft : m_dwLeft);
}



//---------------------------------
//	������ �����Ϸ���ҡ�����
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyRight(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIRight : m_dwRight);
}



//---------------------------------
//	������ �����Ϸ���˳�������
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyEscape(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIEscape : m_dwEscape);
}



//---------------------------------
//	������ �����Ϸ�����顱����
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� ���������
//---------------------------------
DWORD CSetting::GetKeyCheck(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDICheck : m_dwCheck);
}



//------------------------
//	������ �����Ļ���
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/04/04
//	����ֵ�� DWORD
//------------------------
DWORD CSetting::GetScreenWidth()
{	
	switch(m_dwShowMode)
	{
	case 1:
	case 4:
		return 640;
	case 2:
	case 5:
		return 800;
	case 3:
	case 6:
		return 1024;
	}

	return 0;
}



//------------------------
//	������ �����Ļ�߶�
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/04/04
//	����ֵ�� DWORD
//------------------------
DWORD CSetting::GetScreenHeight()
{
	switch(m_dwShowMode)
	{
	case 1:
	case 4:
		return 480;
	case 2:
	case 5:
		return 600;
	case 3:
	case 6:
		return 768;
	}

	return 0;
}



//------------------------------------
//	������ ������Ϸ���Ƿ񲥷�����
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� true ���ţ�false ������
//------------------------------------
bool CSetting::IsPlayMusic()
{
	return m_bMusic;
}


//------------------------------------
//	������ �Ƿ�����ʾ
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//	����ֵ�� true ���壬false ȫ��
//------------------------------------
bool CSetting::IsWindowMode()
{
	switch(m_dwShowMode)
	{
	case 1:
	case 2:
	case 3:
		return true;
	default:
		return false;
	}

	return false;
}


//------------------------
//	������ Ĭ�Ϲ��캯��
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//------------------------
CSetting::CSetting()
{
	m_bMusic	 = false;
	m_dwShowMode = 0;

	m_dwUp		 = 0;
	m_dwDown	 = 0;
	m_dwLeft	 = 0;
	m_dwRight	 = 0;
	m_dwCheck	 = 0;
	m_dwEscape	 = 0;

	m_dwDIUp	 = 0;
	m_dwDIDown	 = 0;
	m_dwDILeft	 = 0;
	m_dwDIRight	 = 0;
	m_dwDICheck	 = 0;
	m_dwDIEscape = 0;
}



//------------------------
//	������ Ĭ����������
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/02/10
//------------------------
CSetting::~CSetting()
{}



//------------------------------------
//	������ DI �ļ���ת���������
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/05/14
//	������ DI �ļ���
//	����ֵ�� ��Ӧ�������
//------------------------------------
long CSetting::DI2VK(long nDI)
{
	switch(nDI)
	{
	case 0:
	default:
		break;
	}

	return 0;
}


//----------------------------------------
//	������ �����ת���� DI ���루��̬��
//	���ߣ� KudoXinyiji
//	���ڣ� 2005/05/14
//	������ �����
//	����ֵ�� ��Ӧ�� DI ����
//----------------------------------------
long CSetting::VK2DI(long nVK)
{
	switch(nVK)
	{
	case VK_UP:
		return DIK_UP;
	case VK_DOWN:
		return DIK_DOWN;
	case VK_LEFT:
		return DIK_LEFT;
	case VK_RIGHT:
		return DIK_RIGHT;

	case VK_ESCAPE:
		return DIK_ESCAPE;
	case VK_BACK:
		return DIK_BACK;
	case VK_TAB:
		return DIK_TAB;
	case VK_LSHIFT:
		return DIK_LSHIFT;
	case VK_RSHIFT:
		return DIK_RSHIFT;
	case VK_SPACE:
		return DIK_SPACE;
	case VK_RETURN:
		return DIK_RETURN;

//	case '-':
//		return DIK_MINUS;
	case '+':
		return DIK_EQUALS;
	case ',':
		return DIK_COMMA;
//	case '.':
//		return DIK_PERIOD;

	case 'A':
		return DIK_A;
	case 'B':
		return DIK_B;
	case 'C':
		return DIK_C;
	case 'D':
		return DIK_D;
	case 'E':
		return DIK_E;
	case 'F':
		return DIK_F;
	case 'G':
		return DIK_G;
	case 'H':
		return DIK_H;
	case 'I':
		return DIK_I;
	case 'J':
		return DIK_J;
	case 'K':
		return DIK_K;
	case 'L':
		return DIK_L;
	case 'M':
		return DIK_M;
	case 'N':
		return DIK_N;
	case 'O':
		return DIK_O;
	case 'P':
		return DIK_P;
	case 'Q':
		return DIK_Q;
	case 'R':
		return DIK_R;
	case 'S':
		return DIK_S;
	case 'T':
		return DIK_T;
	case 'U':
		return DIK_U;
	case 'V':
		return DIK_V;
	case 'W':
		return DIK_W;
	case 'X':
		return DIK_X;
	case 'Y':
		return DIK_Y;
	case 'Z':
		return DIK_Z;

	case VK_MULTIPLY:
		return DIK_MULTIPLY;
	case VK_NUMPAD7:
		return DIK_NUMPAD7;
	case VK_NUMPAD8:
		return DIK_NUMPAD8;
	case VK_NUMPAD9:
		return DIK_NUMPAD9;
	case VK_SUBTRACT:
		return DIK_SUBTRACT;
	case VK_NUMPAD4:
		return DIK_NUMPAD4;
	case VK_NUMPAD5:
		return DIK_NUMPAD5;
	case VK_NUMPAD6:
		return DIK_NUMPAD6;
	case VK_NUMPAD1:
		return DIK_NUMPAD1;
	case VK_NUMPAD2:
		return DIK_NUMPAD2;
	case VK_NUMPAD3:
		return DIK_NUMPAD3;
	case VK_DECIMAL:
		return DIK_DECIMAL;

	case VK_HOME:
		return DIK_HOME;
	case VK_END:
		return DIK_END;
	case VK_PRIOR:
		return DIK_PRIOR;
	case VK_NEXT:
		return DIK_NEXT;
	case VK_INSERT:
		return DIK_INSERT;
	case VK_DELETE:
		return DIK_DELETE;

	default:
		break;
	}

	return 0;
}


/*
int main()
{
	if(false == CSetting::InitInstance())
	{
		cout << "CSetting::InitInstance() return false" << endl;
		cout << "Because \"Common.ini\" may have error or not exist." << endl;
	}
	else
	{
		cout << CSetting::Instance()->GetPath().c_str() << endl;

		cout << "UP      =  " <<  CSetting::Instance()->GetKeyUp() << endl;
		cout << "DOWN    =  " <<  CSetting::Instance()->GetKeyDown() << endl;
		cout << "LEFT    =  " <<  CSetting::Instance()->GetKeyLeft() << endl;
		cout << "RIGHT   =  " <<  CSetting::Instance()->GetKeyRight() << endl;
		cout << "CHECK   =  " <<  CSetting::Instance()->GetKeyCheck() << endl;
		cout << "ESCAPE  =  " <<  CSetting::Instance()->GetKeyEscape() << endl;
		cout << "MUSIC   =  " << (CSetting::Instance()->IsMusic() ? "ON" : "OFF") << endl;

		CSetting::ExitInstance();
	}

	system("pause");
}

*/