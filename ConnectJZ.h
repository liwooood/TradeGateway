#ifndef CONNECT_JZ_H
#define CONNECT_JZ_H

#include "IConnect.h"


class ConnectJZ : public IConnect
{
public:
	ConnectJZ(int connId, Counter counter);
	~ConnectJZ(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
};

#endif