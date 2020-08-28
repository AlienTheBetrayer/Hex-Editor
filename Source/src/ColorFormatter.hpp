// v1.0.1
//
// ColorFormatter library

// Examples:
// 1. console::clrprint("{color:light_blue=Hello}, {color:light_red=world}!\n");
// 2. console::clrprint("%v + %v = %v\n", a, b, a + b);

// Check if OS is Windows and C++ version is 17+ to make sure <variant> will work
#if (defined(_WIN32) && (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#if !defined(COLOR_FORMATTING_LIBRARY)
#define COLOR_FORMATTING_LIBRARY

// Reduce Windows header size
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_MEAN_AND_LEAN
#endif

// Regular library includes
#include <Windows.h>
#include <stdio.h>
#include <cstdint>
#include <vector>
#include <array>
#include <string>
#include <variant> // C++17+
#include <utility>
#include <unordered_map>

// All the types variable formatter supports
using types = std::variant<int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float, long, std::string, const char*, bool, double>;

class console {
public:
	// All available colors
	static inline std::unordered_map<std::string, int32_t> colors = {
		{"dark_blue", 1},
		{"green", 2},
		{"light_blue", 3},
		{"red", 4},
		{"purple", 5},
		{"orange", 6},
		{"light_gray", 7},
		{"dark_gray", 8},
		{"blue", 9},
		{"light_green", 10},
		{"cyan", 11},
		{"light_red", 12},
		{"pink", 13},
		{"yellow", 14},
		{"white", 15}
	};

	// Sets color to the specified color
	__forceinline static auto colorset(const int32_t color) noexcept -> void {
		const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hOut, color);
	}

	// Prints out text with the specified color
	__forceinline static auto colorwrite(const int32_t color, const std::string& str) noexcept -> void {
		colorset(color);
		printf(str.c_str());
	}

	// The main color print formatting function
	template<typename... Ts>
	static inline auto colorformat(const std::string& str, Ts&&... format) noexcept -> void {
		std::vector<std::string> parts = {};
		parse(str, parts, std::forward<Ts>(format)...);

		for (const auto& n : parts) {
			if (n[0] != '{' && n[n.length() - 1] != '}') {
				int32_t color = colors["light_gray"];
				colorwrite(color, n);
				continue;
			}

			const std::size_t semicolon = n.find(":");

			if (semicolon == std::string::npos)
				continue;

			const std::size_t whitespace = n.find("=");

			std::string color = "";

			if (n.substr(1, semicolon - 1) == "color") {
				color = n.substr(semicolon + 1, whitespace - 1 - semicolon);
			}

			const std::string text = n.substr(whitespace + 1, n.length() - whitespace - 2);

			int32_t textColor = !colors[color] ? colors["light_gray"] : colors[color];

			colorwrite(textColor, text);
		}

		colorset(colors["light_gray"]);
	}
private:
	// Convert any supported T type to std::string
	template<typename T>
	static inline constexpr auto convert(T arg) -> std::string {
		std::string str;

		if constexpr (std::is_same<decltype(arg), bool>::value) {
			str = arg ? "true" : "false";
		}
		else if constexpr (std::is_integral<decltype(arg)>::value || std::is_floating_point<decltype(arg)>::value) {
			str = std::to_string(arg);
		}
		else if constexpr (std::is_same<decltype(arg), const char*>::value) {
			str = std::string(arg);
		}
		else {
			str = arg;
		}

		return str;
	}

	// Parsing a string into parts
	template<typename... Ts>
	static inline auto parse(const std::string& str, std::vector<std::string>& parts, Ts&&... format) -> void {
		if (str.empty())
			return;

		// Variable parsing
		std::string tStr = str;

		std::size_t last_ = -1;

		std::array<types, sizeof...(Ts)> args = { std::forward<Ts>(format)... };

		for (const types& v : args) {
			const std::size_t found = tStr.find("%v", last_ + 1);

			if (found == std::string::npos)
				break;

			last_ = found;
			tStr.erase(found, 2);

			tStr.insert(found, std::visit([&](auto& arg) noexcept -> std::string {
				return convert(arg);
				}, v));
		}

		std::size_t first = tStr.find("{");
		std::size_t last = tStr.find("}", first);

		// If the string does not contain '{' or '}'
		if (first == std::string::npos || last == std::string::npos) {
			parts.emplace_back(tStr);
			return;
		}

		// If there is some text before '{'	
		if (first != 0) {
			parts.emplace_back(tStr.substr(0, first));
			tStr.erase(0, first);
		}

		first = tStr.find("{");
		last = tStr.find("}", first);

		parts.emplace_back(tStr.substr(first, last - first + 1));
		tStr.erase(first, last + 1);

		parse(tStr, parts);
	}
};

#endif
#else
#error Make sure you are on Windows and C++ version is 17+
#endif
