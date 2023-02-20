/**
 * @file	ScrollBar.h
 * @brief	HiGUI �ؼ���֧���������ؼ�
 * @author	huidong
*/

#pragma once

#include "Button.h"

#include "../HiMouseDrag.h"

#include <time.h>

namespace HiEasyX
{
	/**
	 * @brief �������ؼ�
	*/
	class ScrollBar : public ControlBase
	{
	public:

		/**
		 * @brief ��������Ϣ
		*/
		struct ScrollBarInfo
		{
			int btnW, btnH;			///< ��ͨ��ť�ߴ�
			int slider_free_len;	///< �������ռ�ݵ����س���
			int slider_move_len;	///< ���鶥������ƶ������س���
			int slider_len;			///< �������س���
		};

	protected:

		int m_nBtnHeight = 20;				///< ��ť�߶ȣ���ֱ����ʱ��Ч��
		int m_nBtnWidth = 20;				///< ��ť��ȣ�ˮƽ����ʱ��Ч��

		bool m_bHorizontal = false;			///< �Ƿ�ˮƽ����

		int m_nDrawInterval = 6;			///< ���Ƽ�϶

		// ��ť
		Button m_btnTop;
		Button m_btnUp;
		Button m_btnDown;
		Button m_btnBottom;
		Button m_btnDrag;
		MouseDrag m_MouseDrag;
		bool m_bDragging = false;			///< �Ƿ������϶�
		int m_nSliderSpeed = 20;			///< ��ס��ťʱ����ÿ�����е����ݳ���
		clock_t m_tPressed = 0;				///< ��ť��ס��ʱ

		float m_fPos = 0;					///< ��������λ��
		int m_nLen = 0;						///< �����ܳ���
		float m_fPosRatio = 0;				///< ��������λ�ñ���

		ScrollBarInfo m_info = {};			///< ��������Ϣ

		int m_nViewLen = 10;				///< ��Ұ���ݳ���
		float m_fViewRatio = 1;				///< ��Ұ��Χռ�ܳ��ȵı�

		bool m_bSliderPosChanged = false;	///< ��ǻ���λ�øı�

		RECT m_rctOnWheel = { 0 };			///< ��Ӧ������Ϣ�����򣨿ͻ������꣩
		bool m_bSetOnWheelRct = false;		///< �Ƿ������˹�����Ϣ����Ӧ����

		/**
		 * @brief ��ʼ��
		*/
		virtual void Init();

		/**
		 * @brief ��Ӧ���ⰴť��Ϣ��Top �� Bottom��
		 * @param[in] pThis	��ָ��
		 * @param[in] pCtrl	�ؼ�
		 * @param[in] msgid	��Ϣ ID
		 * @param[in] msg	��Ϣ����
		*/
		static void OnSpecialButtonMsg(void* pThis, ControlBase* pCtrl, int msgid, ExMessage msg);

		/**
		 * @brief ��Ӧ��ͨ��ť��Ϣ��Up �� Down��
		*/
		virtual void OnNormalButtonMsg();

		/**
		 * @brief ���»���λ������
		*/
		virtual void UpdateSliderRect();

		/**
		 * @brief ����λ�ñ���
		*/
		void UpdatePosRatio();

		/**
		 * @brief ������Ұ����
		*/
		void UpdateViewRatio();

		/**
		 * @brief ���¹�������Ϣ
		*/
		void UpdateScrollBarInfo();

	public:

		ScrollBar();

		ScrollBar(int x, int y, int w, int h, int len, int pos, bool bHorizontal = false);

		int GetButtonHeight() const { return m_nBtnHeight; }

		/**
		 * @brief ���ð�ť�߶ȣ���ֱ����ʱ��Ч��
		 * @param[in] h �߶�
		*/
		void SetButtonHeight(int h);

		int GetButtonWidth() const { return m_nBtnWidth; }

		/**
		 * @brief ���ð�ť��ȣ�ˮƽ����ʱ��Ч��
		 * @param[in] w ���
		*/
		void SetButtonWidth(int w);

		/**
		 * @brief �ж��Ƿ������϶�
		*/
		bool IsDragging() const { return m_bDragging; }

		/**
		 * @brief ��ȡ�������س���
		*/
		int GetSliderLength() const { return m_info.slider_len; }

		/**
		 * @brief ��ȡ��������λ��
		*/
		int GetSliderContentPos() const { return (int)m_fPos; }

		/**
		 * @brief ���û�������λ��
		 * @param[in] pos ����λ��
		*/
		void SetSliderContentPos(float pos);

		/**
		 * @brief ����ƶ����������λ��
		 * @param[in] d ����λ��
		*/
		void MoveSlider(float d);

		/**
		 * @brief ��ȡ���������ݳ���
		*/
		int GetContentLength() const { return m_nLen; }

		/**
		 * @brief ���ù��������ݳ���
		 * @param[in] len ���ݳ���
		*/
		void SetContentLength(int len);

		int GetViewLength() const { return m_nViewLen; }

		/**
		 * @brief ������Ұ���ݳ���
		 * @param[in] len ��Ұ���ݳ���
		*/
		void SetViewLength(int len);

		int GetSliderSpeed() const { return m_nSliderSpeed; }

		/**
		 * @brief ���ð��°�ťʱ����������ٶ�
		 * @param[in] speed �����ٶȣ�ÿ�뾭�������ݳ��ȣ�
		*/
		void SetSliderSpeed(int speed);

		bool IsHorizontal() const { return m_bHorizontal; }

		/**
		 * @brief ����ˮƽ����
		 * @param[in] enable �Ƿ�����
		*/
		void EnableHorizontal(bool enable);

		/**
		 * @brief �жϻ���λ���Ƿ�ı�
		*/
		bool IsSliderPosChanged();

		/**
		 * @brief ��ȡ��Ӧ������Ϣ������δ�Զ���ʱ���ؿ�����
		*/
		RECT GetOnWheelRect() const { return m_rctOnWheel; }

		/**
		 * @brief ������Ӧ������Ϣ�����򣨿ͻ������꣩
		 * @param[in] rct ��Ϣ��Ӧ����
		*/
		void SetOnWheelRect(RECT rct);

		void UpdateRect(RECT rctOld) override;

		void UpdateMessage(ExMessage msg) override;

		void Draw(bool draw_child = true) override;

	};
}

