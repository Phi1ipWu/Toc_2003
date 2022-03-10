//---------------------------------
//	描述：	游戏的主程序
//	模式：	无
//---------------------------------

#include "TocMain.h"



//	WinMain 主函数
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow)
{

	//::timeBeginPeriod(8);

    WNDCLASSEX wc = {	sizeof(WNDCLASSEX),
						CS_CLASSDC,
						WndProc,
						0L,
						0L, 
						GetModuleHandle(NULL),
						LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)),
						0,
						0,
						0,
						NAME,
						0
                    };

    if (RegisterClassEx(&wc) == 0)    return -1;

	HWND hWnd = CreateWindow( NAME,NAME,WS_MYSTYLE,	//WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                              GetDesktopWindow(),NULL,wc.hInstance,NULL);
    if (hWnd == 0)
	{
		MessageBox(hWnd, "注册窗口类失败！", "错误", MB_ICONERROR);
		return -1;
	}

	if(0 == GameInit(hWnd))
	{
		GameClean();
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
/*
	MSG msg;
	for(;;)
	{
		for(; PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);)
		{
			if(WM_QUIT == msg.message || TocCore::CGame::Instance()->IsQuit())
			{
				goto GameEnd;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(g_bActive)
			TocCore::CGame::Instance()->CallDraw();
		else
			WaitMessage();
	}

GameEnd:

*/

	MSG     msg;
	for(;;)
    {
		if(PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE))
        {
			if(0 == GetMessage(&msg, NULL, 0, 0))
			{
				break;
			}

			if(true == TocCore::CGame::Instance()->IsQuit())
			{
				break;
			}

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
			if(g_bActive)
            {
//				try
//				{
					//	游戏的每一帧的显示
					if(false == TocCore::CGame::Instance()->CallDraw())
					{
						//MessageBox(hWnd, "绘制下一帧时失败", 0, MB_ICONERROR);
						TocCore::CGame::Instance()->Quit();
					}
					//strLog += "Draw\n";
//				}
//				catch(...)
//				{
//					TocCore::CGame::Instance()->Quit();
//				}
            }
            else
			{
				WaitMessage();
			}
        } // end of if
    } // end of while

	GameClean();

	return 0;
}

//	消息处理
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch(msg)
    {
		case WM_CREATE:
			break;

		case WM_KEYDOWN:
			TocCore::CGame::Instance()->CallKeyDown((DWORD)wParam);
            return 0L;
        
		case WM_SIZE:
             if (SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam)
                    g_bActive = false;
             else   g_bActive = true;
             break;

		case WM_SETCURSOR:          //隐藏光标
			if(TocCore::CSetting::Instance()->IsWindowMode())
			{
				//	游戏以窗口模式显示，显示正常的箭头光标
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			else
			{
				//	游戏以全屏模式显示，不显示光标
				SetCursor(0);
			}
            return 0L;
        
		case WM_MOVE:
			if (DxWrapper::CDDraw::Instance())
				 DxWrapper::CDDraw::Instance()->UpdateBounds();

			 return 0L;

        case WM_ACTIVATEAPP:
			if(TocCore::CSetting::Instance()->IsWindowMode())
			{
				if(g_bActive)
				{
					DxWrapper::CDInput::Instance()->Acquire();
				}
				else
				{
					DxWrapper::CDInput::Instance()->UnAcquire();
				}
			}
			return 0L;

		case WM_SYSCOMMAND:
            switch(wParam)
            {  case SC_MOVE:
               case SC_SIZE:
               case SC_MAXIMIZE:
               case SC_MONITORPOWER:
                    return 0L;
            }
            break;
        
		case WM_PAINT:
			hdc = ::BeginPaint(hWnd, &ps);
			::EndPaint(hWnd, &ps);
			return 0L;

		case WM_DESTROY:
             PostQuitMessage(0);
             return 0L;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



//	2005-10-04
//	游戏的初始化
long GameInit(HWND hWnd)
{
	//	主游戏类最先起来，用以收集可能发现的错误信息
	if(false == TocCore::CGame::InitInstance())
	{
		MessageBox(hWnd, "主游戏对象初始化失败！", "错误", MB_ICONERROR);
		return 0;
	}

	//	游戏的设置接着起来（起设置的时候会读取配置文件）
	if(false == TocCore::CSetting::InitInstance())
	{
		MessageBox(hWnd, "游戏设置读取失败！", "错误", MB_ICONERROR);
		return 0;
	}

	//	初始化 DDraw
	DxWrapper::CDDraw::InitInstance();
	long nWidth  = TocCore::CSetting::Instance()->GetScreenWidth();
	long nHeight = TocCore::CSetting::Instance()->GetScreenHeight();
	if(false == DxWrapper::CDDraw::Instance()->Init(hWnd, nWidth, nHeight, 16, TocCore::CSetting::Instance()->IsWindowMode()))
	{
		MessageBox(hWnd, "DirectDraw 对象初始化失败！", "错误", MB_ICONERROR);
		return 0;
	}

	if(false == TocCore::CPc::InitInstance())
	{
		MessageBox(hWnd, "游戏角色对象初始化失败！", "错误", MB_ICONERROR);
		return 0;
	}
	TocCore::CPc::Instance()->SetTileNo(51);
	//DxWrapper::CDDraw::Instance()->InitSurface(51);


	//	初始化游戏的地板类
	if(false == TocCore::CGround::InitInstance())
	{
		MessageBox(hWnd, "游戏场景对象初始化失败！", "错误", MB_ICONERROR);
		return 0;
	}


	DxWrapper::CDMusic::InitInstance();
	//	如果设置播放背景音乐则 初始化 DirectAudio 对象。
	if(true == TocCore::CSetting::Instance()->IsPlayMusic())
	{
		if(false == DxWrapper::CDMusic::Instance()->Init(hWnd))
		{
			MessageBox(hWnd, "DirectMusic 对象初始化失败。", "错误", MB_ICONERROR);
			return 0;
		}
/*
		if(false == DxWrapper::CDMusic::Instance()->Play(14))
		{
			Toc::CGame::ExitInstance();
			Toc::CSetting::ExitInstance();
			FreeDirectDraw();
			DxWrapper::CDDraw::ExitInstance();
			Toc::CPc::ExitInstance();
			Toc::CGround::ExitInstance();
			//FreeDirectMusic();
			DxWrapper::CDMusic::ExitInstance();
			MessageBox(hWnd, "DxWrapper::CDMusic::Instance()->Play(14)", 0, MB_OK);
			return 0;
		}
*/	}


	DxWrapper::CDInput::InitInstance();
	if(false == DxWrapper::CDInput::Instance()->Init_Keyboard(hWnd))
	{
		MessageBox(hWnd, "DirectInput 初始化失败", "错误", MB_ICONERROR);
		return 0;
	}
	else
	{
		//	手柄初始化失败也没有关系咯
		DxWrapper::CDInput::Instance()->Init_Joystick(hWnd);
	}

	TocCore::CGame::Instance()->SetDraw(TocCore::CGround::Instance());
	TocCore::CGame::Instance()->SetEventProc(TocCore::CGround::Instance());

	//	根据锚点来设置角色位置
	TocCore::CGround::Instance()->ChangeByAnchor(0);

	return 1L;
}



//	2005-10-04
//	游戏对象的清理
void GameClean()
{
	TocCore::CGame::ExitInstance();
	TocCore::CSetting::ExitInstance();
	TocCore::CGround::ExitInstance();
	TocCore::CPc::ExitInstance();
	DxWrapper::CDDraw::ExitInstance();
	DxWrapper::CDInput::ExitInstance();
	DxWrapper::CDMusic::ExitInstance();
}



//	根据错误码弹出 MESSAGEBOX
void ReportErrorMsg(DWORD wErrCode)
{
	if(0 == wErrCode) return;	//	错误码为零，表示游戏正常退出，无需报错

	TocCore::CSetting* pSetting = TocCore::CSetting::Instance();
	if(0 == pSetting) return;

	CReader reader(pSetting->GetDataPath() + "Errdest.erm");
	reader.SetGroupName("ERRCODE");

	char szBuffer[16];
	string strErrCode;
	string strParam;
	string strValue;

	sprintf(szBuffer, "%04d", wErrCode);
	strErrCode = szBuffer;
	memset(szBuffer, 0, 16);

	if(reader.Open())
	{
		while(reader.GetLine(strParam, strValue))
		{
			if(strParam == strErrCode)
			{
				MessageBox(0, strValue.c_str(), 0, MB_ICONERROR);
				break;
			}
		}
	}
	else
	{
		MessageBox(0, "错误描述文件没有找到！", 0, MB_ICONERROR);
	}

	reader.Close();
}

