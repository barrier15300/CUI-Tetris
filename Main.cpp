#include <array>
#include <utility>
#include <deque>
#include <chrono>
#include <thread>

#define NOMINMAX
#include <windows.h>

#define ESCAPE_CLS "\033[2J"
#define ESCAPE_MOVELINE(n) "\033["#n"A"
#define ESCAPE_MOVEUPPER ESCAPE_MOVELINE(64)

enum class Color : int {
	Black = 30,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan,
	White,
	Default = 0,
};

std::string GetColorEscape(Color c) {
	std::string ret(8, '\0');
	int res = sprintf_s(ret.data(), ret.size(), "\033[%dm", static_cast<int>(c));
	if (res < 0) {
		throw std::runtime_error("");
	}
	ret.resize(res);
	return ret;
}

enum class Myno : BYTE {
	Null,
	I,
	O,
	S,
	Z,
	J,
	L,
	T,
	DummyBit = 0x80,
	dI,
	dO,
	dS,
	dZ,
	dJ,
	dL,
	dT,
};

auto Board = []() {
	std::array<std::array<Myno, 10>, 30> ret;
	for (auto& line : ret) {
		for (auto& myno : line) {
			// random includs color and dummy
			myno = static_cast<Myno>(((rand() % 7) + 1) | (0x80 & (((BYTE)rand() & 1) - 1)));

			// set null
			//myno = Myno::Null;
		}
	}
	return ret;
}();
std::deque<std::deque<Myno>> BoxQueue;

void DrawBoard() {
	static std::string BoardOutPut = []() {
		std::string ret;
		ret.reserve(65536);
		return ret;
	}();
	BoardOutPut.clear();
	for (auto& line : Board) {
		BoardOutPut += "|";
		for (auto& myno : line) {
			std::string box(20, '\0');
			bool dummy = (BYTE)myno & 0x80;
			auto m = static_cast<Myno>((BYTE)myno & ~(0x80));
			switch (m) {
			case Myno::Null:
				box += GetColorEscape(Color::Black);
				break;
			case Myno::I:
				box += GetColorEscape(Color::Cyan);
				break;
			case Myno::O:
				box += GetColorEscape(Color::Yellow);
				break;
			case Myno::S:
				box += GetColorEscape(Color::Green);
				break;
			case Myno::Z:
				box += GetColorEscape(Color::Red);
				break;
			case Myno::J:
				box += GetColorEscape(Color::Blue);
				break;
			case Myno::L:
				box += GetColorEscape(Color::White);
				break;
			case Myno::T:
				box += GetColorEscape(Color::Magenta);
				break;
			}
			box += (myno != Myno::Null) ? ((dummy) ? ("□") : ("██")) : ("  ");
			box += GetColorEscape(Color::Default);
			BoardOutPut += box;
		}
		BoardOutPut += "|\n";
	}
	fwrite(BoardOutPut.c_str(), sizeof(char), BoardOutPut.size(), stdout);
}

void Draw() {
	static auto tp = std::chrono::high_resolution_clock::now();
	static std::chrono::nanoseconds frame = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)) / 60;

	fwrite(ESCAPE_MOVEUPPER "\r", sizeof(char), sizeof(ESCAPE_MOVEUPPER "\r"), stdout);
	DrawBoard();
	std::this_thread::sleep_until(tp += frame);
}

int main(int argc, char** argv) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(h, &mode);
	SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	SetConsoleScreenBufferSize(h, {64,256});

	system("chcp 65001");
	system("cls");
	

	HWND console = GetConsoleWindow();
	SetProcessDPIAware();
	int x = GetSystemMetrics(SM_CXSCREEN) / 2;
	int y = GetSystemMetrics(SM_CYSCREEN) / 2;
	int width = 320;
	int height = 720;
	MoveWindow(console, x - (width / 2), y - (height / 2), width, height, TRUE);

	bool key[256]{};
	while (true) {

		// key update
		for (int i = 0;  auto& k : key) {
			k = (bool)(GetAsyncKeyState(i) & 0x8000);
			++i;
		}
		
		if (!IsWindow(console)) {
			break;
		}

		if (key[VK_ESCAPE]) {
			break;
		}
		
		Draw();
	}

	system("cls");

	return 0;
}