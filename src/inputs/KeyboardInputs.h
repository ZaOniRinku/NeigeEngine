#pragma once

enum struct KeyState {
	PRESSED,
	HELD,
	RELEASED,
	NONE
};

struct KeyboardInputs {
	KeyState qKey = KeyState::NONE;
	KeyState wKey = KeyState::NONE;
	KeyState eKey = KeyState::NONE;
	KeyState rKey = KeyState::NONE;
	KeyState tKey = KeyState::NONE;
	KeyState yKey = KeyState::NONE;
	KeyState uKey = KeyState::NONE;
	KeyState iKey = KeyState::NONE;
	KeyState oKey = KeyState::NONE;
	KeyState pKey = KeyState::NONE;
	KeyState aKey = KeyState::NONE;
	KeyState sKey = KeyState::NONE;
	KeyState dKey = KeyState::NONE;
	KeyState fKey = KeyState::NONE;
	KeyState gKey = KeyState::NONE;
	KeyState hKey = KeyState::NONE;
	KeyState jKey = KeyState::NONE;
	KeyState kKey = KeyState::NONE;
	KeyState lKey = KeyState::NONE;
	KeyState zKey = KeyState::NONE;
	KeyState xKey = KeyState::NONE;
	KeyState cKey = KeyState::NONE;
	KeyState vKey = KeyState::NONE;
	KeyState bKey = KeyState::NONE;
	KeyState nKey = KeyState::NONE;
	KeyState mKey = KeyState::NONE;
	KeyState leftKey = KeyState::NONE;
	KeyState rightKey = KeyState::NONE;
	KeyState upKey = KeyState::NONE;
	KeyState downKey = KeyState::NONE;
	KeyState escapeKey = KeyState::NONE;
	KeyState spaceKey = KeyState::NONE;
	KeyState shiftKey = KeyState::NONE;

	void update();
};