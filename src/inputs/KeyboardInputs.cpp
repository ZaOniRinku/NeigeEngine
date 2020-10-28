#include "KeyboardInputs.h"

void KeyboardInputs::update() {
	if (qKey == PRESSED) {
		qKey = HELD;
	} else if (qKey == RELEASED) {
		qKey = NONE;
	}

	if (wKey == PRESSED) {
		wKey = HELD;
	}
	else if (wKey == RELEASED) {
		wKey = NONE;
	}

	if (eKey == PRESSED) {
		eKey = HELD;
	}
	else if (eKey == RELEASED) {
		eKey = NONE;
	}

	if (rKey == PRESSED) {
		rKey = HELD;
	}
	else if (rKey == RELEASED) {
		rKey = NONE;
	}

	if (tKey == PRESSED) {
		tKey = HELD;
	}
	else if (tKey == RELEASED) {
		tKey = NONE;
	}

	if (yKey == PRESSED) {
		yKey = HELD;
	}
	else if (yKey == RELEASED) {
		yKey = NONE;
	}

	if (uKey == PRESSED) {
		uKey = HELD;
	}
	else if (uKey == RELEASED) {
		uKey = NONE;
	}

	if (iKey == PRESSED) {
		iKey = HELD;
	}
	else if (iKey == RELEASED) {
		iKey = NONE;
	}

	if (oKey == PRESSED) {
		oKey = HELD;
	}
	else if (oKey == RELEASED) {
		oKey = NONE;
	}

	if (pKey == PRESSED) {
		pKey = HELD;
	}
	else if (pKey == RELEASED) {
		pKey = NONE;
	}

	if (aKey == PRESSED) {
		aKey = HELD;
	}
	else if (aKey == RELEASED) {
		aKey = NONE;
	}

	if (sKey == PRESSED) {
		sKey = HELD;
	}
	else if (sKey == RELEASED) {
		sKey = NONE;
	}

	if (dKey == PRESSED) {
		dKey = HELD;
	}
	else if (dKey == RELEASED) {
		dKey = NONE;
	}

	if (fKey == PRESSED) {
		fKey = HELD;
	}
	else if (fKey == RELEASED) {
		fKey = NONE;
	}

	if (gKey == PRESSED) {
		gKey = HELD;
	}
	else if (gKey == RELEASED) {
		gKey = NONE;
	}

	if (hKey == PRESSED) {
		hKey = HELD;
	}
	else if (hKey == RELEASED) {
		hKey = NONE;
	}

	if (jKey == PRESSED) {
		jKey = HELD;
	}
	else if (jKey == RELEASED) {
		jKey = NONE;
	}

	if (kKey == PRESSED) {
		kKey = HELD;
	}
	else if (kKey == RELEASED) {
		kKey = NONE;
	}

	if (lKey == PRESSED) {
		lKey = HELD;
	}
	else if (lKey == RELEASED) {
		lKey = NONE;
	}

	if (zKey == PRESSED) {
		zKey = HELD;
	}
	else if (zKey == RELEASED) {
		zKey = NONE;
	}

	if (xKey == PRESSED) {
		xKey = HELD;
	}
	else if (xKey == RELEASED) {
		xKey = NONE;
	}

	if (cKey == PRESSED) {
		cKey = HELD;
	}
	else if (cKey == RELEASED) {
		cKey = NONE;
	}

	if (vKey == PRESSED) {
		vKey = HELD;
	}
	else if (vKey == RELEASED) {
		vKey = NONE;
	}

	if (bKey == PRESSED) {
		bKey = HELD;
	}
	else if (bKey == RELEASED) {
		bKey = NONE;
	}

	if (nKey == PRESSED) {
		nKey = HELD;
	}
	else if (nKey == RELEASED) {
		nKey = NONE;
	}

	if (mKey == PRESSED) {
		mKey = HELD;
	}
	else if (mKey == RELEASED) {
		mKey = NONE;
	}

	if (escapeKey == PRESSED) {
		escapeKey = HELD;
	}
	else if (escapeKey == RELEASED) {
		escapeKey = NONE;
	}

	if (spaceKey == PRESSED) {
		spaceKey = HELD;
	}
	else if (spaceKey == RELEASED) {
		spaceKey = NONE;
	}

	if (shiftKey == PRESSED) {
		shiftKey = HELD;
	}
	else if (shiftKey == RELEASED) {
		shiftKey = NONE;
	}
}
