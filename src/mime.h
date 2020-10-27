#ifndef SHELOB_MIME_H
#define SHELOB_MIME_H 1

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>

#include "global.h"

class Mime {
	public:
		bool readMimeConfig(string filename);
		string getMimeFromExtension(string filename);
	private:
		map<string, string> mimemap;

};

#endif 
