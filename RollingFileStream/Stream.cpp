#include "pch.h"
#include "Stream.h"


FileStream::FileStream(const std::string& path) : IStream()
{
	m_fout.open(path.c_str(), std::ios::out | std::ios::binary);
	if (!m_fout)
	{
		std::cout << "Error! The filestream object hasn't been created successfully!" << std::endl;
		m_flag = false;
	}
}

bool FileStream::Write(const char* buffer, int blockSize)
{
	if (!CreatedSuccessfully())
	{
		std::cout << "Error! The filestream object hasn't been created successfully!" << std::endl;
		return false;
	}
	if (blockSize < 0)
	{
		std::cout << "The number of bytes to writing cannot be less than zero!" << std::endl;
		return false;
	}

	m_fout.write(buffer, blockSize);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<IStream> FileSystem::Create(const std::string& path)
{
	std::unique_ptr<FileStream> object{ new FileStream(path) };

	if (object->CreatedSuccessfully())
	{
		return std::unique_ptr<IStream>(object.release());
	}
	
	std::cout << "Error! The filestream object hasn't been created successfully!" << std::endl;
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RollingFileStream::RollingFileStream(std::string fileName, int maxFileSize, int maxFilesNumber, IStreamFactory& factory) : m_fileName(fileName),
			m_maxFileSize(maxFileSize), m_maxFilesNumber(maxFilesNumber), m_factory(factory)
{
}

int RollingFileStream::CalculateNumberOfFiles(int numberOfBytes, bool& lastFileNotFullSize)
{
	int filesCount = numberOfBytes / m_maxFileSize;

	if (numberOfBytes % m_maxFileSize)
	{
		filesCount++;
		lastFileNotFullSize = true;
	}

	return filesCount;
}

std::unique_ptr<IStream> RollingFileStream::CreateFile(const std::string& path, int& currentFileIndex, int& currentFilesNumber)
{
	m_stream = m_factory.Create(path);
	if (!m_stream)
	{
		std::cout << "Error! The file object hasn't been created successfully!" << std::endl;
		return nullptr;
	}
	++currentFileIndex;

	if (currentFilesNumber < m_maxFilesNumber)
	{
		++currentFilesNumber;
	}

	return std::unique_ptr<IStream>(m_stream.release());
}

void RollingFileStream::RemoveFirstFile()
{
	std::string path;
	if (m_firstFileIndex == 0)
	{
		path = m_fileName;
	}
	else
	{
		path = std::to_string(m_firstFileIndex) + m_fileName;
	}

	std::remove(path.c_str());

	++m_firstFileIndex;
}

int RollingFileStream::Write(const char* buffer, int numberOfBytes)
{
	std::string currentFilePath = "";
	int currentFilesNumber = 0;
	int currentFileIndex = 0;
	bool lastFileNotFullSize = false;

	int bufferPosition = 0;

	int filesCount = CalculateNumberOfFiles(numberOfBytes, lastFileNotFullSize);
	
	m_stream = CreateFile(m_fileName, currentFileIndex, currentFilesNumber);

	while (filesCount && currentFilesNumber <= m_maxFilesNumber)
	{
		if (currentFilesNumber == m_maxFilesNumber)
		{
			RemoveFirstFile();
		}

		if (!m_stream)
		{
			currentFilePath = std::to_string(currentFileIndex) + m_fileName;
			m_stream = CreateFile(currentFilePath, currentFileIndex, currentFilesNumber);
		}
		
		if (filesCount > 1 || (filesCount == 1 && !lastFileNotFullSize))
		{
			m_stream->Write(buffer + bufferPosition, m_maxFileSize);
			bufferPosition += m_maxFileSize;
		}
		else if(filesCount == 1 && lastFileNotFullSize)
		{
			m_stream->Write(buffer + bufferPosition, numberOfBytes % m_maxFileSize);
			bufferPosition += numberOfBytes % m_maxFileSize;
		}
		
		m_stream.reset(nullptr);

		--filesCount;
	}

	return numberOfBytes;
}
