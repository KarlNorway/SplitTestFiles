// SplitTestFiles.cpp : main project file.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>
#include <algorithm>

uint32_t endline;


std::string fixFileName(std::string &fileName, int partNo) {

	std::string newFile;
	int len = fileName.length();
	std::string fileType = fileName.substr(len - 4);
	fileName.erase(len - 4);
	std::string n = std::to_string(partNo);
	newFile = fileName + "_split_" + n + fileType;
	return newFile;

}
void writeArray( std::fstream& arrFile, int partOfFile, int startLine, std::string fromFile, int16_t numlines)
{
	//std::fstream fIn;
	arrFile.seekg(0, std::ios::beg);
	std::fstream fOut;
	std::string saveFileName = fixFileName(fromFile, partOfFile);
	//fIn.open(fromFile);
	fOut.open(saveFileName, std::ios::app);
	std::string placeHolder;
	for (int f = 0; f < startLine; f++)
	{
		getline(arrFile, placeHolder);
	}
	for (int r = 0 ; r < numlines + 1 ; r++)

	{
		getline(arrFile, placeHolder);
		fOut << placeHolder << std::endl;
		//std::cout << r << " "<<  placeHolder << std::endl;
		std::cout << r << "\r";
	}

	fOut.close();
}

int32_t findLine(std::fstream& fileLines, double detNo, int32_t startpos)
{
	uint32_t lineNo = 0;
	std::string s;
		
	for (int p = 0; p < startpos; p++)
	{
		getline(fileLines, s);
		++lineNo;
	}
	//std::cout << lineNo << std::endl;
	while (getline(fileLines, s))
	{
		std::string::size_type pos = s.find('\t');
		std::string comp = s.substr(pos, '\t');
		comp = comp.substr(0, ',');
		std::replace(comp.begin(), comp.end(), ',', '.');
		double intComp = std::atof(comp.c_str());
		//std::cout << intComp << "\r" ;
		if ((detNo -0.4) < intComp && intComp < (detNo+0.5)) 
		{
			break;
		}
		++lineNo;
		//std::cout << "Lineno: "<< lineNo << " intComp: " << intComp << " detNo: " << detNo << std::endl;
	}
	

	//std::cout << "Found: " << detNo << " @ line " << lineNo << std::endl;
	return lineNo;
}
int detectSampleTime(std::string &fileName)  // function to detect time scale of log file
{

	int timing;
	std::fstream fIn;
	fIn.open(fileName, std::ios::in);
	std::string s1, s2;
	for (int g = 0; g < 200; g++)
	{
		getline(fIn, s1);
		//std::cout << g << std::endl;
	}
	getline(fIn, s1);
	getline(fIn, s2);

	s1 = s1.substr(0, '\t');
	s2 = s2.substr(0, '\t');
	std::replace(s1.begin(), s1.end(), ',', '.');
	std::replace(s2.begin(), s2.end(), ',', '.');

	//std::cout << "S1: " << s1 << " S2: " << s2 << std::endl;

	double ts1 = std::stod(s1);
	double ts2 = std::stod(s2);
	double diff = ts2 - ts1;
	//std::cout << "Timestamp 1: " << ts1 << " Timestamp 2: " << ts2 << " Diff: " << diff << std::endl;
	if (diff > 0.15)
	{
		timing = 5;
	}
	else if (diff > 0.07)
	{
		timing = 10;
	}
	else {
		timing = 20;
	}
	std::cout << "Time difference is set to: " << timing << std::endl;
	return timing;
}

int arraySize(int timeDiff, std::fstream& fName, int32_t startingPos, std::string fromFile, int filePart) { //calculates the total numbers of line to save as file part

	uint32_t lineNo = findLine(fName, -3, startingPos);
	uint32_t startLineNo = lineNo - (120 * timeDiff);
	uint32_t endLineNo = lineNo + ( 440 * timeDiff);

	uint32_t numberOfLines = endLineNo - startLineNo ;
	endline = endLineNo;
	std::cout << "Lines: " << lineNo << " " << startLineNo << " " << endLineNo << std::endl;
	std::cout<< " Total number of lines to be stores in new file: " << numberOfLines << std::endl;
	writeArray(fName, filePart, startLineNo, fromFile, numberOfLines);
	return numberOfLines;
}

int32_t makeFilePartArray(std::string fileFrom, int timeDiff, int partNo, int32_t line) {
	

	std::fstream fIn;
	fIn.open(fileFrom);
	int arSz = arraySize(timeDiff, fIn, line, fileFrom, partNo);
	fIn.close();
	return arSz;
}


//int main(int argc, char* argv[])
int main()
{
	clock_t t;
	t = clock();
	std::fstream fIn; 
	std::string fileName, saveFileName;
		// Check the number of parameters
	//if (argc < 2) {
	//	 Tell the user how to run the program
	//	std::cerr << "Usage: " << argv[0] << " File path and name" << std::endl;
	//	/* "Usage messages" are a conventional way of telling the user
	//	* how to run a program if they enter the command incorrectly.
	//	*/
	//	return 1;

	//}
	//std::string current_exec_name = argv[0]; // Name of the current exec program
	//std::vector<std::string> all_args;

	//if (argc > 1) {

	//	fileName = argv[1];

	//	all_args.assign(argv + 1, argv + argc);
	//}
	//std::cout << argv[0];
	//std::cout << " is trying to open: " << fileName << std::endl;
	fileName = "C:\\Users\\karlk\\Documents\\General.Testing\\GK test data\\Cellpreformance\\LOGS\\18650-MJ1-20171026_edited.csv";
	fIn.open(fileName, std::ios::in);
	int partNo = 1;
	int32_t lines;

	if (fIn.is_open())
	{
		int32_t lineNo = 25;
		int time = detectSampleTime(fileName);
		for (t = 0; t < 11; t++) {
			lines = makeFilePartArray(fileName, time, partNo, lineNo);
			partNo++;
			//std::cout << "file part number: " << partNo << std::endl;
			lineNo = endline + (600*time);
		}
		fIn.close();
	}
	else {
		std::cout << "Error opening file " << errno << " " << std::endl;
	}
	t = clock() - t;
	std::cout << "Minutes: " << (t*1.0 / CLOCKS_PER_SEC) / 60 << std::endl;
}
