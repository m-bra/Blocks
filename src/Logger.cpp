#include "precompiled.hpp"

#include "Logger.hpp"

#if LOG_THREAD
#include <thread>
#include <mutex>
#include <algorithm>
#endif

namespace Log
{


#if LOG_THREAD
std::map<std::thread::id, std::string> threadIds;

void setThisThreadName(std::string const &name)
{
	// um yes, this function's gotta be thread safe!
	static std::mutex threadIds_mutex;
	std::lock_guard<std::mutex> lockGuard(threadIds_mutex);

	threadIds[std::this_thread::get_id()] = name;
}
#endif

namespace detail {

void _log_str(LogType logType, std::string const &func, std::string const &file, int line, std::string const &msg)
{
	// return if debug not wished
#ifdef LOG_NO_DEBUG
	if (logType == DEBUG)
		return;
#endif

	// print nice stuff
	std::cout << ">>>> ";

	// print location
	std::cout << func;// << ", " << file << ":" << line;

	// print thread
#if LOG_THREAD
	static std::mutex log_mutex;
	std::lock_guard<std::mutex> lockGuard(log_mutex);

	std::cout << ", " << "Thread ";

	auto thisThreadId = threadIds.find(std::this_thread::get_id());

	if (thisThreadId == threadIds.end())
		std::cout << std::hex << std::uppercase << std::this_thread::get_id() << std::nouppercase << std::dec;
	else
		std::cout << '"' << (*thisThreadId).second << '"';
#endif

	std::cout << "\n";

	switch (logType)
	{
	case ERR:
		std::cout << "ERROR: ";
		break;
	case FATAL_ERR:
		std::cout << "FATAL ERROR: ";
		break;
	case WARNING:
		std::cout << "WARNING: ";
		break;
	case DEBUG:
		std::cout << "DEBUG: ";
		break;
	case MSG:
		std::cout << "MSG: ";
	}
	std::cout << msg << "\n";

	// print nice stuff
	std::cout << "\n\n";

	if (logType == FATAL_ERR)
		exit(EXIT_FAILURE);
}

} }
