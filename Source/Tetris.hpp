#ifndef TETRIS_HPP
#define TETRIS_HPP

#include <array>
#include <deque>
#include <cstdint>
#include <algorithm>
#include <random>
#include <bitset>
#include <complex>
#include <string_view>
#include <ranges>

#define _def_logic_op_aux(e, op)\
static constexpr e operator##op##(e lhs, e rhs) { return static_cast<e>(static_cast<uint8_t>(lhs) op (uint8_t)rhs); }\
static constexpr e operator##op##(e lhs, uint8_t rhs) { return static_cast<e>(static_cast<uint8_t>(lhs) op rhs); }\
static constexpr e operator##op##(uint8_t lhs, e rhs) { return static_cast<e>(lhs op static_cast<uint8_t>(rhs)); }
#define _def_logic_op(e) \
_def_logic_op_aux(e, &)\
_def_logic_op_aux(e, |)\
_def_logic_op_aux(e, ^)

namespace tetris {

using pair_t = std::pair<int, int>;

enum class Mino : uint8_t {
	Null,
	I,
	O,
	S,
	Z,
	L,
	J,
	T,
	AppliedBit = 0x40,
	pI,
	pO,
	pS,
	pZ,
	pL,
	pJ,
	pT,
	DummyBit = 0x80,
	dI,
	dO,
	dS,
	dZ,
	dL,
	dJ,
	dT,
	Mask = ~(AppliedBit | DummyBit)
};

enum class CollisionType : uint8_t {
	N = 0, // Null
	I, // Instance
	S, // Spin
	M // SpinMini
};

_def_logic_op(Mino);

class MinoObject {
	using collision_field = std::array<std::array<CollisionType, 4>, 4>;
	using col_t = CollisionType;

	collision_field Collision;
	int x = 0;
	int y = 0;
	Mino Type = Mino::Null;

	MinoObject(Mino t) : Type(t) {}

public:

	MinoObject() = default;
	MinoObject(const MinoObject&) = default;
	MinoObject(MinoObject&&) = default;
	MinoObject& operator=(const MinoObject&) = default;
	MinoObject& operator=(MinoObject&&) = default;

	auto GetFieldSize() const -> pair_t {
		switch (Type) {
		case Mino::I:
		case Mino::O:
			return pair_t(4, 4);
		case Mino::S:
		case Mino::Z:
		case Mino::L:
		case Mino::J:
		case Mino::T:
			return pair_t(3, 3);
		default:
			return pair_t(0, 0);
		}
	}
	auto GetCollision() const {
		return Collision;
	}
	auto GetType() const -> Mino {
		return Type;
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
	void Clockwise() {
		auto [w, h] = GetFieldSize();
		collision_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[j][w - 1 - i];
			}
		}
		Collision = ret;
	}
	void CounterClockwise() {
		auto [w, h] = GetFieldSize();
		collision_field ret;
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				ret[i][j] = Collision[h - 1 - j][i];
			}
		}
		Collision = ret;
	}
	void SetPosition(int _x, int _y) {
		x = _x;
		y = _y;
	}
	void AddPosition(int _x, int _y) {
		x += _x;
		y += _y;
	}

	auto GetPosition() const -> pair_t {
		return pair_t(x, y);
	}

	static auto I() -> MinoObject {
		MinoObject ret(Mino::I);
		ret.Collision = collision_field({
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::I, col_t::I, col_t::I, col_t::I},
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto O() -> MinoObject {
		MinoObject ret(Mino::O);
		ret.Collision = collision_field({
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::I, col_t::I, col_t::N},
			{col_t::N, col_t::I, col_t::I, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto S() -> MinoObject {
		MinoObject ret(Mino::S);
		ret.Collision = collision_field({
			{col_t::N, col_t::I, col_t::I, col_t::N},
			{col_t::I, col_t::I, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto Z() -> MinoObject {
		MinoObject ret(Mino::Z);
		ret.Collision = collision_field({
			{col_t::I, col_t::I, col_t::N, col_t::N},
			{col_t::N, col_t::I, col_t::I, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto L() -> MinoObject {
		MinoObject ret(Mino::L);
		ret.Collision = collision_field({
			{col_t::N, col_t::N, col_t::I, col_t::N},
			{col_t::I, col_t::I, col_t::I, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto J() -> MinoObject {
		MinoObject ret(Mino::J);
		ret.Collision = collision_field({
			{col_t::I, col_t::N, col_t::N, col_t::N},
			{col_t::I, col_t::I, col_t::I, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto T() -> MinoObject {
		MinoObject ret(Mino::T);
		ret.Collision = collision_field({
			{col_t::S, col_t::I, col_t::S, col_t::N},
			{col_t::I, col_t::I, col_t::I, col_t::N},
			{col_t::S, col_t::M, col_t::S, col_t::N},
			{col_t::N, col_t::N, col_t::N, col_t::N}
		});
		return ret;
	}
	static auto Make(Mino t) -> MinoObject {
		switch (t) {
		case Mino::I: return I();
		case Mino::O: return O();
		case Mino::S: return S();
		case Mino::Z: return Z();
		case Mino::L: return L();
		case Mino::J: return J();
		case Mino::T: return T();
		}
		return MinoObject(t);
	}
};

class TetrisEngine {

	enum class SpinFlag {
		None,
		Spin,
		Mini
	};
	struct SpinInfo {
		Mino Type = Mino::Null;
		int Num = 0;
		bool Mini = false;
	};

	struct Board {
		using board_t = std::vector<std::vector<Mino>>;

		static constexpr int MaxHeight = 128;

		Board(int width, int height) : Width(width), Height(height) {
			Data = board_t(MaxHeight, typename board_t::value_type(Width, Mino::Null));
		}

		auto CheckBound(int xi, int yj) const -> bool {
			return (0 <= xi && xi < Width) && (0 <= yj && yj < MaxHeight);
		}
		auto Placeable(Mino from) const -> bool {
			auto checkbit = [](Mino v, Mino bit) -> bool {
				return (v & bit) == bit;
			};
			return from == Mino::Null || checkbit(from, Mino::DummyBit);
		}

		auto Check(const MinoObject& obj) const -> bool {
			const auto [w, h] = obj.GetFieldSize();
			const auto [x, y] = obj.GetPosition();
			auto&& c = obj.GetCollision();
			auto&& b = GetData();
			
			for (int j = 0; j < h; ++j) {
				for (int i = 0; i < w; ++i) {
					auto [xi, yj] = pair_t(x + i, y + j);
					if (c[j][i] != CollisionType::I) {
						continue;
					}
					if (not CheckBound(xi, yj)) {
						return false;
					}
					if (not Placeable(b[yj][xi])) {
						return false;
					}
				}
			}

			return true;
		}
		auto LineClear() -> int {
			std::deque<size_t> clearline;
			for (auto it = Data.rbegin(), end = Data.rend(); it != end; ++it) {
				auto& line = *it;
				bool clear = true;
				for (const auto& mino : line) {
					if ((bool)(mino & Mino::AppliedBit)) {
						continue;
					}
					if (Placeable(mino)) {
						clear = false;
						break;
					}
				}
				if (clear) {
					for (auto& mino : line) {
						mino = Mino::Null;
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
					if (idx >= Data.size()) {
						break;
					}
					std::swap(Data[idx], Data[idx - offset - 1]);
				}
				offset += 1;
			}
			return count;
		}

		using place_ret_t = bool;
		auto Place(const MinoObject& info, Mino t) -> place_ret_t {
			if (not Check(info)) {
				return false;
			}

			const auto [w, h] = info.GetFieldSize();
			const auto [x, y] = info.GetPosition();
			auto&& c = info.GetCollision();
			auto&& b = RefData();

			for (int j = 0; j < h; ++j) {
				for (int i = 0; i < w; ++i) {
					auto [xi, yj] = pair_t(x + i, y + j);
					if (c[j][i] != CollisionType::I) {
						continue;
					}
					b[yj][xi] = t;
				}
			}
			
			return true;
		}
		auto Place(const MinoObject& obj) -> place_ret_t {
			return Place(obj, obj.GetType());
		}
		auto Dummy(const MinoObject& obj) -> place_ret_t {
			return Place(obj, obj.GetType() | Mino::DummyBit);
		}
		auto Apply(const MinoObject& obj) -> place_ret_t {
			return Place(obj, obj.GetType() | Mino::AppliedBit);
		}
		auto Clear(const MinoObject& obj) -> place_ret_t {
			return Place(obj, Mino::Null);
		}

		auto GetData() const -> const board_t& {
			return Data;
		}
		auto GetSize() const -> pair_t {
			return pair_t(Width, Height);
		}

		auto InitialSpwanPosition() const -> pair_t {
			return pair_t(Width / 2, std::max(MaxHeight - Height - 1, 0));
		}

	private:
		auto RefData() -> board_t& {
			return Data;
		}

		int Width = 0;
		int Height = 0;
		board_t Data;
	};

	// RotationSystem
	struct RotationSystem {
		struct SRS {
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
			static constexpr auto SRSTableIMinoRight = std::array<tableline, 4>{
				tableline{std::pair{-2,0},{+1,0},{-2,+1},{+1,-2}},
				tableline{std::pair{-1,0},{+2,0},{-1,-2},{+2,+1}},
				tableline{std::pair{+2,0},{-1,0},{+2,+1},{-1,+2}},
				tableline{std::pair{+1,0},{-2,0},{+1,-2},{-2,-1}},
			};
			static constexpr auto SRSTableIMinoLeft = []() {
				std::decay_t<decltype(SRSTableIMinoRight)> ret;
				for (size_t j = 0; auto& tl : SRSTableIMinoRight) {
					for (size_t i = 0; auto& test : tl) {
						auto& [dx, dy] = ret[SRSTableIMinoRight.size() - 1 - j][i];
						auto& [tx, ty] = test;
						std::tie(dx, dy) = std::pair{-tx, ty};
						++i;
					}
					++j;
				}
				return ret;
			}();
		};
	};

	struct BagQueue {
		static constexpr std::array<Mino, 7> DefaultList = {
			Mino::I, Mino::O, Mino::S, Mino::Z, Mino::J, Mino::L, Mino::T
		};

		BagQueue(int baglen, uint32_t seed) : BagLength(baglen) {
			SetMinoList(DefaultList);
			Engine->seed(seed);
			for (auto i : std::views::iota(0, BagLength)) {
				Generate();
			}
		}

		Mino GetFront() {
			auto& bag = Queue.front();
			auto ret = std::move(bag.front());
			bag.pop_front();
			if (bag.empty()) {
				Queue.pop_front();
				Generate();
			}
			return ret;
		}
		void Generate() {
			Queue.push_back(MakeBag());
		}

		template<std::ranges::range R>
			requires std::is_same_v<std::ranges::range_value_t<R>, Mino>
		void SetMinoList(const R& l) {
			List = std::vector<Mino>(std::ranges::begin(l), std::ranges::end(l));
		}

		std::vector<Mino> GetQueue(int len = 5) const {
			std::vector<Mino> ret;
			ret.reserve(List.size() * BagLength);
			for (int i = 0; auto& bag : Queue) {
				for (auto& elem : bag) {
					ret.push_back(elem);
					++i;
				}
				if (!(i < len)) {
					break;
				}
			}
			return ret;
		}

	private:

		using RandomEngine = std::unique_ptr<std::mt19937>;

		std::deque<Mino> MakeBag() {
			std::deque<Mino> ret(List.begin(), List.end());
			std::ranges::shuffle(ret, *Engine);
			return ret;
		}

		int BagLength;
		std::vector<Mino> List;
		RandomEngine Engine = RandomEngine(new std::mt19937());
		std::deque<std::deque<Mino>> Queue;
	};

	struct Score {

		struct Inner {
			uint64_t Score;
			uint32_t Line;
			uint32_t Combo;
			uint32_t BackToBack;
		};

		enum class ScoreIndex : int {
			Null,
			Single,
			Double,
			Triple,
			Tetris,
			Combo,
			SoftDrop,
			HardDrop,
			TSpinNull,
			TSpinSingle,
			TSpinDouble,
			TSpinTriple,
			TSpinMiniNull,
			TSpinMiniSingle,
			TSpinMiniDouble,
		};
		static constexpr auto ScoreTable = std::array{
			0,    // Null
				  
			100,  // Single
			300,  // Double
			500,  // Triple
			800,  // Tetris
				  
			50,   // Combo
				  
			1,    // Soft Drop (per cell)
			2,    // Hard Drop (per cell)
				  
			400,  // T-Spin no lines
			800,  // T-Spin Single
			1200, // T-Spin Double
			1600, // T-Spin Triple
				  
			100,  // T-Spin Mini no lines
			200,  // T-Spin Mini Single
			400,  // T-Spin Mini Double
		};
		static constexpr auto DifficultyTable = std::array{
			false, // Null

			false, // Single
			false, // Double
			false, // Triple
			true,  // Tetris

			false, // Combo

			false, // Soft Drop
			false, // Hard Drop

			false, // T-Spin no lines
			true,  // T-Spin Single
			true,  // T-Spin Double
			true,  // T-Spin Triple

			false, // T-Spin Mini no lines
			true,  // T-Spin Mini Single
			true,  // T-Spin Mini Double
		};

		void ApplyLines(int line, SpinFlag spinflags) {
			int idx = line;
			switch (spinflags) {
			case SpinFlag::Spin:
				idx += (int)ScoreIndex::TSpinNull;
				break;
			case SpinFlag::Mini:
				idx += (int)ScoreIndex::TSpinMiniNull;
				break;
			}

			Data.Score += ScoreTable[(int)idx];
			if (static_cast<ScoreIndex>(idx) == ScoreIndex::Null) {
				PrevClear = false;
				Data.Combo = 0;
				return;
			}
			Data.Line += line;
			Data.Combo += PrevClear;
			Data.BackToBack = DifficultyTable[(int)idx] ? Data.BackToBack + 1 : 0;
			Data.Score += ScoreTable[(int)ScoreIndex::Combo] * Data.Combo;
			PrevClear = true;
		}
		void ApplyDrop(bool hard, int cells) {
			if (hard) {
				Data.Score += ScoreTable[(int)ScoreIndex::HardDrop] * cells;
				return;
			}
			Data.Score += ScoreTable[(int)ScoreIndex::SoftDrop];
		}

		const Inner& GetData() const {
			return Data;
		}

	private:
		bool PrevClear = false;
		Inner Data;
	};

	MinoObject Current;


public:

	void Init(int w, int h) {

	}
};

} // namespace tetris

#undef _def_logic_op_aux
#undef _def_logic_op

#endif // TETRIS_HPP
