#include "Gear-FileStream.hxx"

Gear::FileStream::FileStream() {
	m_path = "";
	m_openMode = FileOpenMode::UNDEFINED;
	m_dataMode = FileDataMode::UNDEFINED;
}

Gear::FileStream::FileStream(const std::string path, FileOpenMode openMode, FileDataMode dataMode) {
	this->reopen(path, openMode, dataMode);
}

std::string Gear::FileStream::getPath() const {
	return m_path;
}

Gear::FileOpenMode Gear::FileStream::getOpenMode() const {
	return m_openMode;
}

Gear::FileDataMode Gear::FileStream::getDataMode() const {
	return m_dataMode;
}

std::fstream& Gear::FileStream::getHandle() {
	return m_handle;
}

bool Gear::FileStream::isOpen() const {
	return m_handle.is_open();
}

void Gear::FileStream::reopen(const std::string path, FileOpenMode openMode, FileDataMode dataMode) {
	if (m_handle.is_open()) m_handle.close();

	std::ios_base::openmode mode = (std::ios_base::openmode)(openMode == FileOpenMode::WRITE ? std::ios_base::out : std::ios_base::in);
	if (dataMode == FileDataMode::BINARY) mode |= (std::ios_base::openmode)std::ios_base::binary;

	m_path = path;
	m_openMode = openMode;
	m_dataMode = dataMode;
	m_handle = std::fstream(path, mode);

	if (m_dataMode == FileDataMode::BINARY) m_handle >> std::noskipws;
}

void Gear::FileStream::close() {
	m_handle.close();
}

size_t Gear::FileStream::getLength() {
	size_t length = 0;

	if (m_openMode == FileOpenMode::READ) {
		m_handle.seekg(0, std::ios::end);
		length = (size_t)m_handle.tellg();
		m_handle.seekg(0, std::ios::beg);
	}
	else if (m_openMode == FileOpenMode::WRITE) length = (size_t)m_handle.tellp();

	return length;
}

std::string Gear::FileStream::readText() {
	std::string text;
	text.reserve(this->getLength());
	text.assign((std::istreambuf_iterator<char>(m_handle)), std::istreambuf_iterator<char>());
	return text;
}

std::vector<unsigned char> Gear::FileStream::readData() {
	std::vector<unsigned char> data((std::istream_iterator<unsigned char>(m_handle)), (std::istream_iterator<unsigned char>()));
	return data;
}

void Gear::FileStream::writeText(const std::string text) {
	m_handle << text;
}

void Gear::FileStream::writeData(const std::vector<unsigned char>& data) {
	std::copy(data.cbegin(), data.cend(), std::ostreambuf_iterator<char>(m_handle));
}