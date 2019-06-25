#pragma once
#include "pch.h"



class IStream
{
public:
	IStream() {}
	virtual ~IStream() {}
	virtual bool Write(const char* buffer, int blockSize) = 0;
};

class FileStream : public IStream
{
public:
	explicit FileStream(const std::string& path);

	bool Write(const char* buffer, int blockSize) override;

	bool CreatedSuccessfully() { return m_flag; }

private:
	std::ofstream m_fout;
	bool m_flag = true;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class IStreamFactory 
{
public:
	virtual std::unique_ptr<IStream> Create(const std::string& path) = 0;
};

class FileSystem : public IStreamFactory
{
public:
	std::unique_ptr<IStream> Create(const std::string& path) override;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RollingFileStream
{
public:
	RollingFileStream(std::string fileName, int maxFileSize, int maxFilesNumber, IStreamFactory& factory);

	int Write(const char* buffer, int numberOfBytes);

private:
	int CalculateNumberOfFiles(int numberOfBytes, bool& lastFileNotFullSize);
	std::unique_ptr<IStream> CreateFile(const std::string& path, int& currentFileIndex, int& currentFilesNumber);
	void RemoveFirstFile();

private:
	std::string m_fileName;
	int m_maxFileSize;
	int m_maxFilesNumber;
	IStreamFactory& m_factory;
	std::unique_ptr<IStream> m_stream;
	int m_firstFileIndex = 0;
};

