//---------------------------------
//	类名：	CSetting
//	描述：	游戏设置类
//	模式：	单例
//---------------------------------

#include <dinput.h>
#include "CSetting.h"

#include "..\..\Common\CReader.h"
using namespace TocCore;

//	游戏设置唯一实例置 NULL
CSetting* CSetting::m_pGsObj = 0;


//-----------------------------------------------------
//	描述： 设置对象初始化
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： bool - true 初始化成功，false 初始化失败
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
//	描述： 初始化游戏运行的路径（string）对象
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： bool 初始化游戏路径是否成功
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
//	描述： 返回游戏主程式所在目录
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： string
//-----------------------------------
string CSetting::GetRootPath()
{
	return m_strPath;
}


//-----------------------------------
//	描述： 返回游戏语言所在目录
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： string
//-----------------------------------
string CSetting::GetLangPath()
{
	return (m_strPath + "Data\\Chs\\");
}

//-----------------------------------
//	描述： 返回游戏数据所在目录
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： string
//-----------------------------------
string CSetting::GetDataPath()
{
	return (m_strPath + "Data\\");
}

//-----------------------------------
//	描述： 获得设置唯一对象（静态）
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 唯一设置对象的引用
//-----------------------------------
CSetting* CSetting::Instance()
{
	return m_pGsObj;
}


//---------------------------------
//	描述： 退出时销毁对象（静态）
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
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
//	描述： 获得游戏“上”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyUp(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIUp : m_dwUp);
}


//---------------------------------
//	描述： 获得游戏“下”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyDown(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIDown : m_dwDown);
}


//---------------------------------
//	描述： 获得游戏“左”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyLeft(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDILeft : m_dwLeft);
}



//---------------------------------
//	描述： 获得游戏“右”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyRight(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIRight : m_dwRight);
}



//---------------------------------
//	描述： 获得游戏“退出”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyEscape(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDIEscape : m_dwEscape);
}



//---------------------------------
//	描述： 获得游戏“调查”按键
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： 按键的虚键
//---------------------------------
DWORD CSetting::GetKeyCheck(long nKeyType)
{
	return (nKeyType == TOC_DIKEY ? m_dwDICheck : m_dwCheck);
}



//------------------------
//	描述： 获得屏幕宽度
//	作者： KudoXinyiji
//	日期： 2005/04/04
//	返回值： DWORD
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
//	描述： 获得屏幕高度
//	作者： KudoXinyiji
//	日期： 2005/04/04
//	返回值： DWORD
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
//	描述： 返回游戏中是否播放声音
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： true 播放，false 不播放
//------------------------------------
bool CSetting::IsPlayMusic()
{
	return m_bMusic;
}


//------------------------------------
//	描述： 是否窗体显示
//	作者： KudoXinyiji
//	日期： 2005/02/10
//	返回值： true 窗体，false 全屏
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
//	描述： 默认构造函数
//	作者： KudoXinyiji
//	日期： 2005/02/10
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
//	描述： 默认析构函数
//	作者： KudoXinyiji
//	日期： 2005/02/10
//------------------------
CSetting::~CSetting()
{}



//------------------------------------
//	描述： DI 的键码转换成虚键码
//	作者： KudoXinyiji
//	日期： 2005/05/14
//	参数： DI 的键码
//	返回值： 对应的虚键码
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
//	描述： 虚键码转换成 DI 键码（静态）
//	作者： KudoXinyiji
//	日期： 2005/05/14
//	参数： 虚键码
//	返回值： 对应的 DI 键码
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