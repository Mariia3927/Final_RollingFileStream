#include "pch.h"
#include "Stream.h"


FileStream::FileStream(std::string path) : IStream()
{
	m_Fout.open(path.c_str(), std::ios::out | std::ios::binary);
	if (!m_Fout)
	{
		std::cout << "Error! The filestream object hasn't been created successfully!" << std::endl;
		m_Flag = false;
	}
}

FileStream::~FileStream()
{
	if (m_Fout)
	{
		m_Fout.close();
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

	m_Fout.write(buffer, blockSize);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IStream* FileSystem::Create(std::string path)
{
	FileStream* object = new FileStream(path);

	if (object->CreatedSuccessfully())
	{
		return object;
	}
	
	std::cout << "Error! The filestream object hasn't been created successfully!" << std::endl;
	return nullptr;
}

void FileSystem::Delete(IStream* object)
{
	delete object;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RollingFileStream::RollingFileStream(std::string fileName, int maxFileSize, int maxFilesNumber, IStreamFactory& factory) : m_FileName(fileName),
			m_MaxFileSize(maxFileSize), m_MaxFilesNumber(maxFilesNumber), m_Factory(factory)
{
}

bool RollingFileStream::Write(const char* buffer, int numberOfBytes)
{
	std::string currentFilePath = "";
	int currentFilesNumber = 0;
	int firstFileIndex = 0;
	int currentFileIndex = 1;
	bool lastFileNotFullSize = false;

	int bufferPosition = 0;

	int filesCount = numberOfBytes / m_MaxFileSize;
	if (numberOfBytes % m_MaxFileSize)
	{
		filesCount++;
		lastFileNotFullSize = true;
	}
	
	m_Stream = m_Factory.Create(m_FileName);
	if (!m_Stream)
	{
		std::cout << "Error! The file object hasn't been created successfully!" << std::endl;
		return false;
	}
	++currentFilesNumber;

	while (filesCount && currentFilesNumber <= m_MaxFilesNumber)
	{
		if (currentFilesNumber == m_MaxFilesNumber)
		{
			if (m_FirstFileIndex == 0)
			{
				currentFilePath = m_FileName;
			}
			else
			{
				currentFilePath = std::to_string(m_FirstFileIndex) + m_FileName;
			}
			
			std::remove(currentFilePath.c_str());
			
			++firstFileIndex;
		}

		if (!m_Stream)
		{
			currentFilePath = std::to_string(currentFileIndex) + m_FileName;
			m_Stream = m_Factory.Create(currentFilePath);
			if (!m_Stream)
			{
				std::cout << "Error! The file object hasn't been created successfully!" << std::endl;
				return false;
			}
			++currentFileIndex;

			if (currentFilesNumber < m_MaxFilesNumber)
			{
				++currentFilesNumber;
			}
		}
		
		if (filesCount > 1 || (filesCount == 1 && !lastFileNotFullSize))
		{
			m_Stream->Write(buffer + bufferPosition, m_MaxFileSize);
			bufferPosition += m_MaxFileSize;
		}
		else if(filesCount == 1 && lastFileNotFullSize)
		{
			m_Stream->Write(buffer + bufferPosition, numberOfBytes % m_MaxFileSize);
			bufferPosition += numberOfBytes % m_MaxFileSize;
		}
		

		m_Factory.Delete(m_Stream);
		m_Stream = nullptr;

		--filesCount;
	}

	return true;
}
