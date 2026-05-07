#include <array>
#include <utility>
#include <deque>
#include <chrono>
#include <thread>

#define NOMINMAX
#include <windows.h>

#include "Source/Tetris.h"

int main(int argc, char** argv) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(h, &mode);
	SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	SetConsoleScreenBufferSize(h, {128,256});

	system("chcp 65001");
	system("cls");

	HWND console = GetConsoleWindow();
	SetProcessDPIAware();
	int x = GetSystemMetrics(SM_CXSCREEN) / 2;
	int y = GetSystemMetrics(SM_CYSCREEN) / 2;
	int width = 480;
	int Height = 720;
	MoveWindow(console, x - (width / 2), y - (Height / 2), width, Height, TRUE);

	auto tp = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds frame = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)) / 60;

	bool key[256]{};

	Game game;
	game.Init(10, 20);

	while (true) {

		for (int i = 0; auto& k : key) {
			k = (bool)(GetAsyncKeyState(i) & 0x8000);
			++i;
		}

		if (!IsWindow(console)) {
			break;
		}

		if (game.IsGameEnd()) {
			break;
		}

		game.Proc(key);

		PutString(escape::LineMoveUp(256) + escape::LineMoveBegin());

		game.Draw();

		std::this_thread::sleep_until(tp += frame);
	}

	game.End();

	return 0;
}