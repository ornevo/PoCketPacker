/**
 *
 */

#include <iostream>
#include "Logger.h"


using namespace std;


unsigned char Logger::logLevelMask = LOG_LEVEL_ERROR | LOG_LEVEL_INFO | LOG_LEVEL_DEBUG;

void Logger::log(string logmsg, unsigned char logLevel) {
    // If should log according to mask, log
    if(Logger::logLevelMask & logLevel) {
        // First log the prefix of the log type.
        switch (logLevel) {
            case LOG_LEVEL_ERROR:
                cout << LOG_PREFIX_ERROR;
            break;
            case LOG_LEVEL_INFO:
                cout << LOG_PREFIX_INFO;
            break;
            default:
            case LOG_LEVEL_DEBUG:
                cout << LOG_PREFIX_DEBUG;
            break;
        }

        // Print the message
        cout << logmsg << endl;
    }
}

void Logger::throwErr(string errmsg) {
    cerr << LOG_PREFIX_ERROR << errmsg << "\n\nExit status: 1." << endl;
    exit(1);
}
