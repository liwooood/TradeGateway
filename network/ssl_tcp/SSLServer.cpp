#include "stdafx.h"

#include "SSLServer.h"

#include "FileLog.h"


SSLServer::SSLServer(unsigned short port, QueueType& q, int msgType, int ioThreadNum)
		:iosPool(*boost::factory<IOServicePool*>()(ioThreadNum))
		,queue(q)
		,acceptor(iosPool.get(),boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		,context(boost::asio::ssl::context::sslv23)
		//,m_session()
{
	this->msgType = msgType;
	this->port = port;
	logFile = "网络\\network_" + boost::lexical_cast<std::string>(port);

	acceptor.set_option(AcceptorType::reuse_address(true));

/*
context_.set_options(
        boost::asio::ssl::context::default_workarounds
        | boost::asio::ssl::context::no_sslv2
        | boost::asio::ssl::context::single_dh_use);
    context_.set_password_callback(boost::bind(&server::get_password, this));
    context_.use_certificate_chain_file("server.pem");
    context_.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
    context_.use_tmp_dh_file("dh512.pem");
*/
	/*
	if (gConfigManager::instance().m_nAuth)
	{
		context_.set_verify_mode(boost::asio::ssl::context::verify_peer  | boost::asio::ssl::context::verify_fail_if_no_peer_cert);
		context_.load_verify_file(gConfigManager::instance().m_sPath  + "\\ca.crt");
	}
		  
		  */

	context.use_certificate_file(gConfigManager::instance().m_sPath + "\\server.crt", boost::asio::ssl::context::pem);
	context.use_private_key_file(gConfigManager::instance().m_sPath + "\\serverkey.pem", boost::asio::ssl::context::pem);

	StartAccept();
}

SSLServer::SSLServer(IOServicePool& ios, unsigned short port, QueueType& q, int msgType)
		:iosPool(ios)
		,queue(q)
		,acceptor(iosPool.get(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
		,context(boost::asio::ssl::context::sslv23)
		//,m_session()
{
	this->msgType = msgType;
	this->port = port;

	logFile = "network_" + boost::lexical_cast<std::string>(port);

	acceptor.set_option(AcceptorType::reuse_address(true));

	/*
	if (gConfigManager::instance().m_nAuth)
	{
		context_.set_verify_mode(boost::asio::ssl::context::verify_peer  | boost::asio::ssl::context::verify_fail_if_no_peer_cert);
		context_.load_verify_file(gConfigManager::instance().m_sPath  + "\\ca.crt");
	}
	*/

	context.use_certificate_file(gConfigManager::instance().m_sPath + "\\server.crt", boost::asio::ssl::context::pem);
	context.use_private_key_file(gConfigManager::instance().m_sPath + "\\serverkey.pem", boost::asio::ssl::context::pem);

	StartAccept();
}


void SSLServer::StartAccept()
{
//	m_session.reset(new SSLSession(ios_pool_.get(), queue_, m_msgType, context_));
	SSLSessionPtr session = boost::factory<SSLSessionPtr>()(iosPool.get(), queue, msgType, port, context);

	acceptor.async_accept(session->getSocket(), 
		boost::bind(&SSLServer::OnAccept, 
		this, 
		boost::asio::placeholders::error, 
		session));
}

// sess不要和session冲突
void SSLServer::OnAccept(const boost::system::error_code& error, SSLSessionPtr session)
{
	StartAccept();

	if (error)
	{
		gFileLog::instance().error(logFile, "SSLServer OnAccept，错误代码:" + boost::lexical_cast<std::string>(error.value()) + "，错误消息:" + error.message());

		session->close();	
	}
	else
	{
		session->start();
	}
}


void SSLServer::start()
{
	iosPool.start();
}



void SSLServer::run()
{
	iosPool.run();
}

void SSLServer::stop()
{
	iosPool.stop();
}

/*
bool SSLServer::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
	// The verify callback can be used to check whether the certificate that is
	// being presented is valid for the peer. For example, RFC 2818 describes
	// the steps involved in doing this for HTTPS. Consult the OpenSSL
	// documentation for more details. Note that the callback is called once
	// for each certificate in the certificate chain, starting from the root
	// certificate authority.

	// In this example we will simply print the certificate's subject name.
	char subject_name[256];
	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	OutputDebugString("Verifying " );
	OutputDebugString(subject_name);
	OutputDebugString("\n");

	return preverified;
}

std::string SSLServer::get_password()
{
	return "chenhf2011";
}


*/