#pragma once

struct InputFlag {

	enum class State : uint8_t {
		Down = 0b0001,
		Press = 0b0010,
		Up = 0b0100,
		Release = 0b1000
	};

	bool Down() const {
		return state == State::Down;
	}

	bool Press() const {
		return state == State::Press || Down();
	}

	bool Up() const {
		return state == State::Up;
	}

	bool Release() const {
		return state == State::Release || Up();
	}

	void Update(bool flag) {
		//switch (state) {
		//case State::Down:
		//case State::Press:
		//	(byte&)state = (((int)flag - 1) ^ 0b0011) & 0b0110;
		//	break;
		//case State::Up:
		//case State::Release:
		//	(byte&)state = (((int)flag - 1) ^ 0b0011) & 0b1001;
		//	break;
		//}

		state = static_cast<State>(
			((((uint8_t)flag - 1) ^ 0b0011) & (((-((int8_t)state & 0b0011)) >> 7) ^ 0b1001)) & 0b1111
			);
	}

	State state = State::Release;
};
