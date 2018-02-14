
// MyServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyServerDlg ��ȭ ����



CMyServerDlg::CMyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMyServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for(int i = 0; i < MAX_COUNT; i++) {
		m_client_list[i].h_socket = INVALID_SOCKET;
	}

	m_cursor_pos.x = 0;
	m_cursor_pos.y = 0;
}

void CMyServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_event_list);
}

BEGIN_MESSAGE_MAP(CMyServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CMyServerDlg::On25001)
	ON_MESSAGE(25002, &CMyServerDlg::On25002)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMyServerDlg �޽��� ó����

BOOL CMyServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("192.168.0.135");
	srv_addr.sin_port = htons(25790);

	bind(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));

	listen(mh_socket, 1);

	WSAAsyncSelect(mh_socket, m_hWnd, 25001, FD_ACCEPT);


	AddEvent("���� ���񽺸� �����մϴ�.");

	ShowWindow(SW_SHOWMAXIMIZED);

	//������� 
	CRect r;
	GetClientRect(r);

	m_bugs.SetArea(r.Width(), r.Height());

	SetTimer(1, 50, NULL);
	SetTimer(2, 1000, NULL);
	//������� �߰�
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//�׸��� �κ��� �߰����ش�
		CPaintDC dc(this);

		CPen line_pen;
		CBrush ellipse_brush;
		Bug *p_pos;
		int count = m_bugs.GetCount(), bug_size;
		const T_COLOR *p_color;

		for (int i = 0; i < count; i++) {
			p_pos = m_bugs.GetBug(i);
			p_color = p_pos->GetColor();
			line_pen.CreatePen(PS_SOLID, 1, RGB(p_color->red, p_color->green, p_color->blue));
			ellipse_brush.CreateSolidBrush(RGB(p_color->red, p_color->green, p_color->blue));

			CPen *p_old_pen = dc.SelectObject(&line_pen);
			CBrush *p_old_brush = dc.SelectObject(&ellipse_brush);

			//dc.MoveTo(p_pos->GetStartPos()->x, p_pos->GetStartPos()->y);
			//dc.LineTo(p_pos->GetEndPos()->x, p_pos->GetEndPos()->y);

			bug_size = p_pos->GetSize() / 2;
			dc.Ellipse(p_pos->GetPosX() - bug_size,
				p_pos->GetPosY() - bug_size,
				p_pos->GetPosX() + bug_size,
				p_pos->GetPosY() + bug_size);

			dc.SelectObject(p_old_pen);
			dc.SelectObject(p_old_brush);

			line_pen.DeleteObject();
			ellipse_brush.DeleteObject();
			p_pos++;

		}

		//���ھ� ��� �߰�
		CString str;
		str.Format("score = %d", m_bugs.GetScore());
		dc.TextOut(0, 0, str);

		//���콺 ������ ����, ���ڰ� ��� Ÿ����
		line_pen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		CPen *p_old_pen = dc.SelectObject(&line_pen);

		dc.MoveTo(m_cursor_pos.x, m_cursor_pos.y - 100);//����
		dc.LineTo(m_cursor_pos.x, m_cursor_pos.y + 100);
		dc.MoveTo(m_cursor_pos.x - 100, m_cursor_pos.y);//����
		dc.LineTo(m_cursor_pos.x + 100, m_cursor_pos.y);

		dc.SelectObject(p_old_pen);//�� ���
		line_pen.DeleteObject();//�� ����

		//CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CMyServerDlg::On25001(WPARAM wParam, LPARAM lParam)
{
	SOCKET h_socket = (SOCKET)wParam;

	sockaddr_in client_addr;
	int sockaddr_in_size = sizeof(sockaddr_in), i;

	for(i = 0; i < MAX_COUNT; i++) {
		if(m_client_list[i].h_socket == INVALID_SOCKET) break;
	}
	
	if(i < MAX_COUNT) {
		m_client_list[i].h_socket = accept(h_socket,
									 (LPSOCKADDR)&client_addr,
									 &sockaddr_in_size);
		strcpy(m_client_list[i].ip_address, 
					inet_ntoa(client_addr.sin_addr));

		CString str = "���ο� ����ڰ� �����߽��ϴ� : ";
		str += m_client_list[i].ip_address;
		AddEvent(str);

		WSAAsyncSelect(m_client_list[i].h_socket,
					   m_hWnd, 25002, FD_READ | FD_CLOSE);

	} else {
		closesocket(h_socket);
	}



	return 0;
}

void CMyServerDlg::AddEvent(CString a_string)
{
	while(m_event_list.GetCount() > 1000) {
		m_event_list.DeleteString(0);
	}

	int index = m_event_list.InsertString(-1, a_string);
	m_event_list.SetCurSel(index);
}

void CMyServerDlg::SendFrameData(SOCKET ah_socket,
								 unsigned char a_msg_id,
								 const char *ap_data,
								 unsigned short int a_data_size)
{
	int send_data_size = a_data_size + 4;
	char *p_send_data = new char[send_data_size];

	*p_send_data = 27;
	*(p_send_data + 1) = a_msg_id;
	*(unsigned short int *)(p_send_data + 2) = a_data_size;
	memcpy(p_send_data + 4, ap_data, a_data_size);
	send(ah_socket, p_send_data, send_data_size, 0);
	delete[] p_send_data;
}


afx_msg LRESULT CMyServerDlg::On25002(WPARAM wParam, LPARAM lParam)
{
	ClientSocket *p_user;

	for(int i = 0; i < MAX_COUNT; i++) {
		if(m_client_list[i].h_socket == wParam) {
			p_user = &m_client_list[i];
			break;
		}
	}

	if(WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(p_user->h_socket,
							   m_hWnd, 25002, FD_CLOSE);
		unsigned char check, msg_id;
		unsigned short int body_size;

		recv(p_user->h_socket, (char *)&check, 1, 0);
		if (check == 27) {
			recv(p_user->h_socket, (char *)&msg_id, 1, 0);
			recv(p_user->h_socket, (char *)&body_size, 2, 0);

			if (body_size > 0) {
				char *p_body_data = new char[body_size];
				recv(p_user->h_socket, p_body_data, body_size, 0);
				if (msg_id == 1) {

					CString str;
					str = p_user->ip_address;
					str += " : ";
					str += p_body_data;

					AddEvent(str);//���ڿ� ���

					for (int i = 0; i < MAX_COUNT; i++) {//�ٽ� �ڵ������� ����??
						if (m_client_list[i].h_socket != INVALID_SOCKET) {
							SendFrameData(m_client_list[i].h_socket,
								1, str, str.GetLength() + 1);
						}
					}

					//if ("0" == p_body_data) {//�����̵� �� ����
					//	keybd_event(VK_DOWN, VK_DOWN, 0, 0);
					//	keybd_event(VK_DOWN, VK_DOWN, KEYEVENTF_KEYUP, 0);
					//}

					//HWND h_find_wnd = ::FindWindow("POWERPNT", NULL); //FindWindow �θ�Ŭ���� ���� �� ã�´�
					//if (h_find_wnd != NULL) {
					//	HWND h_edit_wnd = ::FindWindowExA(h_find_wnd, NULL, "Edit", NULL);//�θ�Ŭ����,�̹� ã�� �ڽ� Ŭ����,Ŭ���������� ã�� ��, ĸ������ ã�� ��
					//	if (h_edit_wnd != NULL) {
					//		::PostMessage(h_edit_wnd, WM_KEYDOWN, 0x00000041, 0x001E0001);
					//		::PostMessage(h_edit_wnd, WM_KEYUP, 0x00000041, 0xC01E0001);
					//	}
					//}


				

				}
				else if (msg_id == 2) {
					switch (*(int *)p_body_data) {
						case 0: // �����̵� �� ����
							keybd_event(VK_F5, 0x3F, 0, 0);
							keybd_event(VK_F5, 0x3F, KEYEVENTF_KEYUP, 0);
							break;
						case 1: // �����̵� �� ����
							keybd_event(VK_ESCAPE, 1, 0, 0);
							keybd_event(VK_ESCAPE, 1, KEYEVENTF_KEYUP, 0);
							break;
						case 2: // ���� ������
							keybd_event(VK_LEFT, 0x4B, KEYEVENTF_EXTENDEDKEY, 0);
							keybd_event(VK_LEFT, 0x4B, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
							break;
						case 3: // ���� ������
							keybd_event(VK_RIGHT, 0x4D, KEYEVENTF_EXTENDEDKEY, 0);
							keybd_event(VK_RIGHT, 0x4D, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
							break;
						default:
							PostMessage(WM_LBUTTONDOWN, 0, MAKELPARAM(m_cursor_pos.x, m_cursor_pos.y));
							break;
					}

				}
				else if (msg_id == 22) {
					float *p_sensor_data = (float *)p_body_data;
					CString str;
					str.Format("%8.4f, %8.4f, %8.4f", p_sensor_data[0], p_sensor_data[1], p_sensor_data[2]);
					
					if (m_start_flag) {
						float z_value = p_sensor_data[0] - m_base_z_value;//���̷� ������ z��(z_value)�� ȭ����� x��(p_sensor_data[0])
																			//���� ������ x�� - ó�� ���� ������ x��(���ذ�) = ��ǥ?
						if (z_value > 180.0f) z_value -= 360.0f;//0~360�� ������ ���� -180~180������ ������ ����
						if (z_value < -180.0f) z_value += 360.0f;

						float x_value = p_sensor_data[1] - m_base_x_value;//���̷� ������ x��(x_value)�� ȭ����� y��(p_sensor_data[1])
						if (x_value > 180.0f) x_value -= 360.0f;
						if (x_value < -180.0f) x_value += 360.0f;

						//int cursor_pos_x, cursor_pos_y;//��� ������ ����
						int center_x_pos = 1280, center_y_pos = 800;//ȭ���� �߽���, ���� ��ǻ���� �ػ� ����

						// z_value : 40 = X : center_x_pos
						// 40 : center_x_pos = z_value : X
						// X = center_x_pos * z_value / 40
						m_cursor_pos.x = center_x_pos + center_x_pos * z_value / 20;
						m_cursor_pos.y = center_y_pos + center_y_pos * x_value / 10;

						ScreenToClient(&m_cursor_pos);//��üȭ�� ��ǥ�� ������ ��ǥ�� �ٲ��ش�
						//������ ����ϴ� setcursor�� ����
						//SetCursorPos(cursor_pos_x, cursor_pos_y);
					}
					else {
						if (m_check_time == 0) {//�������ڸ��� ~2�ʵ����� ��������ȭ�� ���� ������ �۵����� �ʵ��� ����
							m_check_time = GetTickCount();

						}
						else if (GetTickCount() - m_check_time > 2000) {//2�ʰ� ���� �� ���� �۵�
							m_base_z_value = p_sensor_data[0];//������ ó�� x��
							m_base_x_value = p_sensor_data[1];//������ ó�� y��

							m_start_flag = 1;
						}
					}
					//AddEvent(str);
				}
				delete[] p_body_data;
			}

			WSAAsyncSelect(p_user->h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
		}
	} else {
		closesocket(p_user->h_socket); // m_client_list[i].h_socket
		p_user->h_socket = INVALID_SOCKET;
	}

	return 0;
}


void CMyServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (1 == nIDEvent) {
		m_bugs.Move();
		Invalidate();
	}
	else if (2 == nIDEvent) {
		if (0 == m_bugs.Add()) KillTimer(2);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMyServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_bugs.Check(point.x, point.y);

	CDialogEx::OnLButtonDown(nFlags, point);
}
