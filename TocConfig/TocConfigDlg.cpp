// TocConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TocConfig.h"
#include "TocConfigDlg.h"
#include ".\tocconfigdlg.h"
//#include "CReader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTocConfigDlg 对话框



CTocConfigDlg::CTocConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTocConfigDlg::IDD, pParent)
	, m_dwMapWav(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTocConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbDrawMode);
	DDX_Control(pDX, IDC_COMBO4, m_cmbUp);
	DDX_Control(pDX, IDC_COMBO5, m_cmbDown);
	DDX_Control(pDX, IDC_COMBO6, m_cmbLeft);
	DDX_Control(pDX, IDC_COMBO7, m_cmbRight);
	DDX_Control(pDX, IDC_COMBO8, m_cmbCheck);
	DDX_Control(pDX, IDC_COMBO9, m_cmbEscape);
	DDX_Control(pDX, IDC_COMBO2, m_cmbShowFPS);
	DDX_Control(pDX, IDC_COMBO3, m_cmbMusic);
	DDX_Control(pDX, IDC_BUTTON1, m_btnOK);
	DDX_Control(pDX, IDC_BUTTON2, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CTocConfigDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedSave)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnCbnSelchangeUp)
	ON_CBN_SELCHANGE(IDC_COMBO5, OnCbnSelchangeDown)
	ON_CBN_SELCHANGE(IDC_COMBO6, OnCbnSelchangeLeft)
	ON_CBN_SELCHANGE(IDC_COMBO7, OnCbnSelchangeRight)
	ON_CBN_SELCHANGE(IDC_COMBO8, OnCbnSelchangeCheck)
	ON_CBN_SELCHANGE(IDC_COMBO9, OnCbnSelchangeEscape)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeDrawMode)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeShowFps)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnCbnSelchangeMusic)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedCancel)
END_MESSAGE_MAP()


// CTocConfigDlg 消息处理程序
//	7/12
BOOL CTocConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
/*
	// 将\“关于...\”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
*/
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	char szBufferA[MAX_PATH];
	GetModuleFileName(0, szBufferA, MAX_PATH);
	m_strCfgPath = szBufferA;
	m_strCfgPath = m_strCfgPath.Mid(0, m_strCfgPath.ReverseFind('\\'));

	if("" == m_strCfgPath)
	{
		m_strCfgPath = ".";
	}

	short	shBtnColor = 30;
/*
	m_btnOK.SetIcon(IDI_OK);
	m_btnOK.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	m_btnOK.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(0, 128, 0));

	m_btnCancel.SetIcon(IDI_CANCEL);
	m_btnCancel.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
	m_btnCancel.SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(128, 0, 0));
*/
	CFileFind ff;
	if(!ff.FindFile(m_strCfgPath + "\\Toc.exe"))
	{
		//	没有找到游戏的主程序

		AfxMessageBox("没有找到游戏的主程序。", MB_ICONERROR);
		OnCancel();
	}

	//	保存 游戏配置文件 的绝对路径
	m_strCfgPath += "\\Config.cfg";

	m_dwDrawMode = GetPrivateProfileInt("SCREEN", "DRAWMODE", 1, m_strCfgPath);
	m_dwShowFps  = GetPrivateProfileInt("SCREEN", "SHOWFPS", 0, m_strCfgPath);
	m_dwUp       = GetPrivateProfileInt("KEY", "UP", 38, m_strCfgPath);
	m_dwDown     = GetPrivateProfileInt("KEY", "DOWN", 40, m_strCfgPath);
	m_dwLeft     = GetPrivateProfileInt("KEY", "LEFT", 37, m_strCfgPath);
	m_dwRight    = GetPrivateProfileInt("KEY", "RIGHT", 39, m_strCfgPath);
	m_dwCheck    = GetPrivateProfileInt("KEY", "CHECK", 32, m_strCfgPath);
	m_dwEscape   = GetPrivateProfileInt("KEY", "ESCAPE", 27, m_strCfgPath);
	m_dwMusic	 = GetPrivateProfileInt("SOUND", "MUSIC", 1, m_strCfgPath);
	m_dwMapWav	 = GetPrivateProfileInt("MAPCREATOR", "WAV", 0, m_strCfgPath);

	CString strTemp;

	m_dwDrawMode = (m_dwDrawMode - 1) % 6;
	m_dwShowFps  = m_dwShowFps % 3;
	m_dwMusic	 = !!m_dwMusic;

	m_cmbDrawMode.SetCurSel(m_dwDrawMode);
	m_cmbShowFPS.SetCurSel(m_dwShowFps);
	m_cmbMusic.SetCurSel(m_dwMusic);

	//	显示帧数设置禁用
	m_cmbShowFPS.EnableWindow(FALSE);

	int i;
	//	设置“上”
	for(i = 0; i < m_cmbUp.GetCount(); ++i)
	{
		m_cmbUp.GetLBText(i, strTemp);
		if(m_dwUp == GetKeyCode(strTemp))
		{
			m_cmbUp.SetCurSel(i);
			break;
		}
	}

	//	设置“下”
	for(i = 0; i < m_cmbDown.GetCount(); ++i)
	{
		m_cmbDown.GetLBText(i, strTemp);
		if(m_dwDown == GetKeyCode(strTemp))
		{
			m_cmbDown.SetCurSel(i);
			break;
		}
	}

	//	设置“左”
	for(i = 0; i < m_cmbLeft.GetCount(); ++i)
	{
		m_cmbLeft.GetLBText(i, strTemp);
		if(m_dwLeft == GetKeyCode(strTemp))
		{
			m_cmbLeft.SetCurSel(i);
			break;
		}
	}
	//	设置“右”
	for(i = 0; i < m_cmbRight.GetCount(); ++i)
	{
		m_cmbRight.GetLBText(i, strTemp);
		if(m_dwRight == GetKeyCode(strTemp))
		{
			m_cmbRight.SetCurSel(i);
			break;
		}
	}
	//	设置“调查”
	for(i = 0; i < m_cmbCheck.GetCount(); ++i)
	{
		m_cmbCheck.GetLBText(i, strTemp);
		if(m_dwCheck == GetKeyCode(strTemp))
		{
			m_cmbCheck.SetCurSel(i);
			break;
		}
	}

	//	设置“取消”
	for(i = 0; i < m_cmbEscape.GetCount(); ++i)
	{
		m_cmbEscape.GetLBText(i, strTemp);
		if(m_dwEscape == GetKeyCode(strTemp))
		{
			m_cmbEscape.SetCurSel(i);
			break;
		}
	}
	
	//	向导自动生成
	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

void CTocConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTocConfigDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR CTocConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//根据字符串获得相应的虚拟键码
DWORD CTocConfigDlg::GetKeyCode(CString strKey)
{
	if(strKey == "-")
	{
		return '-';
	}
	else if(strKey == "=")
	{
		return '=';
	}
	else if(strKey == "Backspace")
	{
		return VK_BACK;
	}
	else if(strKey == "Escape")
	{
		return VK_ESCAPE;
	}
	else if(strKey == "Tab")
	{
		return VK_TAB;
	}
	else if(strKey == "Q")
	{
		return 0x51;
	}
	else if(strKey == "W")
	{
		return 0x57;
	}
	else if(strKey == "E")
	{
		return 0x45;
	}
	else if(strKey == "R")
	{
		return 0x52;
	}
	else if(strKey == "T")
	{
		return 0x54;
	}
	else if(strKey == "Y")
	{
		return 0x59;
	}
	else if(strKey == "U")
	{
		return 0x55;
	}
	else if(strKey == "I")
	{
		return 0x49;
	}
	else if(strKey == "O")
	{
		return 0x4F;
	}
	else if(strKey == "P")
	{
		return 0x50;
	}
	else if(strKey == "[")
	{
		return '[';
	}
	else if(strKey == "]")
	{
		return ']';
	}
	else if(strKey == "Enter")
	{
		return VK_RETURN;
	}
	else if(strKey == "A")
	{
		return 0x41;
	}
	else if(strKey == "S")
	{
		return 0x53;
	}
	else if(strKey == "D")
	{
		return 0x44;
	}
	else if(strKey == "F")
	{
		return 0x46;
	}
	else if(strKey == "G")
	{
		return 0x47;
	}
	else if(strKey == "H")
	{
		return 0x48;
	}
	else if(strKey == "J")
	{
		return 0x4A;
	}
	else if(strKey == "K")
	{
		return 0x4B;
	}
	else if(strKey == "L")
	{
		return 0x4C;
	}
	else if(strKey == "Left Shift")
	{
		return VK_LSHIFT;
	}
	else if(strKey == "Z")
	{
		return 0x5A;
	}
	else if(strKey == "X")
	{
		return 0x58;
	}
	else if(strKey == "C")
	{
		return 0x43;
	}
	else if(strKey == "V")
	{
		return 0x56;
	}
	else if(strKey == "B")
	{
		return 0x42;
	}
	else if(strKey == "N")
	{
		return 0x4E;
	}
	else if(strKey == "M")
	{
		return 0x4D;
	}
	else if(strKey == ",")
	{
		return ',';
	}
	else if(strKey == ".")
	{
		return '.';
	}
	else if(strKey == "Right Shift")
	{
		return VK_RSHIFT;
	}
	else if(strKey == "Space")
	{
		return VK_SPACE;
	}
	else if(strKey == "Numpad *")
	{
		return VK_MULTIPLY;
	}
	else if(strKey == "Numpad 7")
	{
		return VK_NUMPAD7;
	}
	else if(strKey == "Numpad 8")
	{
		return VK_NUMPAD8;
	}
	else if(strKey == "Numpad 9")
	{
		return VK_NUMPAD9;
	}
	else if(strKey == "Numpad -")
	{
		return VK_SUBTRACT;
	}
	else if(strKey == "Numpad 4")
	{
		return VK_NUMPAD4;
	}
	else if(strKey == "Numpad 5")
	{
		return VK_NUMPAD5;
	}
	else if(strKey == "Numpad 6")
	{
		return VK_NUMPAD6;
	}
	else if(strKey == "Numpad 1")
	{
		return VK_NUMPAD1;
	}
	else if(strKey == "Numpad 2")
	{
		return VK_NUMPAD2;
	}
	else if(strKey == "Numpad 3")
	{
		return VK_NUMPAD3;
	}
	else if(strKey == "Numpad 0")
	{
		return VK_NUMPAD0;
	}
	else if(strKey == "Numpad .")
	{
		return VK_DECIMAL;
	}
	else if(strKey == "Up Arrow")
	{
		return VK_UP;
	}
	else if(strKey == "Down Arrow")
	{
		return VK_DOWN;
	}
	else if(strKey == "Left Arrow")
	{
		return VK_LEFT;
	}
	else if(strKey == "Right Arrow")
	{
		return VK_RIGHT;
	}
	else if(strKey == "Home")
	{
		return VK_HOME;
	}
	else if(strKey == "End")
	{
		return VK_END;
	}
	else if(strKey == "PgUp")
	{
		return VK_PRIOR;
	}
	else if(strKey == "PgDn")
	{
		return VK_NEXT;
	}
	else if(strKey == "Insert")
	{
		return VK_INSERT;
	}
	else if(strKey == "Delete")
	{
		return VK_DELETE;
	}

	return 0;
}

void CTocConfigDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strMsg;
	CString strTemp;

	strMsg += "//  游戏的设置\r\n";
	strMsg += "//  表乱改\r\n";
	strMsg += "\r\n";
	strMsg += "[SCREEN]\r\n";

	strTemp.Format("DRAWMODE = %d", m_dwDrawMode + 1);
	strMsg += strTemp + "\r\n";
	strTemp.Format("SHOWFPS = %d", this->m_dwShowFps);
	strMsg += strTemp + "\r\n\r\n";

	strMsg += "[KEY]\r\n";

	strTemp.Format("UP = %d", m_dwUp);
	strMsg += strTemp + "\r\n";
	strTemp.Format("DOWN = %d", m_dwDown);
	strMsg += strTemp + "\r\n";
	strTemp.Format("LEFT = %d", m_dwLeft);
	strMsg += strTemp + "\r\n";
	strTemp.Format("RIGHT = %d", m_dwRight);
	strMsg += strTemp + "\r\n";
	strTemp.Format("CHECK = %d", m_dwCheck);
	strMsg += strTemp + "\r\n";
	strTemp.Format("ESCAPE = %d", m_dwEscape);
	strMsg += strTemp + "\r\n\r\n";

	strMsg += "[SOUND]\r\n";
	strTemp.Format("MUSIC = %d", m_dwMusic);
	//strMsg += m_dwMusic == 0 ? "MUSIC = ON" : "MUSIC = OFF";
	strMsg += strTemp;
	strMsg += "\r\n\r\n";

	strMsg += "[MAPCREATOR]\r\n";
	strTemp.Format("WAV = %d", m_dwMapWav);
	strMsg += strTemp + "\r\n\r\n";

	strMsg += "[MODIFY]\r\n";

	CTime time;
	time = time.GetCurrentTime();
	strTemp.Format("DATE = %d-%02d-%02d", time.GetYear(), time.GetMonth(), time.GetDay());
	strMsg += strTemp + "\r\n";
	strTemp.Format("TIME = %02d:%02d:%02d", time.GetHour(), time.GetMinute(), time.GetSecond());
	strMsg += strTemp + "\r\n";
	strMsg += "VER = 0.01\r\n";

//	AfxMessageBox(strMsg);

	CFile file;
	if(file.Open(m_strCfgPath, CFile::modeCreate | CFile::modeWrite))
	{
		file.Write(strMsg.GetBuffer(), strMsg.GetLength());
		file.Close();
		OnOK();
	}
	else
	{
		AfxMessageBox("写入配置文件错误。", MB_ICONERROR);
	}
}

void CTocConfigDlg::OnCbnSelchangeUp()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strText;

	m_cmbUp.GetWindowText(strText);
	DWORD dwUp = GetKeyCode(strText);

	if(dwUp > 0)
	{
		m_dwUp = dwUp;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeDown()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_cmbDown.GetWindowText(strText);
	DWORD dwDown = GetKeyCode(strText);

	if(dwDown > 0)
	{
		m_dwDown = dwDown;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeLeft()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_cmbLeft.GetWindowText(strText);
	DWORD dwLeft = GetKeyCode(strText);

	if(dwLeft > 0)
	{
		m_dwLeft = dwLeft;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeRight()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_cmbRight.GetWindowText(strText);
	DWORD dwRight = GetKeyCode(strText);

	if(dwRight > 0)
	{
		m_dwRight = dwRight;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_cmbCheck.GetWindowText(strText);
	DWORD dwCheck = GetKeyCode(strText);

	if(dwCheck > 0)
	{
		m_dwCheck = dwCheck;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeEscape()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strText;

	m_cmbEscape.GetWindowText(strText);
	DWORD dwEscape = GetKeyCode(strText);

	if(dwEscape > 0)
	{
		m_dwEscape = dwEscape;
	}
	m_btnOK.EnableWindow(KeyInvalid());
}

void CTocConfigDlg::OnCbnSelchangeDrawMode()
{
	// TODO: 在此添加控件通知处理程序代码

	m_dwDrawMode = m_cmbDrawMode.GetCurSel();
}

void CTocConfigDlg::OnCbnSelchangeShowFps()
{
	// TODO: 在此添加控件通知处理程序代码

	m_dwShowFps = m_cmbShowFPS.GetCurSel();
}

void CTocConfigDlg::OnCbnSelchangeMusic()
{
	// TODO: 在此添加控件通知处理程序代码

	m_dwMusic = m_cmbMusic.GetCurSel();
}

BOOL CTocConfigDlg::KeyInvalid(void)
{
	int nSel[6];

	nSel[0] = m_cmbUp.GetCurSel();
	nSel[1] = m_cmbDown.GetCurSel();
	nSel[2] = m_cmbLeft.GetCurSel();
	nSel[3] = m_cmbRight.GetCurSel();
	nSel[4] = m_cmbCheck.GetCurSel();
	nSel[5] = m_cmbEscape.GetCurSel();

	for(int i = 0; i < 6; ++i)
	{
		for(int j = i + 1; j < 6; ++j)
		{
			if(nSel[i] == nSel[j])
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//	7/12
void CTocConfigDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
