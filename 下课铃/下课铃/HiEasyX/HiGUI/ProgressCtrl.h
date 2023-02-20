/**
 * @file	ProgressCtrl.h
 * @brief	HiGUI �ؼ���֧���������ؼ�
 * @author	huidong
*/

#pragma once

#include "ControlBase.h"

#include <time.h>

namespace HiEasyX
{
	/**
	 * @brief �������ؼ�
	*/
	class ProgressCtrl : public ControlBase
	{
	protected:

		bool m_bEnableAnimation = true;					///< �Ƿ����ö���Ч��

		int m_nLightPos = 0;							///< ��Դλ��
		clock_t m_tClock = 0;							///< ������һ�ζ����Ļ���ʱ��
		float m_fSpeedRatio = 0.4f;						///< �����ٶȣ�ÿ�뾭���ܳ��ȵĶ��٣�

		float m_fLenRatio = 0.8f;						///< ��������Ч������ռ��

		float m_fBarColorLightness = 0.35f;				///< ��������ɫ����
		float m_fBarLightLightness = 0.41f;				///< ��������Դ����

		COLORREF m_cBar;								///< ��������ɫ�����������Ȳ�����

		float m_fH = 0;									///< ɫ����Ϣ
		float m_fS = 0;									///< ���Ͷ���Ϣ

		int m_nPos = 0;									///< ����������
		int m_nLen = 100;								///< �����ܳ���

		virtual void Init();

	public:

		ProgressCtrl();

		ProgressCtrl(int x, int y, int w, int h, int len);

		/**
		 * @brief ��ȡ���ݳ���
		*/
		virtual int GetContentLength() const { return m_nLen; }

		/**
		 * @brief �������ݳ���
		 * @param[in] len ���ݳ���
		*/
		virtual void SetContentLength(int len);

		/**
		 * @brief ��ȡ���ȣ����ݳ��ȼ�Ϊ�ܽ��ȣ�
		*/
		virtual int GetProcess() const { return m_nPos; }

		/**
		 * @brief ���ý��ȣ����ݳ��ȼ�Ϊ�ܽ��ȣ�
		 * @param[in] pos ����
		*/
		virtual void SetProcess(int pos);

		/**
		 * @brief ���ȼ�һ
		*/
		virtual void Step();

		/**
		 * @brief ��ȡ��������ɫ
		*/
		virtual COLORREF GetBarColor() const { return m_cBar; }

		/**
		 * @brief ���ý�������ɫ
		 * @param[in] cBar ��������ɫ
		*/
		virtual void SetBarColor(COLORREF cBar);

		/**
		 * @brief ��ȡ��������״̬
		*/
		virtual bool GetAnimationState() const { return m_bEnableAnimation; }

		/**
		 * @brief �����Ƿ����ö���
		 * @param[in] enable �Ƿ�����
		*/
		virtual void EnableAnimation(bool enable);

		void Draw(bool draw_child = true) override;
	};
}

