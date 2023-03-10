#include <bits/stdc++.h>
#include <windows.h>
#include "HiEasyX.h"
#include <thread>
#include <io.h>
#include <mmsystem.h>
#include <vfw.h>
#include <Netlistmgr.h>
#include <Wininet.h>
#include <ShObjIdl.h>
#include <ShlGuid.h>
#include <shlobj.h>
#include <tlhelp32.h>

#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>

#include <json/allocator.h>
#include <json/assertions.h>
#include <json/config.h>
#include <json/forwards.h>
#include <json/json.h>
#include <json/json_features.h>
#include <json/reader.h>
#include <json/value.h>
#include <json/version.h>
#include <json/writer.h>

#pragma comment(lib, "MSIMG32.LIB")
#pragma comment(lib, "WINMM.LIB")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Dwmapi.lib")

using namespace std;

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define BeginDraw() BEGIN_TASK()
#define EndDraw() END_TASK(false); REDRAW_WINDOW()
#define Sleep(int) this_thread::sleep_for(chrono::milliseconds(int))
#define Test() MessageBox(NULL, L"标记处", L"标记", MB_OK | MB_SYSTEMMODAL)

LOGFONT font;//统一字体样式
RECT words_rect;
SYSTEMTIME sys_time;//系统时间暂存

wstring edition = L"V1.0RC1";
wstring data_edition = L"20230220.02";
bool already;

//当前状态
struct
{
	tuple<int, int> begin = make_tuple(-1, -1);//开始时刻
	tuple<int, int> end = make_tuple(-1, -1);//结束时刻
	double type = -1;//0 休息时段 1 课间休息 1.1 课间休息（下面是休息时段） 2 普通课程，3 静默课程，4 常显示课程，5 课前准备
	wstring name;//时段名称

	//特有变量
	int num;//课程编号
	double last_type = -1;

	int special;//-1 : 考试模式
}state;
bool state_reset;
double zoom = -1;

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
//时段数量
int number_of_courses[8];
//课前准备时长
int preparation_time_before_class;

//字符串集
string global_path;//全局路径
struct
{
	wstring province, city, area;
	wstring id, lat, lon;
}location;//地理位置
struct
{
	int show_num;

	struct epidemic_set
	{
		wstring name = L"--";
		wstring conNum = L"--";//累积确诊
		wstring econNum = L"--";//现存确诊
		wstring conadd = L"--";//昨日新增
	};
	epidemic_set country;
	epidemic_set province;
	epidemic_set city;

	epidemic_set show;
}epidemic;//疫情数据

//时间控制集
int delay;//延迟,(-)程序时间比正常时间慢,(+)程序时间比正常时间快
int Second, Minute, Hour, DayOfWeek, Day, Month, Year;
struct
{
	pair<int, int> lunch;
	pair<int, int> dinner;
	int morning = 0, afternoon = 0, evening = 0;
}interval_time[8];
struct
{
	wstring preparation_before_class;
}message_send;
chrono::steady_clock::time_point last_click_time;

//声音控制集
int volume;
hiex::MusicMCI finish_class;

//媒体资源管理
IMAGE sign_img[5];//标志图像
IMAGE background_img[6];//背景图像
map<wstring, IMAGE> ico_img;
int background_num;//背景播放进度
IMAGE back;//背景图像采集
bool clew_tone;
IMAGE weathar_img[71];
IMAGE weathar_gray_img[71];
map<tuple<int, int, int>, pair<wstring, wstring>> dictum;//每日一言

bool grey_pattern;
bool red_color;//false：升色；true：降色
int red_color_num = 250;//红色闪烁控制变量

bool bilateral_window_update = true;
wstring copyright_bing;

//窗口标识
HWND main_window;//主窗口
HWND launch_window;//启动窗口
int window_status = -1, target_window_status = -1;//（目标）窗口状态，-1 初始化，0 隐藏，1 展开，2 免打扰提示

//窗口控制集
struct
{
	int x, y;
	int translucent;

	struct
	{
		int x, y;
		int height, width;
		bool move;
	}middle;
	struct
	{
		bool is_open;
		int x, y;
		int height, width;
	}left;
	struct
	{
		bool is_open;
		int x, y;
		int height, width;
	}right;

	struct
	{
		bool is_open;
		int x, y;
		int height, width;
	}massage;
} windows;

COLORREF rgb(int r, int g, int b)
{
	if (grey_pattern)
	{
		int grey = (r * 229 + g * 587 + b * 114 + 500) / 1000;
		return RGB(grey, grey, grey);
	}
	else return RGB(r, g, b);
}

//窗口透明度设置
void setWindowTransparent(HWND HWnd, bool enable, int alpha = 0xFF)
{
	LONG nRet = ::GetWindowLong(HWnd, GWL_EXSTYLE);
	nRet |= WS_EX_LAYERED;
	::SetWindowLong(HWnd, GWL_EXSTYLE, nRet);
	if (!enable) alpha = 0xFF;
	SetLayeredWindowAttributes(HWnd, 0, alpha, LWA_ALPHA);
}
//网络状态获取
static bool checkIsNetwork()
{
	CoInitialize(NULL);
	//  通过NLA接口获取网络状态
	IUnknown* pUnknown = NULL;
	BOOL   bOnline = TRUE;//是否在线
	HRESULT Result = CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL,
		IID_IUnknown, (void**)&pUnknown);
	if (SUCCEEDED(Result))
	{
		INetworkListManager* pNetworkListManager = NULL;
		if (pUnknown)
			Result = pUnknown->QueryInterface(IID_INetworkListManager, (void
				**)&pNetworkListManager);
		if (SUCCEEDED(Result))
		{
			VARIANT_BOOL IsConnect = VARIANT_FALSE;
			if (pNetworkListManager)
				Result = pNetworkListManager->get_IsConnectedToInternet(&IsConnect);
			if (SUCCEEDED(Result))
			{
				bOnline = (IsConnect == VARIANT_TRUE) ? true : false;
			}
		}
		if (pNetworkListManager)
			pNetworkListManager->Release();
	}
	if (pUnknown)
		pUnknown->Release();
	CoUninitialize();
	return bOnline;
}
//程序进程状态获取
DWORD GetProcessidFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (wcscmp(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;

			break;
		}
	}
	CloseHandle(hSnapshot);
	return id;
}
//string 转 wstring
wstring convert_to_wstring(const string s)
{
	LPCSTR pszSrc = s.c_str();
	int nLen = s.size();

	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	WCHAR* pwszDst = new WCHAR[nSize + 1];
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;
	if (pwszDst[0] == 0xFEFF) // skip Oxfeff
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];
	wstring wcharString(pwszDst);
	delete pwszDst;
	return wcharString;
}
//wstring 转 string
string convert_to_string(const wstring str)
{
	int size = WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), nullptr, 0, nullptr, nullptr);
	auto p_str(std::make_unique<char[]>(size + 1));
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), p_str.get(), size, nullptr, nullptr);
	return std::string(p_str.get());
}
//string 转 urlencode
string convert_to_urlencode(string str)
{
	int size(str.size() * 3 + 1);
	auto pstr(std::make_unique<char[]>(size + 1));
	int i(0);

	for (const auto& x : str)
	{
		if (x >= 0 && x <= 0x80)
		{
			if (x >= 0x41 && x <= 0x5A || x >= 0x61 && x <= 0x7A)
			{
				pstr.get()[i] = x;
				++i;
				continue;
			}
		}
		sprintf(pstr.get() + i, "%%%02X", static_cast<unsigned char>(x));
		i += 3;
	}
	str = pstr.get();
	return str;
}
//GBK 转 utf-8
string convert_to_utf8(string str)
{
	wstring x = convert_to_wstring(str);

	int size = WideCharToMultiByte(CP_UTF8, 0, x.c_str(), x.size(), nullptr, 0, nullptr, nullptr);
	auto p_str(std::make_unique<char[]>(size + 1));
	WideCharToMultiByte(CP_UTF8, 0, x.c_str(), x.size(), p_str.get(), size, nullptr, nullptr);
	str = p_str.get();
	return str;
}
//utf-8 转 GBK
string convert_to_gbk(string strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	TCHAR* wszGBK = new TCHAR[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
//string 转 LPCWSTR
LPCWSTR stringtoLPCWSTR(string str)
{
	size_t size = str.length();
	int wLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t* buffer = new wchar_t[wLen + 1];
	memset(buffer, 0, (wLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), size, (LPWSTR)buffer, wLen);
	return buffer;
}
//彩色图像转换为灰度图像
IMAGE* ColorToGray(IMAGE* tmp)
{
	IMAGE* pimg = tmp;

	DWORD* p = GetImageBuffer(pimg);	// 获取显示缓冲区指针
	COLORREF c;

	// 逐个像素点读取计算
	for (int i = pimg->getwidth() * pimg->getheight() - 1; i >= 0; i--)
	{
		c = BGR(p[i]);
		c = (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114 + 500) / 1000;
		p[i] = rgb(c, c, c);	// 由于是灰度值，无需再执行 BGR 转换
	}

	return pimg;
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
			if (1) //1:自动时间；0：手动时间
			{
				Second = wSecond;
				Minute = wMinute;
				Hour = wHour;
				DayOfWeek = wDayOfWeek;
			}
			else
			{
				Second = wSecond;
				Minute = 58;
				Hour = 17;
				DayOfWeek = 3;
			}
		}

		Sleep(5);
	}
}
//窗口清空
void clear_background(HWND tmp = main_window)
{
	if (tmp == main_window)
	{
		background_num++;
		if (background_num > 600) background_num = 1;
		loadimage(background_img + 1, (convert_to_wstring(global_path) + L"img\\background\\Image" + to_wstring(background_num) + L".jpg").c_str(), windows.middle.width, windows.middle.height, true);

		if (grey_pattern) putimage(windows.middle.x, windows.middle.y, ColorToGray(background_img + 1));
		else putimage(windows.middle.x, windows.middle.y, background_img + 1);
	}
	else putimage(0, 0, grey_pattern ? ColorToGray(background_img + 2) : background_img + 2);
}
//窗口状态设置
void window_form_management()
{
	while (1)
	{
		Sleep(5);

		if ((window_status == 0 || window_status == 2) && target_window_status == 1)
		{
			if (windows.y > 5)
			{
				for (int i = windows.y; i >= 5; i -= 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = 5, windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}
			else
			{
				for (int i = windows.y; i <= 5; i += 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = 5, windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}

			window_status = 1;
		}
		else if (window_status == 1 && target_window_status == 0)
		{
			for (int i = windows.y; i >= int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)); i -= 5)
			{
				SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
				Sleep(5);
			}
			SetWindowPos(main_window, NULL, windows.x, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)), windows.middle.width, windows.middle.height, SWP_NOSIZE);

			window_status = 0;
		}
		else if (window_status == 0 && windows.y != int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)) && !windows.middle.move)
		{
			if (windows.y > int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)))
			{
				for (int i = windows.y; i >= int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)); i -= 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)), windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}
			else
			{
				for (int i = windows.y; i <= int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)); i += 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)), windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}
		}
		else if (window_status == 1 && windows.y != 5 && !windows.middle.move)
		{
			if (windows.y > 5)
			{
				for (int i = windows.y; i >= 5; i -= 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = 5, windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}
			else
			{
				for (int i = windows.y; i <= 5; i += 5)
				{
					SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
					Sleep(5);
				}
				SetWindowPos(main_window, NULL, windows.x, windows.y = 5, windows.middle.width, windows.middle.height, SWP_NOSIZE);
			}
		}

		else if (window_status == 0 && target_window_status == 2)
		{
			for (int i = windows.y; i <= int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 11) * zoom - 8)); i += 2)
			{
				SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
				Sleep(5);
			}
			SetWindowPos(main_window, NULL, windows.x, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 11) * zoom - 8)), windows.middle.width, windows.middle.height, SWP_NOSIZE);

			window_status = 2;
		}
		else if (window_status == 2 && target_window_status == 0)
		{
			for (int i = windows.y; i >= int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)); i -= 2)
			{
				SetWindowPos(main_window, NULL, windows.x, windows.y = i, windows.middle.width, windows.middle.height, SWP_NOSIZE);
				Sleep(5);
			}
			SetWindowPos(main_window, NULL, windows.x, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)), windows.middle.width, windows.middle.height, SWP_NOSIZE);

			window_status = 0;
		}
	}
}
//设置系统音量
bool SetVolum(int level)
{
	//-2 恢复静音
	//-1 静音
	//0~100 音量比例

	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;

	try {
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";

		if (level == -2) {
			hr = pAudioEndpointVolume->SetMute(FALSE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else if (level == -1) {
			hr = pAudioEndpointVolume->SetMute(TRUE, NULL);
			if (FAILED(hr)) throw "SetMute";
		}
		else {
			if (level < 0) level = 0;
			if (level > 100) level = 100;

			float fVolume;
			fVolume = level / 100.0f;
			hr = pAudioEndpointVolume->SetMasterVolumeLevelScalar(fVolume, &GUID_NULL);
			if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

			pAudioClient->Release();
			pAudioEndpointVolume->Release();
			pDevice->Release();
			pDeviceEnumerator->Release();
			return true;
		}
	}
	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
		throw;
	}
	return false;
}
//获取系统音量
int GetVolum()
{
	HRESULT hr;
	IMMDeviceEnumerator* pDeviceEnumerator = 0;
	IMMDevice* pDevice = 0;
	IAudioEndpointVolume* pAudioEndpointVolume = 0;
	IAudioClient* pAudioClient = 0;

	try {
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pDeviceEnumerator);
		if (FAILED(hr)) throw "CoCreateInstance";
		hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
		if (FAILED(hr)) throw "GetDefaultAudioEndpoint";
		hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pAudioEndpointVolume);
		if (FAILED(hr)) throw "pDevice->Active";
		hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
		if (FAILED(hr)) throw "pDevice->Active";

		float fVolume;

		hr = pAudioEndpointVolume->GetMasterVolumeLevelScalar(&fVolume);

		if (FAILED(hr)) throw "SetMasterVolumeLevelScalar";

		pAudioClient->Release();
		pAudioEndpointVolume->Release();
		pDevice->Release();
		pDeviceEnumerator->Release();

		int intVolume = fVolume * 100;
		if (fVolume > 100)
		{
			fVolume = 100;
		}
		return intVolume;
	}

	catch (...) {
		if (pAudioClient) pAudioClient->Release();
		if (pAudioEndpointVolume) pAudioEndpointVolume->Release();
		if (pDevice) pDevice->Release();
		if (pDeviceEnumerator) pDeviceEnumerator->Release();
		throw;
	}
}
//下课播放提示音
void play_clew_tone()
{
	finish_class.open((convert_to_wstring(global_path) + L"wma\\classover.wma").c_str());

	while (1)
	{
		Sleep(10);

		if (clew_tone == true)
		{
			int tmp = GetVolum();
			if (GetVolum() != volume) SetVolum(volume);

			finish_class.play();
			Sleep(4000);
			finish_class.stop();

			if (GetVolum() != tmp) SetVolum(tmp);

			clew_tone = false;
		}
	}
}
//读取数据库
void read_database()
{
	int size;
	wstring useless;

	wifstream infile;
	infile.imbue(locale("zh_CN.UTF8"));
	infile.open(convert_to_wstring(global_path) + L"opt\\timetable.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		//课程设置读取
		for (int i : {1, 2, 3, 4, 5, 6, 7})
		{
			getline(infile, useless);

			//间隔时间读取
			getline(infile, useless);
			infile >> interval_time[i].lunch.first >> interval_time[i].lunch.second >> interval_time[i].dinner.first >> interval_time[i].dinner.second;
			getline(infile, useless);

			//时段
			{
				getline(infile, useless);
				infile >> size;
				getline(infile, useless);
				number_of_courses[i] = size;

				interval_time[i].morning = interval_time[i].afternoon = interval_time[i].evening = 0;

				for (int z = 1; z <= size; z++)
				{
					wstring tmp, name;
					int num, l, r, len, type;

					infile >> num;
					getline(infile, tmp, L':');
					l = _wtoi(tmp.c_str());
					infile >> r >> len;

					getline(infile, useless, L'\"');
					getline(infile, name, L'\"');
					infile >> type;
					getline(infile, useless);

					timetable[i][z].hour = l;
					timetable[i][z].minute = r;
					timetable[i][z].length = len;
					timetable[i][z].type = type;
					timetable[i][z].name = name;

					if (type != 0)
					{
						if (l * 60 + r < interval_time[i].lunch.first * 60 + interval_time[i].lunch.second) interval_time[i].morning++;
						else if (l * 60 + r < interval_time[i].dinner.first * 60 + interval_time[i].dinner.second) interval_time[i].afternoon++;
						else interval_time[i].evening++;
					}
				}
			}
		}

		//调课读取
		{
			adjusting_classes.clear();

			getline(infile, useless);
			infile >> size;
			getline(infile, useless);

			for (int i = 1; i <= size; i++)
			{
				int y, m, d, num, type;
				wstring name;

				infile >> y >> m >> d >> num;

				getline(infile, useless, L'\"');
				getline(infile, name, L'\"');
				infile >> type;
				getline(infile, useless);

				adjusting_classes[make_tuple(y, m, d, num)] = make_tuple(true, name, type);
			}
		}
	}

	infile.close();

	infile.open(convert_to_wstring(global_path) + L"opt\\option.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		//延迟读取
		getline(infile, useless);
		infile >> delay;
		getline(infile, useless);

		//音量读取
		getline(infile, useless);
		infile >> volume;
		getline(infile, useless);

		//缩放读取
		getline(infile, useless);
		infile >> useless;
		getline(infile, useless);

		//课前准备时长读取
		getline(infile, useless);
		infile >> preparation_time_before_class;
		getline(infile, useless);

		//特殊模式读取
		getline(infile, useless);
		infile >> state.special;
		if (state.special == -1) target_window_status = 1;
		getline(infile, useless);
	}

	infile.close();

	infile.open(convert_to_wstring(global_path) + L"opt\\location.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		infile >> location.id;
		infile >> location.lat;
		infile >> location.lon;
		infile >> location.province;
		infile >> location.city;
		infile >> location.area;
	}

	infile.close();

	infile.open(convert_to_wstring(global_path) + L"opt\\reliance\\dictum.txt");

	{
		dictum.clear();

		int year, month, day;
		wstring t1, t2;

		while (1)
		{
			infile >> year;
			if (year == -1) break;

			infile >> month;
			infile >> day;

			getline(infile, useless, L'\"');
			getline(infile, t1, L'\"');
			getline(infile, useless, L'\"');
			getline(infile, t2, L'\"');

			dictum[make_tuple(year, month, day)] = make_pair(t1, t2);
		}
	}

	infile.close();

	infile.open(convert_to_wstring(global_path) + L"opt\\message.txt");

	getline(infile, useless);
	if (useless == L"V1.0")
	{
		wstring t1, t2;

		while (1)
		{
			infile >> t1;
			if (t1 == L"EOF") break;

			getline(infile, useless, L'\"');
			getline(infile, t2, L'\"');

			if (t1 == L"preparation_before_class") message_send.preparation_before_class = t2;
		}
	}

	infile.close();
}
//状态获取
void confirmation_period()
{
	state.last_type = state.type;
	int idx = 0;

	int time = Second + Minute * 60 + Hour * 3600;
	//时段判断
	for (int i = 1; i <= number_of_courses[DayOfWeek]; i++)
	{
		if (timetable[DayOfWeek][i].type != 0) idx++;

		if (timetable[DayOfWeek][i].type != 0 && timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60 <= time && time <= timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60 + timetable[DayOfWeek][i].length * 60)
		{
			state.begin = make_tuple(timetable[DayOfWeek][i].hour, timetable[DayOfWeek][i].minute);

			int tmp = timetable[DayOfWeek][i].hour * 60 + timetable[DayOfWeek][i].minute + timetable[DayOfWeek][i].length;
			int t1 = tmp % 60;
			int t2 = (tmp - t1) / 60;

			state.end = make_tuple(t2, t1);

			if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, i)]) == true)
			{
				state.type = get<2>(adjusting_classes[make_tuple(Year, Month, Day, i)]) + 2;
				state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, i)]);
			}
			else
			{
				state.type = timetable[DayOfWeek][i].type + 1;
				state.name = timetable[DayOfWeek][i].name;
			}

			state.num = idx;

			return;
		}
		else if (timetable[DayOfWeek][i].type == 0 && timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60 <= time && time <= timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60 + timetable[DayOfWeek][i].length * 60)
		{
			state.begin = make_tuple(timetable[DayOfWeek][i].hour, timetable[DayOfWeek][i].minute);

			int tmp = timetable[DayOfWeek][i].hour * 60 + timetable[DayOfWeek][i].minute + timetable[DayOfWeek][i].length;
			int t1 = tmp % 60;
			int t2 = (tmp - t1) / 60;

			state.end = make_tuple(t2, t1);

			state.type = 0;
			state.name = timetable[DayOfWeek][i].name;

			return;
		}
		else if (i != 1 && timetable[DayOfWeek][i - 1].hour * 3600 + timetable[DayOfWeek][i - 1].minute * 60 + timetable[DayOfWeek][i - 1].length < time && time < timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60)
		{
			if (timetable[DayOfWeek][i].type != 0 && time >= timetable[DayOfWeek][i].hour * 3600 + timetable[DayOfWeek][i].minute * 60 - preparation_time_before_class * 60)
			{
				int tmp = timetable[DayOfWeek][i].hour * 60 + timetable[DayOfWeek][i].minute - preparation_time_before_class;
				int t1 = tmp % 60;
				int t2 = (tmp - t1) / 60;

				state.begin = make_tuple(t2, t1);
				state.end = make_tuple(timetable[DayOfWeek][i].hour, timetable[DayOfWeek][i].minute);
				state.type = 5;

				if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, i)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, i)]);
				else state.name = timetable[DayOfWeek][i].name;

				state.num = idx;
			}
			else
			{
				int tmp = timetable[DayOfWeek][i - 1].hour * 60 + timetable[DayOfWeek][i - 1].minute + timetable[DayOfWeek][i - 1].length;
				int t1 = tmp % 60;
				int t2 = (tmp - t1) / 60;

				state.begin = make_tuple(t2, t1);

				if (timetable[DayOfWeek][i].type == 0) tmp = timetable[DayOfWeek][i].hour * 60 + timetable[DayOfWeek][i].minute;
				else tmp = timetable[DayOfWeek][i].hour * 60 + timetable[DayOfWeek][i].minute - preparation_time_before_class;
				t1 = tmp % 60;
				t2 = (tmp - t1) / 60;

				state.end = make_tuple(t2, t1);

				if (timetable[DayOfWeek][i].type != 0)
				{
					state.type = 1;

					if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, i)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, i)]);
					else state.name = timetable[DayOfWeek][i].name;
				}
				else
				{
					state.type = 1.1;

					if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, i)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, i)]);
					else state.name = timetable[DayOfWeek][i].name;
				}
			}

			return;
		}
	}
	//杂项判断
	{
		int time = Second + Minute * 60 + Hour * 3600;

		if (time < timetable[DayOfWeek][1].hour * 3600 + timetable[DayOfWeek][1].minute * 60)
		{
			if (timetable[DayOfWeek][1].type != 0 && time >= timetable[DayOfWeek][1].hour * 3600 + timetable[DayOfWeek][1].minute * 60 - preparation_time_before_class * 60)
			{
				int tmp = timetable[DayOfWeek][1].hour * 60 + timetable[DayOfWeek][1].minute - preparation_time_before_class;
				int t1 = tmp % 60;
				int t2 = (tmp - t1) / 60;

				state.begin = make_tuple(t2, t1);
				state.end = make_tuple(timetable[DayOfWeek][1].hour, timetable[DayOfWeek][1].minute);
				state.type = 5;

				if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, 1)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, 1)]);
				else state.name = timetable[DayOfWeek][1].name;

				state.num = 1;
			}
			else
			{
				state.begin = make_tuple(Hour, Minute);

				int tmp = timetable[DayOfWeek][1].hour * 60 + timetable[DayOfWeek][1].minute - preparation_time_before_class;
				int t1 = tmp % 60;
				int t2 = (tmp - t1) / 60;

				state.end = make_tuple(t2, t1);

				if (timetable[DayOfWeek][1].type != 0)
				{
					state.type = 1;

					if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, 1)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, 1)]);
					else state.name = timetable[DayOfWeek][1].name;
				}
				else
				{
					state.type = 1.1;

					if (get<0>(adjusting_classes[make_tuple(Year, Month, Day, 1)]) == true) state.name = get<1>(adjusting_classes[make_tuple(Year, Month, Day, 1)]);
					else state.name = timetable[DayOfWeek][1].name;
				}
			}

			return;
		}
	}
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
		pMem[i] = rgb(r / m, g / m, b / m);
	}
}
//透明圆角矩形绘制
void solidroundrect_alpha(int left, int top, int right, int bottom, int ellipsewidth, int ellipseheight, int alpha, COLORREF color = rgb(255, 255, 255), IMAGE* image = hiex::GetWindowImage())
{
	DRAW_TNS_INIT_GRAPHICS(right - left + 1, bottom - top + 1);
	{
		graphics.SolidRoundRect(0, 0, right - left, bottom - top, ellipsewidth, ellipseheight, true, color);
	}
	DRAW_TNS_RENDER_TO(left, top, image, alpha);
}

//窗口自动置顶
void automatic_topping()
{
	while (1)
	{
		Sleep(1);
		SetWindowPos(main_window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
//窗口拖动交互
void SeekBar(ExMessage m)
{
	if (!KEY_DOWN(VK_LBUTTON)) return;

	windows.middle.move = true;
	if (m.lbutton)
	{
		POINT p;
		GetCursorPos(&p);

		int pop_x = p.x;
		int pop_y = p.y;

		while (1)
		{
			if (!KEY_DOWN(VK_LBUTTON)) break;

			POINT p;
			GetCursorPos(&p);

			pop_x = p.x;
			pop_y = p.y;
			SetWindowPos(main_window, NULL, windows.x, windows.y = min(GetSystemMetrics(SM_CYSCREEN) / 10, max(p.y - m.y, int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)))), windows.middle.width, windows.middle.height, SWP_NOSIZE);
		}
	}
	windows.middle.move = false;

	{
		if (window_status == 1)
		{
			if (windows.y < -10) target_window_status = 0;
			if (windows.y > 50)
			{
				wofstream outfile;
				outfile.imbue(locale("zh_CN.UTF8"));
				outfile.open(convert_to_wstring(global_path) + L"opt\\edition.txt");
				outfile << edition;
				outfile.close();

				if (!GetProcessidFromName(L"settings.exe"))
				{
					//CreateProcess
					WinExec("settings.exe", SW_NORMAL);
				}
			}
		}
		if (window_status == 0)
		{
			if (windows.y > -90) target_window_status = 1;
		}
		if (window_status == 2) target_window_status = 1;
	}

	return;
}
//窗口鼠标交互
void mouse_interaction()
{
	while (!already) Sleep(10);

	ExMessage m;
	while (1)
	{
		getmessage(&m, EM_MOUSE);

		if (m.x >= windows.massage.x && m.y >= windows.massage.y && m.x <= windows.massage.x + windows.massage.width && m.y <= windows.massage.y + windows.massage.height)
		{
			if (m.lbutton)
			{
				HRGN whole_rgn = CreateRoundRectRgn(windows.middle.x, windows.middle.y, windows.middle.x + windows.middle.width, windows.middle.y + windows.middle.height, 15, 15);
				if (windows.left.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.left.x, windows.left.y, windows.left.x + windows.left.width, windows.left.y + windows.left.height, 15, 15), RGN_OR);
				if (windows.right.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.right.x, windows.right.y, windows.right.x + windows.right.width, windows.right.y + windows.right.height, 15, 15), RGN_OR);

				SetWindowRgn(main_window, whole_rgn, true);

				windows.massage.is_open = false;
			}
		}
		else if (m.lbutton)
		{
			int lx = m.x, ly = m.y;
			while (1)
			{
				ExMessage m = getmessage(EM_MOUSE);
				if (windows.y == 5 && abs(m.x - lx) <= windows.middle.height / 10 && (abs(m.y - ly) <= windows.middle.width / 50))
				{
					if (!m.lbutton)
					{
						if (window_status == 1) target_window_status = 0;
						else if (window_status == 0) target_window_status = 1;

						break;
					}
				}
				else if (m.x == lx && m.y == ly)
				{
					if (!m.lbutton)
					{
						if (window_status == 1 || window_status == 2) target_window_status = 0;
						else if (window_status == 0) target_window_status = 1;

						break;
					}
				}
				else
				{
					SeekBar(m);

					flushmessage(/*EX_MOUSE*/);

					break;
				}
			}
		}
	}
}
//窗口点击时算
void click_time()
{
	last_click_time = chrono::steady_clock::now();

	while (1)
	{
		if (KEY_DOWN(VK_LBUTTON) || KEY_DOWN(VK_LEFT) || KEY_DOWN(VK_UP) || KEY_DOWN(VK_RIGHT) || KEY_DOWN(VK_DOWN) || KEY_DOWN(VK_SPACE)) last_click_time = chrono::steady_clock::now();
		Sleep(1);
	}
}

//程序操作管理
void program_operation_management()
{
	wofstream outfile;
	outfile.imbue(locale("zh_CN.UTF8"));
	outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
	outfile << L"0";
	outfile.close();

	int tmp;

	while (1)
	{
		Sleep(10);

		wifstream infile;
		infile.imbue(locale("zh_CN.UTF8"));
		infile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
		infile >> tmp;
		infile.close();

		if (tmp == 1)
		{
			wofstream outfile;
			outfile.imbue(locale("zh_CN.UTF8"));
			outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
			outfile << L"0";
			outfile.close();

			exit(0);
		}
		else if (tmp == 2)
		{
			wofstream outfile;
			outfile.imbue(locale("zh_CN.UTF8"));
			outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
			outfile << L"0";
			outfile.close();

			read_database();
			state_reset = true;
		}
	}
}
//程序自动更新
void automatic_update()
{
	while (!already)
	{
		Sleep(10);
	}

	//每日壁纸下载
	if (checkIsNetwork())
	{
		HRESULT res1;
		{
			res1 = URLDownloadToFileW( // 从网络上下载数据到本地文件
				nullptr,                  // 在这里，写 nullptr 就行
				L"https://cn.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1&mkt=zh-CN", // 在这里写上网址
				stringtoLPCWSTR(global_path + "tmp\\background_bing.txt"),            // 文件名写在这
				0,                        // 写 0 就对了
				nullptr                   // 也是，在这里写 nullptr 就行
			);
		}

		if (res1 == S_OK)
		{
			wstring url, copyright, useless;

			wifstream infile;
			infile.imbue(locale("zh_CN.UTF8"));
			infile.open(convert_to_wstring(global_path) + L"tmp\\background_bing.txt");

			getline(infile, useless, L'[');
			getline(infile, useless, L':');
			getline(infile, useless, L':');
			getline(infile, useless, L':');
			getline(infile, useless, L':');
			getline(infile, useless, L'\"');
			getline(infile, url, L'\"');
			getline(infile, useless, L':');
			getline(infile, useless, L':');
			getline(infile, useless, L'\"');
			getline(infile, copyright, L'\"');

			infile.close();

			HRESULT res2;
			{
				res2 = URLDownloadToFileW( // 从网络上下载数据到本地文件
					nullptr,                  // 在这里，写 nullptr 就行
					(L"https://cn.bing.com" + url).c_str(), // 在这里写上网址
					stringtoLPCWSTR(global_path + "tmp\\background_bing.jpg"),            // 文件名写在这
					0,                        // 写 0 就对了
					nullptr                   // 也是，在这里写 nullptr 就行
				);
			}

			if (res2 == S_OK)
			{
				_wsystem((L"move \"" + convert_to_wstring(global_path) + L"tmp\\background_bing.jpg" + L"\" \"" + convert_to_wstring(global_path) + L"img\\" + L"\"").c_str());

				wofstream outfile;
				outfile.imbue(locale("zh_CN.UTF8"));
				outfile.open(convert_to_wstring(global_path) + L"opt\\reliance\\background_copyright.txt");

				outfile << copyright;

				outfile.close();
			}
		}
	}

	while (1)
	{
		Sleep(60000);

		//悼念灰白模式
		if (Minute == 0)
		{
			if (checkIsNetwork())
			{
				HRESULT res;
				{
					res = URLDownloadToFileW( // 从网络上下载数据到本地文件
						nullptr,                  // 在这里，写 nullptr 就行
						L"https://www.baidu.com/", // 在这里写上网址
						stringtoLPCWSTR(global_path + "tmp\\web.txt"),            // 文件名写在这
						0,                        // 写 0 就对了
						nullptr                   // 也是，在这里写 nullptr 就行
					);
				}

				if (res == S_OK) {
					wstring tmp;

					wifstream infile;
					infile.imbue(locale("zh_CN.UTF8"));
					infile.open(convert_to_wstring(global_path) + L"tmp\\web.txt");

					bool flag = false;
					while (!infile.eof())
					{
						getline(infile, tmp);
						if (tmp.find(L"filter: gray;") != string::npos || tmp.find(L"filter:gray;") != string::npos)
						{
							flag = true;
							break;
						}
					}
					if (!grey_pattern && flag) grey_pattern = true, bilateral_window_update = true;
					else grey_pattern = false;

					infile.close();
				}
			}
		}

		//疫情天气状态获取
		if (Minute % 10 == 0)
		{
			//疫情状态获取
			/*
			if (checkIsNetwork())
			{
				HRESULT res;
				{
					res = URLDownloadToFileW( // 从网络上下载数据到本地文件
						nullptr,                  // 在这里，写 nullptr 就行
						L"https://interface.sina.cn/news/wap/fymap2020_data.d.json", // 在这里写上网址
						stringtoLPCWSTR(global_path + "tmp\\epidemic.txt"),            // 文件名写在这
						0,                        // 写 0 就对了
						nullptr                   // 也是，在这里写 nullptr 就行
					);
				}

				if (res == S_OK)
				{
					Json::Reader reader;
					Json::Value root;

					ifstream injson;
					injson.imbue(locale("zh_CN.UTF8"));
					injson.open(convert_to_string(convert_to_wstring(global_path) + L"tmp\\epidemic.txt").c_str());

					if (reader.parse(injson, root))
					{
						epidemic.country.name = L"中国";
						epidemic.country.conNum = convert_to_wstring(convert_to_gbk(root["data"]["gntotal"].asString()));
						epidemic.country.econNum = convert_to_wstring(convert_to_gbk(root["data"]["econNum"].asString()));
						epidemic.country.conadd = convert_to_wstring(convert_to_gbk(root["data"]["othertotal"]["ecertain_inc"].asString()));

						int province_id = -1;
						for (int i = 0; i < root["data"]["list"].size(); i++)
						{
							if (location.province.find(convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["name"].asString()))) != string::npos || convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["name"].asString())).find(location.province) != string::npos)
							{
								epidemic.province.name = convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["name"].asString()));
								epidemic.province.conNum = convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["value"].asString()));
								epidemic.province.econNum = convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["econNum"].asString()));
								epidemic.province.conadd = convert_to_wstring(convert_to_gbk(root["data"]["list"][i]["conadd"].asString()));

								province_id = i;
								break;
							}
						}

						if (province_id != -1)
						{
							for (int i = 0; i < root["data"]["list"][i]["city"].size(); i++)
							{
								if (location.city.find(convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["name"].asString()))) != string::npos || convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["name"].asString())).find(location.city) != string::npos)
								{
									epidemic.city.name = convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["name"].asString()));
									epidemic.city.conNum = convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["conNum"].asString()));
									epidemic.city.econNum = convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["econNum"].asString()));
									epidemic.city.conadd = convert_to_wstring(convert_to_gbk(root["data"]["list"][province_id]["city"][i]["conadd"].asString()));

									break;
								}
							}
						}
					}

					injson.close();
				}
			}
			*/
		}

		{
			/*
			epidemic.show_num++;
			if (epidemic.show_num > 3) epidemic.show_num = 1;
			bilateral_window_update = true;
			*/
		}
	}
}
//程序消息灵动岛
void massagepop(wstring notice)
{
	{
		wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
		for (int i = 60;; i--)
		{
			font.lfHeight = i;
			settextstyle(&font);

			if (textheight(notice.c_str()) <= 50 * zoom - 20 * zoom) break;
		}
		settextcolor(rgb(0, 0, 0));
	}

	windows.massage.width = max(windows.middle.width, textwidth(notice.c_str()) + 60 * zoom);
	windows.massage.height = 50 * zoom;
	windows.massage.x = windows.middle.x + windows.middle.width / 2 - windows.massage.width / 2;
	windows.massage.y = windows.middle.y + windows.middle.height + 10;

	loadimage(background_img + 5, (convert_to_wstring(global_path) + L"img\\background.png").c_str(), windows.massage.width, windows.massage.height, true);
	if (grey_pattern) putimage(windows.massage.x, windows.massage.y, ColorToGray(background_img + 5));
	else putimage(windows.massage.x, windows.massage.y, background_img + 5);

	{
		words_rect.left = windows.massage.x + 10 * zoom;
		words_rect.top = windows.massage.y;
		words_rect.right = windows.massage.x + windows.massage.width - 50 * zoom;
		words_rect.bottom = windows.massage.y + windows.massage.height;
	}
	drawtext(notice.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	if (grey_pattern) transparentimage(hiex::GetWindowImage(), windows.massage.x + windows.massage.width - 45 * zoom, windows.massage.y + 5 * zoom, &ico_img[L"colse-grey"]);
	else transparentimage(hiex::GetWindowImage(), windows.massage.x + windows.massage.width - 45 * zoom, windows.massage.y + 5 * zoom, &ico_img[L"colse"]);

	HRGN whole_rgn = CreateRoundRectRgn(windows.massage.x, windows.massage.y, windows.massage.x + windows.massage.width, windows.massage.y + windows.massage.height, 15, 15);
	HRGN frame_rgn = whole_rgn;

	CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.middle.x, windows.middle.y, windows.middle.x + windows.middle.width, windows.middle.y + windows.middle.height, 15, 15), RGN_OR);
	if (windows.left.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.left.x, windows.left.y, windows.left.x + windows.left.width, windows.left.y + windows.left.height, 15, 15), RGN_OR);
	if (windows.right.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.right.x, windows.right.y, windows.right.x + windows.right.width, windows.right.y + windows.right.height, 15, 15), RGN_OR);

	FrameRgn(GetImageHDC(hiex::GetWindowImage()), frame_rgn, CreateSolidBrush(RGB(4, 88, 184)), 2, 2);
	SetWindowRgn(main_window, whole_rgn, true);

	windows.massage.is_open = true;
}
//程序消息弹窗
void massagebox(wstring notice, wstring titles, UINT type, wstring id = L"", IMAGE img = ico_img[L"info"])
{
	if (type == MB_OK)
	{
		struct edge
		{
			int x, y;
			int height, width;
		};
		edge words, title, bottom, whole;

		wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
		font.lfHeight = 25;
		settextstyle(&font);

		RECT t;
		drawtext(notice.c_str(), &t, DT_CALCRECT);
		words.width = t.right - t.left + 30, words.height = t.bottom - t.top + 20;
		words.x = 360 - words.width / 2, words.y = 202 - words.height / 2;
		title.width = words.width + 10, title.height = 50;
		title.x = words.x - 5, title.y = words.y - 50;
		bottom.width = title.width, bottom.height = 50;
		bottom.x = title.x, bottom.y = words.y + words.height;
		whole.width = title.width, whole.height = bottom.y + bottom.height - title.y;
		whole.x = title.x, whole.y = title.y;

		settextcolor(RGB(0, 0, 0));
		setlinecolor(RGB(35, 96, 235));
		setlinestyle(PS_SOLID, 1);

		solidroundrect_alpha(whole.x, whole.y, whole.x + whole.width, whole.y + whole.height, 10, 10, 240);
		roundrect(whole.x, whole.y, whole.x + whole.width, whole.y + whole.height, 10, 10);

		{
			words_rect.left = words.x + 15;
			words_rect.top = words.y + 10;
			words_rect.right = words.x + words.width - 15;
			words_rect.bottom = words.y + words.height - 10;
		}
		drawtext(notice.c_str(), &words_rect, NULL);

		font.lfHeight = 30;
		settextstyle(&font);
		transparentimage(hiex::GetWindowImage(), title.x + 11, title.y + 9, &img);
		{
			words_rect.left = title.x + 50;
			words_rect.top = title.y;
			words_rect.right = title.x + title.width;
			words_rect.bottom = title.y + title.height;
		}
		drawtext(titles.c_str(), &words_rect, DT_VCENTER | DT_SINGLELINE);
	}
}