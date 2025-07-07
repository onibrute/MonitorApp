#include "pch.h"
#include "framework.h"
#include "MonitorApp.h"
#include "MonitorAppDlg.h"
#include "resource.h"
#include <algorithm>
#include <numeric>   
#include <atlimage.h> 
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <tchar.h>
#include "CHelpDialog.h"
#pragma comment(lib, "setupapi.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CMonitorAppDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_HSCROLL()
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_WM_DRAWITEM()

    ON_BN_CLICKED(IDC_BUTTON_STARTSTOP, &CMonitorAppDlg::OnBnClickedButtonStartstop)
    ON_BN_CLICKED(IDC_BUTTON_HELP, &CMonitorAppDlg::OnBnClickedButtonHelp)
    ON_BN_CLICKED(IDC_BUTTON_APPLYINTERVAL, &CMonitorAppDlg::OnBnClickedButtonApplyInterval)
    ON_BN_CLICKED(IDC_BUTTON_RESET, &CMonitorAppDlg::OnBnClickedButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_EXPORT_CSV, &CMonitorAppDlg::OnBnClickedExportCsv)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CMonitorAppDlg::OnBnClickedSaveImage)
    ON_BN_CLICKED(IDC_BUTTON_LOG, &CMonitorAppDlg::OnBnClickedButtonLog)
    ON_BN_CLICKED(IDC_CHECK_CH0, &CMonitorAppDlg::OnBnClickedCheckCh1)
    ON_BN_CLICKED(IDC_CHECK_CH1, &CMonitorAppDlg::OnBnClickedCheckCh2)
    ON_BN_CLICKED(IDC_CHECK_CH2, &CMonitorAppDlg::OnBnClickedCheckCh3)
    ON_BN_CLICKED(IDC_CHECK_CH3, &CMonitorAppDlg::OnBnClickedCheckCh4)
    ON_BN_CLICKED(IDC_CHECK_DARKMODE, &CMonitorAppDlg::OnBnClickedCheckDarkMode)
    ON_BN_CLICKED(IDC_CHECK_AUTOSCALE_Y, &CMonitorAppDlg::OnBnClickedCheckAutoscaleY)
    ON_BN_CLICKED(IDC_CHECK_SHOWPEAKS, &CMonitorAppDlg::OnBnClickedCheckShowPeaks)
    ON_EN_CHANGE(IDC_EDIT_INTERVAL, &CMonitorAppDlg::OnEnChangeEditInterval)
    
    ON_EN_CHANGE(IDC_EDIT_THRESH_MAX, &CMonitorAppDlg::OnEnChangeEditThreshMax)
    ON_EN_CHANGE(IDC_EDIT_THRESH_MIN, &CMonitorAppDlg::OnEnChangeEditThreshMin)
    ON_BN_CLICKED(IDC_SAVE_POINT_1, &CMonitorAppDlg::OnBnClickedSavePoint1)
    ON_BN_CLICKED(IDC_SAVE_POINT_2, &CMonitorAppDlg::OnBnClickedSavePoint2)
    ON_BN_CLICKED(IDC_BUTTON_DUMMY_TEST, &CMonitorAppDlg::OnBnClickedButtonDummyTest)
    ON_BN_CLICKED(IDC_CALIBRATE, &CMonitorAppDlg::OnBnClickedCalibrate)
    ON_CBN_SELCHANGE(IDC_COM_PORT_LIST, &CMonitorAppDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


COLORREF GetColorForLevel(int ch, WarningLevel level)
{
    static COLORREF normal[kNumChannels] = {
        RGB(0, 255, 255),
        RGB(128, 0, 128),
        RGB(0, 200, 0),
        RGB(200, 0, 200)
    };

    if (ch < 0 || ch >= kNumChannels)
        ch = 0;

    switch (level)
    {
    case NORMAL:   return normal[ch];
    case WARNING:  return RGB(255, 255, 0);
    case ALARM:    return RGB(255, 0, 0);
    default:       return normal[ch];
    }
}


CMonitorAppDlg::CMonitorAppDlg(CWnd* pParent)
    : CDialogEx(IDD_VOLTAGEMONITORAPP_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_channels[0].reserve(m_historySize);
    m_channels[1].reserve(m_historySize);
    m_channels[2].reserve(m_historySize);
    m_channels[3].reserve(m_historySize);

    for (int i = 0; i < kNumChannels; ++i)
    {
        m_lastRawRef1[i] = 0.0;
        m_lastRawRef2[i] = 0.0;
        m_lastRealRef1[i] = 0.0;
        m_lastRealRef2[i] = 0.0;
    }

}

CMonitorAppDlg::~CMonitorAppDlg()
{
    if (m_hbrBackground) {
        DeleteObject(m_hbrBackground);
        m_hbrBackground = nullptr;
    }

}

void CMonitorAppDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);


    DDX_Control(pDX, IDC_COM_PORT_LIST, m_comboComPorts);
    DDX_Control(pDX, IDC_STATIC_GRAPH, m_graphStatic);
    DDX_Control(pDX, IDC_SLIDER_HISTORY, m_sliderHistory);


    DDX_Control(pDX, IDC_RICH_WARNINGS, m_warnings);
    DDX_Control(pDX, IDC_RICH_ALARMS, m_alarms);


    DDX_Control(pDX, IDC_EDIT_THRESH_MIN, m_editThreshMin);
    DDX_Control(pDX, IDC_EDIT_THRESH_MAX, m_editThreshMax);


    DDX_Control(pDX, IDC_STATIC_AMPER_CH0, m_staticVoltage[0]);
    DDX_Control(pDX, IDC_STATIC_AVG_CH0, m_staticAverage[0]);
    DDX_Control(pDX, IDC_STATIC_AMPER_CH1, m_staticVoltage[1]);
    DDX_Control(pDX, IDC_STATIC_AVG_CH1, m_staticAverage[1]);
    DDX_Control(pDX, IDC_STATIC_AMPER_CH2, m_staticVoltage[2]);
    DDX_Control(pDX, IDC_STATIC_AVG_CH2, m_staticAverage[2]);
    DDX_Control(pDX, IDC_STATIC_AMPER_CH3, m_staticVoltage[3]);
    DDX_Control(pDX, IDC_STATIC_AVG_CH3, m_staticAverage[3]);


    DDX_Control(pDX, IDC_STATIC_HYSTERESIS_CH0, m_staticHystLabel[0]);
    DDX_Control(pDX, IDC_EDIT_HYSTERESIS_CH0, m_editHysteresis[0]);
    DDX_Control(pDX, IDC_STATIC_HYSTERESIS_CH1, m_staticHystLabel[1]);
    DDX_Control(pDX, IDC_EDIT_HYSTERESIS_CH1, m_editHysteresis[1]);
    DDX_Control(pDX, IDC_STATIC_HYSTERESIS_CH2, m_staticHystLabel[2]);
    DDX_Control(pDX, IDC_EDIT_HYSTERESIS_CH2, m_editHysteresis[2]);
    DDX_Control(pDX, IDC_STATIC_HYSTERESIS_CH3, m_staticHystLabel[3]);
    DDX_Control(pDX, IDC_EDIT_HYSTERESIS_CH3, m_editHysteresis[3]);


    DDX_Control(pDX, IDC_REF_CH0, m_editRefReal[0]);
    DDX_Control(pDX, IDC_REF_CH1, m_editRefReal[1]);
    DDX_Control(pDX, IDC_REF_CH2, m_editRefReal[2]);
    DDX_Control(pDX, IDC_REF_CH3, m_editRefReal[3]);
    DDX_Control(pDX, IDC_EDIT_COMMON, m_editRefCommon);
    DDX_Control(pDX, IDC_EDIT_SLAVE_ID, m_editSlaveId);


    DDX_Control(pDX, IDC_BUTTON_STARTSTOP, m_btnStartStop);
    DDX_Control(pDX, IDC_BUTTON_APPLYINTERVAL, m_btnApplyInterval);
    DDX_Control(pDX, IDC_BUTTON_RESET, m_btnReset);
    DDX_Control(pDX, IDC_BUTTON_EXPORT_CSV, m_btnExportCSV);
    DDX_Control(pDX, IDC_BUTTON_SAVE_IMAGE, m_btnSaveImage);
    DDX_Control(pDX, IDC_BUTTON_LOG, m_btnLog);
    DDX_Control(pDX, IDC_BUTTON_HELP, m_btnHelp);
    DDX_Control(pDX, IDC_BUTTON_DUMMY_TEST, m_btnDummyTest);
    DDX_Control(pDX, IDC_CALIBRATE, m_btnCalibrate);
    DDX_Control(pDX, IDC_SAVE_POINT_1, m_btnSavePoint1);
    DDX_Control(pDX, IDC_SAVE_POINT_2, m_btnSavePoint2);


    DDX_Control(pDX, IDC_CHECK_CH0, m_chkCh0);
    DDX_Control(pDX, IDC_CHECK_CH1, m_chkCh1);
    DDX_Control(pDX, IDC_CHECK_CH2, m_chkCh2);
    DDX_Control(pDX, IDC_CHECK_CH3, m_chkCh3);
    DDX_Control(pDX, IDC_CHECK_DARKMODE, m_chkDarkMode);
    DDX_Control(pDX, IDC_CHECK_AUTOSCALE_Y, m_chkAutoScaleY);
    DDX_Control(pDX, IDC_CHECK_SHOWPEAKS, m_chkShowPeaks);


    DDX_Control(pDX, IDC_STATIC_STATUS, m_staticStatus);
}


std::vector<CString> EnumerateCOMPorts()
{
    std::vector<CString> result;

    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS,
        nullptr,
        nullptr,
        DIGCF_PRESENT
    );

    if (hDevInfo == INVALID_HANDLE_VALUE)
        return result;

    SP_DEVINFO_DATA devInfoData = {};
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    TCHAR buffer[256];
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i)
    {
        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &devInfoData,
            SPDRP_FRIENDLYNAME,
            nullptr,
            (PBYTE)buffer,
            sizeof(buffer),
            nullptr))
        {
            CString name(buffer);
            int pos = name.Find(_T("(COM"));
            if (pos >= 0)
            {
                CString port = name.Mid(pos + 1);
                port = port.Left(port.GetLength() - 1);
                result.push_back(port);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return result;
}


BOOL CMonitorAppDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    AfxInitRichEdit2();


    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);


    m_comboComPorts.ResetContent();
    auto ports = EnumerateCOMPorts();
    for (const auto& port : ports)
    {
        m_comboComPorts.AddString(port);
    }

    if (m_comboComPorts.GetCount() > 0)
    {
        m_comboComPorts.SetCurSel(0);
        m_comboComPorts.GetLBText(0, m_selectedPort);

        CT2A ascii(m_selectedPort);
        std::string portName(ascii);
        m_serial.Open(portName, 9600);
        m_modbus = new ModbusRTU(&m_serial);
    }

    int count = m_comboComPorts.GetCount();
    CString msg;
    msg.Format(_T("Am detectat %d porturi COM"), count);
    AfxMessageBox(msg);



    SetDlgItemText(IDC_STATIC_STATUS, _T("Status: Oprit"));
    m_graphStatic.ModifyStyle(0, SS_NOTIFY);
    m_sliderHistory.SetRange(0, (int)(m_historySize - m_windowSize));
    m_sliderHistory.SetPos(m_sliderPos);

    m_warnings.SetWindowText(_T(""));
    m_alarms.SetWindowText(_T(""));

    m_alarms.SetEventMask(m_alarms.GetEventMask() | ENM_SCROLL);
    m_alarms.SetOptions(ECOOP_OR, ECO_AUTOVSCROLL | ECO_AUTOHSCROLL);
    m_alarms.SetSel(-1, -1);

    CString tmp;
    tmp.Format(_T("%.2f"), m_threshMax);         m_editThreshMax.SetWindowText(tmp);
    tmp.Format(_T("%.2f"), m_threshMin);         m_editThreshMin.SetWindowText(tmp);

    m_editSlaveId.SetWindowText(_T("2"));


    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        tmp.Format(_T("%.2f"), m_hysteresis[ch]);
        m_editHysteresis[ch].SetWindowText(tmp);
    }


    CheckDlgButton(IDC_CHECK_CH0, BST_CHECKED);
    CheckDlgButton(IDC_CHECK_CH1, BST_CHECKED);
    CheckDlgButton(IDC_CHECK_CH2, BST_CHECKED);
    CheckDlgButton(IDC_CHECK_CH3, BST_CHECKED);
    CheckDlgButton(IDC_CHECK_DARKMODE, BST_UNCHECKED);
    CheckDlgButton(IDC_CHECK_AUTOSCALE_Y, BST_UNCHECKED);
    CheckDlgButton(IDC_CHECK_SHOWPEAKS, BST_UNCHECKED);


    DrawGraph();
    m_graphStatic.SetFocus();


    m_hAccel = LoadAccelerators(
        AfxGetInstanceHandle(),
        MAKEINTRESOURCE(IDR_ACCELERATOR1)
    );

    if (m_darkMode)
    {
        SetBackgroundColor(RGB(30, 30, 30));  
    }
    UpdateDarkModeUI();  

    return TRUE;
}



void CMonitorAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialogEx::OnSysCommand(nID, lParam);
}

void CMonitorAppDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);
        int cx = GetSystemMetrics(SM_CXICON), cy = GetSystemMetrics(SM_CYICON);
        CRect rect; GetClientRect(&rect);
        int x = (rect.Width() - cx + 1) / 2;
        int y = (rect.Height() - cy + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}



void CMonitorAppDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
    if (!m_darkMode)
    {
        CDialogEx::OnDrawItem(nIDCtl, lpDIS);
        return;
    }

    CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    CRect rc = lpDIS->rcItem;

    pDC->FillSolidRect(&rc, RGB(30, 30, 30));
    pDC->DrawEdge(&rc, EDGE_RAISED, BF_RECT);

    CString text;
    GetDlgItem(lpDIS->CtlID)->GetWindowText(text);
    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(text, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}




HCURSOR CMonitorAppDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CMonitorAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (!m_darkMode)
        return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    const int ctrlId = pWnd->GetDlgCtrlID();

    pDC->SetTextColor(RGB(255, 255, 255));
    pDC->SetBkColor(RGB(30, 30, 30));
    pDC->SetBkMode(TRANSPARENT);

    switch (ctrlId)
    {
        // Group boxes
    case IDC_STATIC_MENU:
    case IDC_STATIC_CH:
    case IDC_STATIC_HYSTERESIS:
    case IDC_STATIC_PORTS:
    case IDC_STATIC_POINT1:
    case IDC_STATIC_POINT2:

        
    case IDC_CHECK_CH0:
    case IDC_CHECK_CH1:
    case IDC_CHECK_CH2:
    case IDC_CHECK_CH3:
    case IDC_CHECK_DARKMODE:
    case IDC_CHECK_AUTOSCALE_Y:
    case IDC_CHECK_SHOWPEAKS:

        return m_hbrBackground;

    default:
        break;
    }

    switch (nCtlColor)
    {
    case CTLCOLOR_DLG:
    case CTLCOLOR_STATIC:
    case CTLCOLOR_EDIT:
    case CTLCOLOR_BTN:
    case CTLCOLOR_LISTBOX:
    case CTLCOLOR_MSGBOX:
    case CTLCOLOR_SCROLLBAR:
        return m_hbrBackground;
    }

    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}




BOOL CMonitorAppDlg::OnEraseBkgnd(CDC* pDC)
{
    if (m_darkMode)
    {
        CRect rect;
        GetClientRect(&rect);
        pDC->FillSolidRect(&rect, RGB(30, 30, 30));
        return TRUE;
    }

    return CDialogEx::OnEraseBkgnd(pDC);
}



BOOL CMonitorAppDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    double factor = 1.1;
    if (GetAsyncKeyState(VK_CONTROL) < 0)
        m_zoomFactorY *= (zDelta > 0 ? factor : 1.0 / factor);
    else
        m_zoomFactorX *= (zDelta > 0 ? factor : 1.0 / factor);
    DrawGraph();
    return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}

void CMonitorAppDlg::OnLButtonDown(UINT nFlags, CPoint pt)
{
    CRect rc; m_graphStatic.GetWindowRect(&rc);
    ScreenToClient(&rc);
    if (rc.PtInRect(pt))
    {
        m_isPanning = true;
        m_lastMousePos = pt;
        SetCapture();
    }
    CDialogEx::OnLButtonDown(nFlags, pt);
}

void CMonitorAppDlg::OnLButtonUp(UINT nFlags, CPoint pt)
{
    if (m_isPanning)
    {
        m_isPanning = false;
        ReleaseCapture();
    }
    CDialogEx::OnLButtonUp(nFlags, pt);
}


void CMonitorAppDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pSB)
{
    if (pSB->GetSafeHwnd() == m_sliderHistory.GetSafeHwnd())
    {
        m_sliderPos = m_sliderHistory.GetPos();
        DrawGraph();
    }
    CDialogEx::OnHScroll(nSBCode, nPos, pSB);
}


void CMonitorAppDlg::OnBnClickedButtonStartstop()
{
    if (!m_isRunning)
    {
        m_isRunning = true;
        m_sampleCount = 0;
        for (auto& buf : m_channels) buf.clear();
        ResetGraph();
        m_startLogTick = GetTickCount64();
        SetTimer(m_timerId, m_samplingIntervalMs, nullptr);
        SetDlgItemText(IDC_BUTTON_STARTSTOP, _T("Stop"));
        SetDlgItemText(IDC_STATIC_STATUS, _T("Status: Rulează"));
    }
    else
    {
        m_isRunning = false;
        KillTimer(m_timerId);
        SetDlgItemText(IDC_BUTTON_STARTSTOP, _T("Start"));
        SetDlgItemText(IDC_STATIC_STATUS, _T("Status: Oprit"));
    }
}

void CMonitorAppDlg::OnBnClickedButtonLog()
{
    if (!m_isLogging)
    {
        if (m_logFilePath.IsEmpty())
        {
            CFileDialog dlg(FALSE, _T("csv"), _T("log.csv"),
                OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                _T("CSV Files (*.csv)|*.csv||"), this);
            if (dlg.DoModal() != IDOK) return;
            m_logFilePath = dlg.GetPathName();
        }
        if (m_logFile.Open(m_logFilePath,
            CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::typeText))
        {
            if (m_logFile.GetLength() == 0)
                m_logFile.WriteString(_T("Time_ms,Ch1,Ch2,Ch3,Ch4\r\n"));
            m_logFile.SeekToEnd();
            m_startLogTick = GetTickCount64();
            m_isLogging = true;
            SetDlgItemText(IDC_BUTTON_LOG, _T("Stop Logging"));
            if (!m_isRunning) OnBnClickedButtonStartstop();
        }
        else AfxMessageBox(_T("Nu pot deschide: ") + m_logFilePath);
    }
    else
    {
        m_logFile.Close();
        m_isLogging = false;
        SetDlgItemText(IDC_BUTTON_LOG, _T("Start Logging"));
    }
}


void CMonitorAppDlg::OnBnClickedExportCsv()
{
    CFileDialog dlg(FALSE, _T("csv"), _T("export.csv"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("CSV Files (*.csv)|*.csv||"), this);
    if (dlg.DoModal() != IDOK) return;
    CString path = dlg.GetPathName();
    CStdioFile f;
    if (f.Open(path, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
    {
        f.WriteString(_T("Index,Ch1,Ch2,Ch3,Ch4\r\n"));
        for (int i = 0; i < (int)m_channels[0].size(); ++i)
        {
            CString L;
            L.Format(_T("%d,%.3f,%.3f,%.3f,%.3f\r\n"),
                i,
                m_channels[0][i].y / 10.0,
                m_channels[1][i].y / 10.0,
                m_channels[2][i].y / 10.0,
                m_channels[3][i].y / 10.0
            );
            f.WriteString(L);
        }
        f.Close();
    }
}


void CMonitorAppDlg::OnBnClickedSaveImage()
{
    CFileDialog dlg(FALSE, _T("png"), _T("graph.png"),
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T("PNG Files (*.png)|*.png||BMP Files (*.bmp)|*.bmp||"),
        this);
    if (dlg.DoModal() != IDOK) return;

    CString path = dlg.GetPathName();
    DrawGraph(path); 
}



void CMonitorAppDlg::OnBnClickedButtonHelp()
{
    CHelpDialog dlg(this);
    dlg.DoModal();
}


void CMonitorAppDlg::OnEnChangeEditInterval() {}




void CMonitorAppDlg::OnBnClickedCheckCh1() { m_enableCh0 = !m_enableCh0; DrawGraph(); }
void CMonitorAppDlg::OnBnClickedCheckCh2() { m_enableCh1 = !m_enableCh1; DrawGraph(); }
void CMonitorAppDlg::OnBnClickedCheckCh3() { m_enableCh2 = !m_enableCh2; DrawGraph(); }
void CMonitorAppDlg::OnBnClickedCheckCh4() { m_enableCh3 = !m_enableCh3; DrawGraph(); }

void CMonitorAppDlg::OnBnClickedCheckDarkMode()
{
    m_darkMode = (IsDlgButtonChecked(IDC_CHECK_DARKMODE) == BST_CHECKED);
    UpdateDarkModeUI();
}


void CMonitorAppDlg::UpdateDarkModeUI()
{
    
    if (m_hbrBackground)
        ::DeleteObject(m_hbrBackground);
    m_hbrBackground = ::CreateSolidBrush(m_darkMode ? RGB(30, 30, 30) : GetSysColor(COLOR_BTNFACE));

   
    SetClassLongPtr(m_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)m_hbrBackground);

    
    for (CWnd* c = GetWindow(GW_CHILD); c; c = c->GetNextWindow())
        ::SetWindowTheme(c->GetSafeHwnd(), m_darkMode ? L"" : L"Explorer", nullptr);

    
    COLORREF bg = m_darkMode ? RGB(30, 30, 30) : GetSysColor(COLOR_WINDOW);
    COLORREF fg = m_darkMode ? RGB(255, 255, 255) : GetSysColor(COLOR_WINDOWTEXT);
    COLORREF face = m_darkMode ? RGB(30, 30, 30) : GetSysColor(COLOR_BTNFACE);

   
    m_warnings.SetBackgroundColor(FALSE, bg);
    m_alarms.SetBackgroundColor(FALSE, bg);

    CHARFORMAT2 cf = {};
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR | CFM_BACKCOLOR;
    cf.crTextColor = fg;
    cf.crBackColor = bg;

    m_warnings.SetDefaultCharFormat(cf);
    m_alarms.SetDefaultCharFormat(cf);

    
    auto themeButton = [&](CMFCButton& b) {
        b.SetFaceColor(face, TRUE);  
        b.SetTextColor(fg);          
        b.RedrawWindow();
        Invalidate(); 
        UpdateWindow();

        };


    
    themeButton(m_btnStartStop);
    themeButton(m_btnApplyInterval);
    themeButton(m_btnReset);
    themeButton(m_btnExportCSV);
    themeButton(m_btnSaveImage);
    themeButton(m_btnLog);
    themeButton(m_btnHelp);
    themeButton(m_btnDummyTest);
    themeButton(m_btnCalibrate);
    themeButton(m_btnSavePoint1);  
    themeButton(m_btnSavePoint2);  

    
    for (CWnd* pWnd = GetWindow(GW_CHILD); pWnd; pWnd = pWnd->GetNextWindow()) {
        pWnd->Invalidate();
        pWnd->UpdateWindow();
    }

    RedrawWindow(nullptr, nullptr,
        RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);

    DrawGraph();
}


void CMonitorAppDlg::OnBnClickedCheckAutoscaleY()
{
    m_autoScaleY = !m_autoScaleY; DrawGraph();
}
void CMonitorAppDlg::OnBnClickedCheckShowPeaks()
{
    m_showPeaks = (IsDlgButtonChecked(IDC_CHECK_SHOWPEAKS) == BST_CHECKED); DrawGraph();
}

void CMonitorAppDlg::OnEnChangeEditThreshMax()
{
    CString s; m_editThreshMax.GetWindowText(s); m_threshMax = _tstof(s);
}
void CMonitorAppDlg::OnEnChangeEditThreshMin()
{
    CString s; m_editThreshMin.GetWindowText(s); m_threshMin = _tstof(s);
}

void CMonitorAppDlg::OnBnClickedButtonApplyInterval()
{
    BOOL ok; int v = GetDlgItemInt(IDC_EDIT_INTERVAL, &ok, TRUE);
    if (ok && v > 0)
    {
        m_samplingIntervalMs = v;
        if (m_isRunning)
        {
            KillTimer(m_timerId);
            SetTimer(m_timerId, m_samplingIntervalMs, nullptr);
        }
    }
}


void CMonitorAppDlg::OnBnClickedButtonReset()
{
    for (auto& b : m_channels) b.clear();
    m_panOffsetX_px = 0.0;
    m_zoomFactorX = m_zoomFactorY = 1.0;
    m_sliderPos = (int)(m_historySize - m_windowSize);
    m_sliderHistory.SetPos(m_sliderPos);
    m_warnings.SetWindowText(_T(""));
    DrawGraph();
}
void CMonitorAppDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == m_timerId && m_isRunning)
    {

        CString tmp;
        for (int ch = 0; ch < kNumChannels; ++ch)
        {
            m_editHysteresis[ch].GetWindowText(tmp);
            m_hysteresis[ch] = _tstof(tmp);
        }

        m_editThreshMin.GetWindowText(tmp); m_threshMin = _tstof(tmp);
        m_editThreshMax.GetWindowText(tmp); m_threshMax = _tstof(tmp);

        double v[4] = { 0 };


        if (m_useSimulation)
        {
            v[0] = 5.0 * sin(m_sampleCount * 0.10) + 5.0;
            v[1] = 3.0 * cos(m_sampleCount * 0.08) + 5.0;
            v[2] = 4.0 * sin(m_sampleCount * 0.13 + 1.0) + 5.0;
            v[3] = 2.0 * cos(m_sampleCount * 0.05 + 0.5) + 5.0;
        }
        else
        {
            std::vector<uint16_t> regs;
            CString errorMsg;

            CString s;
            m_editSlaveId.GetWindowText(s);
            int slaveId = _ttoi(s);

            if (m_modbus && m_modbus->ReadHoldingRegisters(slaveId, 0, 4, regs, errorMsg))

            {
                for (int i = 0; i < 4; ++i)
                {
                    double raw = regs[i] / 1000.0;
                    v[i] = (m_slope[i] != 0.0) ? m_slope[i] * raw + m_offset[i] : raw;
                }
            }
            else
            {
                for (int i = 0; i < 4; ++i)
                    v[i] = 0.0;

                static int errCount = 0;
                if (++errCount % 10 == 0 && !errorMsg.IsEmpty())
                    AppendToAlarms(errorMsg, RGB(255, 0, 0));
            }
        }


        for (int ch = 0; ch < kNumChannels; ++ch)
            m_lastVoltage[ch] = v[ch];


        auto evalWithHyst = [&](double val, WarningLevel oldLvl, int ch) {
            double h = m_hysteresis[ch];
            double aOnH = m_threshMax, aOffH = m_threshMax - h;
            double aOnL = m_threshMin, aOffL = m_threshMin + h;
            double wOnH = m_threshMax * 0.9, wOffH = wOnH - h;
            double wOnL = m_threshMin * 1.1, wOffL = wOnL + h;

            if (oldLvl == ALARM && (val > aOffH || val < aOffL)) return ALARM;
            if (val > aOnH || val < aOnL) return ALARM;

            if (oldLvl == WARNING && (val > wOffH || val < wOffL)) return WARNING;
            if (val > wOnH || val < wOnL) return WARNING;

            return NORMAL;
            };

        for (int ch = 0; ch < kNumChannels; ++ch)
        {
            WarningLevel old = m_lastLevel[ch];
            m_lastLevel[ch] = evalWithHyst(m_lastVoltage[ch], old, ch);

            if (m_lastLevel[ch] == ALARM && old != ALARM)
            {
                ULONGLONG elapsed = GetTickCount64() - m_startLogTick;
                ULONGLONG s = elapsed / 1000;
                ULONGLONG ms = elapsed % 1000;
                CString msg;
                msg.Format(_T("Alarmă CH%d: %.2f mA @ %llu.%03llus"),
                    ch + 1, m_lastVoltage[ch], s, ms);
                m_alarms.SetSel(-1, -1);
                CHARFORMAT cf = { sizeof(cf) };
                cf.dwMask = CFM_COLOR; cf.crTextColor = RGB(255, 0, 0);
                m_alarms.SetSelectionCharFormat(cf);
                m_alarms.ReplaceSel(msg + _T("\r\n"));
                m_alarms.LineScroll(m_alarms.GetLineCount());
                cf.crTextColor = RGB(0, 0, 0);
                m_alarms.SetSelectionCharFormat(cf);
            }
        }
        if (m_isLogging)
        {
            ULONGLONG t = GetTickCount64() - m_startLogTick;
            CString csv;
            csv.Format(_T("%llu,%.3f,%.3f,%.3f,%.3f\r\n"),
                t, v[0], v[1], v[2], v[3]);
            m_logFile.WriteString(csv);
        }


        for (int ch = 0; ch < kNumChannels; ++ch)
        {
            auto& buf = m_channels[ch];
            if (buf.size() >= m_historySize) buf.erase(buf.begin());
            buf.push_back(CPoint((int)m_sampleCount, (int)(m_lastVoltage[ch] * 10)));
        }

        DrawGraph();
        ++m_sampleCount;
    }

    CDialogEx::OnTimer(nIDEvent);
}



void CMonitorAppDlg::ResetGraph()
{
    for (auto& buf : m_channels)
        buf.clear();

    if (CWnd* p = GetDlgItem(IDC_STATIC_GRAPH))
    {
        p->Invalidate(FALSE);
        p->UpdateWindow();
    }
}


void CMonitorAppDlg::ComputePeaks()
{
    m_peaksIdx.clear();
    m_peaksCh.clear();

    auto detect = [&](int ch, const std::vector<CPoint>& buf){
        if ((ch == 0 && !m_enableCh0) ||
            (ch == 1 && !m_enableCh1) ||
            (ch == 2 && !m_enableCh2) ||
            (ch == 3 && !m_enableCh3))
            return;

        for (size_t i = 1; i + 1 < buf.size(); ++i)
        {
            int y0 = buf[i - 1].y, y1 = buf[i].y, y2 = buf[i + 1].y;
            if ((y1 >= y0 && y1 >= y2) && (y1 > y0 || y1 > y2)) {
                m_peaksIdx.push_back((int)i);
                m_peaksCh.push_back(ch);
            }
        }
    };

    for (int ch = 0; ch < kNumChannels; ++ch)
        detect(ch, m_channels[ch]);
}


void CMonitorAppDlg::DrawGraph(const CString& savePath)
{
    CWnd* pg = GetDlgItem(IDC_STATIC_GRAPH);
    if (!pg) return;


    if (m_showPeaks)
        ComputePeaks();


    CClientDC dc(pg);
    CRect r; pg->GetClientRect(&r);
    CDC mdc; mdc.CreateCompatibleDC(&dc);
    CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, r.Width(), r.Height());
    CBitmap* oldBmp = mdc.SelectObject(&bmp);


    COLORREF bg = m_darkMode ? RGB(30, 30, 30) : RGB(255, 255, 255);
    mdc.FillSolidRect(r, bg);
    mdc.SetTextColor(m_darkMode ? RGB(220, 220, 220) : RGB(0, 0, 0));
    mdc.SetBkMode(TRANSPARENT);


    const int ml = 40, mr = 10, mt = 10, mb = 25;
    int ox = ml, oy = r.Height() - mb;
    int plotW = r.Width() - ml - mr, plotH = r.Height() - mt - mb;

    double yMin = 0, yMax = 10;
    if (m_autoScaleY)
    {
        yMin = 1e9; yMax = -1e9;
        for (int ch = 0; ch < kNumChannels; ++ch)
            for (auto& p : m_channels[ch]) {
                double v = p.y / 10.0;
                yMin = min(yMin, v);
                yMax = max(yMax, v);
            }
        if (yMin >= yMax) { yMin--; yMax++; }
    }
    double yRange = m_autoScaleY ? (yMax - yMin) : 10.0;
    double xScale = (plotW / (double)m_windowSize) * m_zoomFactorX;
    double yScale = (plotH / yRange) * m_zoomFactorY;


    COLORREF gc = m_darkMode ? RGB(80, 80, 80) : RGB(220, 220, 220);
    CPen gridPen(PS_DOT, 1, gc), * oldPen;
    for (int i = 0; i <= 10; ++i)
    {
        int y = oy - int(i * (yRange / 10.0) * yScale);
        CString lbl; lbl.Format(_T("%.1fmA"), yMin + i * (yRange / 10.0));
        mdc.TextOutW(5, y - 7, lbl);
        oldPen = mdc.SelectObject(&gridPen);
        mdc.MoveTo(ox, y); mdc.LineTo(r.Width() - mr, y);
        mdc.SelectObject(oldPen);
    }
    for (int i = 0; i <= m_windowSize; i += 20)
    {
        int x = ox + int(i * xScale);
        CString lbl; lbl.Format(_T("%d"), i);
        mdc.TextOutW(x - 10, oy + 5, lbl);
        oldPen = mdc.SelectObject(&gridPen);
        mdc.MoveTo(x, oy); mdc.LineTo(x, mt);
        mdc.SelectObject(oldPen);
    }


    int savedDC = mdc.SaveDC();
    CRgn clip; clip.CreateRectRgn(ox, mt, r.Width() - mr, oy);
    mdc.SelectClipRgn(&clip);


    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        bool en = (ch == 0 && m_enableCh0) ||
            (ch == 1 && m_enableCh1) ||
            (ch == 2 && m_enableCh2) ||
            (ch == 3 && m_enableCh3);


        if (!en) continue;
        WarningLevel lvl = m_lastLevel[ch];
        CPen tracePen(PS_SOLID, 2, GetColorForLevel(ch, lvl));
        oldPen = mdc.SelectObject(&tracePen);
        if (!oldPen) continue;

        auto& buf = m_channels[ch];
        int cnt = (int)buf.size();
        if (cnt > 0)
        {
            int base = max(0, cnt - m_windowSize);
            base = min(base, m_sliderPos);
            int s0 = buf[base].x;
            for (int i = base; i < min(cnt, base + m_windowSize); ++i)
            {
                int dx = buf[i].x - s0;
                int xx = ox + int(dx * xScale) + int(m_panOffsetX_px);
                int yy = oy - int(((buf[i].y / 10.0) - yMin) * yScale);
                if (i == base) mdc.MoveTo(xx, yy);
                else         mdc.LineTo(xx, yy);
            }
        }
        mdc.SelectObject(oldPen);
    }


    if (m_showPeaks)
    {
        int base[kNumChannels], s0[kNumChannels];
        for (int ch = 0; ch < kNumChannels; ++ch)
        {
            int cnt = (int)m_channels[ch].size();
            base[ch] = max(0, cnt - m_windowSize);
            base[ch] = min(base[ch], m_sliderPos);
            s0[ch] = cnt > 0 ? m_channels[ch][base[ch]].x : 0;
        }

        for (size_t i = 0; i < m_peaksIdx.size(); ++i)
        {

            int ch = m_peaksCh[i];
            bool channelEnabled = false;
            switch (ch)
            {
            case 0: channelEnabled = m_enableCh0; break;
            case 1: channelEnabled = m_enableCh1; break;
            case 2: channelEnabled = m_enableCh2; break;
            case 3: channelEnabled = m_enableCh3; break;
            }

            if (!channelEnabled) continue;


            int idx = m_peaksIdx[i];
            if (idx < base[ch] || idx >= base[ch] + m_windowSize) continue;


            CPen peakPen(PS_SOLID, 2, GetColorForLevel(ch, m_lastLevel[ch]));
            CPen* oldPen = mdc.SelectObject(&peakPen);
            if (!oldPen) continue;

            int dx = m_channels[ch][idx].x - s0[ch];
            int xx = ox + int(dx * xScale) + int(m_panOffsetX_px);
            int yy = oy - int(((m_channels[ch][idx].y / 10.0) - yMin) * yScale);
            mdc.Ellipse(xx - 4, yy - 4, xx + 4, yy + 4);

            mdc.SelectObject(oldPen);
        }

        if (m_darkMode)
            mdc.SetBkColor(RGB(30, 30, 30));
        else
            mdc.SetBkColor(RGB(255, 255, 255));

        if (m_darkMode)
            mdc.SetTextColor(RGB(220, 220, 220));
        else
            mdc.SetTextColor(RGB(0, 0, 0));

    }

    mdc.RestoreDC(savedDC);
    dc.BitBlt(0, 0, r.Width(), r.Height(), &mdc, 0, 0, SRCCOPY);
    mdc.SelectObject(oldBmp);

    if (!savePath.IsEmpty())
    {
        CImage img;
        img.Attach((HBITMAP)bmp.Detach());
        img.Save(savePath);
        img.Destroy();
    }


    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        CString itxt, atxt;

        double current = m_lastVoltage[ch];
        itxt.Format(_T("Curent CH%d: %.2f mA"), ch, current);

        double sum = 0;
        for (auto& p : m_channels[ch]) sum += p.y / 10.0;
        double avg = m_channels[ch].empty() ? 0.0 : (sum / m_channels[ch].size());
        avg = m_slope[ch] * avg + m_offset[ch];
        atxt.Format(_T("Average CH%d: %.2f mA"), ch, avg);

        m_staticVoltage[ch].SetWindowText(itxt);
        m_staticAverage[ch].SetWindowText(atxt);
    }


    if (m_showPeaks)
    {
       
        int scrollPos = m_warnings.GetScrollPos(SB_VERT);       
        m_warnings.SetRedraw(FALSE);      
        m_warnings.SetWindowText(_T(""));
        size_t count = 1;

        for (size_t i = 0; i < m_peaksIdx.size(); ++i)
        {
            int ch = m_peaksCh[i];
            if (!((ch == 0 && m_enableCh0) ||
                (ch == 1 && m_enableCh1) ||
                (ch == 2 && m_enableCh2) ||
                (ch == 3 && m_enableCh3)))
                continue;

            int idx = m_peaksIdx[i];
            double v = m_channels[ch][idx].y / 10.0;

            CString line;
            line.Format(_T("Peak %zu (CH%d): %.2f mA\r\n"), count++, ch, v);
            m_warnings.ReplaceSel(line);
        }

        
        m_warnings.SetScrollPos(SB_VERT, scrollPos);
        m_warnings.SetRedraw(TRUE);
        m_warnings.Invalidate();
    }
    else
    {
        m_warnings.SetWindowText(_T(""));
    }


}


void CMonitorAppDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_isPanning)
    {
        int dx = point.x - m_lastMousePos.x;
        m_panOffsetX_px += dx;
        m_lastMousePos = point;
        DrawGraph();
    }
    else
    {
        CRect rc;
        m_graphStatic.GetWindowRect(&rc);
        ScreenToClient(&rc);
        if (rc.PtInRect(point) && !m_channels[0].empty())
        {

            const int ml = 40, mr = 10;
            int plotW = rc.Width() - ml - mr;
            double xScale = (plotW / (double)m_windowSize) * m_zoomFactorX;

            int avail = max(0, (int)m_channels[0].size() - m_windowSize);
            int base = min(m_sliderPos, avail);
            int dxPts = point.x - (rc.left + ml) - int(m_panOffsetX_px);
            int idx = base + int(dxPts / xScale + 0.5);

            if (idx >= 0 && idx < (int)m_channels[0].size())
            {
                auto it = std::find(m_peaksIdx.begin(), m_peaksIdx.end(), idx);
                CString msg;
                if (it != m_peaksIdx.end())
                {
                    int peakNum = int(it - m_peaksIdx.begin()) + 1;
                    double peakV = m_channels[0][idx].y / 10.0;
                    msg.Format(_T("Peak %d: %.2f mA"), peakNum, peakV);
                }
                else
                {
                    double v1 = m_channels[0][idx].y / 10.0;
                    double v2 = m_channels[1][idx].y / 10.0;
                    double v3 = m_channels[2][idx].y / 10.0;
                    double v4 = m_channels[3][idx].y / 10.0;
                    msg.Format(_T("Sample %d: C1=%.2fmA C2=%.2fmA C3=%.2fmA C4=%.2fmA"),
                        idx, v1, v2, v3, v4);
                }
                m_warnings.SetWindowText(msg);
            }
            else
            {
                m_warnings.SetWindowText(_T(""));
            }
        }
        else
        {
            m_warnings.SetWindowText(_T(""));
        }
    }

    CDialogEx::OnMouseMove(nFlags, point);
}

void CMonitorAppDlg::OnBnClickedButtonDummyTest()
{
    m_useSimulation = !m_useSimulation;

    CString label = m_useSimulation ? _T("Disable Dummy") : _T("Enable Dummy");
    SetDlgItemText(IDC_BUTTON_DUMMY_TEST, label);

    CString status;
    GetDlgItemText(IDC_STATIC_STATUS, status);
    if (status.Find(_T("Running")) >= 0)
    {
        status.Format(_T("Status: Running (%s)"),
            m_useSimulation ? _T("Simulare") : _T("Protocol"));
        SetDlgItemText(IDC_STATIC_STATUS, status);
    }
}

void CMonitorAppDlg::OnCbnSelchangeCombo1()
{

}
double CMonitorAppDlg::ReadAverageRaw(int ch, int numSamples, int delayMs)
{
    double sum = 0;
    int valid = 0;

    for (int i = 0; i < numSamples; ++i)
    {
        std::vector<uint16_t> regs;
        CString error;
        if (m_modbus && m_modbus->ReadHoldingRegisters(1, ch, 1, regs, error))

        {
            sum += regs[0] / 1000.0;
            ++valid;
        }
        Sleep(delayMs);
    }

    return (valid > 0) ? (sum / valid) : 0.0;
}

void CMonitorAppDlg::OnBnClickedSavePoint2()
{
    CString s;
    m_editRefCommon.GetWindowText(s);
    double realVal = _ttof(s) / 2.0;

    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        double raw = ReadAverageRaw(ch);
        m_lastRawRef2[ch] = raw;
        m_lastRealRef2[ch] = realVal;

        CString msg;
        msg.Format(_T("CH%d: punct comun salvat - brut=%.3f mA, real=%.3f mA"), ch, raw, realVal);
        AppendToAlarms(msg, RGB(0, 128, 0));
    }
}


void CMonitorAppDlg::OnBnClickedSavePoint1()
{
    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        CString s;
        m_editRefReal[ch].GetWindowText(s);
        if (s.Trim().IsEmpty()) continue;

        double realVal = _ttof(s) / 2.0;
        double raw = ReadAverageRaw(ch);
        m_lastRawRef1[ch] = raw;
        m_lastRealRef1[ch] = realVal;

        CString msg;
        msg.Format(_T("CH%d: punct individual salvat - brut=%.3f mA, real=%.3f mA"), ch, raw, realVal);
        AppendToAlarms(msg, RGB(0, 0, 160));
    }
}



void CMonitorAppDlg::OnBnClickedCalibrate()
{
    for (int ch = 0; ch < kNumChannels; ++ch)
    {
        double b0 = m_lastRawRef1[ch];
        double r0 = m_lastRealRef1[ch];
        double b1 = m_lastRawRef2[ch];
        double r1 = m_lastRealRef2[ch];

        double db = b1 - b0;
        if (fabs(db) < 1e-6)
        {
            CString err;
            err.Format(_T("CH%d: eroare – brut prea apropiat (%.3f ≈ %.3f)"), ch, b0, b1);
            AppendToAlarms(err, RGB(200, 0, 0));
            continue;
        }

        double slope = (r1 - r0) / db;
        double offset = r0 - slope * b0;

         if (!_finite(slope) || !_finite(offset) || fabs(slope) > 100 || fabs(offset) > 100)
        {
            CString err;
            err.Format(_T("CH%d: slope/offset invalid – slope=%.4f, offset=%.4f"), ch, slope, offset);
            AppendToAlarms(err, RGB(255, 0, 0));
            continue;
        }

        m_slope[ch] = slope;
        m_offset[ch] = offset;

        CString msg;
        msg.Format(_T("CH%d calibrat: slope=%.4f, offset=%.4f"), ch, slope, offset);
        AppendToAlarms(msg, RGB(0, 128, 0));
    }

    DrawGraph();
}

void CMonitorAppDlg::AppendToAlarms(const CString& msg, COLORREF color)
{
    m_alarms.SetSel(-1, -1);
    CHARFORMAT cf = { sizeof(cf) };
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = color;
    m_alarms.SetSelectionCharFormat(cf);
    m_alarms.ReplaceSel(msg + _T("\r\n"));
}