#include "stdafx.h"
//#define BOOST_FILESYSTEM_VERSION 2
// mongodb用的还是老版本，只支持2，未来会自带boost1.49

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/functional/factory.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/algorithm/string.hpp>
//压缩
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>


// ICU
#include <unicode/putil.h>
#include <unicode/ucnv.h>


#include "TradeServer.h"

#include "common.h"
#include "configmanager.h"

// 金证
#include "TradeBusinessJZ.h"

// 恒生
#include "TradeBusinessHS.h"
// 顶点
#include "TradeBusinessDD.h"
#include "DingDian.h"
// AGC
#include "TradeBusinessJSD.h"
// 新意
//#include "TCPClientSync.h"




#include "FileLog.h"

#include "connectmanager.h"




#include "FileLogManager.h"
#include "errcode.h"

#include "TcpSession.h"
#include "SSLSession.h"

#include "IMessage.h"
#include "TcpMessageOld.h"
#include "SslMessagePB.h"
#include "CustomMessage.h" 

#include "ConnectPool.h"
#include "ConnectT2.h"


TradeServer::TradeServer(int msgType)
	:req_worker_(recvq_, boost::bind(&TradeServer::ProcessRequest, this, _1), gConfigManager::instance().m_nTcpWorkerThreadPool)
	,resp_worker_(sendq_, boost::bind(&TradeServer::ProcessResponse, this, _1), gConfigManager::instance().m_nTcpSendThreadPool)
{
	m_MsgType = msgType;
}

void TradeServer::start()
{
	req_worker_.start();
	resp_worker_.start();
}

void TradeServer::stop()
{
	req_worker_.stop();
	resp_worker_.stop();
}

TradeServer::req_queue_type& TradeServer::recv_queue()
{
	return recvq_;
}

// 处理应答
bool TradeServer::ProcessResponse(IMessage* resp)
{
	if (resp->msgType == MSG_TYPE_TCP_OLD || resp->msgType == MSG_TYPE_TCP_NEW)
		resp->GetTcpSession()->write(resp);

	if (resp->msgType == MSG_TYPE_SSL_PB || resp->msgType == MSG_TYPE_SSL_NEW)
		resp->GetSslSession()->write(resp);

	return true;
}

// 处理请求，由于是线程函数，不要使用共享数据
bool TradeServer::ProcessRequest(IMessage* req)
{
	std::string SOH = "\x01";



	Trade::TradeLog::LogLevel logLevel = Trade::TradeLog::INFO_LEVEL;


	std::string sysNo = "";
	std::string busiType = "";
	int nBusiType;
	std::string sysVer = "";
	std::string funcId = "";
	int nFuncId = -1;
	std::string account = "";
	std::string clientIp = "";

	std::string gatewayIp = "";
	std::string gatewayPort = "";
	std::string gatewayServer = "";

	std::string counterIp = "";
	std::string counterPort = "";
	std::string counterType = "";
	int nCounterType = COUNTER_TYPE_UNKNOWN;
	std::string counterServer = "";
	int counterMode;

	boost::posix_time::ptime ptBeginTime;
	std::string beginTime = "";
	int runtime = 0;


	// utf8
	std::string request_utf8 = req->GetMsgContentString();
	std::string request = request_utf8;
	

	std::string response = "";
	int status = 0;
	std::string errCode = "";
	std::string errMsg = "";


	
	// 以下都是日志所需的信息
	IMessage * resp = NULL;

	
	
	// 开始时间
	ptBeginTime = boost::posix_time::microsec_clock::local_time();
	beginTime = boost::gregorian::to_iso_extended_string(ptBeginTime.date()) + " " + boost::posix_time::to_simple_string(ptBeginTime.time_of_day());;

	
	

	

	if (m_MsgType == MSG_TYPE_TCP_OLD)
	{
		gatewayIp = gConfigManager::instance().m_sIp;
		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpPort);
		clientIp = req->GetTcpSession()->getSocket().remote_endpoint().address().to_v4().to_string();
	}

	if (m_MsgType == MSG_TYPE_SSL_PB)
	{
		gatewayIp = gConfigManager::instance().m_sIp;
		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslPort);
		clientIp = req->GetSslSession()->getSocket().remote_endpoint().address().to_v4().to_string();

		// 只有移动端需要转换字符集
		UErrorCode errcode = U_ZERO_ERROR;
		char dest[20480];
		memset(dest, 0x00, sizeof(dest));
		// 从utf8转成gbk
		int ret = ucnv_convert("gbk", "utf8", dest, sizeof(dest), request_utf8.c_str(), -1, &errcode);
		request = dest;
	}

	if (m_MsgType == MSG_TYPE_TCP_NEW)
	{
		gatewayIp = gConfigManager::instance().m_sIp;
		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nTcpNewPort);
		clientIp = req->GetTcpSession()->getSocket().remote_endpoint().address().to_v4().to_string();
	}

	if (m_MsgType == MSG_TYPE_SSL_NEW)
	{
		gatewayIp = gConfigManager::instance().m_sIp;
		gatewayPort = boost::lexical_cast<std::string>(gConfigManager::instance().m_nSslNewPort);
		clientIp = req->GetSslSession()->getSocket().remote_endpoint().address().to_v4().to_string();
	}
	gatewayServer = gatewayIp + ":" + gatewayPort;
	// 定义结束
							
	

	// 分析请求
	if (!GetSysNoAndBusiType(request, sysNo, busiType, sysVer, account, funcId, errCode, errMsg))
	{

		response = "1" + SOH + "2" + SOH;
		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += errCode;
		response += SOH;
		response += errMsg;
		response += SOH;

		goto FINISH;
	}
	
	// 监控代理心跳功能
	if (funcId == "999999")
	{
		response = "heartbeat";

		goto FINISH;
	}

	try
	{
		nBusiType = boost::lexical_cast<int>(busiType);
	}
	catch(std::exception& exp)
	{
		exp.what();

		errCode = boost::lexical_cast<std::string>(PARAM_FORMAT_ERROR);
		errMsg = gError::instance().GetErrMsg(PARAM_FORMAT_ERROR) + "cssweb_busitype";

		response = "1" + SOH + "2" + SOH;
		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += errCode;
		response += SOH;
		response += errMsg;
		response += SOH;

		goto FINISH;
	}

	
	if (!g_ConnectManager.GetCounterTypeAndAsyncMode(sysNo, busiType, nCounterType, counterMode))
	{
		logLevel = Trade::TradeLog::ERROR_LEVEL;

		errCode = boost::lexical_cast<std::string>(CONFIG_ERROR);
		errMsg = gError::instance().GetErrMsg(CONFIG_ERROR);

		response = "1" + SOH + "2" + SOH;
		response += "cssweb_code";
		response += SOH;
		response += "cssweb_msg";
		response += SOH;
		response += errCode;
		response += SOH;
		response += errMsg;
		response += SOH;

		goto FINISH;
	}


	if (counterMode == COUNTER_MODE_CONNPOOL)
	{
		IBusiness *pBusiness = g_ConnectManager.GetConnect(sysNo, nBusiType, "0000");
		if (pBusiness == NULL)
		{
			logLevel = Trade::TradeLog::ERROR_LEVEL;

			errCode = boost::lexical_cast<std::string>(GET_CONNECT_ERROR);
			errMsg = gError::instance().GetErrMsg(GET_CONNECT_ERROR);
			
			response = "1" + SOH + "2" + SOH;
			response += "cssweb_code";
			response += SOH;
			response += "cssweb_msg";
			response += SOH;
			response += errCode;
			response += SOH;
			response += errMsg;
			response += SOH;

			goto FINISH;
		}
		
		pBusiness->SetGatewayInfo(gatewayIp, gatewayPort);
		pBusiness->Send(request, response, status, errCode, errMsg);

		g_ConnectManager.PushConnect(pBusiness, sysNo, nBusiType, "0000");
	}
	else
	{
	/*	
		//可以用以下语句代替冗长的写法
		//IBusiness& business = req->GetSslSession()->GetCounterConnect(nCounterType);

	
		// 得到配置的柜台服务器数
		int serverCount = 0;
		//g_ConnectManager.GetCounterCount(sysNo, nBusiType, "0000");
		if (serverCount == 0)
		{
		}



		// 连接需要处理负载均衡和故障切换
		// 业务重试还需要考虑吗？
		
		// 如果没有连接，建立连接，处理请求
		// 如果建立连接失败，轮询每一个服务器，如果所有服务器连接失败，返回错误
		// 当前连接失效，send调用失败，重试
		


	
	
		// 业务重试
		ptBeginTime = boost::posix_time::microsec_clock::local_time();
		beginTime = boost::gregorian::to_iso_extended_string(ptBeginTime.date()) + " " + boost::posix_time::to_simple_string(ptBeginTime.time_of_day());;

		for (int retry=0; retry<1; retry++)
		{
			bool bConnect = false;
	
			// 轮询每一个服务器
			for (int i=0; i<serverCount; i++)
			{
				bool bCounterConnected = false;

				if (req->msgType == MSG_TYPE_TCP_OLD || req->msgType == MSG_TYPE_TCP_NEW)
					bCounterConnected = req->GetTcpSession()->GetCounterConnect(nCounterType).IsConnected();

				if (req->msgType == MSG_TYPE_SSL_PB || req->msgType == MSG_TYPE_SSL_NEW)
					bCounterConnected = req->GetSslSession()->GetCounterConnect(nCounterType).IsConnected();
			
				// 是否已建立连接
				if (bCounterConnected)
				{
					// 已建立连接，跳出循环
					bConnect = true;
					break;
				}
				else
				{
					// 准备建立到柜台的连接
					ptBeginTime = boost::posix_time::microsec_clock::local_time();
					beginTime = boost::gregorian::to_iso_extended_string(ptBeginTime.date()) + " " + boost::posix_time::to_simple_string(ptBeginTime.time_of_day());;

				
					Counter * counter = NULL;
					// 得到下一个服务器
//					counter = g_ConnectManager.GetNextCounter(sysNo, nBusiType, "0000");
					counterIp = counter->m_sIP;
					counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
					counterType = boost::lexical_cast<std::string>(counter->m_nCounterType);
				
					bool bRet = false;

					if (req->msgType == MSG_TYPE_TCP_OLD || req->msgType == MSG_TYPE_TCP_NEW)
					{
						req->GetTcpSession()->GetCounterConnect(nCounterType).SetCounter(counter);
						bRet = req->GetTcpSession()->GetCounterConnect(nCounterType).CreateConnect();
					}

					if (req->msgType == MSG_TYPE_SSL_PB || req->msgType == MSG_TYPE_SSL_NEW)
					{
						req->GetSslSession()->GetCounterConnect(nCounterType).SetCounter(counter);
						bRet = req->GetSslSession()->GetCounterConnect(nCounterType).CreateConnect();
					}

				
					if (bRet)
					{
						// 建立连接成功，跳出循环
						bConnect = true;
						break;
					}
					else
					{
						// 建立连接失败
						// 轮询算法：返回下一个服务器
						bConnect = false;

						//Counter * counter = NULL;
						//counter = g_ConnectManager.GetServer(sysNo, gConfigManager::instance().ConvertIntToBusiType(nBusiType), "0000");
						//req->GetSession()->GetCounterConnect(nCounterType)->SetCounterServer(counter);

					
					
						logLevel = Trade::TradeLog::ERROR_LEVEL;

						errCode = boost::lexical_cast<std::string>(CONNECT_COUNTER_ERROR);
						errMsg = gError::instance().GetErrMsg(CONNECT_COUNTER_ERROR);

						boost::posix_time::ptime ptEndTime = boost::posix_time::microsec_clock::local_time();
						runtime = (ptEndTime - ptBeginTime).total_microseconds();// 微秒数

						req->Log(Trade::TradeLog::ERROR_LEVEL, sysNo, sysVer, busiType, funcId, account, clientIp, request, response, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
						gFileLogManager::instance().push(req->log);
					} // end if
				} // end if
			} // end for (int i=0; i<serverCount; i++)


			// 所有服务器连接不上
			if (!bConnect)
			{
				logLevel = Trade::TradeLog::ERROR_LEVEL;

				errCode = boost::lexical_cast<std::string>(CONNECT_ALL_COUNTER_ERROR);
				errMsg = gError::instance().GetErrMsg(CONNECT_ALL_COUNTER_ERROR);

				response = "1" + SOH + "2" + SOH;
				response += "cssweb_code";
				response += SOH;
				response += "cssweb_msg";
				response += SOH;
				response += errCode;
				response += SOH;
				response += errMsg;
				response += SOH;

			
				goto finish;
			}

			// 每次请求都需要记录柜台的ip和port
				
		
			// 处理业务，业务处理失败或成功都算成功的，只有通信失败才需要重试
			bool bNetwork = false;
			if (req->msgType == MSG_TYPE_TCP_OLD || req->msgType == MSG_TYPE_TCP_NEW)
			{
				Counter * counter = req->GetTcpSession()->GetCounterConnect(nCounterType).GetCounter();
				counterIp = counter->m_sIP;
				counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
				counterType = boost::lexical_cast<std::string>(counter->m_nCounterType);
				counterServer = counterIp + ":"+ counterPort;

				bNetwork = req->GetTcpSession()->GetCounterConnect(nCounterType).Send(request, response, status, errCode, errMsg);
			}

			if (req->msgType == MSG_TYPE_SSL_PB || req->msgType == MSG_TYPE_SSL_NEW)
			{
				Counter * counter = req->GetSslSession()->GetCounterConnect(nCounterType).GetCounter();
				counterIp = counter->m_sIP;
				counterPort = boost::lexical_cast<std::string>(counter->m_nPort);
				counterType = boost::lexical_cast<std::string>(counter->m_nCounterType);
				counterServer = counterIp + ":"+ counterPort;

				bNetwork = req->GetSslSession()->GetCounterConnect(nCounterType).Send(request, response, status, errCode, errMsg);
			}

			if (!bNetwork)
			{
				// 通信失败,开始重试
				// 处理一次，写一次日志
				// fileLog.push(req->log)
				//req->Log(Trade::TradeLog::, sysNo, sysVer, busiType, funcId, account, clientIp, request, response, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
				continue;

			}
			else
			{
			
				// 业务失败，重构response
				// 因为业务层，业务处理失败为了返回柜台和网关信息，没有处理，所以这里需要重构
				// status =1，代表处理成功, response已有格式
				if (status == 0)
				{
					response = "1";
					response += SOH;
					response += "4";
					response += SOH;

					response += "cssweb_code";
					response += SOH;
					response += "cssweb_msg";
					response += SOH;
					response += "cssweb_gwInfo";
					response += SOH;
					response += "cssweb_counter";
					response += SOH;

					response += errCode;
					response += SOH;
					response += errMsg;
					response += SOH;
					response += gatewayServer;
					response += SOH;
					response += counterServer;
					response += SOH;
				}

				logLevel = Trade::TradeLog::INFO_LEVEL;

				boost::posix_time::ptime ptEndTime = boost::posix_time::microsec_clock::local_time();
				runtime = (ptEndTime - ptBeginTime).total_microseconds();// 微秒数

				break;
			}// end if

		} // end for retry
		*/
	}


FINISH:

	if (req->msgType == MSG_TYPE_TCP_OLD)
	{
		resp = new TcpMessageOld();

		// 设置会话
		resp->SetTcpSession(req->GetTcpSession());

		// 设置消息头
		int msgHeaderSize = response.size();
		msgHeaderSize = htonl(msgHeaderSize);
		memcpy(&(resp->msgHeader.front()), &msgHeaderSize, 4);

		// 设置消息内容
		resp->SetMsgContent(response);
	}
		
	if (req->msgType ==  MSG_TYPE_SSL_PB)
	{
		resp = new SslMessagePB();

		// 设置会话
		resp->SetSslSession(req->GetSslSession());

		quote::PkgHeader pbHeader;

		if (response.size() > gConfigManager::instance().zlib)
		{
			std::vector<char> compressedMsgContent;

			boost::iostreams::filtering_streambuf<boost::iostreams::output> compress_out;
			compress_out.push(boost::iostreams::zlib_compressor());
			compress_out.push(boost::iostreams::back_inserter(compressedMsgContent));

			boost::iostreams::copy(boost::make_iterator_range(response), compress_out);

			pbHeader.set_zip(true);

			int compressedMsgContentSize = compressedMsgContent.size();
			pbHeader.set_bodysize(compressedMsgContentSize);

			// 设置消息内容
			resp->SetMsgContent(compressedMsgContent);
		}
		else
		{
			pbHeader.set_zip(false);

			pbHeader.set_bodysize(response.size());

			// 设置消息内容
			resp->SetMsgContent(response);

		}
		
		pbHeader.set_ver(1);
		pbHeader.set_enc(false);
		
		pbHeader.set_more(false);
		pbHeader.set_msgtype(quote::PkgHeader::RES_TRADE);
		pbHeader.set_errcode(0);
		
		// 设置消息头
		bool bTmp = pbHeader.SerializeToArray(&(resp->msgHeader.front()), pbHeader.ByteSize());		
	}
	
	if (req->msgType == MSG_TYPE_TCP_NEW)
	{
		resp = new CustomMessage(MSG_TYPE_TCP_NEW);

		// 设置会话
		resp->SetTcpSession(req->GetTcpSession());

		MSG_HEADER binRespMsgHeader;
		binRespMsgHeader.CRC = 0;
		binRespMsgHeader.FunctionNo = nFuncId;
		
		binRespMsgHeader.MsgType = MSG_TYPE_RESPONSE_END;

		if (response.size() > gConfigManager::instance().zlib)
		{
			std::vector<char> compressedMsgContent;

			boost::iostreams::filtering_streambuf<boost::iostreams::output> compress_out;
			compress_out.push(boost::iostreams::zlib_compressor());
			compress_out.push(boost::iostreams::back_inserter(compressedMsgContent));
			boost::iostreams::copy(boost::make_iterator_range(response), compress_out);

			binRespMsgHeader.zip = 1;
			int compressedMsgContentSize = compressedMsgContent.size();
			binRespMsgHeader.MsgContentSize = compressedMsgContentSize;

			// 设置消息内容
			resp->SetMsgContent(compressedMsgContent);
		}
		else
		{
			binRespMsgHeader.zip = 0;
			binRespMsgHeader.MsgContentSize = response.size();
			// 设置消息内容
			resp->SetMsgContent(response);
		}

		// 设置消息头
		memcpy(&(resp->msgHeader.front()), &binRespMsgHeader, sizeof(MSG_HEADER));
	}
	
	if (req->msgType ==  MSG_TYPE_SSL_NEW)
	{
		resp = new CustomMessage(MSG_TYPE_SSL_NEW);

		// 设置会话
		resp->SetSslSession(req->GetSslSession());

		MSG_HEADER binRespMsgHeader;
		binRespMsgHeader.CRC = 0;
		binRespMsgHeader.FunctionNo = nFuncId;
		
		binRespMsgHeader.MsgType = MSG_TYPE_RESPONSE_END;

		if (response.size() > gConfigManager::instance().zlib)
		{
			std::vector<char> compressedMsgContent;
			boost::iostreams::filtering_streambuf<boost::iostreams::output> compress_out;
			compress_out.push(boost::iostreams::zlib_compressor());
			compress_out.push(boost::iostreams::back_inserter(compressedMsgContent));
			boost::iostreams::copy(boost::make_iterator_range(response), compress_out);

			binRespMsgHeader.zip = 1;
			int compressedMsgContentSize = compressedMsgContent.size();
			binRespMsgHeader.MsgContentSize = compressedMsgContentSize;
			// 设置消息内容
			resp->SetMsgContent(compressedMsgContent);

		}
		else
		{
			binRespMsgHeader.zip = 0;
			binRespMsgHeader.MsgContentSize = response.size();
			// 设置消息内容
			resp->SetMsgContent(response);

		}

		//msgHeader.resize(sizeof(MSG_HEADER));

		memcpy(&(resp->msgHeader.front()), &binRespMsgHeader, sizeof(MSG_HEADER));
	}


	// 保存截取后的日志消息
	std::string res = "";
	if (response.length() > gConfigManager::instance().m_nResponseLen)
		res = response.substr(0, gConfigManager::instance().m_nResponseLen);
	else 
		res = response;


	// 生成日志信息
	boost::posix_time::ptime ptEndTime = boost::posix_time::microsec_clock::local_time();
	runtime = (ptEndTime - ptBeginTime).total_microseconds();// 微秒数

	req->Log(logLevel, sysNo, sysVer, busiType, funcId, account, clientIp, request, res, status, errCode, errMsg, beginTime, runtime, gatewayIp, gatewayPort, counterIp, counterPort, counterType);
	resp->log = req->log; 


	// 删除请求消息
	req->destroy();

	// 把应答消息推入应答队列
	sendq_.push(resp);

	
	return true;
}

bool TradeServer::GetSysNoAndBusiType(std::string& request, std::string& sysNo, std::string& busiType, std::string& sysVer, std::string& account, std::string& funcId, std::string& errCode, std::string& errMsg)
{
	if (request.empty() || request.size() < 10)
	{
		errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
		errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + ",无效的请求串";
		return false;
	}

	std::string SOH = "\x01";

	std::vector<std::string> keyvalues;
	boost::split(keyvalues, request, boost::is_any_of(SOH));

	std::string key = "";
	std::string value = "";

	for (std::vector<std::string>::iterator it = keyvalues.begin(); it != keyvalues.end(); it++)
	{
		std::string keyvalue = *it;


		if (keyvalue.empty())
			continue;

		
		std::size_t found = keyvalue.find_first_of("=");
		

		if (found != std::string::npos)
		{
			key = keyvalue.substr(0, found);
			value = keyvalue.substr(found + 1);
			
		}
		else
		{
			return false;
		}


		if (key == "cssweb_sysNo")
		{
			sysNo = value;
			if (sysNo.empty())
			{
				errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
				errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + "cssweb_sysNo";
				return false;
			}
		}

		if (key == "cssweb_busiType")
		{
			busiType = value;
			if (busiType.empty())
			{
				errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
				errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + "cssweb_busiType";
				return false;
			}
		}

		if (key == "cssweb_sysVer")
		{
			sysVer = value;
			if (sysVer.empty())
			{
				errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
				errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + "cssweb_sysVer";
				return false;
			}
		}

		if (key == "cssweb_funcid")
		{
			funcId = value;
			if (funcId.empty())
			{
				errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
				errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + "cssweb_funcid";
				return false;
			}
		}
		
		if (key == "cssweb_account")
		{
			account = value;
			/*
			if (account.empty())
			{
				errCode = boost::lexical_cast<std::string>(PARAM_ERROR);
				errMsg = gError::instance().GetErrMsg(PARAM_ERROR) + "cssweb_account";
				return false;
			}
			*/
		}

	}

	

	return true;
}
