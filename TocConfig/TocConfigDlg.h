// TocConfigDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CTocConfigDlg �Ի���
class CTocConfigDlg : public CDialog
{
// ����
public:
	CTocConfigDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TOCCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:

	CComboBox m_cmbDrawMode;
	CComboBox m_cmbUp;
	CComboBox m_cmbDown;
	CComboBox m_cmbLeft;
	CComboBox m_cmbRight;
	CComboBox m_cmbCheck;
	CComboBox m_cmbEscape;
	CComboBox m_cmbShowFPS;
	CComboBox m_cmbMusic;

	CButton m_btnOK;
	CButton m_btnCancel;
/*
	CComboBoxXP m_cmbDrawMode;
	CComboBoxXP m_cmbUp;
	CComboBoxXP m_cmbDown;
	CComboBoxXP m_cmbLeft;
	CComboBoxXP m_cmbRight;
	CComboBoxXP m_cmbCheck;
	CComboBoxXP m_cmbEscape;
	CComboBoxXP m_cmbShowFPS;
	CComboBoxXP m_cmbMusic;

	CButtonST m_btnOK;
	CButtonST m_btnCancel;
*/

	DWORD m_dwUp;
	DWORD m_dwDown;
	DWORD m_dwLeft;
	DWORD m_dwRight;
	DWORD m_dwEscape;
	DWORD m_dwCheck;

	DWORD m_dwDrawMode;
	DWORD m_dwShowFps;
	DWORD m_dwMusic;
	DWORD m_dwMapWav;

	CString m_strCfgPath;
	BOOL KeyInvalid(void);
	DWORD GetKeyCode(CString strKey);

public:
	afx_msg void OnBnClickedSave();
	afx_msg void OnCbnSelchangeUp();
	afx_msg void OnCbnSelchangeDown();
	afx_msg void OnCbnSelchangeLeft();
	afx_msg void OnCbnSelchangeRight();
	afx_msg void OnCbnSelchangeCheck();
	afx_msg void OnCbnSelchangeEscape();
	afx_msg void OnCbnSelchangeDrawMode();
	afx_msg void OnCbnSelchangeShowFps();
	afx_msg void OnCbnSelchangeMusic();
	afx_msg void OnBnClickedCancel();
private:
};
