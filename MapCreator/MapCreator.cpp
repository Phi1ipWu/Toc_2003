

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
    hwnd  = CreateWindow (g_szAppName, TEXT ("���촫˵��ͼ������ʽ  By: KudoXinyiji"), 
        WS_OVERLAPPEDWINDOW, 
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, hMenu, hInstance, NULL) ;

    ShowWindow (hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow (hwnd) ;

    //  �����ļ����봰��
    DragAcceptFiles(hwnd, TRUE);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
    }

    return (int)msg.wParam ;
}



//---------------------
//  ������Ĵ����ʽ
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
    //  �������ʼ��
    case WM_CREATE:
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

        GetCurrPath();
        if(! LoadTileInfo())
        {
            if(IDNO == MessageBox(hwnd, "��ȡͼ����Ϣʧ�ܣ��Ƿ������", "����", MB_YESNO | MB_ICONERROR))
                SendMessage(hwnd, WM_CLOSE, 0, 0);
        }

        g_bSound = GetPrivateProfileInt("MAPCREATOR", "WAV", 0, ".\\Config.cfg");

        //  ���� ���Ͻ� ��שͼ��ʾ �Ӵ���
        hwndTile = CreateWindow(g_szTileClass, 0, WS_CHILDWINDOW|WS_VISIBLE|WS_VSCROLL, 0,0,0,0,
                    hwnd, (HMENU)(IDG_TILE), hInstance, 0);

        //  ���� �ұ� ��ͼ��ʾ �Ӵ���
        hwndMap  = CreateWindow(g_szMapClass, 0, WS_CHILDWINDOW|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL, 0,0,0,0,
                    hwnd, (HMENU)(IDG_MAP),  hInstance, 0);

        if(TRUE == g_bSound)
        {
            //  �������� ��ʽ��ʼ(Start)
            PlaySound (TEXT (".\\Sounds\\CokucoS.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;
        }
        return 0;


    //  �������С�仯
    case WM_SIZE:
        cxClient = LOWORD (lParam) ;
        cyClient = HIWORD (lParam) ;
        if(cxClient > 275)
        {
            //  ����������Ĵ�С�仯���Ӷ������Ӵ���Ĵ�С
            MoveWindow(hwndTile, 0, 0, 256 + GetSystemMetrics(SM_CXVSCROLL), 480, TRUE);
            MoveWindow(hwndMap, 256 + GetSystemMetrics(SM_CXVSCROLL) + 5, 0, cxClient - 5 - (256 + GetSystemMetrics(SM_CXVSCROLL)), cyClient, TRUE);
        }
        return 0 ;


    //  �����ļ���Ϣ
    case WM_DROPFILES:
        if(! DropMapFile((HDROP)wParam, hwndTile, hwndMap))
            MessageBox(hwnd, "�ļ���ʽ����", "����", MB_ICONERROR);

        break;

    //  �˵���Ϣ
    case WM_COMMAND:
        switch(LOWORD(wParam))
        {

        //  �½�����ʵ�������ô�С��
        case IDM_FILE_NEW:
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_MapSizeDlg), hwnd, (DLGPROC)MapSizeDlgProc);
            return 0;


        //  ��
        case IDM_FILE_OPEN:
            if(FALSE == LoadMapFile(hwndTile, hwndMap))
                MessageBox(hwnd, TEXT("��ȡ�ļ�ʧ�ܣ���"), 0, MB_ICONERROR);

            return 0;


        //  ����
        case IDM_FILE_SAVE:
            if(g_szFileName[0])
            {
                CTileFile tf;
                tf.WriteGroundData(g_szFileName, &g_stGround);
                return 0;
            }


        //  ���Ϊ
        case IDM_FILE_SAVEAS:
            if(FALSE == SaveMapFile())
                MessageBox(hwnd, TEXT("��ͼΪ�գ��޷����棡"), 0, MB_ICONERROR);
            return 0;


        //  ��ȡͼ����Ϣ
        case IDM_FILE_LOADTILEINFO:
            if(IDYES == MessageBox(hwnd, TEXT("�˲�������ԭ�ڴ������ݣ��Ƿ�Ҫ������"), TEXT("ѯ��"), MB_YESNO | MB_ICONQUESTION))
            {
                LoadTileInfo();
                InvalidateRect(hwndTile, 0, TRUE);
            }
            
            return 0;


        //  ����ͼ����Ϣ
        case IDM_FILE_SAVETILEINFO:
            if(IDYES == MessageBox(hwnd, TEXT("�˲���������ԭ�����ݣ��Ƿ�Ҫ������"), TEXT("ѯ��"), MB_YESNO | MB_ICONQUESTION))
                SaveTileInfo();
            
            return 0;


        //  �˳�
        case IDM_FILE_EXIT:
            SendMessage(hwnd, WM_CLOSE, 0, 0);
            return 0;


        //  ���õ�ͼ��С
        case IDM_MAP_SIZE:
            DialogBox(hInstance, MAKEINTRESOURCE(IDD_MapSizeDlg), hwnd, (DLGPROC)MapSizeDlgProc);
            return 0;


        //  ���ò�
        case IDM_MAP_LAYER_ALL:
        case IDM_MAP_LAYER_1:
        case IDM_MAP_LAYER_2:
        case IDM_MAP_LAYER_3:
            SendMessage(hwndMap, WM_SETLAYER, LOWORD(wParam) - IDM_MAP_LAYER_ALL, 0);
            InvalidateRect(hwndMap, 0, TRUE);
            return 0;


        //  ����ͼ
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


        //  ��ͼ���
        case IDM_MAP_CLEAR:
            //  �������� ��ͼ���(Clear)
            //PlaySound (TEXT (".\\Sounds\\CokucoC.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;

            if(0 < g_stGround.mtAreas.GetLineCount() && 0 < g_stGround.mtAreas.GetColumnCount() && 
                IDYES == MessageBox(hwnd, TEXT("�˲��������ȫ�����ݣ��Ƿ�Ҫ������"), TEXT("ѯ��"), MB_YESNO | MB_ICONQUESTION))
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


        //  ��ʾ��ש����
        case IDM_MAP_DRAWINFO:
            g_bDrawInfo = !g_bDrawInfo;
            CheckMenuItem(GetMenu(hwnd), IDM_MAP_DRAWINFO, g_bDrawInfo ? MF_CHECKED : MF_UNCHECKED);
            InvalidateRect(hwndTile, 0, TRUE);
            InvalidateRect(hwndMap , 0, TRUE);
            break;


        //  ��ͼ��Ϣ
        case IDM_MAP_INFO:
            //SaveCsvFile();
            MapInfo(GetParent(hwnd));
            return 0;


        //  ʣ�µľ��Ǹ���ͼ���ˣ�͵��֮�����������޻����
        default:
            //  ����ͼ��
            if(LOWORD(wParam) >= IDM_TILE_01 && LOWORD(wParam) < IDM_TILE_01 + 50)
            {
                //  �������� �л�ͼ��(Tile)
                //PlaySound (TEXT (".\\Sounds\\CokucoT.kmp"), NULL, SND_FILENAME | SND_ASYNC) ;

                SendMessage(hwndTile, WM_CHANGETILE, (LOWORD(wParam) - IDM_TILE_01 + 1), 0);
                InvalidateRect(hwndMap, 0 ,TRUE);
            }
        } // end of switch(LOWORD(wParam))

        return 0;


    case WM_DESTROY:
        if(TRUE == g_bSound)
        {
            //  �������� ��ʽ��ֹ(End)
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
//  ���Ͻǵ�ש�Ӵ��崦��ʽ
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
    static long     xSel, ySel;     //  ѡ�е��кź��к�
    static long     cxSel, cySel;   //  ѡ�е��к��еĿ��
//  static long     nDeltaPerLine, nAccumDelta ; 
    TCHAR           szBuffer[64];

    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    switch(message)
    {

    //  ��שͼ�Ӵ����ʼ��
    case WM_CREATE:
        cxSel = cySel = 1;
        xSel = ySel = g_nSelTile = 0;
        SendMessage(hwnd, WM_CHANGETILE, 1, 0);
        return 0;


    //  ��С�仯
    case WM_SIZE:
        //  ���ô�ֱ�������ķ�Χ
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


    //  �ػ�
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

            //  ��ȡ������λ��
            si.cbSize = sizeof(SCROLLINFO);
            si.fMask  = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &si);

            //  ��ȡλͼ����Ϣ
            GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);
//          BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);

            BitBlt(hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);//��3������ʵ�ʵĻ��� 
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCINVERT);
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcMask , 0, 0, SRCAND);
            BitBlt(hdc, 0, si.nPos * -32, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCINVERT);  //  �ָ�

            DeleteDC(hdcMask);
            DeleteDC(hdcImage);
            DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);
 
            //  ��ʾ��שģʽ
            if(g_bDrawInfo && g_pTileInfo)
            {
                for(long i = si.nPos; i < (si.nPos + 15) && i < bitmap.bmHeight / 32; ++i)
                {
                    for(long j = 0; j < 8; ++j)
                    {
                        if((g_pTileInfo[i * 8 + j] & 0x80) == 0x80)
                        {
                            //  ����棬�˵�ש����������
                            MoveToEx(hdc, (j ) * 32 + 4,     (i - si.nPos) * 32 + 4,     0);
                            LineTo  (hdc, (j  + 1) * 32 - 4, (i - si.nPos + 1) * 32 - 4);
                            MoveToEx(hdc, (j ) * 32 + 4,     (i - si.nPos + 1) * 32 - 4, 0);
                            LineTo  (hdc, (j  + 1) * 32 - 4, (i - si.nPos) * 32 + 4);
                        }
                        else
                        {
                            //  ��Բ����������ȥ
                            Ellipse(hdc, (j ) * 32 + 4, (i - si.nPos) * 32 + 4, 
                                            (j  + 1) * 32 - 4, (i - si.nPos + 1) * 32 - 4);
                        }

                        if((g_pTileInfo[i * 8 + j] & 0x08) == 0x08)
                        {
                            //  �����ߣ���ʾ��������֡��ש�Ŀ�ʼ
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


    //  ���������
    case WM_LBUTTONDOWN:

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        //  ����ʾ��ש��Ϣģʽ���Ƚ�ԭ��ѡ�е������ػ�
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32 + si.nPos;
        cxSel = cySel = 1;

        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  ��ʾ��ש��Ϣģʽ���������Ƿ��������ȥ
        if(g_bDrawInfo && g_pTileInfo)
        {
            //  ����Ƿ��������ȥ��λȡ��
            g_pTileInfo[ySel * 8 + xSel] ^= 0x80;

            SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (xSel + 1) * 32, (ySel - si.nPos + 1) * 32);
            InvalidateRect(hwnd, &rect, TRUE);
        }

        return 0;


    //  �������м�
    case WM_MBUTTONDOWN:

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);

        //  ����ʾ��ש��Ϣģʽ���Ƚ�ԭ��ѡ�е������ػ�
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32 + si.nPos;
        cxSel = cySel = 1;

        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  ��ʾ��ש��Ϣģʽ���������Ƿ����������ŵ�ש
        if(g_bDrawInfo && g_pTileInfo && 0 == (xSel & 3))
        {
            //  ����Ƿ��������ȥ��λȡ��
            g_pTileInfo[ySel * 8 + xSel] ^= 0x08;

            SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (xSel + 1) * 32, (ySel - si.nPos + 1) * 32);
            InvalidateRect(hwnd, &rect, TRUE);
        }

        return 0;


    //  �������Ҽ�
    case WM_RBUTTONDOWN:
//  case WM_LBUTTONUP:

        //  �Ƚ�ԭ��ѡ�е������ػ�
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask  = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        SetRect(&rect, xSel * 32, (ySel - si.nPos) * 32, (cxSel + xSel) * 32, (cySel + ySel - si.nPos) * 32);
        InvalidateRect(hwnd, &rect, TRUE);

        cxSel = LOWORD(lParam) / 32;
        cySel = HIWORD(lParam) / 32 + si.nPos;

        //  ���������λ�����Ҽ�λ�õ��Ҳ࣬Ҫ�������ε�λ��
        //  �Ӷ���֤�����Ͻǵ�λ����ȷ��
        if(cxSel < xSel) { nTemp = cxSel; cxSel = xSel; xSel = nTemp; }
        if(cySel < ySel) { nTemp = cySel; cySel = ySel; ySel = nTemp; }

        cxSel -= (xSel - 1);
        cySel -= (ySel - 1);

        if(cxSel < 0) cxSel--;
        if(cySel < 0) cySel--;
        DrawRect(hwnd, xSel, ySel - si.nPos, cxSel, cySel);

        //  ��ʾ��ש��Ϣģʽ���������Ƿ��������ȥ
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


    //  ����ƶ�
    case WM_MOUSEMOVE:
        //  �ж��ƶ���ʱ���Ƿ�ס���
//        if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        return 0;

/*
    //  ������
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

    //  ��ֱ������
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


    //  �л���ש���
    case WM_CHANGETILE:
        //  Ҫ�л��ı�ź͵�ǰһ�£�ֱ�ӷ��أ������κδ���
        if(g_nSelTile == wParam) return 0;

        //  ��ԭ���Ѿ��򿪵ľ�������ͷ�
        if(g_hBmpImage) DeleteObject(g_hBmpImage);
        if(g_hBmpMask)  DeleteObject(g_hBmpMask);
        //wsprintf(szBuffer, ".\\Graphics\\Bg%03d.kkc", wParam);
        wsprintf(szBuffer, "%s\\Graphics\\Bg%03d.kkc", g_szCurrPath, wParam);

        //  ��ȡ��שͼƬ�ز�
        g_hBmpImage = (HBITMAP)LoadImage(0, szBuffer, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
        if(0 == g_hBmpImage)
        {
            //  ���ز�ʧ��
            MessageBox(hwnd, TEXT("��ȡ�زĿ�ʧ��"), 0, MB_ICONERROR);
            MessageBox(hwnd, szBuffer, 0, MB_OK);
            SendMessage(GetParent(hwnd), WM_CLOSE, 0, 0);
        }

        //  ����ͼ�ز˵��µĲ˵�ѡ����
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_TILE_01 + g_nSelTile - 1, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_TILE_01 + (long)wParam - 1, MF_CHECKED);

        //  ��ͼƬ�زĳɹ�������ͼƬ���
        g_nSelTile = (long)wParam;
        g_stGround.nTile = g_nSelTile;
        GetObject(g_hBmpImage, sizeof(BITMAP), &bitmap);
        g_hBmpMask = CreateBitmap(bitmap.bmWidth, bitmap.bmHeight, 1, 1, 0);
        xSel  = ySel  = 0;  //  ����ѡ�е�ͼ�������ϵ�һ��
        cxSel = cySel = 1;  //  ��С��һ��

        //  ����ͼƬ������ͬ����Ҫ�������ù���������
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


    //  ��ȡѡ�е�ͼ�ر�ţ����Ͻǣ�
    case WM_GETSELTILE:
        return MAKELONG(xSel, ySel);    //  MAKELONG(LOW, HIGH)  (COLUMN, LINE)


    //  ��ȡѡ��ͼ�صĴ�С
    case WM_GETSELSIZE:
        return MAKELONG(cxSel, cySel);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}



//----------------------------------
//  �ұߵ�ͼ�����Ӵ����¼������ʽ
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
    static long     xSel, ySel;             //  ѡ�е��к���
    static long     xSize, ySize;           //  ��ͼ�Ĵ�С��32 x 32��
    static long     cxClient, cyClient;     //  ���ڵĴ�С�����أ�
    static long     nVertPos, nHorzPos;     //  ��ֱ��ˮƽ ������λ��
    static long     nLayer;                 //  ��ǰ�����Ĳ㣨0����ʾȫ���� 1~3 ��ʾ��Ӧ�Ĳ㣩
    Area32x32** pAreas = g_stGround.mtAreas.GetDataRef();

    switch(message)
    {
    //  ��ͼ�����Ӵ����ʼ��
    case WM_CREATE:
        //nLayer = 0;
        SendMessage(hwnd, WM_SETLAYER, nLayer = 0, 0);
        return 0;

    //  ��ͼ�����Ӵ����С�仯���ܸ������С�仯Ӱ�죩
    case WM_SIZE:
        GetClientRect(hwnd, &rect);
        cxClient = rect.right - rect.left;
        cyClient = rect.bottom - rect.top;

        nVertPos = nHorzPos = 0;

        //  ���费ֱ������
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

        //  ����ˮƽ������
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


    //  �ػ�
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
                        //  ֻ��ʾ���㣬��Ե�������ػ�
                        if( pAreas[ySel][xSel].stTile[nLayer - 1].line   >= 0 && pAreas[ySel][xSel].stTile[nLayer - 1].column >= 0)
                        {
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[nLayer - 1].column * 32, pAreas[ySel][xSel].stTile[nLayer - 1].line * 32, SRCCOPY);
                        }
                        break;

                    default:
                        //  ��ʾ��㣬�������һ�㿪ʼ���ƣ�������ʹ��͸������
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
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[1].column * 32, pAreas[ySel][xSel].stTile[1].line * 32, SRCINVERT); //  �ָ�
                        }

                        if(pAreas[ySel][xSel].stTile[2].line   >= 0 && pAreas[ySel][xSel].stTile[2].column >= 0)
                        {
                            //BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCCOPY);
                            BitBlt(hdcMask, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcImage, 0, 0, SRCCOPY);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCINVERT);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcMask , pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCAND);
                            BitBlt(hdc, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, 32, 32, hdcImage, pAreas[ySel][xSel].stTile[2].column * 32, pAreas[ySel][xSel].stTile[2].line * 32, SRCINVERT); //  �ָ�
                        }

                        //  ���Ƹ������Ƿ��������ȥ�ı��
                        if(TRUE == g_bDrawInfo)
                        {
                            if((pAreas[ySel][xSel].nAreaInfo & 0x8000) == 0x8000)
                            {
                                //  ����棬��������ȥ
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 4,     (ySel - nVertPos) * 32 + 4,     0);
                                LineTo  (hdc, (xSel - nHorzPos + 1) * 32 - 4, (ySel - nVertPos + 1) * 32 - 4);
                                MoveToEx(hdc, (xSel - nHorzPos) * 32 + 4,     (ySel - nVertPos + 1) * 32 - 4, 0);
                                LineTo  (hdc, (xSel - nHorzPos + 1) * 32 - 4, (ySel - nVertPos) * 32 + 4);
                            }
                            else
                            {
                                //  ��Բ����������ȥ
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


    //  ����ƶ��¼�
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

        //  ���ƶ���ʱ���ж��Ƿ��������������Ҽ�
        //  ������Ϊ�˷������
        if((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_LBUTTONDOWN, wParam, lParam);
        if((GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0x8000) return SendMessage(hwnd, WM_RBUTTONDOWN, wParam, lParam);
        return 0;


    //  ���������£������ͼ���Ӵ�����ѡ����ͼ�����õ������λ��
    case WM_LBUTTONDOWN:
        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32;
        xSel += nHorzPos;
        ySel += nVertPos;

        //  ��ʾ��ש���ԣ�������Ƿ��������ȥ����ȡ��
        if(g_bDrawInfo && ySel < g_stGround.mtAreas.GetLineCount() && xSel < g_stGround.mtAreas.GetColumnCount())
        {
            pAreas[ySel][xSel].nAreaInfo ^= 0x8000;
            SetRect(&rect, (xSel - nHorzPos) << 5, (ySel - nVertPos) << 5, (xSel - nHorzPos + 1) << 5, (ySel - nVertPos + 1) << 5);
            InvalidateRect(hwnd, &rect, TRUE);
            return 0;
        }

        //  ����ʾ��ש���ԣ�ֱ�����ͼ��
        if(xSel < xSize && ySel < ySize)
        {
            hdc = GetDC(hwnd);
            hdcImage = CreateCompatibleDC(hdc);
            SelectObject(hdcImage, g_hBmpImage);

            //  ���ѡ��ͼ�ؿ��е��к��кţ��Լ�ͼ�ؿ�ѡ�еķ�Χ��С
            nRetSize = (long) SendMessage(GetDlgItem(GetParent(hwnd), IDG_TILE), WM_GETSELSIZE, 0, 0);
            nRetTile = (long) SendMessage(GetDlgItem(GetParent(hwnd), IDG_TILE), WM_GETSELTILE, 0, 0);

            //  ����ȫ����Ҫˢ�µ�ͼ��
            for(i = 0; i < LOWORD(nRetSize); ++i)
            {
                for(j = 0; j < HIWORD(nRetSize); ++j)
                {
                    if(xSel + i < xSize && ySel + j < ySize)
                    {
                        if(1 <= nLayer && nLayer <= 3)
                        {
                            //  ��ӵ�ָ���Ĳ�
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
                                //  ��ѡ��������Ч��������δ����
                                continue;
                            }

                            //  ��ӵ�ϵͳĬ�ϵĲ�
                            //  ���ݶ���õĲ�����������
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

                            //  ���µ�ǰ��ש�Ƿ���Ա�ռ�á�
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

            //  ���Ѿ����ݸ��¹���ͼ�ؾ��η�ΧʧЧ���Ӷ������ڽ�������ʾ����
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
                //  ���Ѿ����ݸ��¹���ͼ�ؾ��η�ΧʧЧ���Ӷ������ڽ�������ʾ����
                SetRect(&rect, (xSel - nHorzPos) << 5, (ySel - nVertPos) << 5, (xSel - nHorzPos + LOWORD(nRetSize)) << 5, (ySel - nVertPos + HIWORD(nRetSize)) << 5);
                InvalidateRect(hwnd, &rect, TRUE);
            }

            DeleteDC(hdcImage);
            ReleaseDC(hwnd, hdc);
        }
        else if(xSize <= 0 || ySize <= 0)
        {
            //  ����û�г�ʼ��������Ҫ��ʼ��
            SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_MAP_SIZE, 0), 0);
        }

        return 0;


    //  ����Ҽ�������ո�ͼ��
    case WM_RBUTTONDOWN:

        xSel = LOWORD(lParam) / 32;
        ySel = HIWORD(lParam) / 32;

        //  �ӽ���������ת��Ϊ��Ӧ����λ�õľ������꣨���Ϲ�������λ�ã�
        xSel += nHorzPos;
        ySel += nVertPos;

        if(xSel < xSize && ySel < ySize)
        {
            GetCursorPos(&pt);
            TrackPopupMenu(GetSubMenu(LoadMenu(0, MAKEINTRESOURCE(IDR_MENU1)), 3), TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, 0);
        }
        return 0;


    //  �Զ�����Ϣ�����õ�ͼ��С 32 x 32
    case WM_SETMAPSIZE:
        ySize = LOWORD(lParam);     //  ����
        xSize = HIWORD(lParam);     //  ����

        //  �������ù�����
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


    //  ���õ�ͼ����ʾ
    case WM_SETLAYER:
        if(wParam >= 1 && wParam <= 3)
        {
            nLayer = (long)wParam;
        }
        else
        {
            nLayer = 0;
        }

        //  ���µ�ͼ�˵��µĵ�ͼ����Ӳ˵��µĲ��ѡ����
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_ALL, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_1, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_2, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_3, MF_UNCHECKED);
        CheckMenuItem(GetMenu(GetParent(hwnd)), IDM_MAP_LAYER_ALL + nLayer, MF_CHECKED);
        return 0;


    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        //  ���ȫ��������
        case IDM_RC_CLEARALL:
        case IDM_RC_CLEARLAYER1:
        case IDM_RC_CLEARLAYER2:
        case IDM_RC_CLEARLAYER3:
            if(xSel < xSize && ySel < ySize)
            {
                //  ����ǵ���һ�㣬�ͶԵ�����������
                //  �������ʾȫ�������ȫ����������
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

                //  ���Ҽ�����������ڽ����Ͻ������
                SetRect(&rect, (xSel - nHorzPos) * 32, (ySel - nVertPos) * 32, (xSel - nHorzPos + 1) * 32, (ySel - nVertPos + 1) * 32);
                InvalidateRect(hwnd, &rect, TRUE);
            }
            else if(xSize <= 0 || ySize <= 0)
            {
                SendMessage(GetParent(hwnd), WM_COMMAND, MAKELONG(IDM_MAP_SIZE, 0), 0);
            }

            return 0;


        //  ���ò�����վ��
        case IDM_RC_NOTSTAND:
            pAreas[ySel][xSel].nAreaInfo |= 0x8000;
            return 0;


        //  ��������վ��
        case IDM_RC_CANSTAND:
            pAreas[ySel][xSel].nAreaInfo &= 0x7FFF;
            return 0;


        //  ����ê��
        case IDM_RC_SETANCHOR:
            pAreas[ySel][xSel].nAreaInfo |= 0x4000;
            pAreas[ySel][xSel].nAnchor = g_nParam1;
            return 0;

        }
        return 0;


    //  ��ֱ��������Ϣ����
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


    //  ˮƽ��������Ϣ����
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
//  ���õ�ͼ��С�Ի������Ϣ����ʽ
//------------------------------------
LRESULT CALLBACK MapSizeDlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
    long xSize, ySize;
    char szBuffer[64];

    switch(message)
    {
    //  �����ʼ��
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
        //  �����ȷ������ť�¼�����
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

        //  �����ȡ������ť�¼������رմ��壩
        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }

    return FALSE;
}



//----------------------------
//  ��ָ�������ϻ��ƿ��ľ���
//----------------------------
//                          ��ͼλ��            ͼ�ش�С
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
//  �����ļ�������
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
//  ��ȡ��Ϸ�ű�
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
    ofn.lpstrFilter     = "�����ļ� (*.dat)\0*.DAT\0ȫ���ļ� (*.*)\0*.*\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(! GetOpenFileName(&ofn))
    {
        //  ���ļ�ȡ�������� TRUE ����ʾû�г���
        return TRUE;
    }

    CTileFile tf;

    if(! tf.ReadDataFile(szFileName, &g_stGround))
    {
        MessageBox(0, "��ȡ����ʧ�ܣ�", 0, MB_ICONERROR);
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
//  ���Ϊ
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
    ofn.lpstrFilter     = "�����ļ� (*.dat)\0*.DAT\0";
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_OVERWRITEPROMPT;

    if(! GetOpenFileName(&ofn))
    {
        //  ���ļ�ȡ�������� TRUE ����ʾû�г���
        return TRUE;
    }

    //  �ж�������ļ�������λ�Ƿ��� .dat��������������
    //  PS��_stricmp ��Сд������
    long nLen = (long)strlen(szFileName);
    if(4 < nLen && _stricmp(szFileName + nLen - 4, ".dat")) 
        strcpy(szFileName, strcat(szFileName, ".dat"));

    //  д�����ļ�
    CTileFile tf;
    if(! tf.WriteGroundData(szFileName, &g_stGround))
    {
        g_szFileName[0] = 0;
        MessageBox(0, "��������д��ʧ�ܣ�", 0, MB_ICONERROR);
        return FALSE;
    }

    //  �����ļ������ļ�·��
    strcpy(g_szFileName, szFileName);
    return TRUE;
}



//-------------------------
//  ����ש��Ϣ�ļ�
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
//  д��ש��Ϣ�ļ�
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
//  ��ȡ��ǰĿ¼
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
        //  �����һ�� \ ����ó� \0 ����
        g_szCurrPath[nPos] = '\0';
    }
}



//-------------------------
//  �ڸ���������ʾ��Ϣ
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
//  ���� CSV �ļ�
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
//  ��һ����ʾ�������ã�
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
    strText += "ͼ�أ�";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", g_nSelTile);
    strText += pBuffer;
    strText += "\n";

    strText += "���֣�";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", g_stGround.nBGM);
    strText += pBuffer;
    strText += + "\n";

    strText += "������";
    memset(pBuffer, 0, 255);
    sprintf(pBuffer, "%ld", nLn);
    strText += pBuffer;
    strText += + "\n";
    
    strText += "������";
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

