#pragma once
#ifndef GEAR_FILESTREAM
#define GEAR_FILESTREAM

#include <string>
#include <fstream>
#include <vector>

namespace Gear {
	enum class FileOpenMode {
		UNDEFINED,
		READ,
		WRITE,
	};

	enum class FileDataMode {
		UNDEFINED,
		TEXT,
		BINARY,
	};

	class FileStream {
	public:
		FileStream();
		FileStream(const std::string path, FileOpenMode openMode = FileOpenMode::READ, FileDataMode dataMode = FileDataMode::TEXT);

		std::string getPath() const;
		FileOpenMode getOpenMode() const;
		FileDataMode getDataMode() const;
		std::fstream& getHandle();

		bool isOpen() const;
		void reopen(const std::string path, FileOpenMode openMode = FileOpenMode::READ, FileDataMode dataMode = FileDataMode::TEXT);
		void close();

		size_t getLength();

		std::string readText();
		std::vector<unsigned char> readData();
		void writeText(const std::string text);
		void writeData(const std::vector<unsigned char>& data);
	private:
		std::string		m_path;
		FileOpenMode	m_openMode;
		FileDataMode	m_dataMode;
		std::fstream	m_handle;
	};
}

#endif