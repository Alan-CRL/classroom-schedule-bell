/**
 * @file	HiEasyX.h
 * @brief	HiEasyX ��ͷ�ļ�
 * @note	�ÿ���� EasyX ʵ�����ڴ��ڡ���ͼ��UI ����෽���ȫ������
 *
 * @version	Ver 0.3.0
 * @envir	VisualStudio 2022 | EasyX_20220901 | Windows 10
 * @site	https://github.com/zouhuidong/HiEasyX
 *
 * @author	huidong
 * @email	mailhuid@163.com
 * @create	2022.07.20
 * @date	2023.01.13
*/

#pragma once

#define __HIEASYX_H__

#include "HiEasyX/HiDef.h"
#include "HiEasyX/HiMacro.h"
#include "HiEasyX/HiFunc.h"
#include "HiEasyX/HiFPS.h"
#include "HiEasyX/HiDrawingProperty.h"
#include "HiEasyX/HiMouseDrag.h"
#include "HiEasyX/HiMusicMCI.h"
#include "HiEasyX/HiGif.h"

// δѡ��ʹ��ԭ�� EasyX
#ifndef _NATIVE_EASYX_
#include "HiEasyX/HiWindow.h"
#include "HiEasyX/HiSysGUI/HiSysGUI.h"
#endif /* _NATIVE_EASYX_ */

#include "HiEasyX/HiGdiplus.h"
#include "HiEasyX/HiCanvas.h"
#include "HiEasyX/HiGUI/HiGUI.h"


// HiEasyX �����ռ���д
namespace hiex = HiEasyX;

// ���ݾɰ� EasyWin32 �����ռ䣬�뾡��ʹ�� HiEasyX �����ռ�
namespace EasyWin32 = HiEasyX;

