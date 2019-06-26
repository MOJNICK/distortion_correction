#include "Logger.hpp"

std::ofstream logStream("log.txt");
std::mutex loggingLock;
DebugLogger logger;
