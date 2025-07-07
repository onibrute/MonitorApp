// MonitorApp.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "MonitorApp.h"
#include "MonitorAppDlg.h"
#include "ISerialProtocol.h"
#include "SerialPort.h"
#include "ModbusRTU.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMonitorAppApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CMonitorAppApp::CMonitorAppApp()
{
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;    
}


CMonitorAppApp theApp;


BOOL CMonitorAppApp::InitInstance()
{
  
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxInitRichEdit2();
    AfxEnableControlContainer();

    CShellManager* pShellManager = new CShellManager;
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    CMonitorAppDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: handle when the dialog is dismissed with OK
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: handle when the dialog is dismissed with Cancel
    }
    else if (nResponse == -1)
    {
        TRACE(traceAppMsg, 0, "Warning: dialog creation failed, terminating unexpectedly.\n");
    }

    if (pShellManager)
        delete pShellManager;

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
    ControlBarCleanUp();
#endif


    return FALSE;
}

