
// TradeGatewayGtjaDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <atomic>

#include <boost/format.hpp>

// ICU
#include <unicode/putil.h>
#include <unicode/ucnv.h>


#include "TradeGatewayGtja.h"
#include "TradeGatewayGtjaDlg.h"

#include "Dogskin.h"



#include "ConnectManager.h"
#include "dingdian.h"

#include "CacheData.h"


#include "LogConnect.h"

#include "FileLog.h"
#include "FileLogManager.h"
#include "DistributedLogManager.h"
#include "LogConnectPool.h"

#include "TradeBusinessJSD.h"
#include "StatusData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTradeGatewayGtjaDlg 对话框




CTradeGatewayGtjaDlg::CTradeGatewayGtjaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTradeGatewayGtjaDlg::IDD, pParent)
	, m_sSslAddr(_T(""))
	, m_sSslStatus(_T(""))
	, m_sSslWorker(_T(""))
	, m_sSslSend(_T(""))
	, m_sConnPoolMin(_T(""))
	, m_sLogFilePath(_T(""))
	, m_sLogGuiShowcount(_T(""))
	, m_sLogLevel(_T(""))
	, m_nLogFileEnable(0)
	, m_sLogFileThreadPool(_T(""))
	, m_sLogMqThreadPool(_T(""))
	, m_sLogShowThreadPool(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTradeGatewayGtjaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	/*
	DDX_Text(pDX, IDC_SSL_ADDR, m_sSslAddr);
	DDX_Text(pDX, IDC_SSL_STATUS, m_sSslStatus);
	


	

	DDX_Text(pDX, IDC_TCP_ADDR, m_sTcpAddr);
	DDX_Text(pDX, IDC_TCP_STATUS, m_sTcpStatus);
	
	
	DDX_Text(pDX, IDC_CONNPOOL_MIN, m_sConnPoolMin);
	DDX_Text(pDX, IDC_CONNPOOL_INCREASE, m_sConnPoolIncrease);
	//DDX_Text(pDX, IDC_CONNPOOL_MAX, m_sConnPoolMax);
	DDX_Text(pDX, IDC_CONN_TIMEOUT, m_sConnTimeout);
	DDX_Text(pDX, IDC_CONN_RETRY, m_sConnRetry);
	DDX_Text(pDX, IDC_CONN_IDLE_TIMEOUT, m_sConnIdleTimeout);
	DDX_Text(pDX, IDC_BUSI_RETRY, m_sBusiRetry);

	

	

	
	DDX_Text(pDX, IDC_LOG_LEVEL, m_sLogLevel);

	DDX_Check(pDX, IDC_LOG_FILE_ENABLE, m_nLogFileEnable);
	DDX_Text(pDX, IDC_LOG_FILE_WORKER, m_sLogFileThreadPool);
	DDX_Text(pDX, IDC_LOG_FILE_PATH, m_sLogFilePath);

	DDX_Check(pDX, IDC_LOG_MQ_ENABLE, m_nLogMQEnable);
	DDX_Text(pDX, IDC_LOG_MQ_WORKER, m_sLogMqThreadPool);

	DDX_Check(pDX, IDC_LOG_GUI_ENABLE, m_nLogGUIEnable);
	DDX_Text(pDX, IDC_LOG_SHOW_WORKER, m_sLogShowThreadPool);
	DDX_Text(pDX, IDC_LOG_GUI_SHOWCOUNT, m_sLogGuiShowcount);
	*/
}

BEGIN_MESSAGE_MAP(CTradeGatewayGtjaDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	

	ON_BN_CLICKED(IDC_OPEN_LOG, &CTradeGatewayGtjaDlg::OnBnClickedOpenLog)
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_EXIT, &CTradeGatewayGtjaDlg::OnBnClickedExit)

	ON_BN_CLICKED(IDC_CLEAR_CACHE, &CTradeGatewayGtjaDlg::OnBnClickedClearCache)
	
	ON_BN_CLICKED(IDC_CHANGELOG, &CTradeGatewayGtjaDlg::OnBnClickedChangelog)
	ON_BN_CLICKED(IDC_SETUP_REG, &CTradeGatewayGtjaDlg::OnBnClickedSetupReg)

	

END_MESSAGE_MAP()


// CTradeGatewayGtjaDlg 消息处理程序

BOOL CTradeGatewayGtjaDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	

	// 初始始化界面
	boost::format fmt("%1%：%2%:%3%");
	boost::format fmt2("%1%：%2%");
	//boost::str();

	fmt % "TLS网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nSslPort;
	GetDlgItem(IDC_SSL_ADDR)->SetWindowText(fmt.str().c_str());
	GetDlgItem(IDC_SSL_STATUS)->SetWindowText("运行状态：停止");
	

	fmt % "TCP网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nTcpPort;
	GetDlgItem(IDC_TCP_ADDR)->SetWindowText(fmt.str().c_str());
	GetDlgItem(IDC_TCP_STATUS)->SetWindowText("运行状态：停止");

	fmt % "TLS New网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nSslNewPort;
	GetDlgItem(IDC_NEWSSL_ADDR)->SetWindowText(fmt.str().c_str());
	GetDlgItem(IDC_NEWSSL_STATUS)->SetWindowText("运行状态：停止");
	

	fmt % "TCP New网关" % gConfigManager::instance().m_sIp % gConfigManager::instance().m_nTcpNewPort;
	GetDlgItem(IDC_NEWTCP_ADDR)->SetWindowText(fmt.str().c_str());
	GetDlgItem(IDC_NEWTCP_STATUS)->SetWindowText("运行状态：停止");
	
	/*
	fmt2 % "柜台连接池最小值" % gConfigManager::instance().m_nConnectPoolMin;
	m_sConnPoolMin = fmt2.str().c_str();
	fmt2 % "柜台连接池增长值" % gConfigManager::instance().m_nConnectPoolIncrease;
	m_sConnPoolIncrease = fmt2.str().c_str();
	
	//fmt2 % "连接超时时间" % gConfigManager::instance().m_nConnectTimeout;
	//m_sConnTimeout = fmt2.str().c_str();
	
	fmt2 % "连接失败重试次数" % gConfigManager::instance().m_nConnectRetry;
	m_sConnRetry = fmt2.str().c_str();
	//fmt2 % "连接空闲超时时间" % gConfigManager::instance().m_nIdleTimeout;
	//m_sConnIdleTimeout = fmt2.str().c_str();
	fmt2 % "业务失败重试次数" % gConfigManager::instance().m_nBusinessRetry;
	m_sBusiRetry = fmt2.str().c_str();

	fmt2 % "日志文件目录" % gConfigManager::instance().m_sLogFilePath;
	m_sLogFilePath = fmt2.str().c_str();
	
	

	fmt2 % "显示记录数" % gConfigManager::instance().m_nLogGuiShowCount;
	m_sLogGuiShowcount = fmt2.str().c_str();


	switch (gConfigManager::instance().m_nLogLevel)
	{
	case 0:
		m_sLogLevel = "日志输出级别：调试";
		break;
	case 1:
		m_sLogLevel = "日志输出级别：信息";
		break;
	case 2:
		m_sLogLevel = "日志输出级别：警告";
		break;
	case 3:
		m_sLogLevel = "日志输出级别：错误";
		break;
	}

	m_nLogFileEnable = gConfigManager::instance().m_nLogFileEnable;

	m_nLogMQEnable = gConfigManager::instance().m_nLogMqEnable;
	m_nLogGUIEnable = gConfigManager::instance().m_nLogGuiEnable;

	fmt2 % "文件日志系统线程池大小" % gConfigManager::instance().m_nLogFileThreadPool;
	m_sLogFileThreadPool = fmt2.str().c_str();
	
	fmt2 % "分布式日志系统线程池大小" % gConfigManager::instance().m_nLogMqThreadPool;
	m_sLogMqThreadPool = fmt2.str().c_str();
	fmt2 % "界面日志系统线程池大小" % gConfigManager::instance().m_nLogShowThreadPool;
	m_sLogShowThreadPool = fmt2.str().c_str();
	*/

	UpdateData(false);

	

	
	//运行即启动
	start();

	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTradeGatewayGtjaDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTradeGatewayGtjaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTradeGatewayGtjaDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ÔÚ´Ë´¦Ìí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂë
}

void CTradeGatewayGtjaDlg::OnClose()
{
	// 释放资源，防止内存泄漏
	stop();

	CDialog::OnClose();
}


void CTradeGatewayGtjaDlg::start()
{
	// 启动缓存线程，用于清除缓存数据
	g_CacheData.Start();

	//文件日志
	gFileLogManager::instance().start();
	gFileLogManager::instance().LoadFieldFilter();
	gFileLogManager::instance().LoadFuncFilter();

	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		if(!gLogConnectPool::instance().CreateConnectPool())
		{
			AfxMessageBox("创建分布式日志连接池失败");
			return;
		}
	}

	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		gDistributedLogManager::instance().start();
		gDistributedLogManager::instance().LoadFieldFilter();
		gDistributedLogManager::instance().LoadFuncFilter();
	}

	

	// tcp 4字节
	if (gConfigManager::instance().m_nTcpEnable)
	{
		pTradeServerTcpOld = new TradeServer(MSG_TYPE_TCP_OLD);

		iospool_tcp_old = new IOServicePool(GetCPUCount());
		pTcpOldServer = new TcpServer(*iospool_tcp_old, gConfigManager::instance().m_nTcpPort, pTradeServerTcpOld->recv_queue(), MSG_TYPE_TCP_OLD);
	

		pTradeServerTcpOld->start();
		pTcpOldServer->start();

		GetDlgItem(IDC_TCP_STATUS)->SetWindowText("运行状态：运行");
	}

	// ssl pb 
	if (gConfigManager::instance().m_nSslEnable)
	{
		pTradeServerSslOld = new TradeServer(MSG_TYPE_SSL_PB);

		iospool_ssl_old = new IOServicePool(GetCPUCount());
		pSslOldServer = new SSLServer(*iospool_ssl_old, gConfigManager::instance().m_nSslPort, pTradeServerSslOld->recv_queue(), MSG_TYPE_SSL_PB);
	

		pTradeServerSslOld->start();
		pSslOldServer->start();

		GetDlgItem(IDC_SSL_STATUS)->SetWindowText("运行状态：运行");
	}
	

	// tcp new
	if (gConfigManager::instance().m_nTcpNewEnable)
	{
		pTradeServerTcpNew = new TradeServer(MSG_TYPE_TCP_NEW);

		iospool_tcp_new = new IOServicePool(GetCPUCount());
		pTcpNewServer = new TcpServer(*iospool_tcp_new, gConfigManager::instance().m_nTcpNewPort, pTradeServerTcpNew->recv_queue(), MSG_TYPE_TCP_NEW);
	

		pTradeServerTcpNew->start();
		pTcpNewServer->start();

		GetDlgItem(IDC_NEWTCP_STATUS)->SetWindowText("运行状态：运行");
	}
	


	// ssl new 
	if (gConfigManager::instance().m_nSslNewEnable)
	{
		pTradeServerSslNew = new TradeServer(MSG_TYPE_SSL_NEW);

		iospool_ssl_new = new IOServicePool(GetCPUCount());
		pSslNewServer = new SSLServer(*iospool_ssl_new, gConfigManager::instance().m_nSslNewPort, pTradeServerSslNew->recv_queue(), MSG_TYPE_SSL_NEW);
	

		pTradeServerSslNew->start();
		pSslNewServer->start();

		GetDlgItem(IDC_NEWSSL_STATUS)->SetWindowText("运行状态：运行");
	}
	
	

	gFileLog::instance().Log("交易网关启动");

	std::string caption = "中软万维证券网关版本：";
	caption += VERSION;
	caption +=" 网络层版本：";
	caption += MY_BOOST_VERSION;
	caption +=" OpenSSL版本：";
	caption += SSL_VERSION;

	SetWindowText(caption.c_str());

	updateStatusDataFinish = true;
	statusDataThread.reset(new boost::thread(boost::bind(&CTradeGatewayGtjaDlg::UpdateStatusData, this)));

}

void CTradeGatewayGtjaDlg::UpdateStatusData()
{
	try
	{
		std::string session1 = "在线连接数：";
		std::string session2 = "最大连接数：";
		long count = 0;

		std::string tcpSession;
		std::string sslSession;
		std::string newTcpSession;
		std::string newSslSession;

		std::string tcpMaxSession;
		std::string sslMaxSession;
		std::string newTcpMaxSession;
		std::string newSslMaxSession;

		while (updateStatusDataFinish)
		{
		
			// tcp old
			count = gStatusData.tcpOldSessions.load();
			tcpSession = session1 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_TCP_SESSIONS)->SetWindowText(tcpSession.c_str());

			count = gStatusData.tcpOldMaxSessions.load();
			tcpMaxSession = session2 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_TCP_MAX_SESSIONS)->SetWindowText(tcpMaxSession.c_str());


			// ssl old
			count = gStatusData.sslOldSessions.load();
			sslSession = session1 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_SSL_SESSIONS)->SetWindowText(sslSession.c_str());

			count = gStatusData.sslOldMaxSessions.load();
			sslMaxSession = session2 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_SSL_MAX_SESSIONS)->SetWindowText(sslMaxSession.c_str());

			// tcp new
			count = gStatusData.tcpNewSessions.load();
			newTcpSession = session1 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_NEWTCP_SESSIONS)->SetWindowText(newTcpSession.c_str());

			count = gStatusData.tcpNewMaxSessions.load();
			tcpMaxSession = session2 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_NEWTCP_MAX_SESSIONS)->SetWindowText(tcpMaxSession.c_str());

			//ssl new
			count = gStatusData.sslNewSessions.load();
			newSslSession = session1 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_NEWSSL_SESSIONS)->SetWindowText(newSslSession.c_str());

			count = gStatusData.sslNewMaxSessions.load();
			sslMaxSession = session2 + boost::lexical_cast<std::string>(count);
			GetDlgItem(IDC_NEWSSL_MAX_SESSIONS)->SetWindowText(sslMaxSession.c_str());
			
			


			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}

		if (!updateStatusDataFinish)
		{
			TRACE("UpdateStatusData is exit................................");
		}
	}
	catch(boost::thread_interrupted& /*e*/)
	{
		TRACE("UpdateStatusData is interrupted................................");
	}
}

// 退出按钮事件
void CTradeGatewayGtjaDlg::OnBnClickedExit()
{
	updateStatusDataFinish = false;

	stop();

	EndDialog(0);
}
void CTradeGatewayGtjaDlg::stop()
{
	g_CacheData.Stop();

	// 文件日志
	gFileLogManager::instance().stop();

	/*
	// 分布式日志
	if (gConfigManager::instance().m_nLogMqEnable)
	{
		gLogConnectPool::instance().CloseConnectPool();
		gDistributedLogManager::instance().stop();
	}
	*/

	if (gConfigManager::instance().m_nTcpEnable)
	{
		if (pTcpOldServer != NULL)
		{
			pTcpOldServer->stop();
			delete pTcpOldServer;
		}
		if (pTradeServerTcpOld != NULL)
		{
			pTradeServerTcpOld->stop();
			delete pTradeServerTcpOld;
		}
		if (iospool_tcp_old != NULL)
		{
			iospool_tcp_old->stop();
			//delete iospool_tcp_old;
		}
	}

	if (gConfigManager::instance().m_nSslEnable)
	{
		if (pSslOldServer != NULL)
		{
			pSslOldServer->stop();
			delete pSslOldServer;
		}
		if (pTradeServerSslOld != NULL)
		{
			pTradeServerSslOld->stop();
			delete pTradeServerSslOld;
		}
		if (iospool_ssl_old != NULL)
		{
			iospool_ssl_old->stop();
		//	delete iospool_ssl_old;
		}
	}
	
	if (gConfigManager::instance().m_nTcpNewEnable)
	{
		if (pTcpNewServer != NULL)
		{
			pTcpNewServer->stop();
			delete pTcpNewServer;
		}
		if (pTradeServerTcpNew != NULL)
		{
			pTradeServerTcpNew->stop();
			delete pTradeServerTcpNew;
		}
		if (iospool_tcp_new != NULL)
		{
			iospool_tcp_new->stop();
//			delete iospool_tcp_new;
		}
	}

	if (gConfigManager::instance().m_nSslNewEnable)
	{
		if (pSslNewServer != NULL)
		{
			pSslNewServer->stop();
			delete pSslNewServer;
		}
		if (pTradeServerSslNew != NULL)
		{
			pTradeServerSslNew->stop();
			delete pTradeServerSslNew;
		}
		if (iospool_ssl_new != NULL)
		{
			iospool_ssl_new->stop();
		//	delete iospool_ssl_new;
		}
	}
	
}




// 手工清除缓存数据
void CTradeGatewayGtjaDlg::OnBnClickedClearCache()
{
	// TODO: 在此添加控件通知处理程序代码
	g_CacheData.m_mapCacheData.clear();
}

void CTradeGatewayGtjaDlg::OnBnClickedChangelog()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string changeLog = "";
	changeLog = gConfigManager::instance().m_sPath + "\\ChangeLog.txt";
	ShellExecute(NULL, "open", changeLog.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void CTradeGatewayGtjaDlg::OnBnClickedSetupReg()
{
	// TODO: 在此添加控件通知处理程序代码
	std::string regFile = gConfigManager::instance().m_sPath + "\\wer.reg";

	ShellExecute(NULL, "open", regFile.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CTradeGatewayGtjaDlg::OnBnClickedAbout()
{
	// TODO: 在此添加控件通知处理程序代码
}

// 打开日志目录
void CTradeGatewayGtjaDlg::OnBnClickedOpenLog()
{
	ShellExecute(NULL, "open", gConfigManager::instance().m_sLogFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

// 得到cpu内核数
int CTradeGatewayGtjaDlg::GetCPUCount()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return systemInfo.dwNumberOfProcessors * 2;

}