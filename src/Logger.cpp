#include "precompiled.hpp"

#include "Logger.hpp"

namespace Log
{

void _log(std::string const &msg, LogType logType)
{
#ifdef LOG_THREAD
	static std::mutex log_mutex;
	std::lock_guard<std::mutex> lockGuard(log_mutex);

	std::cout << "Thread " << std::this_thread::get_id() << ": ";
#endif

	switch (logType)
	{
	case ERROR:
		std::cout << "ERROR";
		break;
	case WARNING:
		std::cout << "WARNING";
		break;
	case DEBUG:
		std::cout << "DEBUG";
		break;
	case MESSAGE:
		std::cout << "MSG";
	}
	std::cout << ": " << msg << "\n";
}

void log(std::string const &msg)
{
	_log(msg, MESSAGE);
}

void log(std::stringstream &ss)
{
	std::string s; ss >> s;
	log(s);
}

void log(char const *msg)
{
	std::string s(msg);
	log(s);
}

void error(std::string const &msg)
{
	_log(msg, ERROR);
}

void error(std::stringstream &ss)
{
	std::string s; ss >> s;
	error(s);
}

void error(char const *msg)
{
	std::string s(msg);
	error(s);
}

void fatalError(std::string const &msg)
{
	_log(msg, ERROR);
	exit(EXIT_FAILURE);
}

void fatalError(std::stringstream &ss)
{
	std::string s; ss >> s;
	fatalError(s);
}

void fatalError(char const *msg)
{
	std::string s(msg);
	fatalError(s);
}

void warning(std::string const &msg)
{
	_log(msg, WARNING);
}

void warning(std::stringstream &ss)
{
	std::string s; ss >> s;
	warning(s);
}

void warning(char const *msg)
{
	std::string s(msg);
	warning(s);
}

void debug(std::string const &msg)
{
#ifndef LOG_NO_DEBUG
	_log(msg, DEBUG);
#endif
}

void debug(char const *msg)
{
	std::string s(msg);
	debug(s);
}

void debug(std::stringstream &ss)
{
	std::string s; ss >> s;
	debug(s);
}

}
