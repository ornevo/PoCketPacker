/**
 *
 */

#ifndef FINALPACKERTRY_LOGGER_H
#define FINALPACKERTRY_LOGGER_H

#include <string>

#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_DEBUG 4

#define LOG_PREFIX_ERROR "[ERROR] "
#define LOG_PREFIX_INFO  "[INFO]  "
#define LOG_PREFIX_DEBUG "[DEBUG] "


using namespace std;


class Logger {
public:
    // To decide which messages to print and which not, we hold a mask.
    // The message's log level get's ANDed with the mask, and if != 0 gets printed.
    static unsigned char logLevelMask;

    /**
     * Logs a message to the screen if the mask enables it to.
     * @param logmsg is the message to be printed
     * @param logLevel is the log level (info / error / debug)
     */
    static void log(string logmsg, unsigned char logLevel=LOG_LEVEL_INFO);

    /**
     * Prints an error to stderr, and exits with status code 1.
     * @param errmsg the error message
     */
    static void throwErr(string errmsg);
};


#endif //FINALPACKERTRY_LOGGER_H
