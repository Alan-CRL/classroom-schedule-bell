#include "main.h"

wstring bkdraw_state;
map<int, int> bkdraw_state_each;
void bkdraw()
{
	while (1)
	{
		Sleep(10);
		if (bkdraw_state == L"������")
		{
			wchar_t tmp[250];
			wsprintf(tmp, L"%04d��%02d��%02d�� %02d:%02d:%02d ", Year, Month, Day, Hour, Minute, Second);

			if (DayOfWeek == 1) wcscat(tmp, L"����һ");
			else if (DayOfWeek == 2) wcscat(tmp, L"���ڶ�");
			else if (DayOfWeek == 3) wcscat(tmp, L"������");
			else if (DayOfWeek == 4) wcscat(tmp, L"������");
			else if (DayOfWeek == 5) wcscat(tmp, L"������");
			else if (DayOfWeek == 6) wcscat(tmp, L"������");
			else if (DayOfWeek == 7) wcscat(tmp, L"������");
			else wcscat(tmp, (L"���ڴ���:" + to_wstring(DayOfWeek)).c_str());

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
			drawtext(L"�ر�������", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			font.lfHeight = 20;
			settextstyle(&font);

			words_rect = { 310, 20, 465, 58 };
			drawtext(L"�ض������ļ�", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			words_rect = { 475, 20, 630, 58 };
			drawtext(L"�޸Ŀα��ļ�", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			font.lfHeight = 20;
			settextstyle(&font);

			words_rect = { 310, 65, 465, 103 };
			drawtext(L"���ÿ�������", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			words_rect = { 475, 65, 630, 103 };
			drawtext(L"�رտ�������", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

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
	if (s == L"������")
	{
		bkdraw_state = s;

		int state;

		ExMessage m;
		while (1)
		{
			getmessage(&m, EM_MOUSE);

			//�ر�������:1
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
						//���ݲ���
						wofstream outfile;
						outfile.imbue(locale("zh_CN.UTF8"));
						outfile.open(convert_to_wstring(global_path) + L"opt\\variable.txt");
						outfile << L"1";
						outfile.close();
						exit(0);
					}
				}
			}
			//�ض������ļ�:2
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
						//���ݲ���
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
			//�α�����:5
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
						//���ݲ���
						ShellExecute(nullptr, _T("open"), (convert_to_wstring(global_path) + L"opt\\timetable.txt").c_str(), _T(""), _T(""), SW_SHOW);

						getmessage(&m, EM_MOUSE);
					}
				}
			}
			//���ÿ�������:11
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
						//���ݲ���
						ModifyRegedit(true);
					}
				}
			}
			//�رտ�������:12
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
						//���ݲ���
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
	//�����ʼ������
	{
		//ȫ��·��Ԥ����
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
		//���̲߳���
		{
			thread settime_thread(settime);
			settime_thread.detach();
			thread bkdraw_thread(bkdraw);
			bkdraw_thread.detach();

			//ModifyRegedit(true);
		}
		//ý����Դ��ȡ
		{
			//����ͼ��
			{
				loadimage(&background_img, (convert_to_wstring(global_path) + L"img\\background.png").c_str(), 650, 350, true);
			}
			//��־ͼ��
			{
				loadimage(sign_img + 1, (convert_to_wstring(global_path) + L"img\\sign\\3.png").c_str(), 540, 158, true);
			}
			//����汾չʾ
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

		//�����ʼ��
		{
			//DPI ��ʼ��
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
			SetWindowTextW(GetHWnd(), L"�¿��� - ѡ���");

			setbkmode(TRANSPARENT);
			setbkcolor(RGB(255, 255, 255));

			BeginDraw;
			clear_background();
			EndDraw;

			Sleep(100);

			DisableResizing(GetHWnd(), true);//��ֹ��������
			SetWindowPos(GetHWnd(), NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);//�����ö�
			SetWindowPos(GetHWnd(), NULL, windows.x = GetSystemMetrics(SM_CXSCREEN) / 2 - 325, windows.y = GetSystemMetrics(SM_CYSCREEN) / 2 - 200, windows.width = 650, windows.height = 350, SWP_NOSIZE);

			setWindowTransparent(GetHWnd(), true, windows.translucent = 230);

			imageblock.CreateCanvas(650, 350);
		}

		//��ȡ���ݿ�
		read_database();
		//�����ʼ������
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

	wcscat(edition, L"���ó���汾��V1.1.1");

	model(L"������");

	return 0;
}