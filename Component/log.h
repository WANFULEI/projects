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
// 在使用之前必须先调用 init
// 使用方式  LOG_DEBUG<<"test string";
// 也可以用boost 中的宏  BOOST_LOG_TRIVIAL(info)<<"test msg";
class COMPONENT_EXPORT Log
{
public:
	Log();
	~Log(void);

	// 在使用之前必须先调用此函数
	static void init(const string & dir);
	static void log(const string & msg );
	static boost::log::sources::severity_logger<boost::log::trivial::severity_level > s_slg;
protected:
private:

};
