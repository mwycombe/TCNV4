#include "Logger.h"

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

