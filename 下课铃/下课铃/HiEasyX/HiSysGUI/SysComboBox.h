/**
 * @file	SysComboBox.h
 * @brief	HiSysGUI �ؼ���֧����Ͽ�
 * @author	huidong
*/

#pragma once

#include "SysControlBase.h"

namespace HiEasyX
{
	/**
	 * @brief ϵͳ��Ͽ�ؼ�
	*/
	class SysComboBox : public SysControlBase
	{
	private:

		long m_lBasicStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWN | CBS_HASSTRINGS | CBS_AUTOHSCROLL;
		int m_nSel = -1;
		bool m_bSelChanged = false;
		bool m_bEdited = false;
		void (*m_pFuncSel)(int sel, std::wstring wstrSelText) = nullptr;
		void (*m_pFuncEdit)(std::wstring wstrText) = nullptr;

		/**
		 * @brief	����û������� CBS_SIMPLE ͬʱ��������
		 *			��������Ҫ����ʵ��
		*/
		bool m_bSimple_No_Edit = false;

	protected:

		void RealCreate(HWND hParent) override;

	public:

		/**
		 * @brief ��Ͽ�ؼ�Ԥ����ʽ�������ڴ���ǰ���ò���Ч��
		*/
		struct PreStyle
		{
			bool always_show_list = false;		///< �Ƿ�������ʾ�б�
			bool editable = false;				///< �Ƿ�ɱ༭
			bool sort = false;					///< �Ƿ��Զ�����
		};

		SysComboBox();

		SysComboBox(HWND hParent, RECT rct, std::wstring strText = L"");

		SysComboBox(HWND hParent, int x, int y, int w, int h, std::wstring strText = L"");

		/**
		 * @brief �ڴ����ؼ�ǰԤ����ʽ
		*/
		void PreSetStyle(PreStyle pre_style);

		LRESULT UpdateMessage(UINT msg, WPARAM wParam, LPARAM lParam, bool& bRet) override;

		/**
		 * @brief ע��ѡ����Ϣ
		 * @param[in] pFunc ��Ϣ��Ӧ����
		*/
		void RegisterSelMessage(void (*pFunc)(int sel, std::wstring wstrSelText));

		/**
		 * @brief ע��༭��Ϣ
		 * @param[in] pFunc ��Ϣ��Ӧ����
		*/
		void RegisterEditMessage(void (*pFunc)(std::wstring wstrText));

		/**
		 * @brief ��ȡѡ�е�����
		*/
		int GetSel() const { return m_nSel; }

		/**
		 * @brief ����ѡ�е�����
		 * @param[in] sel ѡ�е�����
		*/
		void SetSel(int sel);

		/**
		 * @brief ѡ�����ָ���ı���һ��
		 * @param[in] wstrText ָ���ı�
		 * @return �Ƿ�ѡ��ɹ�
		*/
		bool SelectString(std::wstring wstrText);

		/**
		 * @brief ������
		 * @param[in] wstrText ���ı�
		*/
		void AddString(std::wstring wstrText);

		/**
		 * @brief ������
		 * @param[in] index		����λ��
		 * @param[in] wstrText	���ı�
		*/
		void InsertString(int index, std::wstring wstrText);

		/**
		 * @brief ɾ����
		 * @param[in] index	������
		*/
		void DeleteString(int index);

		/**
		 * @brief ��ȡ�б���������
		*/
		int GetCount();

		/**
		 * @brief ����б�
		*/
		void Clear();

		/**
		 * @brief ��ʾ�б�
		 * @param[in] enable �Ƿ���ʾ�б�
		*/
		void ShowDropdown(bool enable);

		/**
		 * @brief �ж�ѡ�����Ƿ�仯
		*/
		bool IsSelChanged();

		/**
		 * @brief �ж��Ƿ񱻱༭
		*/
		bool IsEdited();
	};
}
