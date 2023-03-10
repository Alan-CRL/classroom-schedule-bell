#include "main.h"
//关于源码的环境配置，请查看 github 页面

int main()
{
	hiex::EnableAutoFlush(false);
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
			thread settime_thread(settime);//时间管理进程
			settime_thread.detach();
			thread click_time_thread(click_time);//时间计算进程
			click_time_thread.detach();
			thread automatic_update_thread(automatic_update);
			automatic_update_thread.detach();
			thread window_form_management_thread(window_form_management);//窗口形态管理进程
			window_form_management_thread.detach();
			thread automatic_topping_thread(automatic_topping);
			automatic_topping_thread.detach();
			thread mouse_interaction_thread(mouse_interaction);
			mouse_interaction_thread.detach();
			thread play_clew_tone_thread(play_clew_tone);//提示音管理进程
			play_clew_tone_thread.detach();
			thread program_operation_management_thread(program_operation_management);
			program_operation_management_thread.detach();

			//ModifyRegedit(true);
		}

		//zoom 信息获取
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
				infile >> useless;
				getline(infile, useless);

				//音量读取
				getline(infile, useless);
				infile >> useless;
				getline(infile, useless);

				//缩放读取
				getline(infile, useless);
				infile >> zoom;
				getline(infile, useless);
			}

			infile.close();
		}
		//媒体资源读取
		{
			//背景图像
			{
				loadimage(background_img + 2, (convert_to_wstring(global_path) + L"img\\background.png").c_str(), ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4, (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom, true);
				loadimage(background_img + 3, (convert_to_wstring(global_path) + L"img\\background_bing.jpg").c_str(), 720, 405, true);

				loadimage(background_img + 4, (convert_to_wstring(global_path) + L"img\\background.png").c_str(), (GetSystemMetrics(SM_CXSCREEN)) / 2.0 - (((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4 / 2.0) - 20, (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom, true);
			}
			//标志图像
			{
				loadimage(sign_img + 1, (convert_to_wstring(global_path) + L"img\\sign\\1.png").c_str(), 457, 320, true);

				loadimage(&ico_img[L"colse"], (convert_to_wstring(global_path) + L"img\\ico\\close.png").c_str(), 40 * zoom, 40 * zoom, true);
				loadimage(&ico_img[L"close-grey"], (convert_to_wstring(global_path) + L"img\\ico\\close-grey.png").c_str(), 40 * zoom, 40 * zoom, true);
				loadimage(&ico_img[L"info"], (convert_to_wstring(global_path) + L"img\\ico\\info.png").c_str(), 30, 30, true);
				loadimage(&ico_img[L"warning"], (convert_to_wstring(global_path) + L"img\\ico\\warning.png").c_str(), 30, 30, true);
				loadimage(&ico_img[L"attention"], (convert_to_wstring(global_path) + L"img\\ico\\attention.png").c_str(), 30, 30, true);
			}
			//天气图标
			{
				//正常颜色
				{
					loadimage(weathar_img + 70, (convert_to_wstring(global_path) + L"img\\weather\\common\\999.png").c_str(), 50, 50, true);
				}
				//灰度颜色
				{
					loadimage(weathar_gray_img + 70, (convert_to_wstring(global_path) + L"img\\weather\\grayscale\\999.png").c_str(), 50, 50, true);
				}
			}
		}

		//读取数据库
		read_database();

		//窗口初始化部分
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

			windows.middle.x = (GetSystemMetrics(SM_CXSCREEN)) / 2.0 - (((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4 / 2.0);
			windows.middle.width = ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4, windows.left.height = windows.middle.height = windows.right.height = (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom;

			main_window = initgraph(GetSystemMetrics(SM_CXSCREEN), (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom + 10 + 50 * zoom);
			SetWindowRgn(main_window, CreateRoundRectRgn(windows.middle.x, 0, windows.middle.x + ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4, (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom, 15, 15), true);
			hiex::SetWorkingWindow(main_window);

			setbkmode(TRANSPARENT);
			setbkcolor(rgb(255, 255, 255));

			BeginDraw();
			clear_background();
			hiex::FlushDrawing({ 0 });
			EndDraw();

			DisableResizing(main_window, true);//禁止窗口拉伸
			SetWindowLong(main_window, GWL_STYLE, GetWindowLong(main_window, GWL_STYLE) & ~WS_CAPTION);//隐藏标题栏
			SetWindowPos(main_window, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);//窗口置顶
			SetWindowLong(main_window, GWL_EXSTYLE, WS_EX_TOOLWINDOW);//隐藏任务栏

			SetWindowRgn(main_window, CreateRoundRectRgn(windows.middle.x, 0, windows.middle.x + ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom) * 4, (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom, 15, 15), true);
			SetWindowPos(main_window, NULL, windows.x = 0, windows.y = int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)), GetSystemMetrics(SM_CXSCREEN), (GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom, SWP_NOSIZE);

			window_status = 0;

			setWindowTransparent(main_window, true, windows.translucent = 235);

			BeginDraw();
			setfillcolor(rgb(16, 110, 136));
			solidrectangle(0, windows.middle.height - 15, windows.middle.width, windows.middle.height);
			hiex::FlushDrawing({ 0 }); EndDraw();
		}

		//字体初始化部分
		{
			gettextstyle(&font);
			font.lfOutPrecision = OUT_TT_ONLY_PRECIS;
			font.lfQuality = ANTIALIASED_QUALITY | PROOF_QUALITY;

			AddFontResourceEx((convert_to_wstring(global_path) + L"ttf\\HarmonyOS_Sans_SC_Regular.ttf").c_str(), FR_PRIVATE, NULL);
			AddFontResourceEx((convert_to_wstring(global_path) + L"ttf\\Douyu_Font.otf").c_str(), FR_PRIVATE, NULL);
			AddFontResourceEx((convert_to_wstring(global_path) + L"ttf\\SmileySans-Oblique.ttf").c_str(), FR_PRIVATE, NULL);

			//wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
			//wcscpy(font.lfFaceName, L"DOUYU Font");
			//wcscpy(font.lfFaceName, L"得意黑");
		}
		//goto next;
		/*程序开局动画*/
		{
			wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
			settextstyle(&font);

			{
				wifstream infile;
				infile.imbue(locale("zh_CN.UTF8"));
				infile.open(convert_to_wstring(global_path) + L"opt\\reliance\\background_copyright.txt");

				getline(infile, copyright_bing);

				infile.close();
			}

			launch_window = initgraph(720, 405);
			SetWindowRgn(launch_window, CreateRoundRectRgn(0, 0, 720, 405, 0, 0), true);
			hiex::SetWorkingWindow(launch_window);

			setbkmode(TRANSPARENT);
			setbkcolor(rgb(255, 255, 255));

			BeginDraw();
			putimage(0, 0, background_img + 3);
			hiex::FlushDrawing({ 0 }); EndDraw();

			Sleep(100);

			DisableResizing(launch_window, true);//禁止窗口拉伸
			SetWindowLong(launch_window, GWL_STYLE, GetWindowLong(launch_window, GWL_STYLE) & ~WS_CAPTION);//隐藏标题栏
			SetWindowPos(launch_window, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);//窗口置顶
			SetWindowPos(launch_window, NULL, GetSystemMetrics(SM_CXSCREEN) / 2 - 360, GetSystemMetrics(SM_CYSCREEN) / 2 - 220, 720, 405, SWP_NOSIZE);
			SetWindowLong(launch_window, GWL_EXSTYLE, WS_EX_TOOLWINDOW);//隐藏任务栏

			SetWindowRgn(launch_window, CreateRoundRectRgn(0, 0, 720, 405, 0, 0), true);

			setlinecolor(rgb(0, 0, 0));
			setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 2);

			BeginDraw();
			putimage(0, 0, background_img + 3);
			rectangle(1, 1, 718, 403);
			transparentimage(hiex::GetWindowImage(), 131, 20, sign_img + 1);

			{
				font.lfHeight = 20;
				settextstyle(&font);
				settextcolor(rgb(0, 0, 0));
				{
					words_rect.left = 0;
					words_rect.top = 0;
					words_rect.right = 714;
					words_rect.bottom = 385;
				}
				drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
				{
					words_rect.left = 0;
					words_rect.top = 0;
					words_rect.right = 716;
					words_rect.bottom = 385;
				}
				drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
				{
					words_rect.left = 0;
					words_rect.top = 0;
					words_rect.right = 715;
					words_rect.bottom = 384;
				}
				drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
				{
					words_rect.left = 0;
					words_rect.top = 0;
					words_rect.right = 715;
					words_rect.bottom = 386;
				}
				drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

				font.lfHeight = 20;
				settextstyle(&font);
				settextcolor(rgb(255, 255, 255));
				{
					words_rect.left = 0;
					words_rect.top = 0;
					words_rect.right = 715;
					words_rect.bottom = 385;
				}
				drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
			}

			hiex::FlushDrawing({ 0 }); EndDraw();
			//massagebox(L"地理位置信息获取失败！\n\n这将导致无法正常使用组件功能\n建议前往程序设置手动输入", L"警告", MB_OK, L"warning 1", ico_img[L"warning"]);

			//初始化数值获取
			{
				//地理位置检测获取
				{
					bool already = false;
					if (_waccess((convert_to_wstring(global_path) + L"opt\\location.json").c_str(), 4) == -1) already = true;
					else
					{
						wstring useless;

						Json::Reader reader;
						Json::Value root;

						ifstream injson;
						injson.imbue(locale("zh_CN.UTF8"));
						injson.open(convert_to_string(convert_to_wstring(global_path) + L"opt\\location.json").c_str());

						if (reader.parse(injson, root))
						{
							if (root["edition"].asString() != "V1.1") already = true;
						}
						else already = true;
					}

					if (already && checkIsNetwork())
					{
						HRESULT res1;
						{
							DeleteUrlCacheEntry(L"http://api.a20safe.com");
							res1 = URLDownloadToFileW( // 从网络上下载数据到本地文件
								nullptr,                  // 在这里，写 nullptr 就行
								L"https://api.vore.top/api/IPdata", // 在这里写上网址
								stringtoLPCWSTR(global_path + "tmp\\position.txt"),            // 文件名写在这
								0,                        // 写 0 就对了
								nullptr                   // 也是，在这里写 nullptr 就行
							);
						}

						{
							wstring useless, adm1, adm2, name, adcode;

							Json::Reader reader;
							Json::Value root;

							ifstream injson;
							injson.imbue(locale("zh_CN.UTF8"));
							injson.open(convert_to_string(convert_to_wstring(global_path) + L"tmp\\position.txt").c_str());

							if (reader.parse(injson, root))
							{
								if (root["code"].asInt() == 200)
								{
									adm1 = convert_to_wstring(convert_to_gbk(root["ipdata"]["info1"].asString()));
									adm2 = convert_to_wstring(convert_to_gbk(root["ipdata"]["info2"].asString()));
									name = convert_to_wstring(convert_to_gbk(root["ipdata"]["info3"].asString()));
									adcode = convert_to_wstring(convert_to_gbk(root["adcode"]["a"].asString()));

									injson.close();

									if (adm1 != L"" && adm2 != L"")
									{
										wstring web_file;

										if (name == L"")
										{
											web_file = L"https://geoapi.qweather.com/v2/city/lookup?key=27ece0cdf2ae48f18d46d3593ea2fb5b&number=1&adm=" + convert_to_wstring(convert_to_urlencode(convert_to_utf8(convert_to_string(adm1)))) + L"&location=" + convert_to_wstring(convert_to_urlencode(convert_to_utf8(convert_to_string(adm2))));
										}
										else web_file = L"https://geoapi.qweather.com/v2/city/lookup?key=27ece0cdf2ae48f18d46d3593ea2fb5b&number=1&adm1=" + convert_to_wstring(convert_to_urlencode(convert_to_utf8(convert_to_string(adm1)))) + L"&adm2=" + convert_to_wstring(convert_to_urlencode(convert_to_utf8(convert_to_string(adm2)))) + L"&location=" + convert_to_wstring(convert_to_urlencode(convert_to_utf8(convert_to_string(name))));

										HRESULT res3;
										{
											DeleteUrlCacheEntry(L"https://geoapi.qweather.com");
											res3 = URLDownloadToFileW( // 从网络上下载数据到本地文件
												nullptr,                  // 在这里，写 nullptr 就行
												web_file.c_str(), // 在这里写上网址
												stringtoLPCWSTR(global_path + "tmp\\location.txt"),            // 文件名写在这
												0,                        // 写 0 就对了
												nullptr                   // 也是，在这里写 nullptr 就行
											);
										}

										if (res3 == S_OK)
										{
											wstring useless, id, lat, lon;

											Json::Reader reader;
											Json::Value root;

											ifstream injson;
											injson.imbue(locale("zh_CN.UTF8"));
											injson.open(convert_to_string(convert_to_wstring(global_path) + L"tmp\\location.txt").c_str());

											if (reader.parse(injson, root))
											{
												if (root["code"].asString() == "200")
												{
													name = convert_to_wstring(convert_to_gbk(root["location"][0]["name"].asString()));
													id = convert_to_wstring(convert_to_gbk(root["location"][0]["id"].asString()));
													lat = convert_to_wstring(convert_to_gbk(root["location"][0]["lat"].asString()));
													lon = convert_to_wstring(convert_to_gbk(root["location"][0]["lon"].asString()));
													adm2 = convert_to_wstring(convert_to_gbk(root["location"][0]["adm2"].asString()));
													adm1 = convert_to_wstring(convert_to_gbk(root["location"][0]["adm1"].asString()));

													injson.close();

													Json::StyledWriter outjson;
													Json::Value root;

													root["edition"] = Json::Value("V1.1");
													root["record"]["LocationID"] = Json::Value(convert_to_string(id));
													root["record"]["Adcode"] = Json::Value(convert_to_string(adcode));
													root["record"]["position"]["lat"] = Json::Value(convert_to_string(lat));
													root["record"]["position"]["lon"] = Json::Value(convert_to_string(lon));
													root["record"]["location"]["adm1"] = Json::Value(convert_to_utf8(convert_to_string(adm1)));
													root["record"]["location"]["adm2"] = Json::Value(convert_to_utf8(convert_to_string(adm2)));
													root["record"]["location"]["name"] = Json::Value(convert_to_utf8(convert_to_string(name)));

													ofstream outfile;
													outfile.imbue(locale("zh_CN.UTF8"));
													outfile.open(convert_to_string(convert_to_wstring(global_path) + L"opt\\location.json").c_str());
													outfile << outjson.write(root);
													outfile.close();

													MessageBox(NULL, (L"地理位置获取成功：\nLocationID: " + id + L"\nAdcode: " + adcode + L"\n" + lat + L"," + lon + L"\n" + adm1 + L" " + adm2 + L" " + (name == adm2 ? L"" : name) + L"\n\n可在联网状态下自动获取天气及疫情数据").c_str(), L"提示", MB_OK | MB_SYSTEMMODAL);
												}
											}
										}
									}
								}
							}
						}
					}
				}

				//灰度模式获取
				{
					bool success = false;

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

						if (res == S_OK) success = true;
					}

					if (success)
					{
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
						if (flag) grey_pattern = true;
						else grey_pattern = false;

						infile.close();
					}
				}
			}

			for (int i = 4000; i >= 0; i -= 10)
			{
				BeginDraw();
				setlinecolor(rgb(0, 0, 0));
				setlinestyle(PS_SOLID | PS_ENDCAP_SQUARE, 2);

				putimage(0, 0, background_img + 3);
				transparentimage(hiex::GetWindowImage(), 131, 20, sign_img + 1);

				solidroundrect_alpha(0, 385, 720, 405, 0, 0, 100);
				solidroundrect_alpha(0, 385, 720.0 * (1.0 - double(i) / double(4000)), 405, 0, 0, 200, RGB(35, 96, 235));

				rectangle(1, 1, 718, 403);

				{
					font.lfHeight = 20;
					settextstyle(&font);
					settextcolor(rgb(0, 0, 0));
					{
						words_rect.left = 0;
						words_rect.top = 0;
						words_rect.right = 714;
						words_rect.bottom = 385;
					}
					drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
					{
						words_rect.left = 0;
						words_rect.top = 0;
						words_rect.right = 716;
						words_rect.bottom = 385;
					}
					drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
					{
						words_rect.left = 0;
						words_rect.top = 0;
						words_rect.right = 715;
						words_rect.bottom = 384;
					}
					drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
					{
						words_rect.left = 0;
						words_rect.top = 0;
						words_rect.right = 715;
						words_rect.bottom = 386;
					}
					drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

					font.lfHeight = 20;
					settextstyle(&font);
					settextcolor(rgb(255, 255, 255));
					{
						words_rect.left = 0;
						words_rect.top = 0;
						words_rect.right = 715;
						words_rect.bottom = 385;
					}
					drawtext(copyright_bing.c_str(), &words_rect, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
				}
				hiex::FlushDrawing({ 0 }); EndDraw();

				if (i < 255) setWindowTransparent(launch_window, true, i);
				Sleep(5);
			}

			closegraph(launch_window);
			hiex::SetWorkingWindow(main_window);
		}
	}
next:

	already = true;
	float COL = 0;
	//massagepop(L"全屏幕 DPI 适配，什么分辨率程序都一样");

	while (1)
	{
		//hiex::DelayFPS(60);

		int time = Second + Minute * 60 + Hour * 3600;
		if (state.special != -1 && (state_reset || time <= get<0>(state.begin) * 3600 + get<1>(state.begin) * 60 || time >= get<0>(state.end) * 3600 + get<1>(state.end) * 60))
		{
			confirmation_period();
			if (state_reset) state_reset = false, state.last_type = -1;

			if (state.type == 2 || state.type == 3) target_window_status = 0;

			if (state.type == 5 && state.last_type == -1) target_window_status = 1;

			if ((state.last_type == 2 || state.last_type == 4) && (state.type == 0 || state.type == 1 || state.type == 1.1))
			{
				if (GetVolum() != 0 && volume != 0) clew_tone = true;

				if (chrono::duration_cast<chrono::duration<double>>(chrono::steady_clock::now() - last_click_time).count() < 20)
				{
					target_window_status = 2;
				}
				else target_window_status = 1;
			}
			else if (state.type == 0 || state.type == 1 || state.type == 1.1 || state.type == 4) target_window_status = 1;

			if (state.type == 5 && message_send.preparation_before_class != L"") massagepop(message_send.preparation_before_class);
		}

		//主窗口绘制
		if (target_window_status == 0)
		{
			Sleep(10);

			if (windows.y == int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)) && state.type != 3)
			{
				BeginDraw();
				setfillcolor(rgb(16, 110, 136));
				solidrectangle(windows.middle.x + 0, windows.middle.height - 15, windows.middle.x + windows.middle.width, windows.middle.height);
				setfillcolor(rgb(255, 255, 255));
				solidrectangle(windows.middle.x + 5, windows.middle.height - 4 - windows.middle.height / 15.625, windows.middle.x + windows.middle.width - 6, windows.middle.height - 4);

				{
					//进度
					double speed_of_progress = double(time - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60)) / double((get<0>(state.end) * 3600 + get<1>(state.end) * 60) - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60));

					COL >= 360 ? COL = 0 : COL += 0.5f;
					float COR = COL;
					for (int i = 5; i <= 5 + (windows.middle.width - 11) * speed_of_progress; i++)
					{
						COLORREF color = HSLtoRGB(COR >= 360 ? COR = 0 : COR += 0.5f, 1, 0.7f);
						setfillcolor(rgb(GetRValue(color), GetGValue(color), GetBValue(color)));
						solidrectangle(windows.middle.x + i, windows.middle.height - 4 - windows.middle.height / 15.625, windows.middle.x + i, windows.middle.height - 4);
					}
					setfillcolor(BLACK);
					solidrectangle(windows.middle.x + max(5, 5 + (windows.middle.width - 11) * speed_of_progress - 1), windows.middle.height - 4 - windows.middle.height / 15.625, windows.middle.x + 5 + (windows.middle.width - 11) * speed_of_progress, windows.middle.height - 4);

					setlinecolor(rgb(16, 110, 136));
					setlinestyle(PS_SOLID, 3);
					roundrect(windows.middle.x + 3, windows.middle.height - 15, windows.middle.x + windows.middle.width - 5, windows.middle.height - 2, 15, 15);
				}

				hiex::FlushDrawing({ 0 }); EndDraw();
			}
			if (windows.y != int(-1 * ((GetSystemMetrics(SM_CYSCREEN) / 8.64) * zoom - 8)))
			{
				BeginDraw();
				putimage(windows.middle.x, 0, &back);
				hiex::FlushDrawing({ 0 }); EndDraw();
			}
		}
		else if (target_window_status == 2)
		{
			Sleep(10);
			//this_thread::sleep_for(chrono::nanoseconds(100));

			BeginDraw();
			setfillcolor(rgb(16, 110, 136));
			solidrectangle(windows.middle.x, windows.middle.y, windows.middle.x + windows.middle.width + 1, windows.middle.y + windows.middle.height + 1);

			{
				//下课突破20秒进度
				double speed_of_progress = double(chrono::duration_cast<chrono::duration<double>>(chrono::steady_clock::now() - last_click_time).count()) / 20.0;

				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 25, (windows.middle.height / 100.0) * 75, windows.middle.x + (windows.middle.height / 100.0) * 225, (windows.middle.height / 100.0) * 97, (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 230, (windows.middle.height / 100.0) * 75, windows.middle.x + (windows.middle.height / 100.0) * 395, (windows.middle.height / 100.0) * 97, (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, 200);

				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * (390 * speed_of_progress / 2), (windows.middle.height / 100.0) * 98, windows.middle.x + (windows.middle.height / 100.0) * (395 - (390 * speed_of_progress / 2)), (windows.middle.height / 100.0) * 100, 0, 0, 255, rgb(255, 0, 0));

				transparentimage(hiex::GetWindowImage(), windows.middle.x + (windows.middle.height / 100.0) * 3, (windows.middle.height / 100.0) * 76, &ico_img[L"attention"]);

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 32, (windows.middle.height / 100.0) * 78, L"当前为");
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 185, (windows.middle.height / 100.0) * 78, L"时段");

				wstring sht;
				if (state.type == 1 || state.type == 1.1) sht = L"课间休息";
				else sht = state.name;
				font.lfHeight = (windows.middle.height / 100.0) * 22;
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 82;
					words_rect.top = (windows.middle.height / 100.0) * 76;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 185;
					words_rect.bottom = (windows.middle.height / 100.0) * 97;
				}
				drawtext(sht.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(230, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 235, (windows.middle.height / 100.0) * 78, L"已经下课");

				int t1 = ((Hour * 3600 + Minute * 60 + Second) - (get<0>(state.begin) * 60 + get<1>(state.begin)) * 60) % 60;
				int t2 = (((Hour * 3600 + Minute * 60 + Second) - (get<0>(state.begin) * 60 + get<1>(state.begin)) * 60) - t1) / 60;

				wchar_t delay_time[250];
				wsprintf(delay_time, L"%02d:%02d", t2, t1);
				font.lfHeight = (windows.middle.height / 100.0) * 22;
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 302;
					words_rect.top = (windows.middle.height / 100.0) * 76;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 395;
					words_rect.bottom = (windows.middle.height / 100.0) * 97;
				}
				drawtext(delay_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			hiex::FlushDrawing({ windows.middle.x,windows.middle.y,windows.middle.x + windows.middle.width + 1,windows.middle.y + windows.middle.height + 1 });
			EndDraw();

			if (chrono::duration_cast<chrono::duration<double>>(chrono::steady_clock::now() - last_click_time).count() >= 20) target_window_status = 1;
		}
		else
		{
			Sleep(10);
			//this_thread::sleep_for(chrono::nanoseconds(100));

			BeginDraw();
			clear_background();

			if (state.special == -1)
			{
				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 9, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 55, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 198, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 202, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 35, L"当前为");
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 348, (windows.middle.height / 100.0) * 35, L"时段");

				wcscpy(font.lfFaceName, L"DOUYU Font");
				font.lfHeight = (windows.middle.height / 100.0) * 35;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 70;
					words_rect.top = (windows.middle.height / 100.0) * 17;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 348;
					words_rect.bottom = (windows.middle.height / 100.0) * 62;
				}
				drawtext(L"考试时段", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				wchar_t now_time[250] = L"";
				wsprintf(now_time, L"北京时间 %02d:%02d:%02d", sys_time.wHour, sys_time.wMinute, sys_time.wSecond);

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 25;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 10;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 198;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				wsprintf(now_time, L"广播时间 %02d:%02d:%02d", Hour, Minute, Second);

				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 202;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else if (state.type == 0)
			{
				//休息时段进度
				double speed_of_progress = double(time - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60)) / double((get<0>(state.end) * 3600 + get<1>(state.end) * 60) - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60));

				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 9, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 55, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 178, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 182, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 13, (windows.middle.height / 100.0) * 12, windows.middle.x + (windows.middle.height / 100.0) * (13 + 374 * speed_of_progress), (windows.middle.height / 100.0) * 53, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 120, rgb(0, 120, 255));

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 35, L"当前为");
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 348, (windows.middle.height / 100.0) * 35, L"时段");

				wcscpy(font.lfFaceName, L"DOUYU Font");
				font.lfHeight = (windows.middle.height / 100.0) * 35;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 70;
					words_rect.top = (windows.middle.height / 100.0) * 17;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 348;
					words_rect.bottom = (windows.middle.height / 100.0) * 62;
				}
				drawtext(state.name.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				wchar_t now_time[250] = L"";
				wsprintf(now_time, L"%02d:%02d ~ %02d:%02d", get<0>(state.begin), get<1>(state.begin), get<0>(state.end), get<1>(state.end));

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 25;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 10;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 178;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				wsprintf(now_time, L"%02d:%02d:%02d ", Hour, Minute, Second);
				if (DayOfWeek == 1) wcscat(now_time, L"星期一");
				if (DayOfWeek == 2) wcscat(now_time, L"星期二");
				if (DayOfWeek == 3) wcscat(now_time, L"星期三");
				if (DayOfWeek == 4) wcscat(now_time, L"星期四");
				if (DayOfWeek == 5) wcscat(now_time, L"星期五");
				if (DayOfWeek == 6) wcscat(now_time, L"星期六");
				if (DayOfWeek == 7) wcscat(now_time, L"星期日");

				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 182;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else if (state.type == 2 || state.type == 3 || state.type == 4)
			{
				//课程进度
				double speed_of_progress = double(time - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60)) / double((get<0>(state.end) * 3600 + get<1>(state.end) * 60) - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60));

				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 55, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 178, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 182, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 13, (windows.middle.height / 100.0) * 12, windows.middle.x + (windows.middle.height / 100.0) * (13 + 374 * speed_of_progress), (windows.middle.height / 100.0) * 53, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 120, rgb(0, 120, 255));

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 35, L"本节课是");

				wcscpy(font.lfFaceName, L"DOUYU Font");
				font.lfHeight = (windows.middle.height / 100.0) * 35;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 87;
					words_rect.top = (windows.middle.height / 100.0) * 17;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 62;
				}
				drawtext(state.name.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				int num = state.num;
				wchar_t now_time[250] = L"";
				if (time <= interval_time[DayOfWeek].lunch.first * 3600 + interval_time[DayOfWeek].lunch.second * 60) wcscat(now_time, L"上午第");
				else if (time <= interval_time[DayOfWeek].dinner.first * 3600 + interval_time[DayOfWeek].dinner.second * 60) wcscat(now_time, L"下午第"), num -= interval_time[DayOfWeek].morning;
				else wcscat(now_time, L"晚上第"), num -= interval_time[DayOfWeek].morning + interval_time[DayOfWeek].afternoon;
				wcscat(now_time, to_wstring(num).c_str());
				wcscat(now_time, L"节课");

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 25;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 10;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 178;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				int t1 = ((get<0>(state.end) * 60 + get<1>(state.end)) * 60 - (Hour * 3600 + Minute * 60 + Second)) % 60;
				int t2 = (((get<0>(state.end) * 60 + get<1>(state.end)) * 60 - (Hour * 3600 + Minute * 60 + Second)) - t1) / 60;

				wsprintf(now_time, L"距离下课 %02d′%02d″", t2, t1);

				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 182;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else if (state.type == 1 || state.type == 1.1)
			{
				//课间休息进度
				double speed_of_progress = double(time - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60)) / double((get<0>(state.end) * 3600 + get<1>(state.end) * 60) - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60));

				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 55, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 218, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 222, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 13, (windows.middle.height / 100.0) * 12, windows.middle.x + (windows.middle.height / 100.0) * (13 + 374 * speed_of_progress), (windows.middle.height / 100.0) * 53, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 120, rgb(0, 120, 255));

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 35, L"当前为");
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 348, (windows.middle.height / 100.0) * 35, L"时段");

				wcscpy(font.lfFaceName, L"DOUYU Font");
				font.lfHeight = (windows.middle.height / 100.0) * 35;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 70;
					words_rect.top = (windows.middle.height / 100.0) * 17;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 348;
					words_rect.bottom = (windows.middle.height / 100.0) * 62;
				}
				drawtext(L"课间休息", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				int num = state.num;
				wchar_t now_time[250];
				if (state.type == 1) wcscpy(now_time, L"下节课是 ");
				else wcscpy(now_time, L"接下来是 ");
				wcscat(now_time, state.name.c_str());

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 25;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 10;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 218;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				if (red_color == false)
				{
					red_color_num += 2;
					if (red_color_num > 250) red_color = true, red_color_num = 248;
				}
				else if (red_color == true)
				{
					red_color_num -= 2;
					if (red_color_num < 150) red_color = false, red_color_num = 152;
				}

				settextcolor(rgb(red_color_num, 0, 0));

				int t1 = ((get<0>(state.end) * 60 + get<1>(state.end)) * 60 - (Hour * 3600 + Minute * 60 + Second)) % 60;
				int t2 = (((get<0>(state.end) * 60 + get<1>(state.end)) * 60 - (Hour * 3600 + Minute * 60 + Second)) - t1) / 60;

				if (state.type == 1) wsprintf(now_time, L"距离上课 %02d′%02d″", t2, t1);
				else wsprintf(now_time, L"距下阶段 %02d′%02d″", t2, t1);

				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 222;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else if (state.type == 5)
			{
				//课前预备进度
				double speed_of_progress = double(time - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60)) / double((get<0>(state.end) * 3600 + get<1>(state.end) * 60) - (get<0>(state.begin) * 3600 + get<1>(state.begin) * 60));

				setfillcolor(rgb(255, 255, 255));
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 55, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 198, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 202, (windows.middle.height / 100.0) * 60, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 13, (windows.middle.height / 100.0) * 12, windows.middle.x + (windows.middle.height / 100.0) * (13 + 374 * speed_of_progress), (windows.middle.height / 100.0) * 53, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 120, rgb(0, 120, 255));

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 20;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				outtextxy(windows.middle.x + (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 35, L"本节课是");

				wcscpy(font.lfFaceName, L"DOUYU Font");
				font.lfHeight = (windows.middle.height / 100.0) * 35;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 87;
					words_rect.top = (windows.middle.height / 100.0) * 17;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 62;
				}
				drawtext(state.name.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				int num = state.num;
				wchar_t now_time[250] = L"";
				if (time <= interval_time[DayOfWeek].lunch.first * 3600 + interval_time[DayOfWeek].lunch.second * 60) wcscat(now_time, L"上午第");
				else if (time <= interval_time[DayOfWeek].dinner.first * 3600 + interval_time[DayOfWeek].dinner.second * 60) wcscat(now_time, L"下午第"), num -= interval_time[DayOfWeek].morning;
				else wcscat(now_time, L"晚上第"), num -= interval_time[DayOfWeek].morning + interval_time[DayOfWeek].afternoon;
				wcscat(now_time, to_wstring(num).c_str());
				wcscat(now_time, L"节课");

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * 25;
				settextcolor(rgb(0, 0, 0));
				settextstyle(&font);
				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 10;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 198;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(now_time, &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

				if (red_color == false)
				{
					red_color_num += 2;
					if (red_color_num > 250) red_color = true, red_color_num = 248;
				}
				else if (red_color == true)
				{
					red_color_num -= 2;
					if (red_color_num < 150) red_color = false, red_color_num = 152;
				}

				settextcolor(rgb(red_color_num, 0, 0));

				{
					words_rect.left = windows.middle.x + (windows.middle.height / 100.0) * 202;
					words_rect.top = (windows.middle.height / 100.0) * 63;
					words_rect.right = windows.middle.x + (windows.middle.height / 100.0) * 390;
					words_rect.bottom = (windows.middle.height / 100.0) * 91;
				}
				drawtext(L"课前准备时段", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}

			getimage(&back, windows.middle.x, 0, windows.middle.x + windows.middle.width, windows.middle.height);
			if (windows.middle.move == true)
			{
				IMAGE tmp = back;
				for (int i = 1; i <= 10; i++) blur(&tmp);

				putimage(windows.middle.x, 0, windows.middle.width, windows.middle.height, &tmp, 0, 0);
				solidroundrect_alpha(windows.middle.x + (windows.middle.height / 100.0) * 10, (windows.middle.height / 100.0) * 10, windows.middle.x + (windows.middle.height / 100.0) * 390, (windows.middle.height / 100.0) * 90, (windows.middle.height / 100.0) * 20, (windows.middle.height / 100.0) * 20, 200);

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * min(40, windows.y * (-0.8) + 37.5);
				settextcolor(windows.y < (windows.middle.height / 100.0) * (-10) ? rgb(0, 91, 195) : rgb(0, 0, 0));
				settextstyle(&font);

				if (windows.y <= (windows.middle.height / 100.0) * 20)
				{
					outtextxy(windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"隐藏")) / 2, (windows.middle.height / 100.0) * (min(max(20 - windows.y, 15), 37)), L"隐藏");
					if (windows.y < (windows.middle.height / 100.0) * (-10))
					{
						setlinecolor(rgb(0, 91, 195));
						setlinestyle(PS_SOLID, (windows.middle.height / 100.0) * 2);
						roundrect(windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"隐藏")) / 2 - (windows.middle.height / 100.0) * 15, (windows.middle.height / 100.0) * (min(max(20 - windows.y, 15), 37) - 5), windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"隐藏")) / 2 + textwidth(L"隐藏") + (windows.middle.height / 100.0) * 15, (windows.middle.height / 100.0) * (min(max(20 - windows.y, 15), 37)) + textheight(L"隐藏") + (windows.middle.height / 100.0) * 2, (windows.middle.height / 100.0) * 30, (windows.middle.height / 100.0) * 30);
					}
				}

				wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
				font.lfHeight = (windows.middle.height / 100.0) * min(50, windows.y + 25);
				settextcolor(windows.y > (windows.middle.height / 100.0) * 50 ? rgb(208, 2, 27) : rgb(0, 0, 0));
				settextstyle(&font);

				if (windows.y >= (windows.middle.height / 100.0) * (-10))
				{
					outtextxy(windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"关闭程序与选项")) / 2, (windows.middle.height / 100.0) * (max(min(60 - windows.y, 75), 27)), L"关闭程序与选项");
					if (windows.y > (windows.middle.height / 100.0) * 50)
					{
						setlinecolor(rgb(208, 2, 27));
						setlinestyle(PS_SOLID, (windows.middle.height / 100.0) * 2);
						roundrect(windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"关闭程序与选项")) / 2 - (windows.middle.height / 100.0) * 15, (windows.middle.height / 100.0) * (max(min(60 - windows.y, 75), 27) - 5), windows.middle.x + ((windows.middle.height / 100.0) * 400 - textwidth(L"关闭程序与选项")) / 2 + textwidth(L"关闭程序与选项") + (windows.middle.height / 100.0) * 15, (windows.middle.height / 100.0) * (max(min(60 - windows.y, 75), 27)) + textheight(L"关闭程序与选项") + (windows.middle.height / 100.0) * 2, (windows.middle.height / 100.0) * 30, (windows.middle.height / 100.0) * 30);
					}
				}
			}

			hiex::FlushDrawing({ windows.middle.x,windows.middle.y,windows.middle.x + windows.middle.width + 1,windows.middle.y + windows.middle.height + 1 });
			EndDraw();
		}

		//禁用左右拓展功能
		continue;

		//左右侧窗口绘制
		if (state.type == 0 || state.type == 4)
		{
			if (!windows.left.is_open && (state.type == 0 || state.type == 4) && windows.left.width != windows.middle.x - 20)
			{
				HRGN left_rgn = CreateRoundRectRgn(10, 0, windows.middle.x - 10, windows.left.height, 15, 15);
				HRGN middle_rgn = CreateRoundRectRgn(windows.middle.x, windows.middle.y, windows.middle.x + windows.middle.width, windows.middle.y + windows.middle.height, 15, 15);
				HRGN right_rgn = CreateRoundRectRgn(windows.middle.x + windows.middle.width + 10, 0, GetSystemMetrics(SM_CXSCREEN) - 10, windows.right.height, 15, 15);

				HRGN flanks_rgn = CreateRectRgn(0, 0, 0, 0), whole_rgn = CreateRectRgn(0, 0, 0, 0);
				CombineRgn(flanks_rgn, left_rgn, right_rgn, RGN_OR);
				CombineRgn(whole_rgn, flanks_rgn, middle_rgn, RGN_OR);
				if (windows.massage.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.massage.x, windows.massage.y, windows.massage.x + windows.massage.width, windows.massage.y + windows.massage.height, 15, 15), RGN_OR);

				SetWindowRgn(main_window, whole_rgn, true);

				windows.left.is_open = windows.right.is_open = true;

				windows.left.x = 10;
				windows.left.width = windows.middle.x - 20;

				windows.right.x = windows.middle.x + windows.middle.width + 10;
				windows.right.width = GetSystemMetrics(SM_CXSCREEN) - windows.middle.x - windows.middle.width - 20;
			}

			if (bilateral_window_update)
			{
				bilateral_window_update = false;

				if (epidemic.show_num == 1) epidemic.show = epidemic.city;
				else if (epidemic.show_num == 2) epidemic.show = epidemic.province;
				else epidemic.show = epidemic.country;

				epidemic.show = epidemic.city;

				//窗口背景打印
				{
					if (grey_pattern)
					{
						IMAGE t1 = background_img[4], t2 = background_img[4];
						putimage(windows.left.x, windows.left.y, ColorToGray(&t1));
						putimage(windows.right.x, windows.right.y, ColorToGray(&t2));
					}
					else
					{
						putimage(windows.left.x, windows.left.y, background_img + 4);
						putimage(windows.right.x, windows.right.y, background_img + 4);
					}
				}

				//左侧绘制
				int left_middle_x = windows.left.x + windows.left.width / 2;
				if (grey_pattern);
				else
				{
					transparentimage(hiex::GetWindowImage(), left_middle_x - 25, zoom * 20, weathar_img + 70);

					{
						wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
						for (int i = zoom * 30;; i--)
						{
							font.lfHeight = i;
							settextstyle(&font);

							wstring notice;
							if (location.city == location.area) notice = location.province + location.city + L" 天气信息暂未准备就绪";
							else notice = location.province + location.city + location.area + L" 天气信息暂未准备就绪";
							if (textwidth(notice.c_str()) <= windows.left.width - zoom * 15) break;
						}
						settextcolor(rgb(100, 0, 0));
					}
					wstring notice;
					if (location.city == location.area) notice = location.province + location.city + L" 天气信息暂未准备就绪";
					else notice = location.province + location.city + location.area + L" 天气信息暂未准备就绪";
					outtextxy(left_middle_x - textwidth(notice.c_str()) / 2, zoom * 80, notice.c_str());
				}

				//右侧绘制
				int rigth_right_x = windows.right.x + windows.right.width;
				int rigth_midle_y = windows.right.y + windows.right.height / 2;
				int rigth_bottom_y = windows.right.y + windows.right.height;
				if (grey_pattern);
				else
				{
					solidroundrect_alpha(windows.right.x + zoom * 5, windows.right.y + zoom * 5, windows.right.x + windows.right.width / 4 - zoom * 5, rigth_midle_y - zoom * 5, 15, 15, 200);
					solidroundrect_alpha(windows.right.x + windows.right.width / 4 + zoom * 5, windows.right.y + zoom * 5, windows.right.x + windows.right.width / 2 - zoom * 5, rigth_midle_y - zoom * 5, 15, 15, 200);
					solidroundrect_alpha(windows.right.x + windows.right.width / 2 + zoom * 5, windows.right.y + zoom * 5, windows.right.x + windows.right.width / 4 * 3 - zoom * 5, rigth_midle_y - zoom * 5, 15, 15, 200);
					solidroundrect_alpha(windows.right.x + windows.right.width / 4 * 3 + zoom * 5, windows.right.y + zoom * 5, rigth_right_x - zoom * 6, rigth_midle_y - zoom * 5, 15, 15, 200);

					solidroundrect_alpha(windows.right.x + zoom * 5, rigth_midle_y + zoom * 5, rigth_right_x - zoom * 6, rigth_bottom_y - zoom * 6, 15, 15, 200);

					//疫情数据
					{
						//城市名称
						{
							{
								wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
								for (int i = zoom * 30;; i--)
								{
									font.lfHeight = i;
									settextstyle(&font);

									if (textwidth(epidemic.show.name.c_str()) <= windows.right.width / 4 - zoom * 20) break;
								}
								settextcolor(rgb(0, 0, 0));
							}
							{
								words_rect.left = windows.right.x + zoom * 5;
								words_rect.top = windows.right.y + zoom * 10;
								words_rect.right = windows.right.x + windows.right.width / 4 - zoom * 5;
								words_rect.bottom = rigth_midle_y - zoom * 5;
							}
							drawtext(epidemic.show.name.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}
						//累积确诊
						{
							//part1
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(L"累积确诊") <= windows.right.width / 4 - zoom * 20 && textheight(L"累积确诊") <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 - zoom * 5) break;
									}
									settextcolor(rgb(0, 0, 0));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 4 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 10;
									words_rect.right = windows.right.x + windows.right.width / 2 - zoom * 5;
									words_rect.bottom = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2;
								}
								drawtext(L"累积确诊", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
							//part2
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(epidemic.show.conNum.c_str()) <= windows.right.width / 4 - zoom * 20 && textheight(epidemic.show.conNum.c_str()) <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 + zoom * 5) break;
									}
									settextcolor(rgb(232, 49, 50));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 4 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2 + zoom * 5;
									words_rect.right = windows.right.x + windows.right.width / 2 - zoom * 5;
									words_rect.bottom = rigth_midle_y - zoom * 5;
								}
								drawtext(epidemic.show.conNum.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
						}
						//现存确诊
						{
							//part1
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(L"现存确诊") <= windows.right.width / 4 - zoom * 20 && textheight(L"现存确诊") <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 - zoom * 5) break;
									}
									settextcolor(rgb(0, 0, 0));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 2 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 10;
									words_rect.right = windows.right.x + windows.right.width / 4 * 3 - zoom * 5;
									words_rect.bottom = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2;
								}
								drawtext(L"现存确诊", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
							//part2
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(epidemic.show.econNum.c_str()) <= windows.right.width / 4 - zoom * 20 && textheight(epidemic.show.econNum.c_str()) <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 + zoom * 5) break;
									}
									settextcolor(rgb(232, 109, 72));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 2 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2 + zoom * 5;
									words_rect.right = windows.right.x + windows.right.width / 4 * 3 - zoom * 5;
									words_rect.bottom = rigth_midle_y - zoom * 5;
								}
								drawtext(epidemic.show.econNum.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
						}
						//昨日新增
						{
							//part1
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(L"昨日新增") <= windows.right.width / 4 - zoom * 20 && textheight(L"昨日新增") <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 - zoom * 5) break;
									}
									settextcolor(rgb(0, 0, 0));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 4 * 3 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 10;
									words_rect.right = rigth_right_x - zoom * 6;
									words_rect.bottom = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2;
								}
								drawtext(L"昨日新增", &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
							//part2
							{
								{
									wcscpy(font.lfFaceName, L"HarmonyOS Sans SC");
									for (int i = zoom * 30;; i--)
									{
										font.lfHeight = i;
										settextstyle(&font);

										if (textwidth(epidemic.show.conadd.c_str()) <= windows.right.width / 4 - zoom * 20 && textheight(epidemic.show.conadd.c_str()) <= ((rigth_midle_y - zoom * 5) - (windows.right.y + zoom * 5)) / 2 + zoom * 5) break;
									}
									settextcolor(rgb(255, 106, 87));
								}
								{
									words_rect.left = windows.right.x + windows.right.width / 4 * 3 + zoom * 5;
									words_rect.top = windows.right.y + zoom * 5 + (rigth_midle_y - zoom * 5) / 2 - (windows.right.y + zoom * 5) / 2 + zoom * 5;
									words_rect.right = rigth_right_x - zoom * 6;
									words_rect.bottom = rigth_midle_y - zoom * 5;
								}
								drawtext(epidemic.show.conadd.c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							}
						}
					}

					//每日一言
					{
						{
							wcscpy(font.lfFaceName, L"得意黑");
							for (int i = zoom * 40;; i--)
							{
								font.lfHeight = i;
								settextstyle(&font);

								if (textwidth((dictum[make_tuple(Year, Month, Day)].first + L" ——" + dictum[make_tuple(Year, Month, Day)].second).c_str()) <= windows.right.width - zoom * 15) break;
							}
							settextcolor(rgb(0, 0, 0));
						}
						{
							words_rect.left = windows.right.x + zoom * 5;
							words_rect.top = rigth_midle_y + zoom * 10;
							words_rect.right = rigth_right_x - zoom * 6;
							words_rect.bottom = rigth_bottom_y - zoom * 6;
						}
						drawtext((dictum[make_tuple(Year, Month, Day)].first + L" ——" + dictum[make_tuple(Year, Month, Day)].second).c_str(), &words_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
				}
			}
		}
		else if ((state.type != 0 && state.type != 4) && windows.left.width == windows.middle.x - 20)
		{
			HRGN whole_rgn = CreateRoundRectRgn(windows.middle.x, 0, windows.middle.x + windows.middle.width, windows.middle.height, 15, 15);
			if (windows.massage.is_open) CombineRgn(whole_rgn, whole_rgn, CreateRoundRectRgn(windows.massage.x, windows.massage.y, windows.massage.x + windows.massage.width, windows.massage.y + windows.massage.height, 15, 15), RGN_OR);

			SetWindowRgn(main_window, whole_rgn, true);

			windows.left.is_open = windows.right.is_open = false;

			windows.left.x = 10;
			windows.left.width = 0;

			windows.right.x = windows.middle.x + windows.middle.width + 10;
			windows.right.width = 0;
		}
	}

	return 0;
}