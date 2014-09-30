#ifndef CACHE_DATA_H
#define CACHE_DATA_H

#include <string>
#include <map>

/*
8:30����������ݣ���ʼ��
�����ݣ��ͷ���
û�����ݣ��ӹ�̨��ȡ���ݣ����أ�������
*/
class CCacheData
{
public:
	CCacheData(void);
	~CCacheData(void);

	// key=hash(request), value=response
	std::map<std::string, std::string> m_mapCacheData;



	void Start();
	void Stop();
	void* m_hThread; // HANDLE
	static void Run(void* params);

	bool m_bRunning;
};

extern CCacheData g_CacheData;

#endif
