#ifndef HEX_EDITOR_CLASS
#define HEX_EDITOR_CLASS

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include <fstream>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <filesystem>

#include "ColorFormatter.hpp"
#include "Helper.hpp"

class HexEditor
{
public:
	inline HexEditor(void) noexcept {

	}

	inline explicit HexEditor(const char* path) noexcept {
		openFile(path);
	}
	
	inline explicit HexEditor(const std::string& path) noexcept {
		openFile(path.c_str());
	}

	template<typename T>
	__forceinline auto openFile(T path) noexcept -> void {
		openAndSetup(path);
	}

	~HexEditor() {
		file.close();
		
		if(this->data)
			delete[] this->data;
	}

	auto openAndSetup(const char* newPath) noexcept -> void {
		file.close();

		delete[] this->data;
		data = nullptr;

		file.open(newPath, std::ios::binary);

		file.seekg(0, file.end);
		fileLength = file.tellg();
		file.seekg(0, file.beg);

		this->path = newPath;
		this->fileName = fileName = std::filesystem::path(this->path).filename().string();
	}

	auto changeFile(const char* newPath) noexcept -> void {
		file.close();
	
		openAndSetup(newPath);

		delete[] this->data;
		data = nullptr;	
	}

	auto read(const std::size_t start = 0, const int len = -1) noexcept -> void {
		if (data)
			return;

		file.seekg(0, file.end);
		fileLength = file.tellg();
		file.seekg(0, file.beg);

		std::size_t totalLength = len + start;
		int readLength = len;

		if (len > static_cast<int>(fileLength))
			return;

		if (len == -1) {
			totalLength = fileLength;
			readLength = fileLength;
		}
		else {
			if (start > totalLength) {
				totalLength = fileLength;
			}
		}

		this->start = start;
		this->readLength = readLength;
		
		this->data = new int8_t[totalLength];
		file.read(reinterpret_cast<char*>(data), totalLength);
	}

	//auto print(const std::size_t start = 0, const int len = -1) noexcept -> void {
	auto print(bool dump = false) noexcept -> void {
		if (!this->data)
			return;

		char* filename = new char[MAX_PATH];
		GetModuleFileName(NULL, filename, MAX_PATH);

		const std::string currentPath_(filename);
		const std::size_t last = currentPath_.rfind("\\");
		const std::string currentPath = currentPath_.substr(0, last);

		delete[] filename;

		std::ofstream dumpFile;

		if (dump) {
			std::string fileName = std::filesystem::path(this->path).filename().string();
			
			std::string dumpPath = currentPath + "\\dumps\\Dumped " + fileName + ".txt";
			if (std::filesystem::exists(dumpPath))
				return;

			dumpFile.open(dumpPath);
		}
			
		int& start = this->start;
		int& readLength = this->readLength;

		DWORD address = this->start;

		for (int i = start; i < start + readLength; i += width, address += width) {
			std::stringstream sstream;
			sstream << std::hex << address;

			const std::string addressStr = functions::toUpper(sstream.str());

			if(dump) {
				dumpFile << functions::formatAddress(addressStr) << "\t";
			}
			else {
				console::colorset(console::colors["light_blue"]);
				printf("%s\t", functions::formatAddress(addressStr).c_str());
			}

			std::string text;
			text.reserve(width);

			for (std::size_t j = 0; j < width; ++j) {
				const uint8_t hexByte = static_cast<uint8_t>(this->data[i + j]);

				// Bytes
				if (dump) {
					char* buffer = new char[5];			
					
					snprintf(buffer, 4, hexByte <= 0x0F ? "0%X " : "%X ", hexByte);
					dumpFile << buffer;

					delete[] buffer;
				}
				else {
					console::colorset(console::colors[j & 1 ? "light_gray" : "dark_gray"]);
					printf(hexByte <= 0x0F ? "0%X " : "%X ", hexByte);
				}

				// Text
				text += (hexByte <= 0x20 || hexByte == 0xFF) ? '.' : static_cast<char>(hexByte);
			}

			if (dump) {
				dumpFile << "\t" << text << "\n";
			}
			else {
				console::colorset(console::colors["yellow"]);
				printf("\t%s\n", text.c_str());
			}
		}

		if (dump)
			dumpFile.close();
	}

	_NODISCARD __forceinline auto length(void) noexcept -> std::size_t {
		return this->fileLength;
	}

	_NODISCARD __forceinline auto getPath(void) noexcept -> std::string {
		return this->path;
	}

	_NODISCARD __forceinline auto getFilename(void) noexcept -> std::string {
		return this->fileName;
	}
private:
	int8_t* data = nullptr;
	std::ifstream file;

	int fileLength = 0;
	int start = 0;
	int readLength = 0;

	std::string path = "";
	std::string fileName = "";

	static inline constexpr const int width = 16;
};

#endif