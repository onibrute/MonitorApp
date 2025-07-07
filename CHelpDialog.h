#pragma once
#include "afxdialogex.h"
#include <afxrich.h>
#include <map>

class CHelpDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CHelpDialog)

public:
	CHelpDialog(CWnd* pParent = nullptr);
	virtual ~CHelpDialog();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HELP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnRichHelpLinkClick(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

private:
	CRichEditCtrl m_richHelp;

	enum HelpTopic {
		HT_None,
		HT_Start,
		HT_Interval,
		HT_Reset,
		HT_CSV,
		HT_Image,
		HT_Log,
		HT_Checkbox,
		HT_Auto,
		HT_Peak,
		HT_Calibrare
	};

	void SetHelpText();
	HelpTopic GetHelpTopicFromText(const CString& txt);
};
