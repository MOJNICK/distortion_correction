#ifndef Logger_hpp
#define Logger_hpp
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <mutex>

extern std::ofstream logStream;
extern std::mutex loggingLock;

class Logger
{
protected:
	inline void logRecursive() const;

	template<class Data, class ...Args>
    void logRecursive(Data data, const Args&... args) const;

    inline void logTime() const;

public:
	template<class ...Args>
	void logError(const Args&... args) const;

	template<class ...Args>
	void logInfo(const Args&... args) const;

	template<class ...Args>
	void logDebug(const Args&... args) const;
};

class ErrorLogger : public Logger
{
public:
	template<class ...Args>
    void logError(const Args&... args) const;
};

class InfoLogger : public ErrorLogger
{
public:
	template<class ...Args>
	void logInfo(const Args&... args) const;
};

class DebugLogger : public InfoLogger
{
public:
	template<class ...Args>
	void logDebug(const Args&... args) const;
};


template<class ...Args>
void Logger::logError(const Args&... args) const {}

template<class ...Args>
void Logger::logInfo(const Args&... args) const {}

template<class ...Args>
void Logger::logDebug(const Args&... args) const {}

template<class ...Args>
void ErrorLogger::logError(const Args&... args) const
{
    std::lock_guard<std::mutex> lock(loggingLock);
    logTime();
    logRecursive("Error: ", args...);
}

template<class ...Args>
void InfoLogger::logInfo(const Args&... args) const
{
    std::lock_guard<std::mutex> lock(loggingLock);
    logTime();
    logRecursive("Info: ", args...);
}

template<class ...Args>
void DebugLogger::logDebug(const Args&... args) const
{
    std::lock_guard<std::mutex> lock(loggingLock);
    logTime();
    logRecursive("Debug: ", args...);
}


void Logger::logRecursive() const
{
    logStream << std::endl;
}

template<class Data, class ...Args>
void Logger::logRecursive(Data data, const Args&... args) const
{
    logStream << data << " ";
    logRecursive(args...);
}

void Logger::logTime() const
{
    static auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;

    logStream << diff.count() << " ";
}


extern DebugLogger logger;

#endif
