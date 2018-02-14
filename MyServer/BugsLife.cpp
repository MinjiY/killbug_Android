#include "stdafx.h"

#include "BugsLife.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define T_RGB(r,g,b)          ((unsigned int)(((unsigned char)(r)|((unsigned short int)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))

#define PLATFORM_ANDROID   0
#define PLATFORM_WIN32     1


Bug::Bug()
{
	m_s_pos.x = 0;
	m_s_pos.y = 0;
	m_e_pos.x = 0;
	m_e_pos.y = 0;

	m_a = (float)0;
	m_b = (float)0;

	m_x = 0;
	m_y = 0;
	m_size = 0;

	m_color.red = 0;
	m_color.green = 0;
	m_color.blue = 0;
}

Bug::~Bug()
{

}

void Bug::MakeBugPos(T_POINT *a_pos, int a_side, int a_width, int a_height)
{
	if (a_side == 0) {
		a_pos->x = rand() % (a_width - 100) + 50;
		a_pos->y = 0;
	}
	else if (a_side == 2) {
		a_pos->x = rand() % (a_width - 100) + 50;
		a_pos->y = a_height;
	}
	else if (a_side == 1) {
		a_pos->x = a_width;
		a_pos->y = rand() % (a_height - 100) + 50;
	}
	else if (a_side == 3) {
		a_pos->x = 0;
		a_pos->y = rand() % (a_height - 100) + 50;
	}
}

void Bug::MakeBug(int a_width, int a_height)
{
	int first_side = rand() % 4, second_side; // 0 ~ 3

	MakeBugPos(&m_s_pos, first_side, a_width, a_height);

	do {
		second_side = rand() % 4;
	} while (first_side == second_side);

	MakeBugPos(&m_e_pos, second_side, a_width, a_height);

	if (m_e_pos.x == m_s_pos.x) m_e_pos.x += 30;

	m_color.red = rand() % 256;
	m_color.green = rand() % 256;
	m_color.blue = rand() % 256;

	m_size = rand() % 40 + 20; // 20 ~ 60

	m_x = m_s_pos.x;
	m_y = 0;

	m_a = float(m_e_pos.y - m_s_pos.y) / (m_e_pos.x - m_s_pos.x);
	m_b = m_s_pos.y - m_a*m_s_pos.x;
	Move();
}

int Bug::Move()
{
	int end_flag = 0;
	if (m_e_pos.x > m_s_pos.x) {
		m_x++;
		if (m_x >= m_e_pos.x)  end_flag++;
	}
	else {
		m_x--;
		if (m_x <= m_e_pos.x) end_flag++;
	}

	if(!end_flag) m_y = (int)(m_a*m_x + m_b);   // y = ax + b

	return end_flag;
}

int Bug::Check(int a_x, int a_y)
{
	return int(sqrt((m_x - a_x)*(m_x - a_x) + (m_y - a_y)*(m_y - a_y))) <= (m_size/2);
}

const T_POINT *Bug::GetStartPos()
{
	return &m_s_pos;
}

const T_POINT *Bug::GetEndPos()
{
	return &m_e_pos;
}

int Bug::GetPosX()
{
	return m_x;
}

int Bug::GetPosY()
{
	return m_y;
}

int Bug::GetSize()
{
	return m_size;
}

const T_COLOR *Bug::GetColor()
{
	return &m_color;
}

int Bug::GetBugScore()
{
	int score = int(atan(m_a) * 180 / 3.141592);
	if (score < 0) score *= -1;
	score += (60 - m_size)*3 + 1;
	return score;
}


BugsLife::BugsLife()
{
#if PLATFORM == PLATFORM_WIN32
	static char info[] = "Platform for Win32";
#elif PLATFORM == PLATFORM_ANDROID
	static char info[] = "Platform for Android";
#else
	static char info[] = "Undefined platform";
#endif

	srand((unsigned int)time(NULL));

	m_width = 0;
	m_height = 0;
	m_bug_count = 0;
	m_score = 0;

}

BugsLife::~BugsLife()
{

}

void BugsLife::SetArea(int a_width, int a_height)
{
	m_width = a_width;
	m_height = a_height;
}

void BugsLife::ResetGame()
{
	m_bug_count = 0;
	m_score = 0;
}

int BugsLife::Add()
{
	if (m_bug_count < MAX_BUG_COUNT) {
		m_bug_list[m_bug_count].MakeBug(m_width, m_height);
		m_bug_count++;
		return 1;
	}
	return 0;
}

int BugsLife::GetScore()
{
	return m_score;
}

void BugsLife::Move()
{
	Bug *p_pos = m_bug_list;
	for (int i = 0; i < m_bug_count; i++) {
		if (p_pos->Move() == 1) {
			// 벌래가 밖으로 나간 경우
			m_score -= 5;
			p_pos->MakeBug(m_width, m_height);
		}
		p_pos++;
	}
}

void BugsLife::Check(int a_x, int a_y)
{
	int find_flag = 0;
	Bug *p_pos = m_bug_list;
	for (int i = 0; i < m_bug_count; i++) {
		if (p_pos->Check(a_x, a_y) == 1) {
			// 벌래를 클릭한 경우
			m_score += 10 + p_pos->GetBugScore();
			find_flag = 1;
			p_pos->MakeBug(m_width, m_height);
		}
		p_pos++;
	}

	if(find_flag == 0) m_score -= 10;
}

int BugsLife::GetCount()
{
	return m_bug_count;
}

Bug *BugsLife::GetBug(int a_index)
{
	return m_bug_list + a_index;
}