/*
 * Logger.hpp
 *
 *  Created on: Nov 1, 2014
 *      Author: merlin
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#ifndef LOG_THREAD
#define LOG_THREAD 1
#endif

//#define LOG_NO_DEBUG

#include <string>
#include <iostream>
#include <sstream>

#if LOG_THREAD
#include <thread>
#include <mutex>
#endif

namespace Log
{

enum LogType
{
	ERROR, WARNING, DEBUG, MESSAGE
};

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

void error(std::string const &msg)
{
	_log(msg, ERROR);
}

void error(std::stringstream &ss)
{
	std::string s; ss >> s;
	error(s);
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

void debug(std::string const &msg)
{
#ifndef LOG_NO_DEBUG
	_log(msg, DEBUG);
#endif
}

void debug(std::stringstream &ss)
{
	std::string s; ss >> s;
	debug(s);
}

}

#endif /* LOGGER_HPP_ */
