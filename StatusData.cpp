#include "StatusData.h"


StatusData gStatusData;

StatusData::StatusData(void)
{
	tcpOldSessions = 0;
	tcpOldMaxSessions = 0;

	sslOldSessions = 0;
	sslOldMaxSessions = 0;

	sslNewSessions = 0;
	sslNewMaxSessions = 0;

	tcpNewSessions = 0;
	tcpNewMaxSessions = 0;
}


StatusData::~StatusData(void)
{
}
