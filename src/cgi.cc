#include "cgi.h"


void Cgi::setupEnv(map<string, string> headermap) {
	
	if (headermap.find("AUTH_TYPE") != headermap.end()) {	
		setenv("AUTH_TYPE", headermap["AUTH_TYPE"].c_str(), 1);
	}
	
	if (headermap.find("CONTENT_LENGTH") != headermap.end()) {
		setenv("CONTENT_LENGTH", headermap["CONTENT_LENGTH"].c_str(), 1);
	}
	
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	
	if (headermap.find("QUERY_STRING") != headermap.end()) {
		setenv("QUERY_STRING", headermap["QUERY_STRING"].c_str(), 1);
	}
	
	setenv("REQUEST_METHOD", "GET", 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("SERVER_SOFTWARE", "SHELOB/3.14", 1);
	
}

bool Cgi::executeCGI(string filename, int accept_fd, map<string, string> headermap) {
    int pid;
	char current_path[MAXPATHLEN];
	ostringstream buffer;
	string fullpath;
	
    pid = fork();

    if (pid < 0) {
        perror("ERROR on fork");
	}


    if (pid == 0)  {
    	setupEnv(headermap);

		dup2(accept_fd, STDOUT_FILENO);
		
		filename = filename.substr(7);
		printf("HTTP/1.1 200 OK\r\n");
		
		getcwd(current_path, MAXPATHLEN);
		buffer << current_path << "/htdocs/" << filename;
		fullpath = buffer.str();
		execlp(fullpath.c_str(), filename.c_str(), 0);
		
		perror("CGI error");
		exit(1);
	}

}

