/**
 * @file	Page.h
 * @brief	HiGUI �ؼ���֧��ҳ�ؼ�
 * @author	huidong
*/

#pragma once

#include "ControlBase.h"

namespace HiEasyX
{
	/**
	 * @brief ҳ�ؼ�
	*/
	class Page : public ControlBase
	{
	protected:

		Canvas* m_pCanvas = nullptr;

		virtual void Init(COLORREF cBk = WHITE);

	public:

		Page();

		Page(int w, int h, COLORREF cBk = WHITE);

		Page(Canvas* pCanvas);

		/**
		 * @brief �󶨵���������ȾʱĬ��������˻���
		 * @param[in] pCanvas ����
		*/
		virtual void BindToCanvas(Canvas* pCanvas);

		/**
		 * @brief ����ؼ�
		 * @param[in] pCtrl		�ؼ�
		 * @param[in] offset_x	����ƫ��
		 * @param[in] offset_y	����ƫ��
		*/
		virtual void push(ControlBase* pCtrl, int offset_x = 0, int offset_y = 0);

		virtual void push(const std::list<ControlBase*> list);

		/**
		 * @brief �Ƴ��ؼ�
		 * @param[in] pCtrl �ؼ�
		*/
		virtual void remove(ControlBase* pCtrl);

		/**
		 * @brief ��Ⱦ
		 * @param[in] dst		���廭����Ϊ����������Ѱ󶨻�����
		 * @param[in] pRct		�ڲ�ʹ��
		 * @param[in] pCount	�ڲ�ʹ��
		*/
		void Render(Canvas* dst = nullptr, RECT* pRct = nullptr, int* pCount = 0) override;

		/**
		 * @brief ���¿ؼ����������ĳ�������ؼ��ػ沢��Ⱦ��
		 * @param[in] pCanvas ���廭����Ϊ����������Ѱ󶨻�����
		*/
		void UpdateImage(Canvas* pCanvas = nullptr);
	};
}

