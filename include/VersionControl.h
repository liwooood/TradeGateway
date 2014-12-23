#ifndef VERSION_CONTROL_H
#define VERSION_CONTROL_H

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread/detail/singleton.hpp>

class VersionControl
{
public:
	VersionControl(void);
	~VersionControl(void);

	void Load();

	std::string unforceupdatemsg;
	std::string forceupdatemsg;
	std::string url;

	std::map<std::string/*�汾��*/, std::string/*�Ƿ�ǿ�Ƹ���*/> mapVersions;
};

typedef boost::detail::thread::singleton<VersionControl> gVersionControl;

#endif
