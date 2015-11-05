// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "MFC_OSG_MDI.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    ON_WM_CREATE()
	ON_COMMAND(ID_Fullscreen, &CMainFrame::OnFullscreen)
END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,
    ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    m_bAutoMenuEnable = false;
	m_bFullScreen = false;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }
    if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
        CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
    {
        TRACE0("Failed to create dialogbar\n");
        return -1;        // fail to create
    }

    if (!m_wndReBar.Create(this) ||
        !m_wndReBar.AddBar(&m_wndToolBar) ||
        !m_wndReBar.AddBar(&m_wndDlgBar))
    {
        TRACE0("Failed to create rebar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
          sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // TODO: Remove this if you don't want tool tips
    m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
        CBRS_TOOLTIPS | CBRS_FLYBY);

    return 0;
}


CString CMainFrame::GetExtName(CString fileName)
{
	int pos=fileName.Find("."); //获取 . 的位置
	if(pos==-1)
	{ //如果没有找到，直接返回该字符串
		return fileName; 
	}
	else
	{
		return GetExtName(fileName.Mid(pos+1)); //找到了的话，往深层遍历，直到最底层
	}
}


void CMainFrame::CloseOtherDocuments(BOOL bEndSession)
{
	CWinApp *cApp = AfxGetApp();
	CMainFrame *p_mainFrame= (CMainFrame *)cApp->m_pMainWnd;
	POSITION pos = cApp->GetFirstDocTemplatePosition();
	while(pos!=NULL)  
	{  
		//3、对每个文档模板对象  
		CDocTemplate* pTemplate =cApp->GetNextDocTemplate(pos);
		ASSERT_KINDOF(CDocTemplate,pTemplate);  
		//4、调用文档模板的CloseAllDocuments(bEndSession)函数  
		{
			POSITION pos2=pTemplate->GetFirstDocPosition();  
			while(pos2!=NULL)  
			{  
				//6、对每个文档对象  
				CDocument* pDoc=pTemplate->GetNextDoc(pos2); 
				//CString DocumentName = pDoc->m_strPathName;
				CString DocumentName2 = pDoc->GetPathName();
				CString ext = GetExtName(DocumentName2);

				if(!DocumentName2.IsEmpty())
				{
					//如果能够找到对应的文档，那么Close
					if(bEndSession)  
						pDoc->DisconnectViews();  
					//7、释放文档对象  
					pDoc->OnCloseDocument();  	
				}
			}  
		}
	}  
}




BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnFullscreen()
{
	//调用功能，如果在全屏就不全屏，否则就全屏
	FullScreen(!m_bFullScreen) ;
	//设置标识
	m_bFullScreen = !m_bFullScreen ;

}

void CMainFrame::FullScreen(bool bFullScreen )
{
	//得到当前STYLE
	LONG style = ::GetWindowLong(this->m_hWnd,GWL_STYLE);
	//得到当前WND
	CWnd *pWnd=AfxGetMainWnd();
	if(bFullScreen)//全屏显示
	{
		pWnd->DrawMenuBar();
		//去掉边框
		style &= ~(WS_DLGFRAME | WS_THICKFRAME);
		SetWindowLong(this->m_hWnd,GWL_STYLE, style);
		//最大化视窗
		this->ShowWindow(SW_SHOWMAXIMIZED);
		CRect rect;
		//得到当前视窗口并稍做移动,保证彻底全屏,可能有的机器不同,会保留任务栏,大多数都会全屏
		this->GetWindowRect(&rect);
		::SetWindowPos(this->m_hWnd,HWND_TOPMOST,rect.left, rect.top,
			rect.right-rect.left + 3, rect.bottom-rect.top + 3, SWP_FRAMECHANGED);
	}
	else//窗口显示
	{
		//得到菜单
		CMenu menu;
		//取菜单
		menu.LoadMenu(IDR_MAINFRAME);
		//设置菜单
		pWnd->SetMenu(&menu);
		pWnd->DrawMenuBar();
		menu.Detach();
		style |= WS_DLGFRAME | WS_THICKFRAME;
		SetWindowLong(this->m_hWnd, GWL_STYLE, style);
		this->ShowWindow(SW_SHOWNORMAL);
	} ;

}
