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

	/*
	cssweb_cacheFlag	���ݻ����ʶ
fundList	����ʽ�����б�
fundCompany	����˾
fundProducts	������Ʋ�Ʒ�б�
financingUnderlying	���ʱ��֤ȯ
marginUnderlying	��ȯ���֤ȯ
underlying	���֤ȯ(ȫ��)
collateral	����֤ȯ

	*/
	std::map<std::string/*cssweb_cacheFlag*/, std::string/*response*/> m_mapCacheData;



	void Start();
	void Stop();
	void* m_hThread; // HANDLE
	static void Run(void* params);

	bool m_bRunning;
};

extern CCacheData g_CacheData;

#endif
