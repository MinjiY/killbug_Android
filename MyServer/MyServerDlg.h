
// MyServerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "BugsLife.h"

#define MAX_COUNT    100


struct ClientSocket
{
	SOCKET h_socket;
	char ip_address[16];
};

// CMyServerDlg ��ȭ ����
class CMyServerDlg : public CDialogEx
{
private:
	SOCKET mh_socket;
	ClientSocket m_client_list[MAX_COUNT];
	BugsLife m_bugs;
	char m_start_flag = 0;
	float m_base_z_value = (float)0.0, m_base_x_value = (float)0.0;
	unsigned int m_check_time = 0;
	
	POINT m_cursor_pos;

// �����Դϴ�.
public:
	CMyServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	void AddEvent(CString a_string);
	void SendFrameData(SOCKET ah_socket,
									 unsigned char a_msg_id,
									 const char *ap_data,
									 unsigned short int a_data_size);

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MYSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT On25001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On25002(WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
