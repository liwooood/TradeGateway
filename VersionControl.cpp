#include "stdafx.h"

#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>

#include "VersionControl.h"
#include "ConfigManager.h"



VersionControl::VersionControl(void)
{
}


VersionControl::~VersionControl(void)
{
}

void VersionControl::Load()
{
	boost::property_tree::ptree pt;

	std::string s = gConfigManager::instance().m_sPath + "\\versionandroid.txt";
	boost::property_tree::read_json(s, pt);

	unforceupdatemsg = pt.get<std::string>("unforceupdatemsg");

	forceupdatemsg = pt.get<std::string>("forceupdatemsg");

	url = pt.get<std::string>("url");

	boost::property_tree::ptree list = pt.get_child("versionlist");
	boost::property_tree::ptree pt2;
	for (BOOST_AUTO(pos, list.begin()); pos != list.end(); ++pos)
	{
		std::string first = pos->first;
		pt2 =  pos->second;

		std::string name  = pt2.get<std::string>("name");
		std::string update  = pt2.get<std::string>("update");

		mapVersions[name] = update;
	}

}
