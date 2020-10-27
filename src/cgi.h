#ifndef SHELOB_CGI_H
#define SHELOB_CGI_H 1

#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>

#include <map>
#include <string>
#include <sstream>

#include "global.h"

class Cgi {
	public:
		void setupEnv(map<string, string> headermap);
		bool executeCGI(string filename, int accept_fd, map<string, string> headermap);
};

#endif
