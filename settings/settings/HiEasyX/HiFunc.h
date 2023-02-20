/**
 * @file	HiFunc.h
 * @brief	HiEasyX ��ĳ��������
 * @author	huidong
*/

#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <graphics.h>

/**
 * @brief �洢������Ļ�Ĵ�С��Ϣ������ʾ����
*/
struct ScreenSize
{
	int left;	///< ����ʾ�������Ͻ� x ����
	int top;	///< ����ʾ�������Ͻ� y ����
	int w;		///< ����ʾ�����ܺͿ��
	int h;		///< ����ʾ�����ܺ͸߶�
};

/**
 * @brief ��ȡ����ʾ����С��Ϣ
*/
ScreenSize GetScreenSize();

/**
 * @brief <pre>
 *		��ȡͼ��ߴ�
 * 
 *	��ע��
 *		���Է���ش��� IMAGE ָ��Ϊ�գ���ָ������ͼ���ڵ����
 * </pre>
 * 
 * @param[in] pImg			Ŀ��ͼ��
 * @param[out] width		����ͼ���
 * @param[out] height		����ͼ���
*/
void GetImageSize(IMAGE* pImg, int* width, int* height);

/**
 * @brief <pre>
 *		��תͼ�� Alpha ֵ
 *	
 *	��ע��
 *		�� alpha ֵ��Ϊ 0 ��һ�����ص� alpha ��Ϊ 0��
 *		ͬʱ�� alpha ֵΪ 0 ��һ�����ص� alpha ��Ϊ 255��
 * </pre>
 * 
 * @param[in, out] pBuf		�Դ�ָ��
 * @param[in] size			�Դ��С 
 * @return �Դ�ָ�루��ԭ��һ����
*/
DWORD* ReverseAlpha(DWORD* pBuf, int size);

/**
 * @brief �õ� IMAGE ����� HBITMAP
 * @param[in] img			Ŀ��ͼ��
 *
 * @param[in] enable_alpha <pre>
 *		�Ƿ�����ͼ��� alpha ��Ϣ
 *
 *	ע�⣺
 *		��ͼ�� alpha ֵȫΪ 0�����ʾ������͸�����
 * </pre>
 * 
 * @return ת���õ���λͼ���
*/
HBITMAP Image2Bitmap(IMAGE* img, bool enable_alpha);

/**
 * @brief HBITMAP ת HICON
 * @param[in] hBmp λͼ���
 * @return ͼ����
*/
HICON Bitmap2Icon(HBITMAP hBmp);

/**
 * @brief ��ȷ��ʱ���������Ծ�ȷ�� 1ms������ ��1ms��
 * @author yangw80 <yw80@qq.com>
 * @date 2011-5-4
 * @param[in] ms ��ʱ���ȣ���λ�����룩
*/
void HpSleep(int ms);

/**
 * @brief ���Ƿ�λ�ھ�����
 * @param[in] x		λ��
 * @param[in] y		λ��
 * @param[in] rct		����
 * @return ���Ƿ�λ�ھ�����
*/
bool IsInRect(int x, int y, RECT rct);

/**
 * @brief ��ȡ ExMessage ����Ϣ����
 * @param[in] msg ��Ϣ
 * @return EM_ ��Ϣ�����е�һ�֣���ʧ�ܷ��� 0
*/
UINT GetExMessageType(ExMessage msg);
