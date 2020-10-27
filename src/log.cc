#include "log.h"

bool Log::openLogFile(string filename) 
{
    if (logfile.is_open()) {
        if (DEBUG)
            cout << "Log file already open!\n";
        return true;
    }

    logfile.open(filename.c_str(), ios::out | ios::app);
    if (logfile.is_open()) { 
        if (DEBUG) {
            cout << "Opened log file\n";
        }

        return true;

    } else {
        cerr << "error: Unable to open log file (" << filename << ")" << endl;
        return false;
    }
}


bool Log::closeLogFile() 
{
    if (!logfile.is_open()) {
        logfile.close(); 
        return true;
    } else
        return false;
}



string Log::makeDate()
{
    ostringstream date;
    ostringstream cdate;

    char buf[50];
    time_t ltime;
    struct tm *today;
    ltime = time(NULL);
    today = gmtime(&ltime);

    strftime(buf, sizeof(buf), "[%d/%b/%Y:%H:%M:%S %z]", today);
    
    string buffer(buf);

    return buffer;
}





bool Log::writeLogLine(string ip, string request, int code, int size, string referrer, string agent) 
{
    if (logfile.is_open()) {
        logfile << ip << " - - " << " " << this->makeDate() << " \"" << request << "\"" << " " << code 
                << " " << size << " \"" << referrer << "\" \"" << agent << "\"\n";
        return true;
    } else {
        cerr << "Unable to write to logfile\n";
        return false;
    }
}
