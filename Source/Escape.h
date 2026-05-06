#pragma once
#include <format>
#include <string>
#include <stdexcept>

#define ESCAPE_CLS "\033[2J"
#define ESCAPE_MOVELINE(n) "\033["#n"A"
#define ESCAPE_MOVEUPPER ESCAPE_MOVELINE(64)

enum class Color : int {
	Cyan = 0x348fca,
	Yellow = 0xe7bd22,
	Green = 0x2aa55d,
	Red = 0xda4b3c,
	Blue = 0x246eab,
	Orange = 0xdc7a23,
	Purple = 0x824597,
	Default = 0x80000000,
};

static inline std::string GetColorEscape(int c) {
	bool defaultflag = (bool)(c & 0x80000000);
	if (defaultflag) {
		return "\033[0m";
	}
	uint8_t r = (c >> 16) & 0xff;
	uint8_t g = (c >> 8) & 0xff;
	uint8_t b = (c >> 0) & 0xff;
	std::string ret = std::format("\033[38;2;{};{};{}m", r, g, b);
	return ret;
}
static inline std::string GetColorEscape(Color c) {
	return GetColorEscape(static_cast<int>(c));
}