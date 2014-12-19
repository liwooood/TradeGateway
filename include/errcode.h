#ifndef ERROR_CODE_MSG_H
#define ERROR_CODE_MSG_H

#include <map>
#include <string>
#include <boost/thread/detail/singleton.hpp>

#define CONNECT_COUNTER_ERROR 1000
#define CONNECT_ALL_COUNTER_ERROR 1001
#define PARAM_ERROR 1002
#define MSG_HEADER_ERROR 1003
#define CONFIG_ERROR 1004
#define BUSI_CRYPT_ERROR 1005
#define PARAM_FORMAT_ERROR 1006
#define GET_CONNECT_ERROR 1007


class Error
{
private:
	std::map<int, std::string> err;

public:
	void init()
	{
		err[CONNECT_COUNTER_ERROR] = "连接柜台失败";
		err[CONNECT_ALL_COUNTER_ERROR] = "连接所有柜台失败";
		err[PARAM_ERROR] = "缺少参数";
		err[PARAM_FORMAT_ERROR] = "参数数据格式错误";
		err[MSG_HEADER_ERROR] = "消息头错误";
		err[CONFIG_ERROR] = "参数和配置文件不匹配";
		err[BUSI_CRYPT_ERROR] = "业务层加解密错误";
		err[GET_CONNECT_ERROR] = "获取连接错误";
	}

	std::string GetErrMsg(int errCode)
	{
		return err[errCode];
	}
};
typedef boost::detail::thread::singleton<Error> gError;

#endif
