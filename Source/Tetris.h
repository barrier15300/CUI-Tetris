#pragma once
#include <array>
#include <deque>
#include <cstdint>
#include <algorithm>
#include <random>
#include <bitset>
#include <complex>
#include <string_view>

#include "Escape.h"
#include "Input.h"
#include "timer.hpp"

enum class Myno : uint8_t {
	Null,
	I,
	O,
	S,
	Z,
	J,
	L,
	T,
	PlaceBit = 0x40,
	pI,
	pO,
	pS,
	pZ,
	pJ,
	pL,
	pT,
	DummyBit = 0x80,
	dI,
	dO,
	dS,
	dZ,
	dJ,
	dL,
	dT,
};

#define _def_logic_op(t)\
static inline Myno operator##t##(Myno lhs, Myno rhs) { return static_cast<Myno>((uint8_t)lhs t (uint8_t)rhs); }\
static inline Myno operator##t##(Myno lhs, uint8_t rhs) { return static_cast<Myno>((uint8_t)lhs t rhs); }\
static inline Myno operator##t##(uint8_t lhs, Myno rhs) { return static_cast<Myno>(lhs t (uint8_t)rhs); }

_def_logic_op(&);
_def_logic_op(| );
_def_logic_op(^);
#undef _def_logic_op

static const inline std::string Block = "██";
static const inline std::string EdgeBlock = "░░";
static const inline std::string None = "  ";

enum class Direction : uint8_t {
	Upper,
	Right,
	Lower,
	Left,
	Count
};

class MynoObject {
	using inner_field = std::array<std::bitset<4>, 4>;

	Direction Dir = Direction::Upper;
	Myno Type = Myno::Null;
	inner_field Collision{};
	int x = 0;
	int y = 0;

	inner_field LRotateImpl() {
		auto [w, h] = GetFieldSize();
		inner_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[j][w - 1 - i];
			}
		}
		Dir = RotateDirectionImpl(false);
		return ret;
	}
	inner_field RRotateImpl() {
		auto [w, h] = GetFieldSize();
		inner_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[h - 1 - j][i];
			}
		}
		Dir = RotateDirectionImpl(true);
		return ret;
	}

	Direction RotateDirectionImpl(bool LR) {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		int8_t ret = static_cast<int8_t>(Dir);
		ret = ((ret + (int8_t)std::copysign(1, (int)(!LR) * -1)) % P + P) % P;
		return static_cast<Direction>(ret);
	}

	MynoObject(Myno t) : Type(t) {}

public:

	MynoObject() = default;
	MynoObject(const MynoObject&) = default;
	MynoObject(MynoObject&&) = default;
	MynoObject& operator=(const MynoObject&) = default;
	MynoObject& operator=(MynoObject&&) = default;

	static MynoObject I() {
		MynoObject ret(Myno::I);
		ret.Collision = {
			0b0000,
			0b1111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject O() {
		MynoObject ret(Myno::O);
		ret.Collision = {
			0b0000,
			0b0110,
			0b0110,
			0b0000
		};
		return ret;
	}
	static MynoObject S() {
		MynoObject ret(Myno::S);
		ret.Collision = {
			0b0110,
			0b0011,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject Z() {
		MynoObject ret(Myno::Z);
		ret.Collision = {
			0b0011,
			0b0110,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject J() {
		MynoObject ret(Myno::J);
		ret.Collision = {
			0b0001,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject L() {
		MynoObject ret(Myno::L);
		ret.Collision = {
			0b0100,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject T() {
		MynoObject ret(Myno::T);
		ret.Collision = {
			0b0010,
			0b0111,
			0b0000,
			0b0000
		};
		return ret;
	}
	static MynoObject Make(Myno t) {
		switch (t) {
			case Myno::I: return I();
			case Myno::O: return O();
			case Myno::S: return S();
			case Myno::Z: return Z();
			case Myno::J: return J();
			case Myno::L: return L();
			case Myno::T: return T();
		}
		return MynoObject(t);
	}

	void MoveLeft() {
		x -= 1;
	}
	void MoveRight() {
		x += 1;
	}
	void MoveDown() {
		y += 1;
	}
	void MoveUp() {
		y -= 1;
	}
	void Rotate(bool LR) {
		Collision = (!LR) ? (LRotateImpl()) : (RRotateImpl());
	}
	void RotateLeft() {
		Rotate(false);
	}
	void RotateRight() {
		Rotate(true);
	}
	void SetPosition(int _x, int _y) {
		x = _x; y = _y;
	}
	void AddPosition(int _x, int _y) {
		x += _x; y += _y;
	}
	std::pair<int, int> GetPosition() const {
		return {x, y};
	}
	std::pair<int, int> GetFieldSize() const {
		return (Type == Myno::I || Type == Myno::O) ? std::pair{4, 4} : std::pair{3, 3};
	}
	const inner_field& GetCollision() const {
		return Collision;
	}
	Myno GetType() const {
		return Type;
	}
	template<class T = Direction>
	T GetDirection() const {
		return static_cast<T>(Dir);
	}
	template<class T = Direction>
	T GetDirectionPrevRight() const {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		return static_cast<T>(((GetDirection<int8_t>() - 1) % P + P) % P);
	}
	template<class T = Direction>
	T GetDirectionPrevLeft() const {
		constexpr int8_t P = static_cast<int8_t>(Direction::Count);
		return static_cast<T>(((GetDirection<int8_t>() + 1) % P + P) % P);
	}
	static std::string GetBox(Myno t)  {
		std::string ret;
		bool dummy = (bool)(t & Myno::DummyBit);
		auto m = t & ~(uint8_t)(Myno::DummyBit | Myno::PlaceBit);
		switch (m) {
		case Myno::Null:
			ret += escape::Color(RGBColor::Default);
			break;
		case Myno::I:
			ret += escape::Color(RGBColor::Cyan);
			break;
		case Myno::O:
			ret += escape::Color(RGBColor::Yellow);
			break;
		case Myno::S:
			ret += escape::Color(RGBColor::Green);
			break;
		case Myno::Z:
			ret += escape::Color(RGBColor::Red);
			break;
		case Myno::J:
			ret += escape::Color(RGBColor::Blue);
			break;
		case Myno::L:
			ret += escape::Color(RGBColor::Orange);
			break;
		case Myno::T:
			ret += escape::Color(RGBColor::Purple);
			break;
		}
		ret += (t != Myno::Null) ? ((dummy) ? (EdgeBlock) : (Block)) : (None);
		return ret;
	}
};

class Game {

	libarrier::Timer InGameTimer;
	InputFlag Keyboard[256]{};

	std::string ScreenOutput;
	void MargeOutput(std::initializer_list<std::string_view> buffers) {
		ScreenOutput.clear();
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		std::vector<std::string_view> buf = buffers;
		static auto count = [&]() {
			std::vector<size_t> ret;
			for (auto& b : buf) {
				if (b.empty()) {
					continue;
				}
				size_t e = b.find(escape::LineMoveNext());
				ScreenOutput += b.substr(0, e);
				b = b.substr(e);
				PutString(ScreenOutput);
				CONSOLE_SCREEN_BUFFER_INFO csbi;
				GetConsoleScreenBufferInfo(hConsole, &csbi);
				if (ret.empty()) {
					ret.push_back(csbi.dwCursorPosition.X);
					continue;
				}
				ret.push_back(csbi.dwCursorPosition.X - ret.back());
				PutString(escape::LineMoveBegin());
			}
			return ret;
		}();

		while (true) {
			bool breakflag = true;
			for (size_t i = -1; auto& b : buf) {
				++i;
				if (b.empty()) {
					ScreenOutput += escape::Color(RGBColor::Default);
					ScreenOutput += std::string(count[i], ' ');
					continue;
				}
				breakflag = false;
				size_t e = b.find(escape::LineMoveNext());
				ScreenOutput += b.substr(0, e);
				b = b.substr(e + 1);
			}
			if (breakflag) {
				break;
			}
			ScreenOutput += escape::LineMoveNext();
		}
		ScreenOutput += escape::LineMoveBegin();
	}

	bool EndFlag = false;

	uint64_t Line = 0;
	uint64_t Score = 0;
	uint32_t Combo = 0;
	uint32_t B2B = 0;
	bool Spin = false;
	void DrawScore() const {
		PutString(rformat(
			"Time:  {:0.3f}s" + escape::LineMoveNext() +
			"Score: {:0>12}" + escape::LineMoveNext() +
			"Line:  {}" + escape::LineMoveNext() +
			"Combo: {}" + escape::LineMoveNext() +
			"B2B:   {}" + escape::LineMoveNext(),
			InGameTimer.GetElapsed().Second(), Score, Line, Combo, B2B
		));
	}
	
	std::deque<std::deque<Myno>> BagQueue;
	int NextCount = 5;
	std::string QueueOutput;
	void MakeMynoSets() {
		static std::random_device device;
		std::mt19937 gen(device());

		std::deque<Myno> ret = {Myno::I, Myno::O, Myno::S, Myno::Z, Myno::J, Myno::L, Myno::T};
		std::shuffle(ret.begin(), ret.end(), gen);
		BagQueue.push_back(std::move(ret));
	}
	Myno GetMynoQueue() {
		auto& frontbag = BagQueue.front();
		auto ret = std::move(frontbag.front());
		frontbag.pop_front();
		if (frontbag.empty()) {
			BagQueue.pop_front();
			MakeMynoSets();
		}
		return ret;
	}
	void DrawQueue() {
		QueueOutput.clear();
		for (int n = 0; n < NextCount; ++n) {
			Myno t = (n < BagQueue[0].size() ? BagQueue[0][n] : BagQueue[1][n - BagQueue[0].size()]);
			auto obj = MynoObject::Make(t);
			auto& c = obj.GetCollision();
			for (int j = 0; j < c.size(); ++j) {
				for (int i = 0; i < c[j].size(); ++i) {
					QueueOutput += (c[j][i] ? MynoObject::GetBox(t) : MynoObject::GetBox(Myno::Null));
				}
				QueueOutput += GetBorder() + escape::LineMoveNext();
			}
			for (int i = 0; i < c.size() + 1; ++i) {
				QueueOutput += GetBorder();
			}
			QueueOutput += escape::LineMoveNext();
		}
	}

	int Width = 0;
	int Height = 0;
	std::vector<std::vector<Myno>> Board;
	std::string BoardOutput;
	bool CheckInBoard(const MynoObject& obj) {
		auto [w, h] = obj.GetFieldSize();
		auto [x, y] = obj.GetPosition();
		auto& c = obj.GetCollision();
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				if (!c[j][i]) {
					continue;
				}
				if (!(0 <= x + i && x + i < Width)) {
					return false;
				}
				if (!(0 <= y + j && y + j < Height)) {
					return false;
				}
				auto& ref = Board[y + j][x + i];
				if (!(ref == Myno::Null || ref == obj.GetType() || (bool)(ref & Myno::DummyBit))) {
					return false;
				}
			}
		}
		return true;
	}
	int PlaceBoard(const MynoObject& obj, bool place, bool clear, bool preview) {
		auto [w, h] = obj.GetFieldSize();
		auto [x, y] = obj.GetPosition();
		auto& c = obj.GetCollision();
		int nearcount = 0;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				if (!(0 <= x + i && x + i < Width && 0 <= y + j && y + j < Height)) {
					nearcount += 1;
					continue;
				}
				auto& ref = Board[y + j][x + i];
				if ((bool)(ref & Myno::PlaceBit)) {
					nearcount += 1;
				}
				if (!c[j][i]) {
					continue;
				}
				if (clear) {
					ref = Myno::Null;
					continue;
				}
				if (place) {
					ref = obj.GetType() | Myno::PlaceBit;
					continue;
				}
				if (preview) {
					ref = obj.GetType() | Myno::DummyBit;
					continue;
				}
				ref = obj.GetType();
			}
		}
		return nearcount;
	}
	void ClearBoardPrev(const MynoObject& obj) {
		PlaceBoard(obj, false, true, false);
	}
	void PlaceBoard(const MynoObject& obj) {
		PlaceBoard(obj, false, false, false);
	}
	void ApplyBoard(const MynoObject& obj) {
		HoldOnce = false;
		Myno t = Current.GetType();
		int nearcount = PlaceBoard(obj, true, false, false);
		int clearcount = LineClear();
		bool perfect = IsPerfect();
		if (clearcount == 0) {
			Score += 100;
			Combo = 0;
			return;
		}
		Line += clearcount;
		Score += (1000 + ((int)perfect * 1000)) * clearcount;
		Score += 200 * Combo;
		Score += 200 * B2B;
		(clearcount == 4 || (t == Myno::T && nearcount >= 3 && Spin)) ? (B2B += 1) : (B2B = 0);
		Spin = false;
		Combo += 1;
	}
	void PreviewBoard(const MynoObject& obj) {
		PlaceBoard(obj, false, false, true);
	}
	int LineClear() {
		std::deque<size_t> clearline;
		for (auto it = Board.rbegin(), end = Board.rend(); it != end; ++it) {
			auto& line = *it;
			bool clear = true;
			for (const auto& myno : line) {
				if ((bool)(myno & Myno::PlaceBit)) {
					continue;
				}
				if (myno == Myno::Null) {
					clear = false;
					break;
				}
				else if ((bool)(myno & Myno::DummyBit)) {
					clear = false;
					break;
				}
			}
			if (clear) {
				for (auto& myno : line) {
					myno = Myno::Null;
				}
				clearline.push_back(end - it - 1);
			}
		}
		int count = clearline.size();
		size_t offset = 0;
		while (!clearline.empty()) {
			size_t ibeg = clearline.front(); clearline.pop_front();
			size_t iend = (clearline.empty() ? 0 : clearline.front());
			for (size_t i = ibeg; i != iend; --i) {
				size_t idx = i + offset;
				if (idx >= Board.size()) {
					break;
				}
				std::swap(Board[idx], Board[idx - offset - 1]);
			}
			offset += 1;
		}
		return count;
	}
	bool IsPerfect() {
		for (auto& line : Board) {
			for (auto& t : line) {
				if (t != Myno::Null) {
					return false;
				}
			}
		}
		return true;
	}
	static std::string GetBorder() {
		return escape::Color(0xffffff) + Block + escape::Color(RGBColor::Default);
	}
	static std::string GetNone() {
		return escape::Color(RGBColor::Default) + None;
	}
	void DrawBoard() {
		BoardOutput.clear();
		for (auto& line : Board) {
			BoardOutput += escape::Color(0xffffff) + Block;
			for (auto& myno : line) {
				std::string box(20, '\0');
				box += MynoObject::GetBox(myno);
				box += escape::Color(RGBColor::Default);
				BoardOutput += box;
			}
			BoardOutput += GetBorder() + escape::LineMoveNext();
		}
		for (int i = 0; i < Width + 2; ++i) {
			BoardOutput += GetBorder();
		}
		BoardOutput += escape::LineMoveNext();
	}

	using tableline = std::array<std::pair<int, int>, 4>;
	static constexpr auto SRSTableRight = std::array<tableline, 4>{
		tableline{std::pair{-1,0}, {-1,-1}, {0,+2}, {-1,+2}}, // upper -> right
		tableline{std::pair{+1,0}, {+1,+1}, {0,-2}, {+1,-2}}, // right -> lower
		tableline{std::pair{+1,0}, {+1,-1}, {0,+2}, {+1,+2}}, // lower -> left
		tableline{std::pair{-1,0}, {-1,+1}, {0,-2}, {-1,-2}}, // left  -> upper
		// left rotate is negative x
	};
	static constexpr auto SRSTableLeft = []() {
		std::decay_t<decltype(SRSTableRight)> ret;
		for (size_t j = 0; auto& tl : SRSTableRight) {
			for (size_t i = 0; auto& test : tl) {
				auto& [dx, dy] = ret[j][i];
				auto& [tx, ty] = test;
				std::tie(dx, dy) = std::pair{-tx, ty};
				++i;
			}
			++j;
		}
		return ret;
	}();
	static constexpr auto SRSTableIMynoRight = std::array<tableline, 4>{
		tableline{std::pair{-2,0},{+1,0},{-2,+1},{+1,-2}},
		tableline{std::pair{-1,0},{+2,0},{-1,-2},{+2,+1}},
		tableline{std::pair{+2,0},{-1,0},{+2,+1},{-1,+2}},
		tableline{std::pair{+1,0},{-2,0},{+1,-2},{-2,-1}},
	};
	static constexpr auto SRSTableIMynoLeft = []() {
		std::decay_t<decltype(SRSTableIMynoRight)> ret;
		for (size_t j = 0;  auto& tl : SRSTableIMynoRight) {
			for (size_t i = 0; auto& test : tl) {
				auto& [dx, dy] = ret[SRSTableIMynoRight.size() - 1 - j][i];
				auto& [tx, ty] = test;
				std::tie(dx, dy) = std::pair{-tx, ty};
				++i;
			}
			++j;
		}
		return ret;
	}();
	int TableIndexConverter(bool LR, Direction cur) {
		int ret = [=]() {
			if (LR) {
				switch (cur) {
				case Direction::Upper: return 3;
				case Direction::Right: return 0;
				case Direction::Lower: return 1;
				case Direction::Left: return 2;
				}
			}
			else {
				switch (cur) {
				case Direction::Upper: return 3;
				case Direction::Right: return 2;
				case Direction::Lower: return 1;
				case Direction::Left: return 0;
				}
			}
		}();
		return ret;
	}
	MynoObject Current;
	MynoObject Preview;
	libarrier::Timer LockTimer;
	double LockTime = 0.5;
	int LockCount = 0;
	libarrier::Timer GravityTimer;
	double GravityTime = 1;
	double GravitySpeedRate = 1 - 0.00025;
	double SoftDropRate = 0.03125;
	char MoveState = '\0';
	libarrier::Timer MoveTimer;
	double MoveTime = 0.15;
	void SetCurrent(Myno t) {
		Current = MynoObject::Make(t);
		auto [w, _] = Current.GetFieldSize();
		Current.SetPosition(Width / 2 - 2, 0);
		if (!CheckInBoard(Current)) {
			EndFlag = true;
		}
	}
	void CurrentMoveLeft() {
		if (auto temp = Current; !CheckInBoard((temp.MoveLeft(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveLeft();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentMoveRight() {
		if (auto temp = Current; !CheckInBoard((temp.MoveRight(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveRight();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentRotateLeft() {
		int x = 0;
		int y = 0;
		if (auto temp = Current; !CheckInBoard((temp.RotateLeft(), temp))) {
			int tableidx = TableIndexConverter(false, temp.GetDirection());
			auto& table = (temp.GetType() == Myno::I) ? SRSTableIMynoLeft : SRSTableLeft;
			bool testfound = false;
			for (auto [tx, ty] : table[tableidx]) {
				auto test = temp;
				test.AddPosition(tx, ty);
				if (CheckInBoard(test)) {
					x = tx;
					y = ty;
					testfound = true;
					break;
				}
			}
			if (!testfound) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.AddPosition(x, y);
		Current.RotateLeft();
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
		CurrentSpinFlag();
	}
	void CurrentRotateRight() {
		int x = 0;
		int y = 0;
		if (auto temp = Current; !CheckInBoard((temp.RotateRight(), temp))) {
			int tableidx = TableIndexConverter(true, temp.GetDirection());
			auto& table = (temp.GetType() == Myno::I) ? SRSTableIMynoRight : SRSTableRight;
			bool testfound = false;
			for (auto [tx, ty] : table[tableidx]) {
				auto test = temp;
				test.AddPosition(tx, ty);
				if (CheckInBoard(test)) {
					x = tx;
					y = ty;
					testfound = true;
					break;
				}
			}
			if (!testfound) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.RotateRight();
		Current.AddPosition(x, y);
		CurrentPreview();
		PlaceBoard(Current);
		LockTimeInMove();
		CurrentSpinFlag();
	}
	void CurrentSoftDrop() {
		if (auto temp = Current; !CheckInBoard((temp.MoveDown(), temp))) {
			return;
		}
		ClearBoardPrev(Current);
		Current.MoveDown();
		PlaceBoard(Current);
		LockTimeInMove();
	}
	void CurrentHardDrop() {
		auto [x, y] = Current.GetPosition();
		while (true) {
			auto test = Current;
			test.SetPosition(x, y + 1);
			if (!CheckInBoard(test)) {
				break;
			}
			y += 1;
			if (y >= Height) {
				return;
			}
		}
		ClearBoardPrev(Current);
		Current.SetPosition(x, y);
		ApplyBoard(Current);
		Next();
	}
	void CurrentPreview(bool clear = true) {
		auto [x, y] = Current.GetPosition();
		while (true) {
			auto test = Current;
			test.SetPosition(x, y + 1);
			if (!CheckInBoard(test)) {
				break;
			}
			y += 1;
			if (y >= Height) {
				return;
			}
		}
		if (clear) { ClearBoardPrev(Preview); }
		Preview = Current;
		Preview.SetPosition(x, y);
		PreviewBoard(Preview);
	}
	void CurrentSpinFlag() {
		auto temp = Current;
		Spin = !CheckInBoard((temp.MoveDown(), temp));
	}
	void LockTimeInMove() {
		if (LockCount >= 15) {
			return;
		}
		else if (auto temp = Current; CheckInBoard((temp.MoveDown(), temp))) {
			LockTimer.Reset();
			return;
		}
		LockTimer.Start();
		LockCount += 1;
	}
	void ResetLockTime() {
		LockTimer.Reset();
		LockCount = 0;
		LockTimeInMove();
	}
	void Next() {
		SetCurrent(GetMynoQueue());
		CurrentPreview(false);
		PlaceBoard(Current);
		ResetLockTime();
		GravityTimer.Start();
	}
	void DebugNext() {
		ClearBoardPrev(Current);
		Next();
	}

	MynoObject Hold;
	bool HoldOnce = false;
	std::string HoldOutput;
	void SwapHold() {
		if (HoldOnce) {
			return;
		}
		HoldOnce = true;
		ClearBoardPrev(Current);
		Myno t = Current.GetType();
		SetCurrent(Hold.GetType());
		Hold = MynoObject::Make(t);
		if (Current.GetType() == Myno::Null) {
			ClearBoardPrev(Preview);
			Next();
			return;
		}
		CurrentPreview();
		PlaceBoard(Current);
		ResetLockTime();
	}
	void DrawHold() {
		HoldOutput.clear();
		auto& c = Hold.GetCollision();
		for (int j = 0; j < c.size(); ++j) {
			HoldOutput += GetBorder();
			for (int i = 0; i < c[j].size(); ++i) {
				if (!c[j][i]) {
					HoldOutput += MynoObject::GetBox(Myno::Null);
					continue;
				}
				if (HoldOnce) {
					HoldOutput += escape::Color(0x404040) + Block;
				}
				else {
					HoldOutput += MynoObject::GetBox(Hold.GetType());
				}
			}
			HoldOutput += escape::LineMoveNext();
		}
		for (int i = 0; i < c.size() + 1; ++i) {
			HoldOutput += GetBorder();
		}
		HoldOutput += escape::LineMoveNext();
	}

public:

	void Init(int w, int h) {
		Width = w;
		Height = h + 4;
		Board.resize(Height);
		bool xflag = false;
		bool yflag = false;
		for (auto& line : Board) {
			line.resize(Width);
			for (auto& myno : line) {
				// random includs color and dummy
				//myno = static_cast<Myno>(((rand() % 7) + 1) | (0x80 & (((uint8_t)rand() & 1) - 1)));

				// random color
				//myno = static_cast<Myno>(((rand() % 7) + 1) * (int)(xflag || yflag));

				// set null
				myno = Myno::Null;
				yflag = !yflag;
			}
			xflag = !xflag;
		}
		for (int i = 0; i < 3; ++i) {
			MakeMynoSets();
		}
		Next();
		GravityTimer.Start();
		InGameTimer.Start();
	}
	void End() {
		PutString(escape::LineMoveNext());
		system("pause");
	}
	bool IsGameEnd() const {
		return EndFlag;
	}
	void Proc(bool* key) {
		for (size_t i = 0; auto& k : Keyboard) {
			k.Update(key[i]);
			++i;
		}

		if (LockTimer.GetElapsed().Second() > LockTime) {
			ApplyBoard(Current);
			Next();
		}
		if (GravityTimer.GetElapsed().Second() > GravityTime * (Keyboard['S'].Press() ? SoftDropRate : 1)) {
			GravityTimer.Start();
			CurrentSoftDrop();
		}
		if (InGameTimer.GetElapsed().Second() > 1) {
			GravityTime *= GravitySpeedRate;
		}

		if (Keyboard['P'].Down()) {
			DebugNext();
		}

		if (Keyboard[VK_ESCAPE].Down()) {
			EndFlag = true;
		}

		if (Keyboard['A'].Down()) {
			MoveTimer.Start();
			MoveState = 'A';
			CurrentMoveLeft();
		}
		if (Keyboard['D'].Down()) {
			MoveState = 'D';
			MoveTimer.Start();
			CurrentMoveRight();
		}
		if (MoveState == 'A') {
			if (MoveTimer.GetElapsed().Second() > MoveTime) {
				CurrentMoveLeft();
			}
			if (Keyboard['A'].Up()) {
				MoveState = '\0';
				MoveTimer.Reset();
			}
		}
		if (MoveState == 'D') {
			if (MoveTimer.GetElapsed().Second() > MoveTime) {
				CurrentMoveRight();
			}
			if (Keyboard['D'].Up()) {
				MoveState = '\0';
				MoveTimer.Reset();
			}
		}

		if (Keyboard[VK_SPACE].Down()) {
			CurrentHardDrop();
		}
		if (Keyboard[VK_LSHIFT].Down()) {
			SwapHold();
		}

		if (Keyboard['J'].Down()) {
			CurrentRotateLeft();
		}
		if (Keyboard['L'].Down()) {
			CurrentRotateRight();
		}
	}
	void Draw() {
		DrawBoard();
		DrawHold();
		DrawQueue();
		MargeOutput({HoldOutput, BoardOutput, QueueOutput});
		PutString(ScreenOutput);
		DrawScore();
	}
};
