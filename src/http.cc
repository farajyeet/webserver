#include "http.h"


void Http::printDate(void)
{
    ostringstream date;

    char buf[50];
    time_t ltime;
    struct tm *today;
    ltime = time(NULL);
    today = gmtime(&ltime);
    

    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", today);

    date << "Date: " << buf << "\r\n";

    sock->writeLine(date.str());
}


void Http::printServer(void)
{
    sock->writeLine("Server: FARAJ/0.5 (Unix)\r\n");
}


void Http::printContentType(string type)
{
    ostringstream typebuffer;
    typebuffer << "Content-Type: " << type << "\r\n";
    sock->writeLine(typebuffer.str());
}


void Http::printContentLength(int size)
{ 
	assert(size >= 0);
	
    ostringstream clbuffer;
    clbuffer << "Content-Length: " << size << "\r\n";

    sock->writeLine(clbuffer.str());
}


void Http::printConnectionType(bool keep_alive)
{
    if (keep_alive)
        sock->writeLine("Connection: Keep-Alive\n");
    else
        sock->writeLine("Connection: close\n");
}



void Http::start(int server_port)
{	
	assert(server_port > 0 && server_port <= 443);
	
	bool keep_alive = false;
    int pid;
	sock = new Socket(server_port);


    while (1) {
	
        sock->acceptClient();

        pid = fork();
        if (pid < 0) {
            perror("Fork");
			exit(1);
		}


        if (pid == 0)  {
	
            keep_alive = parseHeader(getHeader());
			while (keep_alive) {
				keep_alive = parseHeader(getHeader());
			}
			
			sock->closeSocket();

            exit(0);
        }


        else {
			sock->closeSocket();
		}
    }

	delete sock;
}


string Http::sanitizeFilename(string filename)
{

    filename = filename.substr(1,filename.size()).c_str();
	

    if (filename == "")
        filename = "index.html";

	filename.insert(0, "htdocs/");


    std::string::size_type pos = filename.find('\n');
    if (pos != std::string::npos)
        filename.erase(pos);
	
	return filename;

}


void Http::sendFile(string filename)
{    

	ifstream file(filename.c_str(), ios::in|ios::binary);
	

	string file_extension = filename.substr(filename.rfind("."), filename.length());


	unsigned long size;
	
	file.seekg (0, ios::end);
	size = file.tellg();


	char *buffer;
	buffer = new char[size+1];
	if (buffer == NULL) {
		cerr << "error allocating buffer" << endl;
		file.close();
		return;
	}


	file.seekg(0, ios::beg);
	file.read(buffer, size);

	if (file.gcount() != size) {
		cerr << "error with read" << endl;
		file.close();	
		return;    
	}


	string filtered;
	if (file_extension == ".shtml" || file_extension == ".shtm") {
		buffer[size] = 0;

		string s_buffer(buffer);
		Filter filter;

		filtered = filter.addFooter(s_buffer);
		size = filtered.length();
	}


	if (file_extension == ".shtml" || file_extension == ".shtm") {
		if (send(sock->accept_fd, filtered.data(), size, 0) == -1) {
			perror("writeLine");

		}	
	} else {
		if (send(sock->accept_fd, buffer, size, 0) == -1) {
			perror("writeLine");

		}	
	}


	file.close();
	delete [] buffer;
}

bool Http::parseHeader(string header)
{
    string request_line;

    vector<string> tokens, tokentmp;
    map<string, string> headermap;

    bool keep_alive = false;

    unsigned int i;

	Token token;

    token.tokenize(header, tokens, "\n");   
  

    request_line = tokens[0];
    token.tokenize(tokens[0], tokentmp, " ");
    headermap[tokentmp[0]] = tokentmp[1];
    
    string name, value;

    for (i = 1; i < tokens.size(); i++) {
        std::string::size_type pos = tokens[i].find(':');
        if (pos != std::string::npos) {
            name  = tokens[i].substr(0, pos);
            value = tokens[i].substr(pos+2, tokens[i].length()-pos);

            headermap[name] = value; 
        }
    }


    if (DEBUG) {
        map<string, string>::iterator iter;
        for(iter = headermap.begin(); iter != headermap.end(); iter++) {
            cout << iter->first << " : " << iter->second << endl;
        } 
    }

    if (headermap["Connection"] == "keep-alive")
        keep_alive = true;
    else
        keep_alive = false;

    if (headermap.size() <= 0) {
		return false;
	}
	
	if (headermap.find("GET") != headermap.end()) {
		processGetRequest(headermap, request_line, keep_alive);	
	} else if (headermap.find("HEAD") != headermap.end()) { 
		processHeadRequest(headermap);
	} else if (headermap.find("POST") != headermap.end()) {
		processPostRequest(headermap);
	}
	
	return keep_alive;
}

void Http::processPostRequest(map<string, string> headermap)
{
	sock->writeLine("yeah right d00d\n");
}

void Http::processHeadRequest(map<string, string> headermap)
{
	sock->writeLine("now you will suffer the torment of kaam\n");	
}

void Http::processGetRequest(map<string, string> headermap, string request_line, bool keep_alive)
{
	string filename = headermap["GET"];
	string file_extension;

	filename = sanitizeFilename(filename);


	ifstream file(filename.c_str(), ios::in|ios::binary);
	

	file_extension = filename.substr(filename.rfind("."), filename.length());

	if (!file.is_open()) {
		sendHeader(404, 0, "text/html", false);
		sock->writeLine("<html><head><title>404</title></head><body>404 not found</body></html>");
		return;
	}


	unsigned long size;
	
	file.seekg (0, ios::end);
	size = file.tellg();
	
	if (file_extension == ".sh") {
		Cgi cgi;
		cgi.executeCGI(filename, sock->accept_fd, headermap);
		sock->closeSocket();
		return;
	}
	

	Log log;
	log.openLogFile("logs/access_log");
	log.writeLogLine(inet_ntoa(sock->client.sin_addr), 
					 request_line, 200, size, 
					 headermap["Referer"],
					 headermap["User-Agent"]);
	log.closeLogFile();
	

	Mime mime;
	mime.readMimeConfig("mime.types");
	

	sendHeader(200, size, mime.getMimeFromExtension(filename), keep_alive);
	sendFile(filename);
}


string Http::getHeader()
{
    string clientBuffer;


    while ( (sock->readLine(&clientBuffer)) ) {
        if ( strstr(clientBuffer.c_str(), "\n\n") != NULL)
            break;
    }

    return clientBuffer;
}


void Http::sendHeader(int code, int size, string file_type, bool keep_alive) 
{
	assert(code > 99 && code < 600);
	assert(size >= 0);
	
    switch (code) {
        case 200:
            sock->writeLine("HTTP/1.1 200 OK\r\n");
            break;
        case 404:
            sock->writeLine("HTTP/1.1 404 Not Found\r\n");
            break;
        default:
            cerr << "wrong http code" << endl;
    }

    printDate();
    printServer();

    if (size != 0)
        printContentLength(size);

    printConnectionType(keep_alive);
    printContentType(file_type);

    sock->writeLine("\n");
}
