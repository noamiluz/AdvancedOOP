/* nadavkareen 316602689; noamiluz 201307436 */
#ifndef __FILESINFRA_H 
#define __FILESINFRA_H 

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>

using namespace std;


/**
* FileParser class. Contains a proccecing, parsing
* and editing files names.
**/
class FileParser {

public:

	// receive a full path to a file and returns its base name
	string get_file_name(const string& full_path);

	// splits a string according to a delimiter. (from recitation)
	vector<string> split(const string &s, char delim);

	// cleans a string from unwanted whitespaces. (from recitation)
	string trim(string& str);

	// given a line read from the configuration file, update the configuration map. (from recitation)
	void processLine(const string& line, map<string, int> &config);
};

/**
* FilesLister class. An object contains a vector of files names.
**/
class FilesLister {

protected:
	vector<string> filesList_;
	string basePath_;

private:
	static string concatenateAbsolutePath(const string& dirPath, const string& fileName)
	{
		if (dirPath.empty())
			return fileName;
		if (dirPath.back() == '/')
			return dirPath + fileName;
		return dirPath + "/" + fileName;
	}

public:
	FilesLister(const string& basePath) : basePath_(basePath){
		this->refresh();
	}

	virtual void refresh();

	vector<string> getFilesList(){
		return this->filesList_;
	}
};


/**
* FilesListerWithSuffix class. An object contains a vector of files names,
* that ends with a given 'suffix'.
**/
class FilesListerWithSuffix : public FilesLister {
protected:
	void filterFiles();

	static inline bool endsWith(std::string value, std::string ending){
		if (ending.size() > value.size())
			return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	string suffix_;

public:
	FilesListerWithSuffix(const string& basePath, const string& suffix) : FilesLister(basePath), suffix_(suffix){
		this->filterFiles();
	}

	virtual void refresh() {
		FilesLister::refresh();
		this->filterFiles();
	}
};


/**
* HousesLister class. An object contains a vector of houses names,
* that ends with a '.house'.
**/
class HousesLister : public FilesListerWithSuffix {
public:
	HousesLister(const string& basePath) : FilesListerWithSuffix(basePath, ".house"){}
};


/**
* AlgorithmsLister class. An object contains a vector of algorithms names,
* that ends with a '.so'.
**/
class AlgorithmsLister : public FilesListerWithSuffix {
public:
	AlgorithmsLister(const string& basePath) : FilesListerWithSuffix(basePath, ".so"){}
};


/**
* ConfigLister class. An object contains a 'config.ini' file.
**/
class ConfigLister : public FilesListerWithSuffix {
public:
	ConfigLister(const string& basePath) : FilesListerWithSuffix(basePath, "config.ini"){}
};


#endif // __FILESINFRA_H
