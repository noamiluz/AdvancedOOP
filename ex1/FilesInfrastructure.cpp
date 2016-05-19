/* nadavkareen 316602689; noamiluz 201307436 */
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>


#ifdef __gnu_linux__
#include <dlfcn.h> 
#include <dirent.h>
#endif

#include "FilesInfrastructure.h"

#ifdef __gnu_linux__
/**
* Function that updates the files list vector,
* with the full paths of the files.
**/
void FilesLister::refresh() {
	DIR *dir;
	struct dirent *ent;
	this->filesList_.clear();
	if ((dir = opendir(this->basePath_.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			this->filesList_.push_back(concatenateAbsolutePath(this->basePath_, ent->d_name));
		}
		closedir(dir);
	}
	else {
		return;
	}
	sort(this->filesList_.begin(), this->filesList_.end());
}


/**
* Function that saves the files name who
* end with the given suffix 'suffix'.
**/
void FilesListerWithSuffix::filterFiles() {
	vector<string> temp = this->filesList_;
	this->filesList_.clear();
	for (vector<string>::iterator itr = temp.begin(); itr != temp.end(); ++itr){
		if (endsWith(*itr, this->suffix_) && !endsWith(*itr, this->_not_equal_to))
		{
			this->filesList_.push_back(*itr);
		}
	}
}

#endif

/**
* Function reciving  a full path to a file and returns its base name.
**/
string FileParser::get_file_name(const string& full_path){
	return string(find_if(full_path.rbegin(), full_path.rend(),
		[](const char c){return c == '/'; }).base(), full_path.end());
}

/**
* Function that splits a string according to a delimiter. (from recitation)
**/
vector<string> FileParser::split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

/**
* Function that cleans a string from unwanted whitespaces. (from recitation)
**/
string FileParser::trim(string& str) {
	str.erase(0, str.find_first_not_of(' '));// prefixing spaces
	str.erase(str.find_last_not_of(' ') + 1);// suffixing spaces
	return str;
}

/**
* Function that given a line read from the configuration file, update the configuration map. (from recitation)
**/
void FileParser::processLine(const string& line, map<string, int> &config, string& temp_invalid, int& count_invalid)
{
	vector<string> tokens = split(line, '=');
	if (tokens.size() != 2)
	{
		return;
	}
	string key = trim(tokens[0]);
	string value = trim(tokens[1]);
	if (key.compare("MaxStepsAfterWinner") && key.compare("BatteryCapacity") && key.compare("BatteryConsumptionRate") && key.compare("BatteryRechargeRate")){
		return;
	}
	if (is_positive_num(value)){ // value is numeric
		config[key] = atoi(value.c_str());
	}
	else {
		count_invalid++;
		temp_invalid += key + ", ";
		config[key] = 0;
	}
	
}

// returns true if s represents a positive number
bool FileParser::is_positive_num(string s){
	if (s.empty()){
		return false;
	}
	char *p;
	if (!s.compare("0")){
		return true;
	}

	long converted = strtol(s.c_str(), &p, 10);
	if ((converted > 0 && !strcmp(p, "")) || (converted > 0 && !strcmp(p, "\r"))){
		return true;
	}
	return false;

}
