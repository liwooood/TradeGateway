#ifndef CACHE_DATA_H
#define CACHE_DATA_H

#include <string>
#include <map>

/*
8:30清掉所有数据，初始化
有数据，就返回
没有数据，从柜台读取数据，返回，并保存
*/
class CCacheData
{
public:
	CCacheData(void);
	~CCacheData(void);

	/*
	cssweb_cacheFlag	数据缓存标识
fundList	开放式基金列表
fundCompany	基金公司
fundProducts	基金理财产品列表
financingUnderlying	融资标的证券
marginUnderlying	融券标的证券
underlying	标的证券(全部)
collateral	担保证券

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
