#include <bits/stdc++.h>
#include <windows.h>
#include "HiEasyX.h"
#include <thread>
#include <io.h>

#pragma comment(lib, "MSIMG32.LIB")

using namespace std;

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define BeginDraw BEGIN_TASK()
#define EndDraw END_TASK(); REDRAW_WINDOW()
#define Sleep(int) this_thread::sleep_for(chrono::milliseconds(int))
#define Test() MessageBox(NULL, L"��Ǵ�", L"���", MB_OK)

LOGFONT font;//ͳһ������ʽ
RECT words_rect;
SYSTEMTIME sys_time;//ϵͳʱ���ݴ�

//�γ̿��Ƽ�
///�α�洢
struct
{
	int hour, minute;//��ʼʱ��
	int length;//�γ̳���
	int type;//�γ�����

	wstring name;//�γ�����
}timetable[8][20];
///��Ϣʱ��
vector<tuple<int, int, int, int, wstring>> rest_period[8];
///��������
map<tuple<int, int, int, int>, tuple<bool, wstring, int>> adjusting_classes;
///�γ̡���Ϣʱ������
int number_of_courses[8], number_of_rest[8];
//��ǰ׼��ʱ��
int preparation_time_before_class;

//�ַ�����
string global_path;//ȫ��·��
wchar_t edition[250] = L"�������汾��";

//ʱ����Ƽ�
int delay;//�ӳ�,(-)����ʱ�������ʱ����,(+)����ʱ�������ʱ���
int Second, Minute, Hour, DayOfWeek, Day, Month, Year;
struct
{
	pair<int, int> lunch;
	pair<int, int> dinner;
	int morning = 0, afternoon = 0, evening = 0;
}interval_time[8];

//�������Ƽ�
int volume;

//ý����Դ����
IMAGE img[2];//ͼ��
IMAGE sign_img[3];//��־ͼ��
IMAGE background_img;//����ͼ��
hiex::ImageBlock imageblock;//����ͼ���

//���ڿ��Ƽ�
struct
{
	int x, y;
	int height, width;
	int translucent;
	bool move;
} windows;

//����͸��������
void setWindowTransparent(HWND HWnd, bool enable, int alpha = 0xFF)
{
	LONG nRet = ::GetWindowLong(HWnd, GWL_EXSTYLE);
	nRet |= WS_EX_LAYERED;
	::SetWindowLong(HWnd, GWL_EXSTYLE, nRet);
	if (!enable) alpha = 0xFF;
	SetLayeredWindowAttributes(HWnd, 0, alpha, LWA_ALPHA);
}
//string ת wstring
wstring convert_to_wstring(const string& s)
{
	LPCSTR pszSrc = s.c_str();
	int nLen = s.size();

	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0)
		return NULL;
	WCHAR* pwszDst = new WCHAR[nSize + 1];
	if (NULL == pwszDst)
		return NULL;
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;
	if (pwszDst[0] == 0xFEFF) // skip Oxfeff
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];
	wstring wcharString(pwszDst);
	delete pwszDst;
	return wcharString;
}
//��������������
bool ModifyRegedit(bool bAutoRun)
{
	wchar_t pFileName[MAX_PATH] = { 0 };
	wcscpy(pFileName, (convert_to_wstring(global_path) + L"�¿���.exe").c_str());

	HKEY hKey;
	LPCTSTR lpRun = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpRun, 0, KEY_WRITE, &hKey);
	if (lRet != ERROR_SUCCESS)
		return false;

	if (bAutoRun)
		RegSetValueEx(hKey, L"xmg_auto", 0, REG_SZ, (const BYTE*)(LPCSTR)pFileName, MAX_PATH);
	else
		RegDeleteValueA(hKey, "xmg_auto");
	RegCloseKey(hKey);
	return true;
}
//png ����
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();

	// �ṹ��ĵ�������Ա��ʾ�����͸���ȣ�0 ��ʾȫ͸����255 ��ʾ��͸����
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	// ʹ�� Windows GDI ����ʵ�ְ�͸��λͼ
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

//ʱ���Զ���ȡ
void settime()
{
	while (1)
	{
		GetLocalTime(&sys_time);
		int wSecond = sys_time.wSecond;
		int wMinute = sys_time.wMinute;
		int wHour = sys_time.wHour;
		int wDayOfWeek = sys_time.wDayOfWeek;
		if (wDayOfWeek == 0) wDayOfWeek = 7;

		Day = sys_time.wDay;
		Month = sys_time.wMonth;
		Year = sys_time.wYear;

		if (delay > 0)
		{
			wSecond += delay;
			if (wSecond >= 60) wMinute += int(wSecond / 60), wSecond %= 60;
			if (wMinute >= 60) wHour += int(wMinute / 60), wMinute %= 60;
			if (wHour >= 24) wDayOfWeek += int(wHour / 60), wHour %= 60;
			if (wDayOfWeek > 7) wDayOfWeek = 1;
		}
		else if (delay < 0)
		{
			wSecond += delay;
			if (wSecond < 0) wMinute -= ceil(double(abs(wSecond)) / 60.0), wSecond = ((wSecond % 60) + 60) % 60;
			if (wMinute < 0) wHour -= ceil(double(abs(wMinute)) / 60.0), wMinute = ((wMinute % 60) + 60) % 60;
			if (wHour < 0) wDayOfWeek -= ceil(double(abs(wHour)) / 60.0), wHour = ((wHour % 60) + 60) % 60;
			if (wDayOfWeek <= 0) wDayOfWeek = 7;
		}

		{
			Second = wSecond;
			Minute = wMinute;
			Hour = wHour;
			DayOfWeek = wDayOfWeek;
		}
		Sleep(5);
	}
}
//�������
void clear_background()
{
	putimage(0, 0, &background_img);
}
//��ȡ���ݿ�
void read_database()
{
	wstring useless;

	wifstream infile;
	infile.imbue(locale("zh_CN.UTF8"));
	infile.open(convert_to_wstring(global_path) + L"opt\\option.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		//�ӳٶ�ȡ
		getline(infile, useless);
		infile >> delay;
		getline(infile, useless);
	}

	infile.close();
}
// ��ͼƬ����ģ������
void blur(IMAGE* pimg)
{
	DWORD* pMem = GetImageBuffer(pimg);

	int	r, g, b;
	int m;
	int	num = 0;
	int	width = pimg->getwidth();
	int	height = pimg->getheight();
	int step = width - 1, f, t;

	// ���� 9 ����ϵ��
	int	cell[9] = { -(width + 1), -1, width - 1, -width, 0, width, -(width - 1), 1,  width + 1 };

	// ������ص��ȡ����
	for (int i = width * height - 1; i >= 0; i--)
	{
		// ���ñ���
		r = g = b = 0;
		m = 9;

		// ��������λ�ã�������Χ���ӵļ��㷶Χ
		if (step == width - 1)
			t = 5, m = 6;
		else
			t = 8;

		if (step == 0)
			f = 3, m = 6, step = width;
		else
			f = 0;

		step--;

		// �ۼ���Χ���ӵ���ɫֵ
		for (int n = f; n <= t; n++)
		{
			// λ�ö�λ
			num = i + cell[n];

			// �ж�λ��ֵ�Ƿ�Խ��
			if (num < 0 || num >= width * height)
				m--;						// ͳ��Խ��������
			else
			{
				// �ۼ���ɫֵ
				r += GetRValue(pMem[num]);
				g += GetGValue(pMem[num]);
				b += GetBValue(pMem[num]);
			}
		}

		// ��ƽ��ֵ��ֵ������
		pMem[i] = RGB(r / m, g / m, b / m);
	}
}
//͸��Բ�Ǿ��λ���
void solidroundrect_alpha(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, int alpha, COLORREF color = WHITE)
{
	//ͼ�������
	imageblock.GetCanvas()->Clear();
	imageblock.GetCanvas()->SolidRoundRect(left, top, right, bottom, ellipsewidth, ellipseheight, true, color);
	ReverseAlpha(imageblock.GetCanvas()->GetBuffer(), imageblock.GetCanvas()->GetBufferSize());

	//����ͼ���͸����
	imageblock.alpha = alpha;
	imageblock.bUseSrcAlpha = true;

	imageblock.Render(hiex::GetWindowImage(), 255);
}
