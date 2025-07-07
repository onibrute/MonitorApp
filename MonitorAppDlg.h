#pragma once

#include "resource.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <afxrich.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include "SerialPort.h"
#include "ModbusRTU.h"


enum WarningLevel {
    NORMAL = 0,
    WARNING,
    ALARM,
    SHUTDOWN
};


static constexpr int kNumChannels = 4;

COLORREF GetColorForLevel(int ch, WarningLevel level);


class CMonitorAppDlg : public CDialogEx
{
public:
    CMonitorAppDlg(CWnd* pParent = nullptr);
    virtual ~CMonitorAppDlg();
    enum { IDD = IDD_MONITORAPP_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    DECLARE_MESSAGE_MAP()

private:
    HICON   m_hIcon;
    HBRUSH  m_hbrBackground = nullptr;
    HBRUSH  m_hbrEdit = nullptr;
    HBRUSH  m_hbrStatic = nullptr;

    CStatic         m_graphStatic;
    CSliderCtrl     m_sliderHistory;
    CRichEditCtrl   m_warnings, m_alarms;
    CEdit           m_editThreshMin, m_editThreshMax;
    CStatic         m_staticVoltage[kNumChannels];
    CStatic         m_staticAverage[kNumChannels];
    CStatic         m_staticHystLabel[kNumChannels];
    CEdit           m_editHysteresis[kNumChannels];
    CEdit           m_editRefCommon;
    CEdit           m_editRefReal[kNumChannels];
    CComboBox       m_comboComPorts;
    CString         m_selectedPort;
    CEdit           m_editSlaveId;

    CMFCButton      m_btnStartStop;
    CMFCButton      m_btnApplyInterval;
    CMFCButton      m_btnReset;
    CMFCButton      m_btnExportCSV;
    CMFCButton      m_btnSaveImage;
    CMFCButton      m_btnLog;
    CMFCButton      m_btnHelp;
    CMFCButton      m_btnDummyTest;
    CMFCButton      m_btnCalibrate;
    CMFCButton      m_btnSavePoint1;
    CMFCButton      m_btnSavePoint2;

    CButton         m_chkShowPeaks;
    CButton         m_chkAutoScaleY;
    CButton         m_chkCh0;
    CButton         m_chkCh1;
    CButton         m_chkCh2;
    CButton         m_chkCh3;
    CButton         m_chkDarkMode;

    CStatic         m_staticStatus;

    std::vector<CPoint> m_channels[kNumChannels];
    std::vector<int>    m_peaksIdx;
    std::vector<int>    m_peaksCh;
    double              m_lastVoltage[kNumChannels] = {};
    WarningLevel        m_lastLevel[kNumChannels] = { NORMAL, NORMAL, NORMAL, NORMAL };
    bool                m_enableCh0 = true;
    bool                m_enableCh1 = true;
    bool                m_enableCh2 = true;
    bool                m_enableCh3 = true;

    double              m_threshMax = 8.0;
    double              m_threshMin = 2.0;
    double              m_hysteresis[kNumChannels] = { 0.2, 0.2, 0.2, 0.2 };
    double              m_slope[kNumChannels] = { 1.0, 1.0, 1.0, 1.0 };
    double              m_offset[kNumChannels] = { 0.0, 0.0, 0.0, 0.0 };
    double              m_lastRawRef1[kNumChannels], m_lastRawRef2[kNumChannels];
    double              m_lastRealRef1[kNumChannels], m_lastRealRef2[kNumChannels];
    bool                m_hasFirstRef[kNumChannels] = { false, false, false, false };

    bool                m_isRunning = false;
    bool                m_useSimulation = false;
    UINT_PTR            m_timerId = 1;
    size_t              m_sampleCount = 0;
    int                 m_samplingIntervalMs = 500;
    size_t              m_historySize = 1000;
    int                 m_windowSize = 100, m_sliderPos = (int)(m_historySize - m_windowSize);

    bool                m_isLogging = false;
    CStdioFile          m_logFile;
    CString             m_logFilePath;
    ULONGLONG           m_startLogTick = 0;

    bool                m_darkMode = false;
    bool                m_autoScaleY = false;
    bool                m_showPeaks = false;
    double              m_zoomFactorX = 1.0, m_zoomFactorY = 1.0;
    double              m_panOffsetX_px = 0.0;
    bool                m_isPanning = false;
    CPoint              m_lastMousePos;

    SerialPort          m_serial;
    ModbusRTU* m_modbus = nullptr;

    HACCEL              m_hAccel = nullptr;

    void ResetGraph();
    void ComputePeaks();
    void DrawGraph(const CString& savePath = _T(""));
    void UpdateDarkModeUI();

    afx_msg void OnSysCommand(UINT, LPARAM);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC*, CWnd*, UINT);
    afx_msg BOOL OnMouseWheel(UINT, short, CPoint);
    afx_msg void OnLButtonDown(UINT, CPoint);
    afx_msg void OnLButtonUp(UINT, CPoint);
    afx_msg void OnMouseMove(UINT, CPoint);
    afx_msg void OnHScroll(UINT, UINT, CScrollBar*);
    afx_msg void OnTimer(UINT_PTR);
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   
    afx_msg void OnBnClickedButtonStartstop();
    afx_msg void OnBnClickedButtonHelp();
    afx_msg void OnBnClickedButtonApplyInterval();
    afx_msg void OnBnClickedButtonReset();
    afx_msg void OnBnClickedExportCsv();
    afx_msg void OnBnClickedSaveImage();
    afx_msg void OnBnClickedButtonLog();
    afx_msg void OnBnClickedButtonDummyTest();
    afx_msg void OnBnClickedCheckCh1();
    afx_msg void OnBnClickedCheckCh2();
    afx_msg void OnBnClickedCheckCh3();
    afx_msg void OnBnClickedCheckCh4();
    afx_msg void OnBnClickedCheckDarkMode();
    afx_msg void OnBnClickedCheckAutoscaleY();
    afx_msg void OnBnClickedCheckShowPeaks();
    afx_msg void OnEnChangeEditInterval();
    afx_msg void OnEnChangeEditThreshMax();
    afx_msg void OnEnChangeEditThreshMin();
    afx_msg void OnCbnSelchangeCombo1();

public:
    afx_msg void OnBnClickedCalibrate();
    afx_msg void OnBnClickedSavePoint1();
    afx_msg void OnBnClickedSavePoint2();
    void AppendToAlarms(const CString& msg, COLORREF color = RGB(0, 0, 0));
    double ReadAverageRaw(int ch, int numSamples = 10, int delayMs = 50);
};
