//	/*
// 	*
// 	* Copyright (C) 2003-2010 Alexandros Economou
//	*
//	* This file is part of Jaangle (http://www.jaangle.com)
// 	*
// 	* This Program is free software; you can redistribute it and/or modify
// 	* it under the terms of the GNU General Public License as published by
// 	* the Free Software Foundation; either version 2, or (at your option)
// 	* any later version.
// 	*
// 	* This Program is distributed in the hope that it will be useful,
// 	* but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// 	* GNU General Public License for more details.
// 	*
// 	* You should have received a copy of the GNU General Public License
// 	* along with GNU Make; see the file COPYING. If not, write to
// 	* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
// 	* http://www.gnu.org/copyleft/gpl.html
// 	*
//	*/ 

#include "stdafx.h"
#include "TeenSpirit.h"
#include "QuizRunningDlg.h"
#include "AppSettings/AppSettings.h"
#include "PrgAPI.h"
#include "TeenSpiritPlayer.h"
#include "SQLManager.h"
#include "ActionManager.h"
#include "TSOnlineServices.h"
#include "SystemTimeFunctions.h"

LPCTSTR CQuizRunningDlg::SettingsCategory = _T("QUIZ");
#define ID_GAMESELECTION 0x600


// CQuizRunningDlg dialog
#define TIMER_CONTINUOUS 100
#define TIMER_DISPLAYRESULTSTIMER 101
#define TIMER_DISPLAYRESULTSTIMER_DELAY 3000
#define TIMER_DISPLAYQUESTIONSTIMER 102
#define TIMER_DISPLAYQUESTIONSTIMER_DELAY 3000

#define START_BUTTON_ID 0x200
//UINT dlgItem[] = {IDC_OPTION1, IDC_OPTION2, IDC_OPTION3, IDC_OPTION4};

IMPLEMENT_DYNAMIC(CQuizRunningDlg, CDialog)

CQuizRunningDlg::CQuizRunningDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQuizRunningDlg::IDD, pParent),
	m_pPlayer(NULL),
m_curTime(0),
m_correctAnswer(0),
m_correctAnswers(0),
m_totalAnswers(0),
m_curGameBestScore(0),
m_LastTickCount(0),
m_poolCount(0),
m_gameTimeStamp(0)
{

}

CQuizRunningDlg::~CQuizRunningDlg()
{
	for (int i = 0; i < BT_Last - BT_Start; i++)
		delete m_pButtons[i];
	if (m_hWnd) DestroyWindow();
	delete m_pPlayer;

}

void CQuizRunningDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_TITLE, m_title);
	DDX_Control(pDX, IDC_SUBTITLE, m_subTitle);
	DDX_Control(pDX, IDC_INFO, m_info);
	DDX_Control(pDX, IDC_RESULT, m_result);
}


BEGIN_MESSAGE_MAP(CQuizRunningDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CQuizRunningDlg message handlers

void CQuizRunningDlg::ConfugureTextControl(CLabelEx& bs, 
										   COLORREF bk, COLORREF fg, INT resText)
{
	bs.SetColor(CLabelEx::COL_Bk, bk);
	bs.SetColor(CLabelEx::COL_Text, fg);
	if (resText != 0)
		bs.SetWindowText(PRGAPI()->GetString(resText));
	else
		bs.SetWindowText(_T(""));
}

const CQuizRunningDlg::GameParameters games[] =
{
	{_T("Short"), 30000, 3000, 0},
	{_T("Normal"), 60000, 3000, 0},
	{_T("Long"), 300000, 3000, 0},
	{_T("Marathon"), 1800000, 10000, 0},
	{_T("Bonus (short)"), 30000, 3000, 1000},
	{_T("Bonus"), 60000, 3000, 1000},
	{_T("Keep it alive"), 5000, 2000, 2000},
	{_T("Just Answer"), 30000, 0, 0},
	{_T("Better be sure"), 30000, 30000, 0}
};



#include "TeenSpiritPlayer.h"
#include "BassEngine.h"
#include "DShowEngine.h"
#include "MultiEngine.h"
#include "VectorPlaylist.h"
#include "NativeVisualization.h"


BOOL CQuizRunningDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pPlayer = new TeenSpiritPlayer;
	MultiEngine* eng = new MultiEngine;
	eng->AddEngine(new BassEngine);

	//eng->AddEngine(new DShowEngine);
	eng->SetVideoContainerHWND(GetDlgItem(IDC_MEDIAPLAYER)->m_hWnd);
	m_pPlayer->SetMediaPlayerEngine(eng);
	m_pPlayer->SetMediaVisualization(new NativeVisualization);
	m_pPlayer->SetMediaPlayList(new VectorPlayList);
	m_pPlayer->SetVisContainerHWND(GetDlgItem(IDC_MEDIAPLAYER)->m_hWnd);

	//m_pPlayer->Init(eng, new VectorPlayList, new NativeVisualization, GetDlgItem(IDC_MEDIAPLAYER)->m_hWnd);
	m_pPlayer->StartVis();

	//m_btn[0].SubclassWindow(::GetDlgItem(m_hWnd, IDC_OPTION1));
	//m_btn[1].SubclassWindow(::GetDlgItem(m_hWnd, IDC_OPTION2));
	//m_btn[2].SubclassWindow(::GetDlgItem(m_hWnd, IDC_OPTION3));
	//m_btn[3].SubclassWindow(::GetDlgItem(m_hWnd, IDC_OPTION4));
	//m_btn[4].SubclassWindow(::GetDlgItem(m_hWnd, IDC_OPTION5));



	PrgAPI* pAPI = PRGAPI();
	for (int i = 0; i < BT_Last - BT_Start; i++)
	{
		m_pButtons[i] = new CMillionaireButton;
		m_pButtons[i]->Create(_T(""), WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), this, START_BUTTON_ID + i);
		m_pButtons[i]->SetPolygonStyle(CMillionaireButton::POL_Diamond);
		m_pButtons[i]->SetFont(&m_largeFont);
		m_pButtons[i]->SetMargin(30);
		m_pButtons[i]->SetTextAlign(DT_CENTER);
	}
	m_pButtons[BT_Start]->SetWindowText(pAPI->GetString(IDS_START));
	m_pButtons[BT_SelectPrev]->SetWindowText(pAPI->GetString(IDS_PREVIOUS));
	m_pButtons[BT_SelectNext]->SetWindowText(pAPI->GetString(IDS_NEXT));
	TCHAR bf[100];
	_sntprintf(bf, 100, _T("5. %s"), pAPI->GetString(IDS_IDONTKNOW));
	m_pButtons[BT_AnswerNo]->SetWindowText(bf);
	m_pButtons[BT_EndGame]->SetWindowText(pAPI->GetString(IDS_END));
	m_pButtons[BT_NewGame]->SetWindowText(pAPI->GetString(IDS_NEW));
	m_pButtons[BT_Submit]->SetWindowText(pAPI->GetString(IDS_SUBMITRESULTS));
	m_pButtons[BT_Exit]->SetWindowText(pAPI->GetString(IDS_EXIT));

	//m_btn[i].SetState(CMillionaireButton::ST_Disabled);
	//m_btn[i].SetTextAlign(DT_LEFT);
	//m_btn[i].EnableWindow(FALSE);
	//m_btn[i].Invalidate();

	ConfugureTextControl(m_title, RGB(50,50,50), RGB(200,200,200), 0);
	ConfugureTextControl(m_subTitle, RGB(30,30,30), RGB(200,200,200), 0);
	m_info.SetBkColor(RGB(0,0,0));
	m_info.SetTextColor(RGB(200,200,200));
	m_info.SetMargins(10, 10);

	m_result.SetBkColor(RGB(0,0,0));
	m_result.SetTextColor(RGB(220,100,100));
	m_result.SetMargins(10, 10);


	m_title.SetMargins(10, 0);
	m_subTitle.SetMargins(10, 0);


	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);
	SQLManager* pSM = pAPI->GetSQLManager();
	TracksFilter tf;
	tf.CollectionType.match = NUMM_Exact;
	tf.CollectionType.val = CTYPE_LocalFolder;
	m_poolCount = pSM->GetTrackCount(tf);
	tf.CollectionType.val = CTYPE_NetFolder;
	m_poolCount += pSM->GetTrackCount(tf);
	pAPI->GetAppSettings()->Read(SettingsCategory, _T("LastGame"), m_curGameIdx, 1);
	m_progress.SetUnit(CCountDownCtrl::UN_ms);
	ShowWindow(SW_MAXIMIZE);
	SetState(ST_Start);
	return TRUE;
}

void CQuizRunningDlg::SetCurrentGame(INT gameIdx)
{
	if (gameIdx < 0)
		gameIdx = sizeof(games)/sizeof(GameParameters) - 1;
	if (gameIdx >= sizeof(games)/sizeof(GameParameters))
		gameIdx = 0;
	m_curGameIdx = gameIdx;
	m_curGame = games[gameIdx];
	TCHAR bf[1000];
	//=== Read registry values
	_sntprintf(bf, 500, _T("%s.best"), m_curGame.gameName);
	PrgAPI* pAPI = PRGAPI();
	pAPI->GetAppSettings()->Read(SettingsCategory, bf, m_curGameBestScore, 0);
	_sntprintf(bf, 500, _T("%s.bestdate"), m_curGame.gameName);
	pAPI->GetAppSettings()->Read(SettingsCategory, bf, m_curGameBestScoreDate, _T(""));
	m_progress.SetRange32(0, m_curGame.totalTime);
	m_progress.SetPos(0);

	//=== Title
	_sntprintf(bf, 500, _T("%s - '%s'"), pAPI->GetString(IDS_MUSICQUIZ), m_curGame.gameName);
	m_title.SetWindowText(bf);

	//=== SubTitle
	//===		Always the same

	//=== Info
	_sntprintf(bf, 1000, 
		_T("<b>%s</b><br>")
		_T("<b>%s:</b> '%s'<br>")
		_T("<b>%s:</b> %0.1fs<br>")
		_T("<b>%s:</b> %0.1fs<br>")
		_T("<b>%s:</b> %0.1fs<br>")
		_T("<b>%s:</b> %d ( %s )<br>")
		_T("<b>%s:</b> %d (%d %s)"),
		pAPI->GetString(IDS_GAMEINFO),
		pAPI->GetString(IDS_NAME), m_curGame.gameName,
		pAPI->GetString(IDS_TOTALTIME), m_curGame.totalTime/1000.0,
		pAPI->GetString(IDS_PENALTY), m_curGame.penaltyTime/1000.0,
		pAPI->GetString(IDS_BONUS), m_curGame.bonusTime/1000.0,
		pAPI->GetString(IDS_BEST), m_curGameBestScore, m_curGameBestScoreDate.c_str(),
		pAPI->GetString(IDS_LEVEL), m_poolCount > 0 ? int(log10((double(m_poolCount))) + 1) : 0, 
		m_poolCount, pAPI->GetString(IDS_TRACKS));

	m_info.SetWindowText(bf);
}


void CQuizRunningDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_CONTINUOUS:
		{ 
			INT timePassed = m_curTime + GetTickCount() - m_LastTickCount;
			if (timePassed > m_curGame.totalTime)
			{
				EvaluateAnswer(4);
				//SetState(ST_GameOver);
				return;
			}
			m_progress.SetPos(timePassed);
		}
		break;
	case TIMER_DISPLAYRESULTSTIMER:
		{
			KillTimer(nIDEvent);
			SetState(m_curTime >= m_curGame.totalTime ? ST_GameOver : ST_DisplayingQuestionsAndWait);
		}
		break;
	case TIMER_DISPLAYQUESTIONSTIMER:
		{
			KillTimer(nIDEvent);
			SetState(ST_RunningQuestions);
		}
		break;

	}

	CDialog::OnTimer(nIDEvent);
}

void CQuizRunningDlg::HideAll()
{
	for (INT i = BT_Start; i < BT_Last; i++)
		m_pButtons[i]->ShowWindow(SW_HIDE);
	m_progress.ShowWindow(SW_HIDE);

}



BOOL CQuizRunningDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CQuizRunningDlg::OnCommand"), wParam);
	if (wParam >= START_BUTTON_ID && wParam < START_BUTTON_ID + BT_Last)
	{
		INT btnIdx = wParam - START_BUTTON_ID;
		switch (btnIdx)
		{
		case BT_Start:
			SetState(ST_DisplayingQuestionsAndWait);
			break;
		case BT_SelectPrev:
			SetCurrentGame(--m_curGameIdx);
			break;
		case BT_SelectNext:
			SetCurrentGame(++m_curGameIdx);
			break;
		case BT_Answer1:
		case BT_Answer2:
		case BT_Answer3:
		case BT_Answer4:
		case BT_AnswerNo:
			EvaluateAnswer(btnIdx - BT_Answer1);
			break;
		case BT_NewGame:
			SetState(ST_Start);
			break;
		case BT_Submit:
			{
				CHAR gameName[200];
				_snprintf(gameName, 200, "QUIZ|%d:%d:%d", m_curGame.totalTime, m_curGame.penaltyTime, m_curGame.bonusTime);
				PrgAPI* pAPI = PRGAPI();
				TSOnlineServices* pTS = pAPI->GetTSOnlineServices();
				pTS->SendGameResults(gameName, m_correctAnswers, m_poolCount, m_gameTimeStamp, TRUE);
			}
			break;
		case BT_Exit:
			EndDialog(0);
			break;
		case BT_EndGame:
			SetState(ST_GameOver);
			break;
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CQuizRunningDlg::EvaluateAnswer(INT qa)
{
	m_lastResponseTime = GetTickCount() - m_LastTickCount;
	KillTimer(TIMER_CONTINUOUS);
	KillTimer(TIMER_DISPLAYRESULTSTIMER);	
	m_pPlayer->SetVolume(40);
	m_totalAnswers++;

	m_pButtons[m_correctAnswer + BT_Answer1]->SetState(CMillionaireButton::ST_Correct);
	if (m_correctAnswer == qa)
	{
		m_curTime -= m_curGame.bonusTime;
		m_correctAnswers++;
		TCHAR bf[500];
		_sntprintf(bf, 500, _T("%s [%d ms]"), PRGAPI()->GetString(IDS_CORRECT), m_lastResponseTime);
		m_subTitle.SetWindowText(bf);
		m_subTitle.SetColor(CLabelEx::COL_Text, RGB(50,250,50));
	}
	else
	{
		m_curTime += m_curGame.penaltyTime;
		m_pButtons[qa + BT_Answer1]->SetState(CMillionaireButton::ST_Wrong);
		m_subTitle.SetWindowText(PRGAPI()->GetString(IDS_MISTAKE));
		m_subTitle.SetColor(CLabelEx::COL_Text, RGB(250,50,50));
	}
	m_curTime += m_lastResponseTime;
	m_progress.SetPos(m_curTime);
	SetState(ST_DisplayingResultsAndWait);
}

BOOL CQuizRunningDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, RGB(0,0,0));
	//pDC->Draw3dRect(&rc, RGB(200,200,200), RGB(100,100,100));
	return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

void CQuizRunningDlg::OnDestroy()
{
	PRGAPI()->GetAppSettings()->Write(SettingsCategory, _T("LastGame"), m_curGameIdx);
	CDialog::OnDestroy();
}


BOOL CQuizRunningDlg::DisplayNextQuestion()
{
	TRACEST(_T("CQuizRunningDlg::DisplayNextQuestion"));
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	TracksFilter tf;
	tf.Duration.match = NUMM_Over;
	tf.Duration.val = 30;
	FullTrackRecordCollection col;
	if (!pAM->GetRandomTrackCollection(col, tf, 4))
		return FALSE;
	UINT trackLength = 0;
	TCHAR trackPath[MAX_PATH];
	m_correctAnswer = INT((rand() * 4) / RAND_MAX);
	FullTrackRecordSP rec;
	for (int i = 0; i < 4; i++)
	{
		TCHAR txt[1000];
		_sntprintf(txt, 1000, _T("%d. %s - %s"), i + 1, col[i]->artist.name.c_str(),col[i]->track.name.c_str());
		m_pButtons[i + BT_Answer1]->SetWindowText(txt);
		if (m_correctAnswer == i)
		{
			_tcsncpy(trackPath, col[i]->track.location.c_str(), MAX_PATH);
			trackLength = col[i]->track.duration;
		}
		m_pButtons[i + BT_Answer1]->ShowWindow(TRUE);
	}

	m_subTitle.SetWindowText(PRGAPI()->GetString(IDS_RECOGNISESONG));
	if (!m_pPlayer->Play(trackPath))
	{
		TRACE(_T("@1CQuizRunningDlg::DisplayNextQuestion. Cannot play the file '%s'\r\n"), trackPath);
		return FALSE;
	}
	m_pPlayer->Pause();
	trackLength = INT(m_pPlayer->GetMediaLength());
	if (trackLength < 20)
	{
		TRACE(_T("@1CQuizRunningDlg::DisplayNextQuestion. trackLength is %d '%s'\r\n"), trackLength, trackPath);
		return FALSE;
	}
	m_subTitle.SetColor(CLabelEx::COL_Text, RGB(200,200,200));
	UINT startSec = (trackLength - 40) * rand() / RAND_MAX + 20;//Select 20sec in the track after the first 20 sec
	m_pPlayer->SetVolume(95);
	m_pPlayer->SetMediaPos((DOUBLE) startSec);


	return TRUE;
}

void CQuizRunningDlg::DisplayRunningResult()
{
	PrgAPI* pAPI = PRGAPI();
	switch (m_curState)
	{
	case ST_Start:
		m_result.SetWindowText(_T(""));
		m_result.ShowWindow(SW_HIDE);
		break;
	case ST_GameOver:
		{
			m_result.ShowWindow(SW_SHOW);
			m_result.SetTextColor(RGB(100,255,100));
			TCHAR bf[1000];
			_sntprintf(bf, 1000,
				_T("<b>%s</b><br>")
				_T("<b>%s:</b> %d<br>")
				_T("<b>%s:</b> %d<br>"),
				pAPI->GetString(IDS_RESULTS),
				pAPI->GetString(IDS_CORRECT), m_correctAnswers,
				pAPI->GetString(IDS_MISTAKE), m_totalAnswers - m_correctAnswers);
			m_result.SetWindowText(bf);
		}
		break;
	default:
		{
			m_result.ShowWindow(SW_SHOW);
			m_result.SetTextColor(RGB(255,100,100));
			TCHAR projection[20];
			if (m_curTime > 0)
				_sntprintf(projection, 20, _T(" [%0.1f]"), 
				(m_correctAnswers * m_curGame.totalTime) / double(m_curTime));
			else
				projection[0] = 0;
			TCHAR bf[1000];
			_sntprintf(bf, 1000,
				_T("<b>%s</b><br>")
				_T("<b>%s:</b> %d%s<br>")
				_T("<b>%s:</b> %d<br>"),
				pAPI->GetString(IDS_RESULTS),
				pAPI->GetString(IDS_CORRECT), m_correctAnswers, projection,
				pAPI->GetString(IDS_MISTAKE), m_totalAnswers - m_correctAnswers);

			m_result.SetWindowText(bf);
		}
		break;
	}
}

//const INT stateStart[] =
//{
//	1, IDS_START,
//	1, IDS_SELECTGAME,
//	0, 0,
//	1, IDS_EXIT
//};
//const INT stateGameOver[] =
//{
//	1, IDS_NEW,
//	1, IDS_SUBMITRESULTS,
//	0, 0,
//	1, IDS_EXIT
//};

void CQuizRunningDlg::SetState(FormStates st)
{
	m_curState = st;
	PrgAPI* pAPI = PRGAPI();
	DisplayRunningResult();
	switch (st)
	{
	case ST_Start:
		SetCurrentGame(m_curGameIdx);
		m_pPlayer->Stop();
		m_curTime = 0;
		m_correctAnswers = 0;
		m_totalAnswers = 0;
		m_lastResponseTime = 0;
		if (m_poolCount < 4)
		{	
			m_subTitle.SetWindowText(pAPI->GetString(IDS_NOTENOUGHTRACKSTOPLAYTHEQUIZ));
		}
		else
		{
			HideAll();
			m_pButtons[BT_Start]->ShowWindow(SW_SHOW);
			m_pButtons[BT_SelectNext]->ShowWindow(SW_SHOW);
			m_pButtons[BT_SelectPrev]->ShowWindow(SW_SHOW);
			m_pButtons[BT_Exit]->ShowWindow(SW_SHOW);

			m_subTitle.SetWindowText(pAPI->GetString(IDS_RECOGNISESONG));
			//m_subTitle.SetColor(CLabelEx::COL_Text, RGB(200,200,200));

		}
		break;
	case ST_DisplayingQuestionsAndWait:
		{
			KillTimer(TIMER_CONTINUOUS);
			if (m_pButtons[BT_Start] ->IsWindowVisible())
				HideAll();
			INT numTimes = 0;
			while (!DisplayNextQuestion() && numTimes<4)
				numTimes++;
			for (INT i = BT_Answer1; i <= BT_AnswerNo; i++)
			{
				m_pButtons[i]->SetState(CMillionaireButton::ST_Disabled);
				m_pButtons[i]->SetTextAlign(DT_LEFT);
				m_pButtons[i]->EnableWindow(FALSE);
				m_pButtons[i]->ShowWindow(SW_SHOW);
				m_pButtons[i]->Invalidate();
			}
			m_progress.ShowWindow(SW_SHOW);
			m_pButtons[BT_EndGame]->ShowWindow(SW_SHOW);
			if (numTimes == 4)
				SetState(ST_GameOver);
			SetTimer(TIMER_DISPLAYQUESTIONSTIMER, TIMER_DISPLAYQUESTIONSTIMER_DELAY, NULL);
		}
		break;
	case ST_RunningQuestions:
		SetFocus();
		for (int i = BT_Answer1; i <= BT_AnswerNo; i++)
		{
			m_pButtons[i]->SetState(CMillionaireButton::ST_Normal);
			m_pButtons[i]->EnableWindow(TRUE);
			m_pButtons[i]->Invalidate();
		}
		m_pPlayer->Start();
		m_progress.ShowWindow(SW_SHOW);
		m_LastTickCount = GetTickCount();
		SetTimer(TIMER_CONTINUOUS, 25, NULL);
		break;
	case ST_DisplayingResultsAndWait:
		KillTimer(TIMER_CONTINUOUS);
		for (int i = BT_Answer1; i <= BT_AnswerNo; i++)
		{
			//m_pButtons[i]->SetState(CMillionaireButton::ST_Normal);
			m_pButtons[i]->EnableWindow(FALSE);
			//m_pButtons[i]->Invalidate();
		}
		SetTimer(TIMER_DISPLAYRESULTSTIMER, TIMER_DISPLAYRESULTSTIMER_DELAY, NULL);
		break;
	case ST_GameOver:
		m_pPlayer->SetVolume(40);
		KillTimer(TIMER_CONTINUOUS);
		KillTimer(TIMER_DISPLAYRESULTSTIMER);
		KillTimer(TIMER_DISPLAYQUESTIONSTIMER);
		HideAll();
		m_pButtons[BT_Submit]->ShowWindow(SW_SHOW);
		m_pButtons[BT_NewGame]->ShowWindow(SW_SHOW);
		m_pButtons[BT_Exit]->ShowWindow(SW_SHOW);
		SYSTEMTIME st;
		GetSystemTime(&st);
		m_gameTimeStamp = SystemTime2UnixTimeStamp(st);

		if (m_curGameBestScore < m_correctAnswers)
		{
			m_curGameBestScore = m_correctAnswers;
			AppSettings* pSettings = pAPI->GetAppSettings();
			TCHAR bf[500];
			_sntprintf(bf, 500, _T("%s.best"), m_curGame.gameName);
			pSettings->Write(SettingsCategory, bf, m_curGameBestScore);
			_sntprintf(bf, 500, _T("%s.bestdate"), m_curGame.gameName);
			TCHAR dateBuffer[200];
			::GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, 0, 0, dateBuffer, 200);
			pSettings->Write(SettingsCategory, bf, dateBuffer);
			m_curGameBestScoreDate = dateBuffer;
		}

		m_subTitle.SetWindowText(pAPI->GetString(IDS_GAMEOVER));
		m_subTitle.SetColor(CLabelEx::COL_Text, RGB(200,200,200));
		break;
	default:
		ASSERT(0);
		break;
	}

}



BOOL CQuizRunningDlg::PreTranslateMessage(MSG* pMsg)
{
	if (m_curState == ST_RunningQuestions && pMsg->message == WM_KEYDOWN)
	{
		TRACE(_T("CQuizRunningDlg::PreTranslateMessage %d\r\n"), pMsg->wParam);
		switch (pMsg->wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			EvaluateAnswer(pMsg->wParam - '1');
			SetFocus();
			return TRUE;//TranslateMessage(pMsg);
		break;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CQuizRunningDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	if (m_subTitle.GetSafeHwnd() == NULL)
		return;

	INT cMargin = cy / 100;
	INT cMainWidth = cx * 2 / 3;
	INT cMainHeight = (cy - cMargin * 5) / 10;

	if (m_largeFont.m_hObject)
	{
		m_largeFont.DeleteObject();
		m_normalFont.DeleteObject();
		m_smallFont.DeleteObject();
	}

	LOGFONT lf;
	lf.lfHeight = cMainHeight * 3 / 5;
	lf.lfWidth = 0;
	lf.lfEscapement = 0; 
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = CLEARTYPE_QUALITY;//ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_DONTCARE;
	_tcscpy(lf.lfFaceName, _T("Trebuchet MS"));
	m_largeFont.CreateFontIndirect(&lf);

	lf.lfHeight = cMainHeight * 1 / 3;
	m_normalFont.CreateFontIndirect(&lf);

	lf.lfHeight = cMainHeight * 1 / 4;
	m_smallFont.CreateFontIndirect(&lf);

	for (INT i = BT_Start; i < BT_Last; i++)
		m_pButtons[i]->SetFont(&m_normalFont);
	m_title.SetFont(&m_largeFont);
	m_subTitle.SetFont(&m_normalFont);
	m_info.SetFont(&m_smallFont);
	m_result.SetFont(&m_smallFont);
	m_progress.SetFont(&m_largeFont);


	INT cury = 0; 
	m_title.MoveWindow(&CRect(0, cury, cx, cury + cMainHeight));
	cury += cMainHeight; 
	m_subTitle.MoveWindow(&CRect(0, cury, cx, cury + cMainHeight / 2));
	cury += cMainHeight / 2;
	cury += cMargin;
	m_info.MoveWindow(&CRect(0, cury, cx / 2, cury + 2 * cMainHeight));
	m_result.MoveWindow(&CRect(cx / 2, cury, cx, cury + 2 * cMainHeight));
	//CRect rcMediaPlayer(mpDividerPos + cMargin, cury, cx - cMargin, cury + 4 * cMainHeight);
	cury += 2 * cMainHeight + 2 * cMargin;

	INT buttonLeft = (cx - cMainWidth) / 2;
	cMainHeight /= 2;
	CRect rcButton(buttonLeft, cury, buttonLeft + cMainWidth, cury + cMainHeight);
	//=== MediaPlayer
	GetDlgItem(IDC_MEDIAPLAYER)->MoveWindow(&rcButton);
	m_pPlayer->SetVisPosition(0, 0, rcButton.Width(), rcButton.Height());

	//=== Progress
	rcButton.OffsetRect(0, cMainHeight + 2 * cMargin);
	m_progress.MoveWindow(rcButton);

	//=== Buttons: Line 1
	rcButton.OffsetRect(0, cMainHeight + 2 * cMargin);
	m_pButtons[BT_Start]->MoveWindow(rcButton);
	m_pButtons[BT_Submit]->MoveWindow(rcButton);
	m_pButtons[BT_Answer1]->MoveWindow(rcButton);

	//=== Buttons: Line 2
	rcButton.OffsetRect(0, cMainHeight + cMargin);
	m_pButtons[BT_SelectPrev]->MoveWindow(rcButton);
	m_pButtons[BT_Answer2]->MoveWindow(rcButton);
	m_pButtons[BT_NewGame]->MoveWindow(rcButton);

	//=== Buttons: Line 3
	rcButton.OffsetRect(0, cMainHeight + cMargin);
	m_pButtons[BT_SelectNext]->MoveWindow(rcButton);
	m_pButtons[BT_Answer3]->MoveWindow(rcButton);

	//=== Buttons: Line 4
	rcButton.OffsetRect(0, cMainHeight + cMargin);
	m_pButtons[BT_Answer4]->MoveWindow(rcButton);

	//=== Buttons: Line 5
	rcButton.OffsetRect(0, cMainHeight + cMargin);
	m_pButtons[BT_AnswerNo]->MoveWindow(rcButton);

	//=== Buttons: Line 6
	rcButton.OffsetRect(0, cMainHeight + 3 * cMargin);
	m_pButtons[BT_EndGame]->MoveWindow(rcButton);
	m_pButtons[BT_Exit]->MoveWindow(rcButton);

}
