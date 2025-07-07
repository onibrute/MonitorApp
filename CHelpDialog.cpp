#include "pch.h"
#include "MonitorApp.h"
#include "CHelpDialog.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CHelpDialog, CDialogEx)

CHelpDialog::CHelpDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HELP_DIALOG, pParent)
{
}

CHelpDialog::~CHelpDialog()
{
}

void CHelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICH_HELP_OVERVIEW, m_richHelp);
}

BEGIN_MESSAGE_MAP(CHelpDialog, CDialogEx)
	ON_NOTIFY(EN_LINK, IDC_RICH_HELP_OVERVIEW, &CHelpDialog::OnRichHelpLinkClick)
END_MESSAGE_MAP()

BOOL CHelpDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_richHelp.SetEventMask(m_richHelp.GetEventMask() | ENM_LINK);
	m_richHelp.SetOptions(ECOOP_OR, ECO_READONLY | ECO_AUTOVSCROLL);
	SetHelpText();

	return TRUE;
}

void CHelpDialog::SetHelpText()
{
	CString text =
		_T("• Start/Stop – pornește/oprește achiziția de date. DetaliiStart\n")
		_T("• Apply Interval – modifică frecvența de eșantionare. DetaliiInterval\n")
		_T("• Reset – șterge datele acumulate. DetaliiReset\n")
		_T("• Export CSV – salvează măsurătorile în fișier. DetaliiCSV\n")
		_T("• Save Image – exportă graficul curent ca imagine. DetaliiImage\n")
		_T("• Log – activează salvarea continuă a datelor. DetaliiLog\n")
		_T("• Canale – selectează canalele vizibile. DetaliiChk\n")
		_T("• AutoScale – scalează automat axa Y. DetaliiAuto\n")
		_T("• Threshold & Peak – setează praguri și detectează vârfuri. DetaliiPeak\n")
		_T("• Calibrare – proces de corectare a măsurătorilor. DetaliiCalibrare\n");

	m_richHelp.SetWindowText(text);

	CString tags[] = {
		_T("DetaliiStart"), _T("DetaliiInterval"), _T("DetaliiReset"),
		_T("DetaliiCSV"), _T("DetaliiImage"), _T("DetaliiLog"),
		_T("DetaliiChk"), _T("DetaliiAuto"), _T("DetaliiPeak"), _T("DetaliiCalibrare")
	};

	for (const auto& tag : tags)
	{
		FINDTEXTEX ft = {};
		ft.chrg.cpMin = 0;
		ft.chrg.cpMax = -1;
		ft.lpstrText = (LPTSTR)(LPCTSTR)tag;

		if (m_richHelp.FindText(FR_DOWN, &ft) >= 0)
		{
			m_richHelp.SetSel(ft.chrgText);

			CHARFORMAT2 cf = {};
			cf.cbSize = sizeof(cf);
			cf.dwMask = CFM_COLOR | CFM_UNDERLINE | CFM_LINK;
			cf.dwEffects = CFE_UNDERLINE | CFE_LINK;
			cf.crTextColor = RGB(0, 0, 255);

			m_richHelp.SetSelectionCharFormat(cf);
		}
	}

	m_richHelp.SetSel(-1, -1);
}

CHelpDialog::HelpTopic CHelpDialog::GetHelpTopicFromText(const CString& txt)
{
	static const std::map<CString, HelpTopic> topicMap = {
		{ _T("DetaliiStart"), HT_Start },
		{ _T("DetaliiInterval"), HT_Interval },
		{ _T("DetaliiReset"), HT_Reset },
		{ _T("DetaliiCSV"), HT_CSV },
		{ _T("DetaliiImage"), HT_Image },
		{ _T("DetaliiLog"), HT_Log },
		{ _T("DetaliiChk"), HT_Checkbox },
		{ _T("DetaliiAuto"), HT_Auto },
		{ _T("DetaliiPeak"), HT_Peak },
		{ _T("DetaliiCalibrare"), HT_Calibrare }
	};

	auto it = topicMap.find(txt);
	return (it != topicMap.end()) ? it->second : HT_None;
}

void CHelpDialog::OnRichHelpLinkClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	ENLINK* pEnLink = reinterpret_cast<ENLINK*>(pNMHDR);
	if (pEnLink->msg == WM_LBUTTONDOWN)
	{
		CString clickedText;
		m_richHelp.GetTextRange(pEnLink->chrg.cpMin, pEnLink->chrg.cpMax, clickedText);

		switch (GetHelpTopicFromText(clickedText))
		{
		case HT_Start:
			::MessageBoxW(m_hWnd,
				L"Start/Stop\n\n"
				L"Această funcție pornește sau oprește achiziția de date în timp real.\n\n"
				L"• Când apeși Start, aplicația activează un timer care citește periodic datele din portul serial și actualizează graficul.\n"
				L"• Când apeși Stop, achiziția se întrerupe și poți analiza datele afișate sau salva fișiere.",
				L"Start/Stop", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Interval:
			::MessageBoxW(m_hWnd,
				L"Apply Interval\n\n"
				L"Modifică perioada de eșantionare (în milisecunde) între două citiri succesive de la dispozitiv.\n\n"
				L"• Poți reduce această valoare pentru a crește frecvența de actualizare (ex: 100ms).\n"
				L"• Sau o poți crește pentru o achiziție lentă și stabilă (ex: 1000ms).\n\n"
				L"Intervalul se aplică imediat ce apeși butonul Apply Interval.",
				L"Apply Interval", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Reset:
			::MessageBoxW(m_hWnd,
				L"Reset\n\n"
				L"Șterge toate datele afișate în grafic și resetează contorul de eșantioane.\n\n"
				L"• Este recomandat să îl folosești la începutul unei sesiuni noi de măsurare.\n"
				L"• Nu afectează setările selectate (canale, praguri, scale etc.)",
				L"Reset", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_CSV:
			::MessageBoxW(m_hWnd,
				L"Export CSV\n\n"
				L"Exportă toate valorile măsurate într-un fișier .CSV compatibil cu Excel.\n\n"
				L"• Coloanele includ timestamp și valorile canalelor active.\n"
				L"• Poți folosi fișierul pentru analiză, graficare externă sau arhivare.\n\n"
				L"Fișierul este salvat în locația selectată de utilizator.",
				L"Export CSV", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Image:
			::MessageBoxW(m_hWnd,
				L"Save Image\n\n"
				L"Salvează imaginea graficului actual în format PNG sau BMP.\n\n"
				L"• Imaginea reflectă exact datele vizibile și starea curentă a scalei și axelor.\n"
				L"• Poți folosi această funcție pentru rapoarte tehnice sau prezentări.",
				L"Save Image", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Log:
			::MessageBoxW(m_hWnd,
				L"Log\n\n"
				L"Activează scrierea automată și continuă a datelor în fișier text.\n\n"
				L"• Fiecare eșantion este salvat imediat.\n"
				L"• Ideal pentru monitorizare de durată lungă și păstrare a unui istoric.",
				L"Log", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Checkbox:
			::MessageBoxW(m_hWnd,
				L"Canale (CH0 – CH3)\n\n"
				L"Activează sau dezactivează afișarea canalelor individuale.\n\n"
				L"• Când un canal este debifat, datele lui nu sunt afișate pe grafic și nu sunt procesate.",
				L"Canale", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Auto:
			::MessageBoxW(m_hWnd,
				L"AutoScale Y\n\n"
				L"Activează ajustarea automată a scalei verticale (Y) pe grafic.\n\n"
				L"• Dacă este bifată, aplicația va modifica dinamic scala pentru a include toate valorile recente.\n"
				L"• Dacă este debifată, scala rămâne fixă la valorile existente în momentul opririi AutoScale.",
				L"AutoScale", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Peak:
			::MessageBoxW(m_hWnd,
				L"Threshold & Peak\n\n"
				L"Permite configurarea unor limite minime și maxime pentru fiecare canal.\n\n"
				L"• Dacă o valoare depășește pragul setat, este marcată ca vârf ('Peak') și apare o alertă.\n"
				L"• Este util pentru a semnala supratensiuni sau depășiri în aplicațiile industriale.",
				L"Threshold & Peak", MB_OK | MB_ICONINFORMATION);
			break;

		case HT_Calibrare:
			::MessageBoxW(m_hWnd,
				L"Calibrare – Corectarea valorilor măsurate\n\n"
				L"Calibrarea se face în 3 etape:\n\n"
				L"1. Punct Comun\n"
				L"• În DCON setezi o valoare de referință identică pe toate canalele (ex: 5V).\n"
				L"• Introdu valoarea reală în câmpul 'Punct Comun' și apasă 'Salvează punct comun'.\n"
				L"• Aplicația va citi 10 valori brute pentru fiecare canal și le va media.\n\n"
				L"2. Puncte Individuale\n"
				L"• Schimbi valoarea în DCON (ex: 9.5V) și o introduci în câmpurile CH0–CH3.\n"
				L"• Apeși 'Salvează puncte' pentru a memora aceste valori.\n\n"
				L"3. Calibrare Finală\n"
				L"• Apeși 'Calibrează' → aplicația calculează coeficienți slope și offset per canal:\n"
				L"  valoare_calibrată = slope × valoare_rawă + offset\n\n"
				L"După acest pas, aplicația va afișa și salva doar valorile reale.",
				L"Calibrare", MB_OK | MB_ICONINFORMATION);
			break;

		default:
			break;
		}
	}
	*pResult = 0;
}


