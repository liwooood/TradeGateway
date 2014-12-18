#ifndef DOGSKIN_H
#define DOGSKIN_H

#include <string>

typedef char * (*fpEncode) (char *, int); 

class Dogskin
{
public:
	Dogskin(void);
	~Dogskin(void);

	fpEncode Encode;
	HMODULE hEncrypt;

	bool LoadDLL();
	void UnLoadDLL();

	std::string EncryptPWD(std::string sPassword);
	
};

extern Dogskin g_Encrypt;

#endif