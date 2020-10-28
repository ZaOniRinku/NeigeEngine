#pragma once
#include <iostream>
#include <bitset>

enum KeyState {
	PRESSED,
	HELD,
	RELEASED,
	NONE
};

struct KeyboardInputs {
	KeyState qKey = NONE;
	KeyState wKey = NONE;
	KeyState eKey = NONE;
	KeyState rKey = NONE;
	KeyState tKey = NONE;
	KeyState yKey = NONE;
	KeyState uKey = NONE;
	KeyState iKey = NONE;
	KeyState oKey = NONE;
	KeyState pKey = NONE;
	KeyState aKey = NONE;
	KeyState sKey = NONE;
	KeyState dKey = NONE;
	KeyState fKey = NONE;
	KeyState gKey = NONE;
	KeyState hKey = NONE;
	KeyState jKey = NONE;
	KeyState kKey = NONE;
	KeyState lKey = NONE;
	KeyState zKey = NONE;
	KeyState xKey = NONE;
	KeyState cKey = NONE;
	KeyState vKey = NONE;
	KeyState bKey = NONE;
	KeyState nKey = NONE;
	KeyState mKey = NONE;
	KeyState escapeKey = NONE;
	KeyState spaceKey = NONE;
	KeyState shiftKey = NONE;

	void update();
};