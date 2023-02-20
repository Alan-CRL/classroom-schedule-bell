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
#define Test() MessageBox(NULL, L"标记处", L"标记", MB_OK)

LOGFONT font;//统一字体样式
RECT words_rect;
SYSTEMTIME sys_time;//系统时间暂存

//课程控制集
///课表存储
struct
{
	int hour, minute;//开始时分
	int length;//课程长度
	int type;//课程类型

	wstring name;//课程名称
}timetable[8][20];
///休息时段
vector<tuple<int, int, int, int, wstring>> rest_period[8];
///调课设置
map<tuple<int, int, int, int>, tuple<bool, wstring, int>> adjusting_classes;
///课程、休息时段数量
int number_of_courses[8], number_of_rest[8];
//课前准备时长
int preparation_time_before_class;

//字符串集
string global_path;//全局路径
wchar_t edition[250] = L"主体程序版本：";

//时间控制集
int delay;//延迟,(-)程序时间比正常时间慢,(+)程序时间比正常时间快
int Second, Minute, Hour, DayOfWeek, Day, Month, Year;
struct
{
	pair<int, int> lunch;
	pair<int, int> dinner;
	int morning = 0, afternoon = 0, evening = 0;
}interval_time[8];

//声音控制集
int volume;

//媒体资源管理
IMAGE img[2];//图像
IMAGE sign_img[3];//标志图像
IMAGE background_img;//背景图像
hiex::ImageBlock imageblock;//公用图像块

//窗口控制集
struct
{
	int x, y;
	int height, width;
	int translucent;
	bool move;
} windows;

//窗口透明度设置
void setWindowTransparent(HWND HWnd, bool enable, int alpha = 0xFF)
{
	LONG nRet = ::GetWindowLong(HWnd, GWL_EXSTYLE);
	nRet |= WS_EX_LAYERED;
	::SetWindowLong(HWnd, GWL_EXSTYLE, nRet);
	if (!enable) alpha = 0xFF;
	SetLayeredWindowAttributes(HWnd, 0, alpha, LWA_ALPHA);
}
//string 转 wstring
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
//开机启动项设置
bool ModifyRegedit(bool bAutoRun)
{
	wchar_t pFileName[MAX_PATH] = { 0 };
	wcscpy(pFileName, (convert_to_wstring(global_path) + L"下课铃.exe").c_str());

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
//png 绘制
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();

	// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	// 使用 Windows GDI 函数实现半透明位图
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

//时间自动获取
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
//窗口清空
void clear_background()
{
	putimage(0, 0, &background_img);
}
//读取数据库
void read_database()
{
	wstring useless;

	wifstream infile;
	infile.imbue(locale("zh_CN.UTF8"));
	infile.open(convert_to_wstring(global_path) + L"opt\\option.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		//延迟读取
		getline(infile, useless);
		infile >> delay;
		getline(infile, useless);
	}

	infile.close();
}
// 将图片进行模糊处理
void blur(IMAGE* pimg)
{
	DWORD* pMem = GetImageBuffer(pimg);

	int	r, g, b;
	int m;
	int	num = 0;
	int	width = pimg->getwidth();
	int	height = pimg->getheight();
	int step = width - 1, f, t;

	// 计算 9 格方向系数
	int	cell[9] = { -(width + 1), -1, width - 1, -width, 0, width, -(width - 1), 1,  width + 1 };

	// 逐个像素点读取计算
	for (int i = width * height - 1; i >= 0; i--)
	{
		// 重置变量
		r = g = b = 0;
		m = 9;

		// 根据像素位置，决定周围格子的计算范围
		if (step == width - 1)
			t = 5, m = 6;
		else
			t = 8;

		if (step == 0)
			f = 3, m = 6, step = width;
		else
			f = 0;

		step--;

		// 累加周围格子的颜色值
		for (int n = f; n <= t; n++)
		{
			// 位置定位
			num = i + cell[n];

			// 判断位置值是否越界
			if (num < 0 || num >= width * height)
				m--;						// 统计越界像素数
			else
			{
				// 累加颜色值
				r += GetRValue(pMem[num]);
				g += GetGValue(pMem[num]);
				b += GetBValue(pMem[num]);
			}
		}

		// 将平均值赋值该像素
		pMem[i] = RGB(r / m, g / m, b / m);
	}
}
//透明圆角矩形绘制
void solidroundrect_alpha(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, int alpha, COLORREF color = WHITE)
{
	//图像块设置
	imageblock.GetCanvas()->Clear();
	imageblock.GetCanvas()->SolidRoundRect(left, top, right, bottom, ellipsewidth, ellipseheight, true, color);
	ReverseAlpha(imageblock.GetCanvas()->GetBuffer(), imageblock.GetCanvas()->GetBufferSize());

	//设置图像块透明度
	imageblock.alpha = alpha;
	imageblock.bUseSrcAlpha = true;

	imageblock.Render(hiex::GetWindowImage(), 255);
}
