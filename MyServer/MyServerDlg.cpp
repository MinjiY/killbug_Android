
// MyServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyServerDlg 대화 상자



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


// CMyServerDlg 메시지 처리기

BOOL CMyServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("192.168.0.135");
	srv_addr.sin_port = htons(25790);

	bind(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));

	listen(mh_socket, 1);

	WSAAsyncSelect(mh_socket, m_hWnd, 25001, FD_ACCEPT);


	AddEvent("서버 서비스를 시작합니다.");

	ShowWindow(SW_SHOWMAXIMIZED);

	//여기부터 
	CRect r;
	GetClientRect(r);

	m_bugs.SetArea(r.Width(), r.Height());

	SetTimer(1, 50, NULL);
	SetTimer(2, 1000, NULL);
	//여기까지 추가
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//그리는 부분을 추가해준다
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

		//스코어 출력 추가
		CString str;
		str.Format("score = %d", m_bugs.GetScore());
		dc.TextOut(0, 0, str);

		//마우스 포인팅 변경, 십자가 모양 타겟팅
		line_pen.CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
		CPen *p_old_pen = dc.SelectObject(&line_pen);

		dc.MoveTo(m_cursor_pos.x, m_cursor_pos.y - 100);//세로
		dc.LineTo(m_cursor_pos.x, m_cursor_pos.y + 100);
		dc.MoveTo(m_cursor_pos.x - 100, m_cursor_pos.y);//가로
		dc.LineTo(m_cursor_pos.x + 100, m_cursor_pos.y);

		dc.SelectObject(p_old_pen);//펜 백업
		line_pen.DeleteObject();//펜 제어

		//CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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

		CString str = "새로운 사용자가 접속했습니다 : ";
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

					AddEvent(str);//문자열 출력

					for (int i = 0; i < MAX_COUNT; i++) {//다시 핸드폰으로 전송??
						if (m_client_list[i].h_socket != INVALID_SOCKET) {
							SendFrameData(m_client_list[i].h_socket,
								1, str, str.GetLength() + 1);
						}
					}

					//if ("0" == p_body_data) {//슬라이드 쇼 시작
					//	keybd_event(VK_DOWN, VK_DOWN, 0, 0);
					//	keybd_event(VK_DOWN, VK_DOWN, KEYEVENTF_KEYUP, 0);
					//}

					//HWND h_find_wnd = ::FindWindow("POWERPNT", NULL); //FindWindow 부모클래스 까지 를 찾는다
					//if (h_find_wnd != NULL) {
					//	HWND h_edit_wnd = ::FindWindowExA(h_find_wnd, NULL, "Edit", NULL);//부모클래스,이미 찾은 자식 클래스,클래스명으로 찾을 때, 캡션으로 찾을 때
					//	if (h_edit_wnd != NULL) {
					//		::PostMessage(h_edit_wnd, WM_KEYDOWN, 0x00000041, 0x001E0001);
					//		::PostMessage(h_edit_wnd, WM_KEYUP, 0x00000041, 0xC01E0001);
					//	}
					//}


				

				}
				else if (msg_id == 2) {
					switch (*(int *)p_body_data) {
						case 0: // 슬라이드 쇼 시작
							keybd_event(VK_F5, 0x3F, 0, 0);
							keybd_event(VK_F5, 0x3F, KEYEVENTF_KEYUP, 0);
							break;
						case 1: // 슬라이드 쇼 종료
							keybd_event(VK_ESCAPE, 1, 0, 0);
							keybd_event(VK_ESCAPE, 1, KEYEVENTF_KEYUP, 0);
							break;
						case 2: // 다음 페이지
							keybd_event(VK_LEFT, 0x4B, KEYEVENTF_EXTENDEDKEY, 0);
							keybd_event(VK_LEFT, 0x4B, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
							break;
						case 3: // 이전 페이지
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
						float z_value = p_sensor_data[0] - m_base_z_value;//자이로 센서의 z축(z_value)이 화면상의 x값(p_sensor_data[0])
																			//현재 센서의 x값 - 처음 잡힌 센서의 x값(기준값) = 좌표?
						if (z_value > 180.0f) z_value -= 360.0f;//0~360도 사이의 값을 -180~180사이의 값으로 조정
						if (z_value < -180.0f) z_value += 360.0f;

						float x_value = p_sensor_data[1] - m_base_x_value;//자이로 센서의 x축(x_value)이 화면상의 y값(p_sensor_data[1])
						if (x_value > 180.0f) x_value -= 360.0f;
						if (x_value < -180.0f) x_value += 360.0f;

						//int cursor_pos_x, cursor_pos_y;//멤버 변수로 선언
						int center_x_pos = 1280, center_y_pos = 800;//화면의 중심점, 현재 컴퓨터의 해상도 기준

						// z_value : 40 = X : center_x_pos
						// 40 : center_x_pos = z_value : X
						// X = center_x_pos * z_value / 40
						m_cursor_pos.x = center_x_pos + center_x_pos * z_value / 20;
						m_cursor_pos.y = center_y_pos + center_y_pos * x_value / 10;

						ScreenToClient(&m_cursor_pos);//전체화면 좌표를 윈도우 좌표로 바꿔준다
						//기존에 사용하던 setcursor는 삭제
						//SetCursorPos(cursor_pos_x, cursor_pos_y);
					}
					else {
						if (m_check_time == 0) {//시작하자마자 ~2초동안은 센서안정화를 위해 센서가 작동하지 않도록 설정
							m_check_time = GetTickCount();

						}
						else if (GetTickCount() - m_check_time > 2000) {//2초가 지난 후 부터 작동
							m_base_z_value = p_sensor_data[0];//센서의 처음 x값
							m_base_x_value = p_sensor_data[1];//센서의 처음 y값

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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bugs.Check(point.x, point.y);

	CDialogEx::OnLButtonDown(nFlags, point);
}
