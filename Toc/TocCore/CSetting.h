//---------------------------------
//	类名：	CSetting
//	描述：	游戏设置类
//	模式：	单例
//---------------------------------


#ifndef _CSETTING_H
#define _CSETTING_H


#define TOC_DIKEY 1		//	DirectInput Key
#define TOC_VKEY  0		//	Windows Virtual Key


#include <windows.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

namespace TocCore
{
/*
	typedef DI_VK
	{
		long nDIKey;
		long nVKey;
	};
*/

	class CSetting
	{
	public:
		static CSetting* Instance();
		static bool InitInstance();
		static void ExitInstance();

		static long DI2VK(long nDI);
		static long VK2DI(long nVK);

		DWORD  GetKeyUp	   (long nKeyType = TOC_VKEY);
		DWORD  GetKeyDown  (long nKeyType = TOC_VKEY);
		DWORD  GetKeyLeft  (long nKeyType = TOC_VKEY);
		DWORD  GetKeyRight (long nKeyType = TOC_VKEY);
		DWORD  GetKeyEscape(long nKeyType = TOC_VKEY);
		DWORD  GetKeyCheck (long nKeyType = TOC_VKEY);
		
		DWORD  GetScreenWidth();
		DWORD  GetScreenHeight();

		string GetRootPath();	//	获得游戏根目录
		string GetDataPath();	//	获取数据目录
		string GetLangPath();	//	获取不同语言对应当前设置的目录
		bool   IsPlayMusic();	//	是否播放音乐
		bool   IsWindowMode();	//	是否窗口模式运行

	private:
		CSetting();
		~CSetting();

		bool InitGamePath();

	private:
		string m_strPath;
		string m_strLanguage;
		static CSetting* m_pGsObj;

		DWORD m_dwUp;
		DWORD m_dwDown;
		DWORD m_dwLeft;
		DWORD m_dwRight;
		DWORD m_dwEscape;
		DWORD m_dwCheck;

		DWORD m_dwDIUp;
		DWORD m_dwDIDown;
		DWORD m_dwDILeft;
		DWORD m_dwDIRight;
		DWORD m_dwDIEscape;
		DWORD m_dwDICheck;

		DWORD m_dwShowFps;
		DWORD m_dwShowMode;

		bool  m_bMusic;
	};
};

#endif