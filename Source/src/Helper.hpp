#include <Windows.h>
#include <string>
#include <chrono>

namespace functions {
	// Converts a string to a upper-cased string, returns it.
	_NODISCARD __forceinline auto toUpper(std::string str) noexcept -> std::string {
		for (char& c : str)
			c = std::toupper(c);

		return str;
	}

	// Returns a formatted address. 9F30 -> 00009F30.
	_NODISCARD __forceinline auto formatAddress(const std::string& address) noexcept -> std::string {
		return std::string(8 - address.size(), '0') + address;
	}

	// Prompts a file selector and returns the path of the selected file.
	_NODISCARD auto selectFile(void) noexcept -> std::string {
		OPENFILENAME ofn;
		memset(&ofn, 0, sizeof(ofn));

		char szFileName[MAX_PATH] = "";

		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFilter = "All Files\0*.*\0\0";
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;

		GetOpenFileName(&ofn);

		return std::string(szFileName);
	}

	_NODISCARD __forceinline auto isPressed(const int key) noexcept -> bool {
		return static_cast<bool>(GetAsyncKeyState(key) & 0x8000);
	}

	_NODISCARD __forceinline auto isPressed(const char key) noexcept -> bool {
		return static_cast<bool>(GetAsyncKeyState(VkKeyScanA(key)) & 0x8000);
	}

	__forceinline auto setTitle(const std::string& title) noexcept -> void {
		SetConsoleTitle(title.c_str());
	}
	
	using timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

	_NODISCARD __forceinline auto timepoint(void) noexcept -> timepoint_t {
		return std::chrono::high_resolution_clock::now();
	}

	_NODISCARD __forceinline auto getDifference(timepoint_t first, timepoint_t last) noexcept -> double {
		return std::chrono::duration<double>(last - first).count();
	}
}