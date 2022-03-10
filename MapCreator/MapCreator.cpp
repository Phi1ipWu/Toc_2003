

#include "..\Common\TileFile.h"
#include "MapCreator.h"

#pragma warning(disable : 4244)


int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
    HWND         hwnd ;
    MSG          msg ;
    WNDCLASS     wndclass ;
    HMENU        hMenu ;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = MainWndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (GRAY_BRUSH) ;
    wndclass.lpszMenuName  = 0 ;
    wndclass.lpszClassName = g_szAppName ;

    if (!RegisterClass (&wndclass))
    {
        MessageBox (NULL, TEXT ("This program requires Windows NT!"),
            g_szAppName, MB_ICONERROR) ;
        return 0 ;
    }

    wndclass.hbrBackground = (HBRUSH) GetStockObject (LTGRAY_BRUSH) ;
    wndclass.lpfnWndProc   = TileWndProc ;
    wndclass.cbWndExtra    = sizeof (long) ;
    wndclass.hIcon         = NULL ;
    wndclass.lpszClassName = g_szTileClass ;
    RegisterClass (&wndclass) ;

//  wndclass.hbrBackground = (HBRUSH) GetStockObject (GRAY_BRUSH) ;
    wndclass.lpfnWndProc   = MapWndProc ;
    wndclass.cbWndExtra    = sizeof (long) ;
    wndclass.hIcon         = NULL ;
    wndclass.lpszClassName = g_szMapClass ;
    RegisterClass (&wndclass) ;

    hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    RemoveMenu(hMenu, 3, MF_BYPOSITION);
    hwnd  = CreateWindow (g_szAppName, TEXT ("创造传说地图制作程式  By: KudoXinyiji"), 
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, hMenu, hInstance, NULL) ;

    ShowWindow (hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow (hwnd) ;

    //  允许将文件拖入窗体
    DragAcceptFiles(hwnd, TRUE);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
    }

    return (int)msg.wParam ;
}



//---------------------
//  主窗体的处理程式
//  2005-09-20
//---------------------
LRESULT CALLBACK MainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    long                i, j;
    static long         cxClient, cyClient, nRetTile;
    static HWND         hwndTile, hwndMap;//, hwndList;
    static HINSTANCE    hInstance;

    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    switch (message)
    {
    //  主窗体初始化
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

        GetCurrPath();
        if(! LoadTileInfo())
        {
            if(IDNO == MessageBox(hwnd, "读取图素信息失败！是否继续？", "错误", MB_YESNO | MB_ICONERROR))
                SendMessage(hwnd, WM_CLOSE, 0, 0);
        }

        g_bSound = GetPrivateProfileInt("MAPCREATOR", "WAV", 0, ".\\Config.cfg");

        //  创建 左上角 地砖图显示 子窗体
        hwndTile = CreateWindow(g_szTileClass, 0, WS_CHILDWINDOW|WS_VISIBLE|WS_VSCROLL, 0,0,0,0,
                    hwnd, (HMENU)(IDG_TILE), hInstance, 0);

        //  创建 右边 地图显示 子窗体
        hwndMap  = CreateWindow(g_szMapClass, 0, WS_CHILDWINDOW|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL, 0,0,0,0,
                    hwnd, (HMENU)(IDG_MAP),  hInstance, 0);

        if(TRUE == g_bSound)
        {
            //  声音放送 程式开始(Start)
            PlaySound (TEXT (".\\Sounds\\CokucoS.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;
        }
        return 0;


    //  主窗体大小变化
    case WM_SIZE:
        cxClient = LOWORD (lParam) ;
        cyClient = HIWORD (lParam) ;
        if(cxClient > 275)
        {
            //  随着主窗体的大小变化，从而调整子窗体的大小
            MoveWindow(hwndTile, 0, 0, 256 + GetSystemMetrics(SM_CXVSCROLL), 480, TRUE);
            MoveWindow(hwndMap, 256 + GetSystemMetrics(SM_CXVSCROLL) + 5, 0, cxClient - 5 - (256 + GetSystemMetrics(SM_CXVSCROLL)), cyClient, TRUE);
        }
        return 0 ;


    //  拖入文件消息
    case WM_DROPFILES:
        if(! DropMapFile((HDROP)wParam, hwndTile, hwndMap))
            MessageBox(hwnd, "文件格式错误！", "错误", MB_ICONERROR);

        break;

    //  菜单消息
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {

        //  新建（其实就是设置大小）
        case IDM_FILE_NEW:
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_MapSizeDlg), hwnd, (DLGPROC)MapSizeDlgProc);
            return 0;


        //  打开
        case IDM_FILE_OPEN:
            if(FALSE == LoadMapFile(hwndTile, hwndMap))
                MessageBox(hwnd, TEXT("读取文件失败！！"), 0, MB_ICONERROR);

            return 0;


        //  保存
        case IDM_FILE_SAVE:
            if(g_szFileName[0])
            {
                CTileFile tf;
                tf.WriteGroundData(g_szFileName, &g_stGround);
                return 0;
            }


        //  另存为
        case IDM_FILE_SAVEAS:
            if(FALSE == SaveMapFile())
                MessageBox(hwnd, TEXT("地图为空，无法保存！"), 0, MB_ICONERROR);
            return 0;


        //  读取图素信息
        case IDM_FILE_LOADTILEINFO:
            if(IDYES == MessageBox(hwnd, TEXT("此操作将还原内存中数据，是否要继续？"), TEXT("询问"), MB_YESNO | MB_ICONQUESTION))
            {
                LoadTileInfo();
                InvalidateRect(hwndTile, 0, TRUE);
            }
            
            return 0;


        //  保存图素信息
        case IDM_FILE_SAVETILEINFO:
            if(IDYES == MessageBox(hwnd, TEXT("此操作将更新原先数据，是否要继续？"), TEXT("询问"), MB_YESNO | MB_ICONQUESTION))
                SaveTileInfo();
            
            return 0;


        //  退出
        case IDM_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;


        //  设置地图大小
        case IDM_MAP_SIZE:
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_MapSizeDlg), hwnd, (DLGPROC)MapSizeDlgProc);
            return 0;


        //  设置层
        case IDM_MAP_LAYER_ALL:
        case IDM_MAP_LAYER_1:
        case IDM_MAP_LAYER_2:
        case IDM_MAP_LAYER_3:
            SendMessage(hwndMap, WM_SETLAYER, LOWORD(wParam) - IDM_MAP_LAYER_ALL, 0);
            InvalidateRect(hwndMap, 0, TRUE);
            return 0;


        //  填充地图
        case IDM_MAP_FILL:
            nRetTile = (long)SendMessage(hwndTile, WM_GETSELTILE, 0, 0);
            for(i = 0; i < g_stGround.mtAreas.GetLineCount(); ++i)
            {   for(j = 0; j < g_stGround.mtAreas.GetColumnCount(); ++j)
                {
                    pAreas[i][j].stTile[0].line   = HIWORD(nRetTile);
                    pAreas[i][j].stTile[0].column = LOWORD(nRetTile);
                }
            }
            InvalidateRect(hwndMap, 0, TRUE);
            return 0;


        //  地图清空
        case IDM_MAP_CLEAR:
            //  声音放送 地图清空(Clear)
            //PlaySound (TEXT (".\\Sounds\\CokucoC.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;

            if(0 < g_stGround.mtAreas.GetLineCount() && 0 < g_stGround.mtAreas.GetColumnCount() && 
                IDYES == MessageBox(hwnd, TEXT("此操作将清空全部数据，是否要继续？"), TEXT("询问"), MB_YESNO | MB_ICONQUESTION))
            {
                for(i = 0; i < g_stGround.mtAreas.GetLineCount(); ++i)
                {
                    for(j = 0; j < g_stGround.mtAreas.GetColumnCount(); ++j)
                    {
                        pAreas[i][j].stTile[0].line   = -1;
                        pAreas[i][j].stTile[0].column = -1;
                        pAreas[i][j].stTile[1].line   = -1;
                        pAreas[i][j].stTile[1].column = -1;
                        pAreas[i][j].stTile[2].line   = -1;
                        pAreas[i][j].stTile[2].column = -1;
                        pAreas[i][j].nAreaInfo = 0;
                    }
                }

                InvalidateRect(hwndMap, 0, TRUE);
            }
            return 0;


        //  显示地砖属性
        case IDM_MAP_DRAWINFO:
            g_bDrawInfo = !g_bDrawInfo;
            CheckMenuItem(GetMenu(hwnd), IDM_MAP_DRAWINFO, g_bDrawInfo ? MF_CHECKED : MF_UNCHECKED);
            InvalidateRect(hwndTile, 0, TRUE);
            InvalidateRect(hwndMap , 0, TRUE);
            break;


        //  地图信息
        case IDM_MAP_INFO:
            //SaveCsvFile();
            MapInfo(GetParent(hwnd));
            return 0;


        //  剩下的就是更改图素了，偷懒之杰作啊！！噢活活活～～
        default:
            //  更改图素
            if(LOWORD(wParam) >= IDM_TILE_01 && LOWORD(wParam) < IDM_TILE_01 + 50)
            {
                //  声音放送 切换图素(Tile)
                //PlaySound (TEXT (".\\Sounds\\CokucoT.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;

                SendMessage(hwndTile, WM_CHANGETILE, (LOWORD(wParam) - IDM_TILE_01 + 1), 0);
                InvalidateRect(hwndMap, 0 ,TRUE);
            }
        } // end of switch(LOWORD(wParam))

        return 0;


    case WM_DESTROY:
        if(TRUE == g_bSound)
        {
            //  声音放送 程式终止(End)
            PlaySound (TEXT (strcat(g_szCurrPath, "\\Sounds\\CokucoE.kmp")), NULL, SND_FILENAME | SND_SYNC) ;
        }

        if(g_hBmpImage) DeleteObject(g_hBmpImage);
        if(g_hBmpMask)  DeleteObject(g_hBmpMask);
        PostQuitMessage (0) ;
        return 0 ;
    }

    return DefWindowProc (hwnd, message, wParam, lParam) ;
}



//----------------------------
//  左上角地砖子窗体处理函式
//  2005-09-22
//----------------------------
LRESULT CALLBACK TileWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC             hdc, hdcImage, hdcMask;
    RECT            rect;
    BITMAP          bitmap;
    SCROLLINFO      si;
    PAINTSTRUCT     ps;
    long            nTemp, nVertPos;
    static long     xSel, ySel;     //  选中的行号和列号
    static long     cxSel, cySel;   //  选中的行和列的宽度
//  static long     nDeltaPerLine, nAccumDelta ; 
    TCHAR           szBuffer[64];

    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    switch(message)
    {

    //  地砖图子窗体初始化
    case WM_CREATE:
        cxSel = cySel = 1;
        xSel = ySel = g_nSelTile = 0;
        SendMessage(hwnd, WM_CHANGETILE, 1, 0);
        return 0;


    //  大小变化
    case WM_SIZE:
        //  设置垂直滚动条的范围
        GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE;
        si.nMin   = 0;
        si.nMax   = bitmap.bmHeight / 32 - 1;
        si.nPage  = 15;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_VERT, &si, FALSE);
        return 0;


    //  重绘
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        if(g_hBmpImage)
        {
            hdcImage = CreateCompatibleDC(hdc);
            hdcMask  = CreateCompatibleDC(hdc);
            SelectObject(hdcImage, g_hBmpImage);
            SelectObject(hdcMask,  g_hBmpMask);
            SetBkColor(hdcImage, RGB(255, 0, 255));
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            SelectObject(hdc, GetStockObject(WHITE_PEN));

            //  获取滚动条位置
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask  = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            //  获取位图的信息
            GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);
//          BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);

            BitBlt(hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);//分3步进行实际的绘制 
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCINVERT);
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcMask , 0, 0, SRCAND);
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCINVERT);  //  恢复

            DeleteDC(hdcMask);
            DeleteDC(hdcImage);
            DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);
 
            //  显示地砖模式
            if(g_bDrawInfo && g_pTileInfo)
            {
                for(long i = si.nPos; i < (si.nPos + 15) && i < bitmap.bmHeight / 32; ++i)
                {
                    for(long j = 0; j < 8; ++j)
                    {
                        if((g_pTileInfo[i * 8 + j] & 0x80) == 0x80)
                        {
                            //  画大叉，此地砖不可能行走
                            MoveToEx(hdc, (j ) * 32 + 4,     (i - si.nPos) * 32 + 4,     0);
                            LineTo  (hdc, (j  + 1) * 32 - 4, (i - si.nPos + 1) * 32 - 4);
                            MoveToEx(hdc, (j ) * 32 + 4,     (i - si.nPos + 1) * 32 - 4, 0);
                            LineTo  (hdc, (j  + 1) * 32 - 4, (i - si.nPos) * 32 + 4);
                        }
                        else
                        {
                            //  画圆，可以走上去
                            Ellipse(hdc, (j ) * 32 + 4, (i - si.nPos) * 32 + 4, 
                                            (j  + 1) * 32 - 4, (i - si.nPos + 1) * 32 - 4);
                        }

                        if((g_pTileInfo[i * 8 + j] & 0x08) == 0x08)
                        {
                            //  画横线，表示是连续四帧地砖的开始
                            MoveToEx(hdc, (j ) * 32 + 16,     (i - si.nPos) * 32 + 16,     0);
                            LineTo  (hdc, (j + 1) * 32 - 4,   (i - si.nPos) * 32 + 16);
                        }

                        if((g_pTileInfo[i * 8 + j] & 0x40) == 0x40)
                            TextOut(hdc, j * 32, (i - si.nPos) * 32, "1", 1);
                        else if((g_pTileInfo[i * 8 + j] & 0x20) == 0x20)
                            TextOut(hdc, j * 32, (i - si.nPos) * 32, "2", 1);
                        else if((g_pTileInfo[i * 8 + j] & 0x10) == 0x10)
                            TextOut(hdc, j * 32, (i - si.nPos) * 32, "3", 1);
                    }
                }
            }
        }

        EndPaint(hwnd, &ps);
        return 0;


    //  点击鼠标左键
    case WM_LBUTTONDOWN:

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        //  非显示地砖信息模式，先将原先选中的区域重绘
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32 + si.nPos;
        cxSel = cySel = 1;

        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  显示地砖信息模式，更改其是否可以走上去
        if(g_bDrawInfo && g_pTileInfo)
        {
            //  标记是否可以走上去那位取反
            g_pTileInfo[ySel * 8 + xSel] ^= 0x80;

            SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (xSel + 1) * 32, (ySel - si.nPos + 1) * 32);
            InvalidateRect(hwnd, &rect, TRUE);
        }

        return 0;


    //  点击鼠标中键
    case WM_MBUTTONDOWN:

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        //  非显示地砖信息模式，先将原先选中的区域重绘
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32 + si.nPos;
        cxSel = cySel = 1;

        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  显示地砖信息模式，更改其是否是连续播放地砖
        if(g_bDrawInfo && g_pTileInfo && 0 == (xSel & 3))
        {
            //  标记是否可以走上去那位取反
            g_pTileInfo[ySel * 8 + xSel] ^= 0x08;

            SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (xSel + 1) * 32, (ySel - si.nPos + 1) * 32);
            InvalidateRect(hwnd, &rect, TRUE);
        }

        return 0;


    //  点击鼠标右键
    case WM_RBUTTONDOWN:
//  case WM_LBUTTONUP:

        //  先将原先选中的区域重绘
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        cxSel = LOWORD(lParam) / 32;
        cySel = HIWORD(lParam) / 32 + si.nPos;

        //  如果左键点的位置在右键位置的右侧，要交换两次的位置
        //  从而保证了左上角的位置正确性
        if(cxSel < xSel) { nTemp = cxSel; cxSel = xSel; xSel = nTemp; }
        if(cySel < ySel) { nTemp = cySel; cySel = ySel; ySel = nTemp; }

        cxSel -= (xSel - 1);
        cySel -= (ySel - 1);

        if(cxSel < 0) cxSel--;
        if(cySel < 0) cySel--;
        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  显示地砖信息模式，更改其是否可以走上去
        if(g_bDrawInfo && g_pTileInfo)
        {
            xSel = LOWORD(lParam) / 32;
            ySel = HIWORD(lParam) / 32 + si.nPos;
            switch(g_pTileInfo[ySel * 8 + xSel] & 0x70)
            {
            case 0x00:
                g_pTileInfo[ySel * 8 + xSel] &= 0x8F;
                g_pTileInfo[ySel * 8 + xSel] |= 0x40;
                break;
            case 0x40:
                g_pTileInfo[ySel * 8 + xSel] &= 0x8F;
                g_pTileInfo[ySel * 8 + xSel] |= 0x20;
                break;
            case 0x20:
                g_pTileInfo[ySel * 8 + xSel] &= 0x8F;
                g_pTileInfo[ySel * 8 + xSel] |= 0x10;
                break;
            case 0x10:
            default:
                g_pTileInfo[ySel * 8 + xSel] &= 0x8F;
                break;
            }

            SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (xSel + 1) * 32, (ySel - si.nPos + 1) * 32);
            InvalidateRect(hwnd, &rect, TRUE);
        }

        return 0;


    //  鼠标移动
    case WM_MOUSEMOVE:
        //  判断移动的时候是否按住左键
//        if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        return 0;

/*
    //  鼠标滚轮
    case WM_MOUSEWHEEL:

        if (nDeltaPerLine == 0) break ;
        nAccumDelta += (short) HIWORD (wParam) ;     // 120 or -120

        while (nAccumDelta >= nDeltaPerLine)
        {             
            SendMessage (hwnd, WM_VSCROLL, SB_LINEUP, 0) ;
            nAccumDelta -= nDeltaPerLine ;
        }

        while (nAccumDelta <= -nDeltaPerLine)
        {
            SendMessage (hwnd, WM_VSCROLL, SB_LINEDOWN, 0) ;
            nAccumDelta += nDeltaPerLine ;
        }

        return 0 ;
*/

    //  垂直滚动条
    case WM_VSCROLL:
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        nVertPos = si.nPos;
        switch(LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos -= 1;
            break;
        case SB_LINEDOWN:
            si.nPos += 1;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        }

        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

        if(nVertPos != si.nPos)
            ScrollWindow(hwnd, 0, (nVertPos - si.nPos) * 32, 0, 0);

        //InvalidateRect(hwnd, 0, TRUE);
        return 0;


    //  切换地砖编号
    case WM_CHANGETILE:
        //  要切换的编号和当前一致，直接返回，不作任何处理
        if(g_nSelTile == wParam) return 0;

        //  对原先已经打开的句柄进行释放
        if(g_hBmpImage) DeleteObject(g_hBmpImage);
        if(g_hBmpMask)  DeleteObject(g_hBmpMask);
        //wsprintf(szBuffer, ".\\Graphics\\Bg%03d.kkc", wParam);
        wsprintf(szBuffer, "%s\\Graphics\\Bg%03d.kkc", g_szCurrPath, wParam);

        //  读取地砖图片素材
        g_hBmpImage = (HBITMAP)LoadImage(0, szBuffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
        if(0 == g_hBmpImage)
        {
            //  打开素材失败
            MessageBox(hwnd, TEXT("读取素材库失败"), 0, MB_ICONERROR);
            MessageBox(hwnd, szBuffer, 0, MB_OK);
            SendMessage(GetParent(hwnd), WM_CLOSE, 0, 0);
        }

        //  更新图素菜单下的菜单选中项
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_TILE_01 + g_nSelTile - 1, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_TILE_01 + (long)wParam - 1, MF_CHECKED);

        //  打开图片素材成功，保存图片编号
        g_nSelTile = (long)wParam;
        g_stGround.nTile = g_nSelTile;
        GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);
        g_hBmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, 0);
        xSel  = ySel  = 0;  //  设置选中的图素是左上第一块
        cxSel = cySel = 1;  //  大小就一块

        //  由于图片不尽相同，需要重新设置滚动条滑块
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nMin   = 0;
        si.nMax   = bitmap.bmHeight / 32 - 1;
        si.nPage  = 15;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        InvalidateRect(hwnd, 0, TRUE);

        g_pTileInfo = g_pAllTileInfo + (g_nSelTile - 1) * TILEINFO_SIZE;
        return 0;


    //  获取选中的图素编号（左上角）
    case WM_GETSELTILE:
        return MAKELONG(xSel, ySel);    //  MAKELONG(LOW, HIGH)  (COLUMN, LINE)


    //  获取选中图素的大小
    case WM_GETSELSIZE:
        return MAKELONG(cxSel, cySel);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}



//----------------------------------
//  右边地图制作子窗体事件处理程式
//  2005-09-22
//----------------------------------
LRESULT CALLBACK MapWndProc (HWND hwnd, UINT message, 
                               WPARAM wParam, LPARAM lParam)
{
    HDC             hdc, hdcImage, hdcMask;
    RECT            rect;
    POINT           pt;
    BITMAP          bitmap;
    SCROLLINFO      si;
    PAINTSTRUCT     ps;
    long            nRetTile, nRetSize, i, j, nVertOldPos, nHorzOldPos;
    static long     xSel, ySel;             //  选中的行和列
    static long     xSize, ySize;           //  地图的大小（32 x 32）
    static long     cxClient, cyClient;     //  窗口的大小（像素）
    static long     nVertPos, nHorzPos;     //  垂直，水平 滚动条位置
    static long     nLayer;                 //  当前操作的层（0：显示全部层 1~3 显示对应的层）
    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    switch(message)
    {
    //  地图制作子窗体初始化
    case WM_CREATE:
        //nLayer = 0;
        SendMessage(hwnd, WM_SETLAYER, nLayer = 0, 0);
        return 0;

    //  地图制作子窗体大小变化（受父窗体大小变化影响）
    case WM_SIZE:
        GetClientRect(hwnd, &rect);
        cxClient = rect.right - rect.left;
        cyClient = rect.bottom - rect.top;

        nVertPos = nHorzPos = 0;

        //  重设垂直滚动条
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nPage  = cyClient / 32 - 1;
        si.nMin   = 0;
        si.nMax   = ySize;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_VERT, &si, FALSE);
        GetScrollInfo(hwnd, SB_VERT, &si);
        nVertPos = si.nPos;

        //  重设水平滚动条
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nPage  = cxClient / 32 - 1;
        si.nMin   = 0;
        si.nMax   = xSize;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_HORZ, &si, FALSE);
        GetScrollInfo(hwnd, SB_HORZ, &si);
        nHorzPos = si.nPos;

        return 0;


    //  重绘
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        if(g_stGround.mtAreas.GetLineCount() > 0 && g_stGround.mtAreas.GetColumnCount() > 0)
        {
            SetRect(&rect, 0, 0, (xSize - nHorzPos) * 32, (ySize - nVertPos) * 32);
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

            GetClientRect(hwnd, &rect);

            hdcImage = CreateCompatibleDC(hdc);
            hdcMask  = CreateCompatibleDC(hdc);
            SelectObject(hdcImage, g_hBmpImage);
            SelectObject(hdcMask,  g_hBmpMask);
            SetBkColor(hdcImage, RGB(255, 0, 255));

            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            SelectObject(hdc, GetStockObject(WHITE_PEN));

            GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);

//            for(ySel = nVertPos; (ySel < (rect.bottom - rect.top) / 32 + nVertPos + 1) && ySel < g_stGround.mtAreas.GetLineCount(); ++ySel)
            for(ySel = ps.rcPaint.top / 32 + nVertPos; ySel < ps.rcPaint.bottom / 32 + nVertPos + 1 && ySel < g_stGround.mtAreas.GetLineCount(); ++ySel)
            {
//                for(xSel = nHorzPos; (xSel < (rect.right - rect.left) / 32 + nHorzPos + 1) && xSel < g_stGround.mtAreas.GetColumnCount(); ++xSel)
                for(xSel = ps.rcPaint.left / 32 + nHorzPos; xSel < ps.rcPaint.right / 32 + nHorzPos + 1 && xSel < g_stGround.mtAreas.GetColumnCount(); ++xSel)
                {
                    switch(nLayer)
                    {
                    case 1:
                    case 2:
                    case 3:
                        //  只显示单层，则对单层进行重绘
                        if( pAreas[ySel][xSel].stTile[nLayer - 1].line   >= 0 && pAreas[ySel][xSel].stTile[nLayer - 1].column >= 0)
                        {
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[nLayer - 1].column * 32, pAreas[ySel][xSel].stTile[nLayer - 1].line * 32, SRCCOPY);
                        }
                        break;

                    default:
                        //  显示多层，从最底下一层开始绘制，后两层使用透明绘制
                        if(pAreas[ySel][xSel].stTile[0].line   >= 0 && pAreas[ySel][xSel].stTile[0].column >= 0)
                        {
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[0].column * 32, pAreas[ySel][xSel].stTile[0].line * 32, SRCCOPY);
                        }

                        if(pAreas[ySel][xSel].stTile[1].line   >= 0 && pAreas[ySel][xSel].stTile[1].column >= 0)
                        {
                            //BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[1].column * 32, pAreas[ySel][xSel].stTile[1].line * 32, SRCCOPY);
                            BitBlt(hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[1].column * 32, pAreas[ySel][xSel].stTile[1].line * 32, SRCINVERT);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcMask , pAreas[ySel][xSel].stTile[1].column * 32, pAreas[ySel][xSel].stTile[1].line * 32, SRCAND);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[1].column * 32, pAreas[ySel][xSel].stTile[1].line * 32, SRCINVERT); //  恢复
                        }

                        if(pAreas[ySel][xSel].stTile[2].line   >= 0 && pAreas[ySel][xSel].stTile[2].column >= 0)
                        {
                            //BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCCOPY);
                            BitBlt(hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCINVERT);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcMask , pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCAND);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCINVERT); //  恢复
                        }

                        //  绘制该区域是否可以走上去的标记
                        if(TRUE == g_bDrawInfo)
                        {
                            if((pAreas[ySel][xSel].nAreaInfo & 0x8000) == 0x8000)
                            {
                                //  画大叉，不能走上去
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 4,     (ySel - nVertPos) * 32 + 4,     0);
                                LineTo  (hdc, (xSel - nHorzPos + 1) * 32 - 4, (ySel - nVertPos + 1) * 32 - 4);
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 4,     (ySel - nVertPos + 1) * 32 - 4, 0);
                                LineTo  (hdc, (xSel - nHorzPos + 1) * 32 - 4, (ySel - nVertPos) * 32 + 4);
                            }
                            else
                            {
                                //  画圆，可以走上去
                                Ellipse(hdc, (xSel - nHorzPos) * 32 + 4, (ySel - nVertPos) * 32 + 4, 
                                             (xSel - nHorzPos + 1) * 32 - 4, (ySel - nVertPos + 1) * 32 - 4);
                            }

							if(pAreas[ySel][xSel].nAreaInfo & 0x0004)
							{
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 16, (ySel - nVertPos) * 32 + 8, 0);
                                LineTo  (hdc, (xSel - nHorzPos) * 32 + 28, (ySel - nVertPos) * 32 + 8);
							}
							if(pAreas[ySel][xSel].nAreaInfo & 0x0002)
							{
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 16, (ySel - nVertPos) * 32 + 16, 0);
                                LineTo  (hdc, (xSel - nHorzPos) * 32 + 28, (ySel - nVertPos) * 32 + 16);
							}
							if(pAreas[ySel][xSel].nAreaInfo & 0x0001)
							{
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 16, (ySel - nVertPos) * 32 + 24, 0);
                                LineTo  (hdc, (xSel - nHorzPos) * 32 + 28, (ySel - nVertPos) * 32 + 24);
							}
                        }

                        break;

                    } //end of switch(nLayer)

                } // end of for(xSel)

            } // end of for(ySel)

            DeleteDC(hdcMask);
            DeleteDC(hdcImage);
        }

        EndPaint(hwnd, &ps);
        return 0;


    //  鼠标移动事件
    case WM_MOUSEMOVE:
        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32;
        xSel += nHorzPos;
        ySel += nVertPos;

        if(0 < g_stGround.mtAreas.GetLineCount() && 0 < g_stGround.mtAreas.GetColumnCount() &&
            ySel < g_stGround.mtAreas.GetLineCount() && xSel < g_stGround.mtAreas.GetColumnCount())
        {
            DrawInfo(::GetParent(hwnd), xSel, ySel);
        }
        else
        {
            DrawInfo(::GetParent(hwnd), -1, -1);
        }

        //  在移动的时候判定是否按下了鼠标左键或右键
        //  纯粹是为了方便操作
        if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_RBUTTONDOWN, wParam, lParam);
        return 0;


    //  鼠标左键按下：将左边图素子窗体中选定的图素设置到鼠标点击位置
    case WM_LBUTTONDOWN:
        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32;
        xSel += nHorzPos;
        ySel += nVertPos;

        //  显示地砖属性，点击后将是否可以走上去属性取反
        if(g_bDrawInfo && ySel < g_stGround.mtAreas.GetLineCount() && xSel < g_stGround.mtAreas.GetColumnCount())
        {
            pAreas[ySel][xSel].nAreaInfo ^= 0x8000;
            SetRect(&rect, (xSel - nHorzPos) << 5, (ySel - nVertPos) << 5, (xSel - nHorzPos + 1) << 5, (ySel - nVertPos + 1) << 5);
            InvalidateRect(hwnd, &rect, TRUE);
            return 0;
        }

        //  不显示地砖属性，直接添加图素
        if(xSel < xSize && ySel < ySize)
        {
            hdc = GetDC(hwnd);
            hdcImage = CreateCompatibleDC(hdc);
            SelectObject(hdcImage, g_hBmpImage);

            //  获得选择图素库中的行号列号，以及图素库选中的范围大小
            nRetSize = (long) SendMessage(GetDlgItem(GetParent(hwnd), IDG_TILE), WM_GETSELSIZE, 0, 0);
            nRetTile = (long) SendMessage(GetDlgItem(GetParent(hwnd), IDG_TILE), WM_GETSELTILE, 0, 0);

            //  遍历全部需要刷新的图素
            for(i = 0; i < LOWORD(nRetSize); ++i)
            {
                for(j = 0; j < HIWORD(nRetSize); ++j)
                {
                    if(xSel + i < xSize && ySel + j < ySize)
                    {
                        if(1 <= nLayer && nLayer <= 3)
                        {
                            //  添加到指定的层
                            pAreas[ySel + j][xSel + i].stTile[nLayer - 1].column = LOWORD(nRetTile) + i;
                            pAreas[ySel + j][xSel + i].stTile[nLayer - 1].line   = HIWORD(nRetTile) + j;

                            if((g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[nLayer - 1].line * 8 + pAreas[ySel + j][xSel + i].stTile[nLayer - 1].column] & 0x08) == 0x08)
							{
								///pAreas[ySel + j][xSel + i].stTile[nLayer - 1].change = 1;
								pAreas[ySel + j][xSel + i].SetChangeTile(nLayer - 1, 1);
							}
                            else
							{
                                ///pAreas[ySel + j][xSel + i].stTile[nLayer - 1].change = 0;
								pAreas[ySel + j][xSel + i].SetChangeTile(nLayer - 1, 0);
							}
                        }
                        else if(51 > g_nSelTile && 0 != g_pTileInfo)
                        {
                            if(0 == g_pTileInfo[(HIWORD(nRetTile) + j) * 8 + LOWORD(nRetTile) + i])
                            {
                                //  此选中区域无效或者数据未设置
                                continue;
                            }

                            //  添加到系统默认的层
                            //  根据定义好的层来设置数据
                            if((g_pTileInfo[(HIWORD(nRetTile) + j) * 8 + LOWORD(nRetTile) + i] & 0x40) == 0x40)
                            {
                                pAreas[ySel + j][xSel + i].stTile[0].column = LOWORD(nRetTile) + i;
                                pAreas[ySel + j][xSel + i].stTile[0].line   = HIWORD(nRetTile) + j;

                                if((g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[0].line * 8 + pAreas[ySel + j][xSel + i].stTile[0].column] & 0x08) == 0x08)
                                    ///pAreas[ySel + j][xSel + i].stTile[0].change = 1;
									pAreas[ySel + j][xSel + i].SetChangeTile(0, 1);
                                else
                                    ///pAreas[ySel + j][xSel + i].stTile[0].change = 0;
									pAreas[ySel + j][xSel + i].SetChangeTile(0, 0);
                            }
                            else if((g_pTileInfo[(HIWORD(nRetTile) + j) * 8 + LOWORD(nRetTile) + i] & 0x20) == 0x20)
                            {
                                pAreas[ySel + j][xSel + i].stTile[1].column = LOWORD(nRetTile) + i;
                                pAreas[ySel + j][xSel + i].stTile[1].line   = HIWORD(nRetTile) + j;

                                if((g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[1].line * 8 + pAreas[ySel + j][xSel + i].stTile[1].column] & 0x08) == 0x08)
                                    ///pAreas[ySel + j][xSel + i].stTile[1].change = 1;
									pAreas[ySel + j][xSel + i].SetChangeTile(1, 1);
                                else
                                    ///pAreas[ySel + j][xSel + i].stTile[1].change = 0;
									pAreas[ySel + j][xSel + i].SetChangeTile(1, 0);
                            }
                            else if((g_pTileInfo[(HIWORD(nRetTile) + j) * 8 + LOWORD(nRetTile) + i] & 0x10) == 0x10)
                            {
                                pAreas[ySel + j][xSel + i].stTile[2].column = LOWORD(nRetTile) + i;
                                pAreas[ySel + j][xSel + i].stTile[2].line   = HIWORD(nRetTile) + j;

                                if((g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[2].line * 8 + pAreas[ySel + j][xSel + i].stTile[2].column] & 0x08) == 0x08)
                                    ///pAreas[ySel + j][xSel + i].stTile[2].change = 1;
									pAreas[ySel + j][xSel + i].SetChangeTile(2, 1);
                                else
                                    ///pAreas[ySel + j][xSel + i].stTile[2].change = 0;
									pAreas[ySel + j][xSel + i].SetChangeTile(2, 0);
                            }

                            //  更新当前地砖是否可以被占用。
                            ///pAreas[ySel + j][xSel + i].nAreaInfo = 0;
                            if(pAreas[ySel + j][xSel + i].stTile[0].line >= 0 && pAreas[ySel + j][xSel + i].stTile[0].column >= 0 &&
                                (g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[0].line * 8 + pAreas[ySel + j][xSel + i].stTile[0].column] & 0x80) == 0x80)
                            {
                                pAreas[ySel + j][xSel + i].nAreaInfo |= 0x8000;
                            }
                            if(pAreas[ySel + j][xSel + i].stTile[1].line >= 0 && pAreas[ySel + j][xSel + i].stTile[1].column >= 0 &&
                                (g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[1].line * 8 + pAreas[ySel + j][xSel + i].stTile[1].column] & 0x80) == 0x80)
                            {
                                pAreas[ySel + j][xSel + i].nAreaInfo |= 0x8000;
                            }
                            if(pAreas[ySel + j][xSel + i].stTile[2].line >= 0 && pAreas[ySel + j][xSel + i].stTile[2].column >= 0 &&
                                (g_pTileInfo[pAreas[ySel + j][xSel + i].stTile[2].line * 8 + pAreas[ySel + j][xSel + i].stTile[2].column] & 0x80) == 0x80)
                            {
                                pAreas[ySel + j][xSel + i].nAreaInfo |= 0x8000;
                            }
                        }
                    }
                }
            }

            //  对已经数据更新过的图素矩形范围失效，从而可以在界面上显示出来
//          SetRect(&rect, (xSel - nHorzPos) << 5, (ySel - nVertPos) << 5, (xSel - nHorzPos + LOWORD(nRetSize)) << 5, (ySel - nVertPos + HIWORD(nRetSize)) << 5);
//          InvalidateRect(hwnd, &rect, TRUE);

            if(1 <= nLayer && nLayer <= 3)
            {
                i = LOWORD(nRetSize) + xSel > xSize ? xSize - xSel : LOWORD(nRetSize);
                j = HIWORD(nRetSize) + ySel > ySize ? ySize - ySel : HIWORD(nRetSize);
                BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, i * 32, j * 32, hdcImage, LOWORD(nRetTile) * 32, HIWORD(nRetTile) * 32, SRCCOPY);
            }
            else
            {
                //  对已经数据更新过的图素矩形范围失效，从而可以在界面上显示出来
                SetRect(&rect, (xSel - nHorzPos) << 5, (ySel - nVertPos) << 5, (xSel - nHorzPos + LOWORD(nRetSize)) << 5, (ySel - nVertPos + HIWORD(nRetSize)) << 5);
                InvalidateRect(hwnd, &rect, TRUE);
            }

            DeleteDC(hdcImage);
            ReleaseDC(hwnd, hdc);
        }
        else if(xSize <= 0 || ySize <= 0)
        {
            //  场景没有初始化过，需要初始化
            SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_MAP_SIZE, 0), 0);
        }

        return 0;


    //  鼠标右键处理：清空该图素
    case WM_RBUTTONDOWN:

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32;

        //  从界面上坐标转换为对应数据位置的绝对坐标（算上滚动条的位置）
        xSel += nHorzPos;
        ySel += nVertPos;

        if(xSel < xSize && ySel < ySize)
        {
            GetCursorPos(&pt);
            TrackPopupMenu(GetSubMenu(LoadMenu(0, MAKEINTRESOURCE(IDR_MENU1)), 3), TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, 0);
        }
        return 0;


    //  自定义消息：设置地图大小 32 x 32
    case WM_SETMAPSIZE:
        ySize = LOWORD(lParam);     //  行数
        xSize = HIWORD(lParam);     //  列数

        //  重新设置滚动条
        GetClientRect(hwnd, &rect);
        cxClient = rect.right - rect.left;
        cyClient = rect.bottom - rect.top;

        nVertPos = nHorzPos = 0;

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nPage  = cyClient / 32 - 1;
        si.nMin   = 0;
        si.nMax   = ySize;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_VERT, &si, FALSE);
        GetScrollInfo(hwnd, SB_VERT, &si);
        nVertPos = si.nPos;

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS;
        si.nPage  = cxClient / 32 - 1;
        si.nMin   = 0;
        si.nMax   = xSize;
        si.nPos   = 0;
        si.nTrackPos = 0;
        SetScrollInfo(hwnd, SB_HORZ, &si, FALSE);
        GetScrollInfo(hwnd, SB_HORZ, &si);
        nHorzPos = si.nPos;

        InvalidateRect(hwnd, 0 ,TRUE);
        return 0;


    //  设置地图层显示
    case WM_SETLAYER:
        if(wParam >= 1 && wParam <= 3)
        {
            nLayer = (long)wParam;
        }
        else
        {
            nLayer = 0;
        }

        //  更新地图菜单下的地图层次子菜单下的层号选中项
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_ALL, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_1, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_2, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_3, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_ALL + nLayer, MF_CHECKED);
        return 0;


    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        //  清空全部层数据
        case IDM_RC_CLEARALL:
        case IDM_RC_CLEARLAYER1:
        case IDM_RC_CLEARLAYER2:
        case IDM_RC_CLEARLAYER3:
            if(xSel < xSize && ySel < ySize)
            {
                //  如果是单独一层，就对单独层进行清空
                //  如果是显示全部，则对全部层进行清空
                switch(LOWORD(wParam) - IDM_RC_CLEARALL)
                {
                case 1:
                case 2:
                case 3:
                    pAreas[ySel][xSel].stTile[LOWORD(wParam) - IDM_RC_CLEARALL].line   = -1;
                    pAreas[ySel][xSel].stTile[LOWORD(wParam) - IDM_RC_CLEARALL].column = -1;
                    ///pAreas[ySel][xSel].stTile[LOWORD(wParam) - IDM_RC_CLEARALL].change = 0;
					pAreas[ySel][xSel].nAreaInfo &= 0xFFF8;
                    break;
                default:
                    pAreas[ySel][xSel].stTile[0].line   = -1;
                    pAreas[ySel][xSel].stTile[0].column = -1;
                    ///pAreas[ySel][xSel].stTile[0].change = 0;
					pAreas[ySel][xSel].nAreaInfo &= 0xFFFB;

                    pAreas[ySel][xSel].stTile[1].line   = -1;
                    pAreas[ySel][xSel].stTile[1].column = -1;
                    ///pAreas[ySel][xSel].stTile[1].change = 0;
					pAreas[ySel][xSel].nAreaInfo &= 0xFFFD;

                    pAreas[ySel][xSel].stTile[2].line   = -1;
                    pAreas[ySel][xSel].stTile[2].column = -1;
                    ///pAreas[ySel][xSel].stTile[2].change = 0;
					pAreas[ySel][xSel].nAreaInfo &= 0xFFFE;

                    pAreas[ySel][xSel].nAreaInfo |= 0x8000;
                    break;
                }

                //  对右键点击的区域在界面上进行清空
                SetRect(&rect, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, (xSel - nHorzPos + 1) * 32, (ySel - nVertPos + 1) * 32);
                InvalidateRect(hwnd, &rect, TRUE);
            }
            else if(xSize <= 0 || ySize <= 0)
            {
                SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_MAP_SIZE, 0), 0);
            }

            return 0;


        //  设置不允许站立
        case IDM_RC_NOTSTAND:
            pAreas[ySel][xSel].nAreaInfo |= 0x8000;
            return 0;


        //  设置允许站立
        case IDM_RC_CANSTAND:
            pAreas[ySel][xSel].nAreaInfo &= 0x7FFF;
            return 0;


        //  设置锚点
        case IDM_RC_SETANCHOR:
            pAreas[ySel][xSel].nAreaInfo |= 0x4000;
            pAreas[ySel][xSel].nAnchor = g_nParam1;
            return 0;

        }
        return 0;


    //  垂直滚动条消息处理
    case WM_VSCROLL:
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        nVertOldPos = si.nPos;
        switch(LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos -= 1;
            break;
        case SB_LINEDOWN:
            si.nPos += 1;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        }
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        GetScrollInfo(hwnd, SB_VERT, &si);
        nVertPos = si.nPos;

        if(nVertOldPos != si.nPos)
            ScrollWindow(hwnd, 0, (nVertOldPos - si.nPos) * 32, 0, 0);

        return 0;


    //  水平滚动条消息处理
    case WM_HSCROLL:
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_HORZ, &si);
        nHorzOldPos = si.nPos;

        switch(LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos -= 1;
            break;
        case SB_LINEDOWN:
            si.nPos += 1;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        }
        si.fMask = SIF_POS;
        SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
        GetScrollInfo(hwnd, SB_HORZ, &si);
        nHorzPos = si.nPos;

        if(nHorzOldPos != si.nPos)
            ScrollWindow(hwnd, (nHorzOldPos - si.nPos) * 32, 0, 0, 0);

        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}



//------------------------------------
//  设置地图大小对话框的消息处理函式
//------------------------------------
LRESULT CALLBACK MapSizeDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    long xSize, ySize;
    char szBuffer[64];

    switch(message)
    {
    //  窗体初始化
    case WM_INITDIALOG:
        //if(0 <= g_stGround.mtAreas.GetLineCount() || 0 <= g_stGround.mtAreas.GetColumnCount())
        //{
        //    xSize = ySize = 0;
        //}
        //else
        {
            ySize = g_stGround.mtAreas.GetLineCount();
            xSize = g_stGround.mtAreas.GetColumnCount();
            sprintf(szBuffer, "%ld", ySize);
            SetWindowText(GetDlgItem(hDlg, IDC_SIZE_VERT), szBuffer);
            sprintf(szBuffer, "%ld", xSize);
            SetWindowText(GetDlgItem(hDlg, IDC_SIZE_HORZ), szBuffer);
            sprintf(szBuffer, "%ld", g_nParam1);
            SetWindowText(GetDlgItem(hDlg, IDC_SIZE_PARAM1), szBuffer);
        }
        return FALSE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        //  点击“确定”按钮事件处理
        case IDOK:
            GetWindowText(GetDlgItem(hDlg, IDC_SIZE_VERT), szBuffer, 64);
            ySize = atol(szBuffer);
            GetWindowText(GetDlgItem(hDlg, IDC_SIZE_HORZ), szBuffer, 64);
            xSize = atol(szBuffer);
            GetWindowText(GetDlgItem(hDlg, IDC_SIZE_PARAM1), szBuffer, 64);
            g_nParam1 = atol(szBuffer);

            if(xSize >= 10 && ySize >= 10 &&
               xSize <= 3000 && ySize <= 3000)
            {
/*                Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();
                for(long i = 0; i < g_stGround.mtAreas.GetLineCount(); ++i)
                {   for(long j = 0; j < g_stGround.mtAreas.GetColumnCount(); ++j)
                    {
                        pAreas[i][j].stTile[0].line   = -1;
                        pAreas[i][j].stTile[0].column = -1;
                        pAreas[i][j].stTile[1].line   = -1;
                        pAreas[i][j].stTile[1].column = -1;
                        pAreas[i][j].stTile[2].line   = -1;
                        pAreas[i][j].stTile[2].column = -1;
                        pAreas[i][j].nAreaInfo = 0;
                    }
                }
*/
                g_stGround.mtAreas.SetSize(ySize, xSize);
                g_szFileName[0] = 0;
                SendMessage(GetDlgItem(GetParent(hDlg), IDG_MAP), WM_SETMAPSIZE, 0, MAKELONG(ySize, xSize));
            }

        //  点击“取消”按钮事件处理（关闭窗体）
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}



//----------------------------
//  由指定窗体上绘制空心矩形
//----------------------------
//                          绘图位置            图素大小
VOID DrawRect(HWND hwnd, long xPos, long yPos, long cx, long cy)
{
    if(g_bDrawInfo) 
        return;

    HDC     hdc;

    hdc = GetDC(hwnd);

    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    SelectObject(hdc, GetStockObject(WHITE_PEN));
    Rectangle(hdc, xPos * 32, yPos * 32, (xPos + cx) * 32, (yPos + cy) * 32);

    ReleaseDC(hwnd, hdc);
}



//------------------------
//  拖入文件处理函数
//------------------------
BOOL DropMapFile(HDROP hDrop, HWND hwndTile, HWND hwndMap)
{
    DragQueryFile(hDrop, 0, g_szFileName, MAX_PATH - 1);
    DragFinish(hDrop);

    if(! g_szFileName[0])
    {
        return FALSE;
    }

    CTileFile tf;

    if(! tf.ReadDataFile(g_szFileName, &g_stGround))
    {
        g_szFileName[0] = 0;
        return FALSE;
    }

    SendMessage(hwndTile, WM_CHANGETILE, g_stGround.nTile, 0);
    SendMessage(hwndMap, WM_SETMAPSIZE, 0, MAKELONG(g_stGround.mtAreas.GetLineCount(), g_stGround.mtAreas.GetColumnCount()));
    SendMessage(hwndMap, WM_SETLAYER, 0, 0);
    InvalidateRect(hwndMap, 0, TRUE);

    return TRUE;
}


//------------------------
//  读取游戏脚本
//------------------------
BOOL LoadMapFile(HWND hwndTile, HWND hwndMap)
{
    TCHAR szFileName[MAX_PATH] = {0};
    OPENFILENAME ofn ;
    memset(&ofn, 0, sizeof(ofn));

    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = GetParent(hwndMap);
    ofn.lpstrFile       = szFileName;
    ofn.nMaxFile        = sizeof(szFileName);
    ofn.lpstrFilter     = "数据文件 (*.dat)\0*.DAT\0全部文件 (*.*)\0*.*\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(! GetOpenFileName(&ofn))
    {
        //  打开文件取消，返回 TRUE 仅表示没有出错
        return TRUE;
    }

    CTileFile tf;

    if(! tf.ReadDataFile(szFileName, &g_stGround))
    {
        MessageBox(0, "读取场景失败！", 0, MB_ICONERROR);
        return FALSE;
    }

    strcpy(g_szFileName, szFileName);
    SendMessage(hwndTile, WM_CHANGETILE, g_stGround.nTile, 0);
    SendMessage(hwndMap, WM_SETMAPSIZE, 0, MAKELONG(g_stGround.mtAreas.GetLineCount(), g_stGround.mtAreas.GetColumnCount()));
    SendMessage(hwndMap, WM_SETLAYER, 0, 0);
    InvalidateRect(hwndMap, 0, TRUE);

    return TRUE;
}



//-------------------------
//  另存为
//-------------------------
BOOL SaveMapFile()
{
    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    if(g_stGround.mtAreas.GetLineCount() <= 0) return FALSE;
    if(g_stGround.mtAreas.GetColumnCount() <= 0) return FALSE;

    TCHAR szFileName[MAX_PATH] = {0};
    OPENFILENAME ofn ;
    memset(&ofn, 0, sizeof(OPENFILENAME));

    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = 0; //GetParent(hwndMap);
    ofn.lpstrFile       = szFileName;
    ofn.nMaxFile        = sizeof(szFileName);
    ofn.lpstrFilter     = "数据文件 (*.dat)\0*.DAT\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_OVERWRITEPROMPT;

    if(! GetOpenFileName(&ofn))
    {
        //  打开文件取消，返回 TRUE 仅表示没有出错
        return TRUE;
    }

    //  判断输入的文件名后四位是否是 .dat，如果不是则加上
    //  PS：_stricmp 大小写不敏感
    long nLen = (long)strlen(szFileName);
    if(4 < nLen && _stricmp(szFileName + nLen - 4, ".dat")) 
        strcpy(szFileName, strcat(szFileName, ".dat"));

    //  写数据文件
    CTileFile tf;
    if(! tf.WriteGroundData(szFileName, &g_stGround))
    {
        g_szFileName[0] = 0;
        MessageBox(0, "场景数据写入失败！", 0, MB_ICONERROR);
        return FALSE;
    }

    //  保存文件名和文件路径
    strcpy(g_szFileName, szFileName);
    return TRUE;
}



//-------------------------
//  读地砖信息文件
//  2005-09-22
//-------------------------
BOOL LoadTileInfo()
{
    char szBuffer[MAX_PATH] = {0};
    FILE* fp;

    if(0 >= g_nSelTile && 51 <= g_nSelTile)
        return FALSE;

    memset(g_pAllTileInfo, 0, ALLTILEINFO_SIZE);
    sprintf(szBuffer, "%s\\Data\\MapCreator.toc", g_szCurrPath);

    if(0 == (fp = fopen(szBuffer, "rb")))
    {
        return FALSE;
    }

    fread(g_pAllTileInfo, sizeof(BYTE), ALLTILEINFO_SIZE, fp);
/*
    memcpy(g_pAllTileInfo + TILEINFO_SIZE * 43, g_pAllTileInfo + TILEINFO_SIZE * 42, 400);
    memcpy(g_pAllTileInfo + TILEINFO_SIZE * 44, g_pAllTileInfo + TILEINFO_SIZE * 42, 400);
    memcpy(g_pAllTileInfo + TILEINFO_SIZE * 45, g_pAllTileInfo + TILEINFO_SIZE * 42, 400);
*/
    fclose(fp);
    return TRUE;
}



//-------------------------
//  写地砖信息文件
//  2005-09-22
//-------------------------
BOOL SaveTileInfo()
{
    char szBuffer[MAX_PATH] = {0};
    FILE* fp;

    sprintf(szBuffer, "%s\\Data\\MapCreator.toc", g_szCurrPath);

    if(0 == (fp = fopen(szBuffer, "wb+")))
    {
        return FALSE;
    }

    fwrite(g_pAllTileInfo, sizeof(BYTE), ALLTILEINFO_SIZE, fp);

    fclose(fp);
    return TRUE;
}



//-------------------------
//  提取当前目录
//-------------------------
VOID GetCurrPath()
{
    GetModuleFileName(0, g_szCurrPath, MAX_PATH);

    long nPos = 0;
    for(long i = 0; i < MAX_PATH; ++ i)
    {
        if(g_szCurrPath[i] == '\\')
            nPos = i;
    }

    if(nPos > 0 && nPos < MAX_PATH)
    {  
        //  将最后一个 \ 设成置成 \0 结束
        g_szCurrPath[nPos] = '\0';
    }
}



//-------------------------
//  在父窗体上显示信息
//-------------------------
VOID DrawInfo(HWND hwnd, long x, long y)
{

    if(0 <= x && 0 <= y)
    {
        HDC hdc = GetDC(hwnd);
        TCHAR szBuffer[MAX_PATH] = {0};

        SetBkColor  (hdc, 0x808080);
        SetTextColor(hdc, 0xFFFFFF);

        wsprintf(szBuffer, "X = %-4d        ", x);
        TextOut(hdc, 20, 480, szBuffer, (int)strlen(szBuffer));
        wsprintf(szBuffer, "Y = %-4d        ", y);
        TextOut(hdc, 80, 480, szBuffer, (int)strlen(szBuffer));

        DeleteDC(hdc);
    }
    else
    {
        RECT rect;
        SetRect(&rect, 0, 480, 275, 520);
        InvalidateRect(hwnd, &rect, TRUE);
    }

}



/*
//-------------------------
//  保存 CSV 文件
//-------------------------
VOID SaveCsvFile()
{
    string strText;
    char szBuffer[MAX_PATH];

    for(long i = 0; i < g_stGround.mtAreas.GetLineCount(); ++i)
    {
        for(long j = 0; j < g_stGround.mtAreas.GetColumnCount(); ++j)
        {
            memset(szBuffer, 0, MAX_PATH);
            sprintf(szBuffer, "%d", g_stGround.mtAreas[i][j].nAnchor);
            strText += szBuffer;
            strText += ",";
        }

        strText += "\n";
    }

    ofstream os;
    os.open("C:\\Test.csv");
    os.write(strText.c_str(), (std::streamsize)strText.size());
    os.close();
}
*/


//-------------------------
//  层一的提示（测试用）
//-------------------------
VOID MapInfo(HWND hwnd)
{
    long nLn, nCol;
    long i, j;
    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    nLn  = g_stGround.mtAreas.GetLineCount();
    nCol = nLn > 0 ? g_stGround.mtAreas.GetColumnCount() : 0;

    char pBuffer[255];
    string strText;
    strText  = g_szFileName;
    strText += "        \n\n";
    strText += "图素：";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", g_nSelTile);
    strText += pBuffer;
    strText += "\n";

    strText += "音乐：";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", g_stGround.nBGM);
    strText += pBuffer;
    strText += + "\n";

    strText += "行数：";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", nLn);
    strText += pBuffer;
    strText += + "\n";
    
    strText += "列数：";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", nCol);
    strText += pBuffer;
    strText += "\n\n";

    if(nLn  > 8) nLn  = 8;
    if(nCol > 8) nCol = 8;
    for(i = 0; i < nLn; ++i)
    {
        for(j = 0; j < nCol; ++j)
        {
            if(pAreas[i][j].stTile[0].line >= 0 && pAreas[i][j].stTile[0].column >= 0)
            {
                memset(pBuffer, 0, 255);
                sprintf(pBuffer, "%ld", pAreas[i][j].stTile[0].column);
                strText += "(";
                strText += pBuffer;
                strText += ",";

                memset(pBuffer, 0, 255);
                sprintf(pBuffer, "%ld", pAreas[i][j].stTile[0].line);
                strText += pBuffer;
                strText += ")\t";
            }
            else
            {
                strText += "(NULL)\t";
            }
        }

        strText += "\n";
    }

    MessageBox(hwnd, strText.c_str(), TEXT("MapInfo"), MB_ICONINFORMATION);
}

