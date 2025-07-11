#pragma once

#include <format>
#include <fstream>

typedef struct _GUID GUID;

#define LOG_PRE(fmt, ...)                                                      \
	logger->write("{} [pre ]: " fmt, __func__, __VA_ARGS__)
#define LOG_POST(fmt, ...)                                                     \
	logger->write("{} [post]: " fmt, __func__, __VA_ARGS__)
#define LOG_ERR(fmt, ...)                                                      \
	logger->write("{} [err ]: " fmt, __func__, __VA_ARGS__)
#define LOG_WARN(fmt, ...)                                                     \
	logger->write("{} [warn]: " fmt, __func__, __VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                     \
	logger->write("{} [info]: " fmt, __func__, __VA_ARGS__)
#define LOG(fmt, ...) logger->write(fmt, __VA_ARGS__)

class Logger;
extern Logger *logger;

std::string guid_to_str(const GUID &guid);

class Logger {
  public:
	Logger(const std::string &filename);

	template <typename... Args>
	void write(const std::format_string<Args...> fmt, Args &&...args) {
		logfile << format(fmt, std::forward<Args>(args)...);
		logfile.flush();
	}

	template <typename... Args>
	void write(const std::wformat_string<Args...> fmt, Args &&...args) {
		logfile << format(fmt, std::forward<Args>(args)...);
		logfile.flush();
	}

  private:
	std::ofstream logfile;

	void open_logfile(const std::string &path);
	void close_logfile();
};