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
	explicit FileStream(std::string path);
	~FileStream();

	bool Write(const char* buffer, int blockSize) override;

	bool CreatedSuccessfully() { return m_Flag; }

private:
	std::ofstream m_Fout;
	bool m_Flag = true;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class IStreamFactory 
{
public:
	virtual IStream* Create(std::string path) = 0;
	virtual void Delete(IStream* object) = 0;
};

class FileSystem : public IStreamFactory
{
public:
	IStream* Create(std::string path) override;
	void Delete(IStream* object) override;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class RollingFileStream
{
public:
	RollingFileStream(std::string fileName, int maxFileSize, int maxFilesNumber, IStreamFactory& factory);

	bool Write(const char* buffer, int numberOfBytes);

private:
	std::string m_FileName;
	int m_MaxFileSize;
	int m_MaxFilesNumber;
	IStreamFactory& m_Factory;
	IStream* m_Stream;
	int m_FirstFileIndex = 0;
};

