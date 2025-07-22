#include <filesystem>
#include <fstream>

#include <windows.h>

#include "log.h"

using namespace std;

static const string logDir = "C:/dinput-detour-log";

Logger *logger = nullptr;

Logger::Logger(const std::string &filename) { open_logfile(filename); }

void Logger::open_logfile(const std::string &path) {
	filesystem::create_directories(logDir);
	logfile.open(filesystem::path(logDir) / path);
}

void Logger::close_logfile() {
	if (logfile.is_open()) {
		logfile.close();
	}
}
