#pragma once

#include <string>
#define BOOST_ALL_DYN_LINK
#include <boost/log/trivial.hpp>
#pragma comment(lib, "boost_log-vc100-mt-1_56.lib")
#include "component_global.h"

using std::string;

#define LOG_DEBUG\
	BOOST_LOG_SEV((Log::s_slg),(boost::log::trivial::debug))
#define LOG_INFO\
	BOOST_LOG_SEV((Log::s_slg),(boost::log::trivial::info))
#define LOG_ERROR\
	BOOST_LOG_SEV((Log::s_slg),(boost::log::trivial::error))
#define LOG_WARNING\
	BOOST_LOG_SEV((Log::s_slg),(boost::log::trivial::warning))
// ��ʹ��֮ǰ�����ȵ��� init
// ʹ�÷�ʽ  LOG_DEBUG<<"test string";
// Ҳ������boost �еĺ�  BOOST_LOG_TRIVIAL(info)<<"test msg";
class COMPONENT_EXPORT Log
{
public:
	Log();
	~Log(void);

	// ��ʹ��֮ǰ�����ȵ��ô˺���
	static void init(const string & dir);
	static void log(const string & msg );
	static boost::log::sources::severity_logger<boost::log::trivial::severity_level > s_slg;
protected:
private:

};
