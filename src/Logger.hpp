#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

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

void log(std::string const &msg);
void log(std::stringstream &ss);
void log(char const *msg);
void error(std::string const &msg);
void error(std::stringstream &ss);
void error(char const *msg);
void fatalError(std::string const &msg);
void fatalError(std::stringstream &ss);
void fatalError(char const *msg);
void warning(std::string const &msg);
void warning(std::stringstream &ss);
void warning(char const *msg);
void debug(std::string const &msg);
void debug(char const *msg);
void debug(std::stringstream &ss);

}

#endif /* LOGGER_HPP_ */
