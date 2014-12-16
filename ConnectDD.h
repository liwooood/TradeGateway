#ifndef CONNECT_DD_H
#define CONNECT_DD_H

#include "IConnect.h"

class ConnectDD : public IConnect
{
public:
	ConnectDD(int connId, Counter counter);
	~ConnectDD(void);

	virtual bool CreateConnect();
	virtual void CloseConnect();
};

#endif