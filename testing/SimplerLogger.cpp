#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    Logger(const std::string& filename) : log_file_(filename, std::ios_base::app) {
        if (!log_file_.is_open()) {
            std::cerr << "Error: Unable to open log file " << filename << std::endl;
        }
    }

    ~Logger() {
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

    void log(LogLevel level, const std::string& message) {
        if (!log_file_.is_open()) {
            return; // Cannot log if file not open
        }

        std::string level_str;
        switch (level) {
            case DEBUG:   level_str = "DEBUG"; break;
            case INFO:    level_str = "INFO"; break;
            case WARNING: level_str = "WARNING"; break;
            case ERROR:   level_str = "ERROR"; break;
        }

        auto now = std::chrono::system_clock::now();
        std::time_t current_time = std::chrono::system_clock::to_time_t(now);
        log_file_ << "[" << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S") << "][" << level_str << "] " << message << std::endl;
    }

private:
    std::ofstream log_file_;
};

int main() {
    Logger my_logger("my_app.log");
    my_logger.log(Logger::INFO, "Application initialized.");
    my_logger.log(Logger::DEBUG, "Detailed debug information.");
    my_logger.log(Logger::WARNING, "Potential issue detected.");
    my_logger.log(Logger::ERROR, "Critical error occurred!");
    return 0;
}