// RollingFileStream.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int main()
{
	FileSystem factory;
	std::string path = "Test.txt";
	RollingFileStream rfs(path, 300, 5, factory);

	std::ifstream fin("SourceFile.txt", std::ios::in | std::ios::binary);
	if (!fin)
	{
		std::cout << "Error! The input file is incorrect!" << std::endl;
		return 0;
	}

	const int numOfBytes = 3150;
	char buffer[numOfBytes];
	fin.read(buffer, numOfBytes);
	fin.close();

	rfs.Write(buffer, numOfBytes);

	return 0;
}
