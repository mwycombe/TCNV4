#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <time.h>

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    Logger(LogLevel minLevel = INFO, const std::string& filename = "") :
        m_minLevel(minLevel) {
        if (!filename.empty()) {
            m_outputFile.open(filename, std::ios::app);
            if (!m_outputFile.is_open()) {
                std::cerr << "Error: Could not open log file: " << filename << std::endl;
            }
        }
    }

    ~Logger() {
        if (m_outputFile.is_open()) {
            m_outputFile.close();
        }
    }

    void setMinLevel(LogLevel level) {
        m_minLevel = level;
    }

    void log(LogLevel level, const std::string& message) {
        if (level >= m_minLevel) {
            std::string levelStr;
            switch (level) {
                case DEBUG: levelStr = "DEBUG"; break;
                case INFO: levelStr = "INFO"; break;
                case WARNING: levelStr = "WARNING"; break;
                case ERROR: levelStr = "ERROR"; break;
                case CRITICAL: levelStr = "CRITICAL"; break;
            }

            // Get current time
            // auto now = std::chrono::system_clock::now();
            // auto time_t_now = std::chrono::system_clock::to_time_t(now);
            // std::tm * tm_struct = time_t_now;
            // localtime(&time_t_now, &tm_struct); // For GCC/Clang
            time_t now = time(0);
            std::tm * tm_struct = localtime(&now);
            std::stringstream ss;
            ss << "[" << std::put_time(tm_struct, "%Y-%m-%d %H:%M:%S") << "] ";
            ss << "[" << levelStr << "] " << message << std::endl;

            // Output to console
            std::cout << ss.str();

            // Output to file if open
            if (m_outputFile.is_open()) {
                m_outputFile << ss.str();
            }
        }
    }

private:
    LogLevel m_minLevel;
    std::ofstream m_outputFile;
};

int main() {
    Logger logger(INFO, "application.log"); // Log INFO and above to console and file

    logger.log(DEBUG, "This is a debug message."); // Will not be logged
    logger.log(INFO, "Application started successfully.");
    logger.log(WARNING, "Potential issue detected.");
    logger.log(ERROR, "An error occurred during processing.");

    logger.setMinLevel(DEBUG); // Change minimum level to DEBUG
    logger.log(DEBUG, "This debug message will now be logged.");

    return 0;
}