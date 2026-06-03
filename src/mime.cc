#include "mime.h"

bool Mime::readMimeConfig(const string& filename)
{
	vector<string> tokens;

	ifstream file(filename.c_str());
	if (!file.is_open()) {
		cerr << "Error: can't open " << filename << endl;
		return false;
	}

	string tmp_line;
	while (getline(file, tmp_line)) {
		tokens.clear();

		if (!tmp_line.empty() && tmp_line[0] != '#') {


			string buf; 
			stringstream ss(tmp_line); 
			while (ss >> buf)
				tokens.push_back(buf);

			for (vector<string>::size_type j = tokens.size(); j > 1; j--) {
				this->mimemap[tokens[j-1]] = tokens[0];
			}
		}
	}

	file.close();
	return true;
}

string Mime::getMimeFromExtension(const string& filename) const
{
	string::size_type dot = filename.rfind(".");
	if (dot == string::npos || dot + 1 >= filename.length())
		return "text/plain";

	string file_extension = filename.substr(dot + 1);
	
	map<string, string>::const_iterator iter = this->mimemap.find(file_extension);
	if (iter == this->mimemap.end())
	    return "text/plain";
	else
		return iter->second;
}