#ifndef SYWG_CONNECT_H
#define SYWG_CONNECT_H




#include <string>

#include <boost/asio.hpp>

#include "IBusiness.h"

#include "sywg.h"

// ����ͬ��ģʽʵ��
// ����boost::asio��ʵ�ֿ�ƽ̨
// ��Ҫ֧��CRC
// ��Ҫ֧��DES�����㷨, botan֧��
// ���Ӻź�des��Կ�;���ҵ���޹�
// Ҫ����ҵ��ӿ�xml

class TradeBusinessJSD : public IBusiness
{
public:
	TradeBusinessJSD(void);
	TradeBusinessJSD(int connId, Counter counter);
	~TradeBusinessJSD(void);

public:
	virtual bool CreateConnect();
	virtual void CloseConnect();
	virtual bool Send(std::string& request, std::string& response, int& status, std::string& errCode, std::string& errMsg);


	WORD GetCRC(void* msg, size_t len);
	int ComputeNetPackSize(int nRealSize);
	int ReadMsgHeader(char * buf);

	bool InitConnect();
	bool GetErrorMsg(int nErrCode, std::string& sErrMsg);

	bool Login(std::string& response, int& status, std::string& errCode, std::string& errMsg);

	bool Send(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	
	bool f4603(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	bool f4605(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	bool f4606(std::string& response, int& status, std::string& errCode, std::string& errMsg);
	

private:
	SOCKET sockfd;

	long cn_id;      	// �������Ӻ�
	BYTE des_key[8];         // DES��Կ(���á�ExpressT����Ϊ��Կdes����)
	std::string branchNo; 

	//boost::asio::io_service ios;
	//boost::asio::ip::tcp::socket socket;
	//std::string m_sIP;
	//int m_nPort;

	

public:
	//int m_nErrCode;
	//std::string m_sErrMsg;
	//void SetErrInfo(int nErrCode, std::string sErrMsg="");
	//void GetErrInfo(std::string& sErrCode, std::string& sErrMsg);

	
	

private:
	int64_t GetMoney(std::string buf);
	std::string GetMoney(int64_t money);

	long GetSmallMoney(std::string buf);
	std::string GetSmallMoney(long price);

	int Send(const char * buf, int len, int flags);
	int Recv(char* buf, int len, int flags);

public:
	double Int64_double( SWIMoney SW_Money);
	void Int64_double(double *l, SWIMoney SW_Money);
	void Int64_char(char * buff, SWIMoney SW_Money, BYTE Is_JgdOpen);
	void char_Int64(SWIMoney *SW_Money, const char * QL_Char);

	uint64_t read_be_dd(const void* p)  ;
	uint64_t read_le_dd(const void* p)  ;

};

#endif