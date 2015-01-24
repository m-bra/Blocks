#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#ifndef LOG_THREAD
#define LOG_THREAD 1
#endif

//#define LOG_NO_DEBUG

#include <string>
#include <iostream>
#include <sstream>

namespace Log
{

#if LOG_THREAD
void setThisThreadName(std::string const &name);
#endif

namespace detail
{

enum LogType
{
	ERR, FATAL_ERR, WARNING, DEBUG, MSG
};

inline void putRecursive(std::stringstream &ss)
{
	; // end condition
}

template <typename T, typename... Args>
void putRecursive(std::stringstream &ss, T t, Args... args)
{
	ss << t;
	putRecursive(ss, args...);
}

template <typename... Args>
void _log(LogType logType, std::string const &func, std::string const &file, int line, Args... args)
{
	std::stringstream ss;
	if (sizeof...(args))
		putRecursive(ss, args...);

	void _log_str(LogType, std::string const&, std::string const&, int, std::string const&);
	_log_str(logType, func, file, line, ss.str());
}

}

}

#if defined(LOG_MSG) || defined(LOG_FATAL) || defined(LOG_ERR) || defined(LOG_WARNING) || defined(LOG_DEBUG)
#error One of LOG_MSG LOG_FATAL LOG_ERR LOG_WARNING LOG_DEBUG has already been defined.
#endif

#define LOG_MSG(args...)     Log::detail::_log(Log::detail::MSG,       __PRETTY_FUNCTION__, __FILE__, __LINE__, args)
#define LOG_FATAL(args...)   Log::detail::_log(Log::detail::FATAL_ERR, __PRETTY_FUNCTION__, __FILE__, __LINE__, args)
#define LOG_ERR(args...)     Log::detail::_log(Log::detail::ERR,       __PRETTY_FUNCTION__, __FILE__, __LINE__, args)
#define LOG_WARNING(args...) Log::detail::_log(Log::detail::WARNING,   __PRETTY_FUNCTION__, __FILE__, __LINE__, args)
#define LOG_DEBUG(args...)   Log::detail::_log(Log::detail::DEBUG,     __PRETTY_FUNCTION__, __FILE__, __LINE__, args)
#define LOG_REACHED          Log::detail::_log(Log::detail::MSG,       __PRETTY_FUNCTION__, __FILE__, __LINE__, "Reached line.")
#undef assert
#define assert(B) {if (!(B)) LOG_ERR("Assertion failed: " #B);}

#endif /* LOGGER_HPP_ */
