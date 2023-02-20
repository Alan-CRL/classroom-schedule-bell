/**
 * @file	ControlBase.h
 * @brief	HiGUI �ؼ���֧���ؼ�����
 * @author	huidong
*/

#pragma once

#include "../HiContainer.h"

#include "../HiMacro.h"
#include "../HiFunc.h"
#include "../HiCanvas.h"

namespace HiEasyX
{
	/**
	 * @brief �ؼ�����
	*/
	enum CtrlMessage
	{
		CM_OTHER,						///< δ�����ʶ��������Ϣ
		CM_HOVER,						///< ��ͣ
		CM_HOVER_OVER,					///< ��ͣ����
		CM_PRESS,						///< ����
		CM_PRESS_OVER,					///< ���½���
		CM_CLICK,						///< ����
		CM_DOUBLE_CLICK,				///< ˫��
		CM_FOCUS,						///< ��ȡ����
		CM_FOCUS_OVER,					///< ��ʧ����

	};

	class ControlBase;

	/**
	 * @brief �ؼ���Ϣ������
	 * @param[in] _Ctrl	����ؼ�ָ��
	 * @param[in] _MsgId	������Ϣ��ʶ����
	 * @param[in] _ExMsg	������Ϣ����������ѱ任���ؼ���
	*/
	typedef void (*MESSAGE_PROC_FUNC)(ControlBase* _Ctrl, int _MsgId, ExMessage _ExMsg);

	/**
	 * @brief ֧�־�̬�ຯ����Ϊ�ؼ���Ϣ������
	*/
	typedef void (*MESSAGE_PROC_FUNC_CLASS)(void* _This, ControlBase* _Ctrl, int _MsgId, ExMessage _ExMsg);

	/**
	 * @brief �ؼ�����
	*/
	class ControlBase : public Container
	{
	protected:

		bool m_bEnabled = true;										///< �Ƿ����
		bool m_bVisible = true;										///< �Ƿ�ɼ�

		// �ػ����Ⱦ��־
		bool m_bAutoRedrawWhenReceiveMsg = true;					///< ��Ĭ����Ϣ���������ܵ���Ϣʱ���Ƿ��Զ���ʶ�ػ����Ⱦ
		bool m_bRedraw = true;										///< ��ʶ��Ҫ�ػ�
		bool m_bRender = true;										///< ��ʶ��Ҫ��Ⱦ
		bool m_bClear = false;										///< ��ʶ��Ҫ���ĳ����
		RECT m_rctClear = { 0 };									///< ��¼��Ҫ��յ�����
		bool m_bAlwaysRedrawAndRender = false;						///< �����ػ����Ⱦ��ռ�ø��ߣ�

		std::wstring m_wstrText;									///< �ؼ��ı�

		Canvas m_canvas;											///< ����
		BYTE m_alpha = 255;											///< ͸����
		bool m_bUseCanvasAlpha = false;								///< �Ƿ�ʹ�û��������͸������Ϣ
		bool m_isAlphaCalculated = false;							///< �����Ƿ��Ѿ�����͸�������ɫ

		COLORREF m_cBorder = MODERN_BORDER_GRAY;					///< �߿���ɫ
		COLORREF m_cBackground = CLASSICGRAY;						///< ����ɫ
		COLORREF m_cText = BLACK;									///< �ı���ɫ

		bool m_bEnableBorder = true;								///< �Ƿ���Ʊ߿�
		int m_nBorderThickness = 1;									///< �߿��ϸ

		bool m_bCompleteFirstSetRect = false;						///< �Ƿ��Ѿ���ɵ�һ����������

		ControlBase* m_pParent = nullptr;							///< ���ؼ�
		std::list<ControlBase*> m_listChild;						///< �ӿؼ�

		bool m_bAutoSizeForChild = false;							///< Ϊ�ӿؼ��Զ��ı��С�����ɿؼ�

		MESSAGE_PROC_FUNC m_funcMessageProc = nullptr;				///< ��Ϣ������
		MESSAGE_PROC_FUNC_CLASS m_funcMessageProc_Class = nullptr;	///< ���󶨵���Ϣ�������Ǿ�̬�ຯ�������¼���ַ
		void* m_pCalledClass = nullptr;								///< ���󶨵���Ϣ�������Ǿ�̬�ຯ�������¼����ָ��

		bool m_bHovered = false;									///< ����Ƿ���ͣ
		bool m_bPressed = false;									///< ����Ƿ���
		bool m_bFocused = false;									///< �Ƿ�ӵ�н���

		/**
		 * @brief ����������Ϣ����
		 * @param[in] rctOld ������
		*/
		void UpdateRect(RECT rctOld) override;

		/**
		 * @brief �����Ҫ�ػ����Ⱦ
		*/
		void MarkNeedRedrawAndRender();

		/**
		 * @brief �����Ҫ��վ�������
		 * @param[in] rct ��Ҫ��յ�����
		*/
		void MarkNeedClearRect(RECT rct);

		/**
		 * @brief �����ӿؼ�
		*/
		virtual void DrawChild();

		/**
		 * @brief ת����Ϣ
		 * @param[in, out] msg Ҫת������Ϣ
		 * @return ת�������Ϣ
		*/
		virtual ExMessage& TransformMessage(ExMessage& msg);

		/**
		 * @brief �ַ���Ϣ���û�����
		 * @param[in] msgid	��Ϣ ID
		 * @param[in] msg		��Ϣ����
		*/
		virtual void CallUserMsgProcFunc(int msgid, ExMessage msg);

		/**
		 * @brief �ӿؼ�������
		 * @param[in] pChild ���������ӿؼ�
		*/
		virtual void ChildRectChanged(ControlBase* pChild);

	private:

		void Init();

	public:

		ControlBase();

		ControlBase(std::wstring wstrText);

		ControlBase(int x, int y, int w = 0, int h = 0, std::wstring wstrText = L"");

		virtual ~ControlBase();

		ControlBase* GetParent() { return m_pParent; }

		/**
		 * @brief ���ø��ؼ������ؼ����� AddChild��
		 * @param[in] p ���ؼ�
		*/
		virtual void SetParent(ControlBase* p);

		virtual bool IsAutoSizeForChild() const { return m_bAutoSizeForChild; }

		/**
		 * @brief Ϊ�ӿؼ��Զ��ı��С�����ɿؼ��������ɸ����겿�֣�
		 * @param[in] enable �Ƿ�����
		*/
		virtual void EnableAutoSizeForChild(bool enable);

		std::list<ControlBase*>& GetChildList();

		/**
		 * @brief ��ȡ�ӿؼ�����
		*/
		size_t GetChildCount();

		virtual void AddChild(ControlBase* p, int offset_x = 0, int offset_y = 0);

		virtual void RemoveChild(ControlBase* p);

		virtual bool IsEnabled() const { return m_bEnabled; }

		virtual void SetEnable(bool enable);

		virtual bool IsVisible() const { return m_bVisible; }

		virtual void SetVisible(bool visible);

		virtual bool GetAutoRedrawState() const { return m_bAutoRedrawWhenReceiveMsg; }

		/**
		 * @brief �����Զ��ػ棨���ܵ�������Ϣ�¼�ʱ�Զ���ʶ��Ҫ�ػ棩
		 * @param[in] enable �Ƿ�����
		*/
		virtual void EnableAutoRedraw(bool enable);

		virtual Canvas& GetCanvas() { return m_canvas; }

		virtual COLORREF GetBkColor() const { return m_cBackground; }

		virtual void SetBkColor(COLORREF color);

		virtual COLORREF GetTextColor() const { return m_cText; }

		virtual void SetTextColor(COLORREF color);

		virtual void EnableBorder(bool bEnableBorder, COLORREF color = BLACK, int thickness = 1);

		virtual void SetAlpha(BYTE alpha, bool bUseCanvasAlpha, bool isAlphaCalculated);

		virtual std::wstring GetText() const { return m_wstrText; }

		virtual void SetText(std::wstring wstr);

		virtual void Draw_Text(int nTextOffsetX = 0, int nTextOffsetY = 0);

		/**
		 * @brief �ػ�ؼ�
		*/
		virtual void Redraw();

		/**
		 * @brief ���ƿؼ�
		 * @param[in] draw_child �Ƿ�����ӿؼ�
		*/
		virtual void Draw(bool draw_child = true);

		/**
		 * @brief ��Ⱦ�ؼ����ⲿ
		 * @param[in] dst			��ȾĿ��
		 * @param[in] pRct			�ڲ�ʹ�ã����븸�ؼ���Ⱦ��������
		 * @param[in, out] pCount	�ڲ�ʹ�ã����븸�ؼ���Ⱦ��������ָ��
		*/
		virtual void Render(Canvas* dst, RECT* pRct = nullptr, int* pCount = 0);

		/**
		 * @brief ������Ϣ��Ӧ����
		 * @param[in] func ��Ϣ��Ӧ����
		*/
		virtual void SetMsgProcFunc(MESSAGE_PROC_FUNC func);

		/**
		 * @brief ������Ϣ��Ӧ����Ϊ��̬�ຯ��
		 * @param[in] static_class_func		��Ϣ��Ӧ��������̬�ຯ����
		 * @param[in] _this					��ָ��
		*/
		virtual void SetMsgProcFunc(MESSAGE_PROC_FUNC_CLASS static_class_func, void* _this);

		/**
		 * @brief ������Ϣ
		 * @param[in] msg ����Ϣ
		*/
		virtual void UpdateMessage(ExMessage msg);

		/**
		 * @brief �ж�����Ƿ���ͣ
		*/
		virtual bool IsHovered() const { return m_bHovered; }

		/**
		 * @brief �ж��Ƿ�ӵ�н���
		*/
		virtual bool IsFocused() const { return m_bFocused; }

		/**
		 * @brief �ж��Ƿ���
		*/
		virtual bool IsPressed() const { return m_bPressed; }
	};
}

