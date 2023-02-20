#include "main.h"

wstring bkdraw_state;
map<int, int> bkdraw_state_each;
void bkdraw()
{
	while (1)
	{
		Sleep(10);
		if (bkdraw_state == L"主界面")
		{
			wchar_t tmp[250];
			wsprintf(tmp, L"%04d年%02d月%02d日 %02d:%02d:%02d ", Year, Month, Day, Hour, Minute, Second);

			if (DayOfWeek == 1) wcscat(tmp, L"星期一");
			else if (DayOfWeek == 2) wcscat(tmp, L"星期二");
			else if (DayOfWeek == 3) wcscat(tmp, L"星期三");
			else if (DayOfWeek == 4) wcscat(tmp, L"星期四");
			else if (DayOfWeek == 5) wcscat(tmp, L"星期五");
			else if (DayOfWeek == 6) wcscat(tmp, L"星期六");
			else if (DayOfWeek == 7) wcscat(tmp, L"星期天");
			else wcscat(tmp, (L"星期错误:" + to_wstring(DayOfWeek)).c_str());

			BeginDraw;

			clear_background();

			solidroundrect_alpha(20, 20, 290, 100, 30, 30, 255, bkdraw_state_each[1] == 0 ? RGB(255, 220, 220) : (bkdraw_state_each[1] == 1 ? RGB(235, 200, 200) : RGB(215, 180, 180)));
			solidroundrect_alpha(310, 20, 465, 55, 30, 30, 150, bkdraw_state_each[2] == 0 ? RGB(255, 255, 255) : (bkdraw_state_each[2] == 1 ? RGB(235, 235, 235) : RGB(215, 215, 215)));
			solidroundrect_alpha(475, 20, 630, 55, 30, 30, 150, bkdraw_state_each[5] == 0 ? RGB(255, 255, 255) : (bkdraw_state_each[5] == 1 ? RGB(235, 235, 235) : RGB(215, 215, 215)));
			solidroundrect_alpha(310, 65, 465, 100, 30, 30, 150, bkdraw_state_each[11] == 0 ? RGB(255, 255, 255) : (bkdraw_state_each[11] == 1 ? RGB(235, 235, 235) : RGB(215, 215, 215)));
			solidroundrect_alpha(475, 65, 630, 100, 30, 30, 150, bkdraw_state_each[12] == 0 ? RGB(255, 255, 255) : (bkdraw_state_each[12] == 1 ? RGB(235, 235, 235) : RGB(215, 215, 215)));

			solidroundrect_alpha(20, 120, 630, 320, 30, 30, 150);

			wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
			font.lfHeight = 40;
			settextstyle(&font);
			settextcolor(RGB(0, 0, 0));

			words_rect = { 20, 20, 290, 105 };
			drawtext(L"关闭主程序", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			font.lfHeight = 20;
			settextstyle(&font);

			words_rect = { 310, 20, 465, 58 };
			drawtext(L"重读配置文件", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			words_rect = { 475, 20, 630, 58 };
			drawtext(L"修改课表文件", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			font.lfHeight = 20;
			settextstyle(&font);

			words_rect = { 310, 65, 465, 103 };
			drawtext(L"设置开机启动", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			words_rect = { 475, 65, 630, 103 };
			drawtext(L"关闭开机启动", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			font.lfHeight = 22;
			settextstyle(&font);

			words_rect = { 20, 120, 630, 165 };
			drawtext(edition, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			transparentimage(hiex::GetWindowImage(), 62, 160, sign_img + 1);

			putimage(20, 322, 610, 28, &background_img, 20, 322);

			font.lfHeight = 20;
			settextstyle(&font);
			words_rect = { 20, 322, 630, 350 };
			drawtext(tmp, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			EndDraw;
		}
	}
}
void model(wstring s)
{
	if (s == L"主界面")
	{
		bkdraw_state = s;

		int state;

		ExMessage m;
		while (1)
		{
			getmessage(&m, EM_MOUSE);

			//关闭主程序:1
			if (m.x >= 20 && m.y >= 20 && m.x <= 290 && m.y <= 100)
			{
				state = 1;
				int ux = 20, uy = 20, dx = 290, dy = 100, r = 30;

				bkdraw_state_each[state] = 1;

				if (m.lbutton)
				{
					bkdraw_state_each[state] = 2;

					bool willin = false;

					while (1)
					{
						m = getmessage(EM_MOUSE);
						if (m.x >= ux && m.y >= uy && m.x <= dx && m.y <= dy)
						{
							bkdraw_state_each[state] = 2;

							if (!m.lbutton)
							{
								bkdraw_state_each[state] = 0;

								willin = true;
								break;
							}
						}
						else
						{
							bkdraw_state_each[state] = 0;

							if (!m.lbutton) break;
						}
					}

					if (willin)
					{
						//内容部分
						wofstream outfile;
						outfile.imbue(locale("zh_CN.UTF8"));
						outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
						outfile << L"1";
						outfile.close();
						exit(0);
					}
				}
			}
			//重读配置文件:2
			else if (m.x >= 310 && m.y >= 20 && m.x <= 465 && m.y <= 55)
			{
				state = 2;
				int ux = 310, uy = 20, dx = 430, dy = 55, r = 30;

				bkdraw_state_each[state] = 1;

				if (m.lbutton)
				{
					bkdraw_state_each[state] = 2;

					bool willin = false;

					while (1)
					{
						m = getmessage(EM_MOUSE);
						if (m.x >= ux && m.y >= uy && m.x <= dx && m.y <= dy)
						{
							bkdraw_state_each[state] = 2;

							if (!m.lbutton)
							{
								bkdraw_state_each[state] = 0;

								willin = true;
								break;
							}
						}
						else
						{
							bkdraw_state_each[state] = 0;

							if (!m.lbutton) break;
						}
					}

					if (willin)
					{
						//内容部分
						read_database();
						wofstream outfile;
						outfile.imbue(locale("zh_CN.UTF8"));
						outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
						outfile << L"2";
						outfile.close();

						getmessage(&m, EM_MOUSE);
					}
				}
			}
			//课表设置:5
			else if (m.x >= 475 && m.y >= 20 && m.x <= 630 && m.y <= 55)
			{
				state = 5;
				int ux = 435, uy = 20, dx = 550, dy = 55, r = 30;

				bkdraw_state_each[state] = 1;

				if (m.lbutton)
				{
					bkdraw_state_each[state] = 2;

					bool willin = false;

					while (1)
					{
						m = getmessage(EM_MOUSE);
						if (m.x >= ux && m.y >= uy && m.x <= dx && m.y <= dy)
						{
							bkdraw_state_each[state] = 2;

							if (!m.lbutton)
							{
								bkdraw_state_each[state] = 0;

								willin = true;
								break;
							}
						}
						else
						{
							bkdraw_state_each[state] = 0;

							if (!m.lbutton) break;
						}
					}

					if (willin)
					{
						//内容部分
						ShellExecute(nullptr, _T("open"), (convert_to_wstring(global_path) + L"opt\\timetable.txt").c_str(), _T(""), _T(""), SW_SHOW);

						getmessage(&m, EM_MOUSE);
					}
				}
			}
			//设置开机启动:11
			else if (m.x >= 310 && m.y >= 65 && m.x <= 465 && m.y <= 100)
			{
				state = 11;
				int ux = 310, uy = 65, dx = 430, dy = 100, r = 30;

				bkdraw_state_each[state] = 1;

				if (m.lbutton)
				{
					bkdraw_state_each[state] = 2;

					bool willin = false;

					while (1)
					{
						m = getmessage(EM_MOUSE);
						if (m.x >= ux && m.y >= uy && m.x <= dx && m.y <= dy)
						{
							bkdraw_state_each[state] = 2;

							if (!m.lbutton)
							{
								bkdraw_state_each[state] = 0;

								willin = true;
								break;
							}
						}
						else
						{
							bkdraw_state_each[state] = 0;

							if (!m.lbutton) break;
						}
					}

					if (willin)
					{
						//内容部分
						ModifyRegedit(true);
					}
				}
			}
			//关闭开机启动:12
			else if (m.x >= 475 && m.y >= 65 && m.x <= 630 && m.y <= 100)
			{
				state = 12;
				int ux = 435, uy = 65, dx = 550, dy = 100, r = 30;

				bkdraw_state_each[state] = 1;

				if (m.lbutton)
				{
					bkdraw_state_each[state] = 2;

					bool willin = false;

					while (1)
					{
						m = getmessage(EM_MOUSE);
						if (m.x >= ux && m.y >= uy && m.x <= dx && m.y <= dy)
						{
							bkdraw_state_each[state] = 2;

							if (!m.lbutton)
							{
								bkdraw_state_each[state] = 0;

								willin = true;
								break;
							}
						}
						else
						{
							bkdraw_state_each[state] = 0;

							if (!m.lbutton) break;
						}
					}

					if (willin)
					{
						//内容部分
						ModifyRegedit(false);
					}
				}
			}

			else
			{
				bkdraw_state_each[state] = 0;
			}
		}
	}
}

int main()
{
	//程序初始化部分
	{
		//全局路径预处理
		{
			global_path = _pgmptr;
			for (int i = int(global_path.length() - 1); i >= 0; i--)
			{
				if (global_path[i] == '\\')
				{
					global_path = global_path.substr(0, i + 1);
					break;
				}
			}
		}
		//多线程并发
		{
			thread settime_thread(settime);
			settime_thread.detach();
			thread bkdraw_thread(bkdraw);
			bkdraw_thread.detach();

			//ModifyRegedit(true);
		}
		//媒体资源读取
		{
			//背景图像
			{
				loadimage(&background_img, (convert_to_wstring(global_path) + L"img\\background.png").c_str(), 650, 350, true);
			}
			//标志图像
			{
				loadimage(sign_img + 1, (convert_to_wstring(global_path) + L"img\\sign\\3.png").c_str(), 540, 158, true);
			}
			//程序版本展示
			{
				wchar_t tmp[50];
				wifstream infile;
				infile.imbue(locale("zh_CN.UTF8"));
				infile.open(convert_to_wstring(global_path) + L"opt\\edition.txt");
				infile >> tmp;
				infile.close();

				wcscat(edition, tmp);
				wcscat(edition, L" - ");
			}
		}

		//程序初始化
		{
			//DPI 初始化
			{
				HINSTANCE hUser32 = LoadLibrary(L"User32.dll");
				if (hUser32)
				{
					typedef BOOL(WINAPI* LPSetProcessDPIAware)(void);
					LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress(hUser32, "SetProcessDPIAware");
					if (pSetProcessDPIAware)
					{
						pSetProcessDPIAware();
					}
					FreeLibrary(hUser32);
				}
			}

			initgraph(650, 350);
			SetWindowTextW(GetHWnd(), L"下课铃 - 选项窗口");

			setbkmode(TRANSPARENT);
			setbkcolor(RGB(255, 255, 255));

			BeginDraw;
			clear_background();
			EndDraw;

			Sleep(100);

			DisableResizing(GetHWnd(), true);//禁止窗口拉伸
			SetWindowPos(GetHWnd(), NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);//窗口置顶
			SetWindowPos(GetHWnd(), NULL, windows.x = GetSystemMetrics(SM_CXSCREEN) / 2 - 325, windows.y = GetSystemMetrics(SM_CYSCREEN) / 2 - 200, windows.width = 650, windows.height = 350, SWP_NOSIZE);

			setWindowTransparent(GetHWnd(), true, windows.translucent = 230);

			imageblock.CreateCanvas(650, 350);
		}

		//读取数据库
		read_database();
		//字体初始化部分
		{
			gettextstyle(&font);
			font.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			font.lfQuality = ANTIALIASED_QUALITY | PROOF_QUALITY;

			AddFontResourceEx((convert_to_wstring(global_path) + L"ttf\\HarmonyOS_Sans_SC_Regular.ttf").c_str(), FR_PRIVATE, NULL);
			AddFontResourceEx((convert_to_wstring(global_path) + L"ttf\\Douyu_Font.otf").c_str(), FR_PRIVATE, NULL);

			//wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
			//wcscpy(font.lfFaceName, L"DOUYU Font");
		}
	}

	wcscat(edition, L"设置程序版本：V1.1.1");

	model(L"主界面");

	return 0;
}