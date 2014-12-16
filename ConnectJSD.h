#ifndef CONNECT_JSD_H
#define CONNECT_JSD_H

#include "IConnect.h"

class ConnectJSD : public IConnect
{
public:
	ConnectJSD(int connId, Counter counter);
	~ConnectJSD(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
};

#endif