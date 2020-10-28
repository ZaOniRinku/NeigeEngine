#include "KeyboardInputs.h"

void KeyboardInputs::update() {
	if (qKey == KeyState::PRESSED) {
		qKey = KeyState::HELD;
	} else if (qKey == KeyState::RELEASED) {
		qKey = KeyState::NONE;
	}

	if (wKey == KeyState::PRESSED) {
		wKey = KeyState::HELD;
	}
	else if (wKey == KeyState::RELEASED) {
		wKey = KeyState::NONE;
	}

	if (eKey == KeyState::PRESSED) {
		eKey = KeyState::HELD;
	}
	else if (eKey == KeyState::RELEASED) {
		eKey = KeyState::NONE;
	}

	if (rKey == KeyState::PRESSED) {
		rKey = KeyState::HELD;
	}
	else if (rKey == KeyState::RELEASED) {
		rKey = KeyState::NONE;
	}

	if (tKey == KeyState::PRESSED) {
		tKey = KeyState::HELD;
	}
	else if (tKey == KeyState::RELEASED) {
		tKey = KeyState::NONE;
	}

	if (yKey == KeyState::PRESSED) {
		yKey = KeyState::HELD;
	}
	else if (yKey == KeyState::RELEASED) {
		yKey = KeyState::NONE;
	}

	if (uKey == KeyState::PRESSED) {
		uKey = KeyState::HELD;
	}
	else if (uKey == KeyState::RELEASED) {
		uKey = KeyState::NONE;
	}

	if (iKey == KeyState::PRESSED) {
		iKey = KeyState::HELD;
	}
	else if (iKey == KeyState::RELEASED) {
		iKey = KeyState::NONE;
	}

	if (oKey == KeyState::PRESSED) {
		oKey = KeyState::HELD;
	}
	else if (oKey == KeyState::RELEASED) {
		oKey = KeyState::NONE;
	}

	if (pKey == KeyState::PRESSED) {
		pKey = KeyState::HELD;
	}
	else if (pKey == KeyState::RELEASED) {
		pKey = KeyState::NONE;
	}

	if (aKey == KeyState::PRESSED) {
		aKey = KeyState::HELD;
	}
	else if (aKey == KeyState::RELEASED) {
		aKey = KeyState::NONE;
	}

	if (sKey == KeyState::PRESSED) {
		sKey = KeyState::HELD;
	}
	else if (sKey == KeyState::RELEASED) {
		sKey = KeyState::NONE;
	}

	if (dKey == KeyState::PRESSED) {
		dKey = KeyState::HELD;
	}
	else if (dKey == KeyState::RELEASED) {
		dKey = KeyState::NONE;
	}

	if (fKey == KeyState::PRESSED) {
		fKey = KeyState::HELD;
	}
	else if (fKey == KeyState::RELEASED) {
		fKey = KeyState::NONE;
	}

	if (gKey == KeyState::PRESSED) {
		gKey = KeyState::HELD;
	}
	else if (gKey == KeyState::RELEASED) {
		gKey = KeyState::NONE;
	}

	if (hKey == KeyState::PRESSED) {
		hKey = KeyState::HELD;
	}
	else if (hKey == KeyState::RELEASED) {
		hKey = KeyState::NONE;
	}

	if (jKey == KeyState::PRESSED) {
		jKey = KeyState::HELD;
	}
	else if (jKey == KeyState::RELEASED) {
		jKey = KeyState::NONE;
	}

	if (kKey == KeyState::PRESSED) {
		kKey = KeyState::HELD;
	}
	else if (kKey == KeyState::RELEASED) {
		kKey = KeyState::NONE;
	}

	if (lKey == KeyState::PRESSED) {
		lKey = KeyState::HELD;
	}
	else if (lKey == KeyState::RELEASED) {
		lKey = KeyState::NONE;
	}

	if (zKey == KeyState::PRESSED) {
		zKey = KeyState::HELD;
	}
	else if (zKey == KeyState::RELEASED) {
		zKey = KeyState::NONE;
	}

	if (xKey == KeyState::PRESSED) {
		xKey = KeyState::HELD;
	}
	else if (xKey == KeyState::RELEASED) {
		xKey = KeyState::NONE;
	}

	if (cKey == KeyState::PRESSED) {
		cKey = KeyState::HELD;
	}
	else if (cKey == KeyState::RELEASED) {
		cKey = KeyState::NONE;
	}

	if (vKey == KeyState::PRESSED) {
		vKey = KeyState::HELD;
	}
	else if (vKey == KeyState::RELEASED) {
		vKey = KeyState::NONE;
	}

	if (bKey == KeyState::PRESSED) {
		bKey = KeyState::HELD;
	}
	else if (bKey == KeyState::RELEASED) {
		bKey = KeyState::NONE;
	}

	if (nKey == KeyState::PRESSED) {
		nKey = KeyState::HELD;
	}
	else if (nKey == KeyState::RELEASED) {
		nKey = KeyState::NONE;
	}

	if (mKey == KeyState::PRESSED) {
		mKey = KeyState::HELD;
	}
	else if (mKey == KeyState::RELEASED) {
		mKey = KeyState::NONE;
	}

	if (leftKey == KeyState::PRESSED) {
		leftKey = KeyState::HELD;
	}
	else if (leftKey == KeyState::RELEASED) {
		leftKey = KeyState::NONE;
	}

	if (rightKey == KeyState::PRESSED) {
		rightKey = KeyState::HELD;
	}
	else if (rightKey == KeyState::RELEASED) {
		rightKey = KeyState::NONE;
	}

	if (upKey == KeyState::PRESSED) {
		upKey = KeyState::HELD;
	}
	else if (upKey == KeyState::RELEASED) {
		upKey = KeyState::NONE;
	}

	if (downKey == KeyState::PRESSED) {
		downKey = KeyState::HELD;
	}
	else if (downKey == KeyState::RELEASED) {
		downKey = KeyState::NONE;
	}

	if (escapeKey == KeyState::PRESSED) {
		escapeKey = KeyState::HELD;
	}
	else if (escapeKey == KeyState::RELEASED) {
		escapeKey = KeyState::NONE;
	}

	if (spaceKey == KeyState::PRESSED) {
		spaceKey = KeyState::HELD;
	}
	else if (spaceKey == KeyState::RELEASED) {
		spaceKey = KeyState::NONE;
	}

	if (shiftKey == KeyState::PRESSED) {
		shiftKey = KeyState::HELD;
	}
	else if (shiftKey == KeyState::RELEASED) {
		shiftKey = KeyState::NONE;
	}
}
