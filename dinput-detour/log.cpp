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

string guid_to_str(const GUID &guid) {
	return format("{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:"
	              "02x}{:02x}{:02x}",
	              guid.Data1, guid.Data2, guid.Data3, guid.Data4[0],
	              guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4],
	              guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}