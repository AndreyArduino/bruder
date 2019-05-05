void fBtnPressed() {

	gState.displaySleepTime = gState.secondFrom2018 + gState.displayWorkTime;

	fBtnBeep();
}

void fBtnPressedReset() {

	gButton.upPressedDone = false;
	gButton.downPressedDone = false;
	gButton.OKPressedDone = false;
	gButton.cancelPressedDone = false;

	gButton.upPressedLongDone = false;
	gButton.downPressedLongDone = false;
	gButton.OKPressedLongDone = false;
	gButton.cancelPressedLongDone = false;
}

void fBtnCheckPress() {
	if (!digitalRead(gDevicePin.btnUp)) {

		if (gButton.upTimePressed == 0) {

			gButton.upTimePressed = millis();
		}
		else if (!gButton.upPressed && (millis() - gButton.upTimePressed > gButton.shortPressedTime)) {
			gButton.upPressed = true;
			fBtnPressed();
		}
		else if (!gButton.upPressedLong && (millis() - gButton.upTimePressed > gButton.longPressedTime)) {
			gButton.upPressedLong = true;
			fBtnPressed();
		}
	}

	else {

		if ((gButton.upTimePressed != 0) && (millis() - gButton.upTimePressed < gButton.shortPressedTime)) {
			gButton.upTimePressed = 0;
		}
		else if (gButton.upTimePressed != 0 && gButton.upPressedLong) {
			gButton.upTimePressed = 0;
			gButton.upPressed = false;
			gButton.upPressedLong = false;
			gButton.upPressedLongDone = true;
		}
		else if (gButton.upTimePressed != 0 && gButton.upPressed) {
			gButton.upTimePressed = 0;
			gButton.upPressed = false;
			gButton.upPressedLong = false;
			gButton.upPressedDone = true;
		}
	}
	if (!digitalRead(gDevicePin.btnDown)) {

		if (gButton.downTimePressed == 0) {
			gButton.downTimePressed = millis();
		}
		else if (!gButton.downPressed && (millis() - gButton.downTimePressed > gButton.shortPressedTime)) {
			gButton.downPressed = true;
			fBtnPressed();
		}
		else if (!gButton.downPressedLong && (millis() - gButton.downTimePressed > gButton.longPressedTime)) {
			gButton.downPressedLong = true;
			fBtnPressed();
		}
	}

	else {

		if ((gButton.downTimePressed != 0) && (millis() - gButton.downTimePressed < gButton.shortPressedTime)) {

			gButton.downTimePressed = 0;
		}
		else if (gButton.downTimePressed != 0 && gButton.downPressedLong) {
			gButton.downTimePressed = 0;
			gButton.downPressed = false;
			gButton.downPressedLong = false;
			gButton.downPressedLongDone = true;
		}
		else if (gButton.downTimePressed != 0 && gButton.downPressed) {
			gButton.downTimePressed = 0;
			gButton.downPressed = false;
			gButton.downPressedLong = false;
			gButton.downPressedDone = true;
		}
	}
	if (!digitalRead(gDevicePin.btnOK)) {

		if (gButton.OKTimePressed == 0) {

			gButton.OKTimePressed = millis();
		}
		else if (!gButton.OKPressed && (millis() - gButton.OKTimePressed > gButton.shortPressedTime)) {
			gButton.OKPressed = true;
			fBtnPressed();
		}
		else if (!gButton.OKPressedLong && (millis() - gButton.OKTimePressed > gButton.longPressedTime)) {
			gButton.OKPressedLong = true;
			fBtnPressed();
		}
	}

	else {

		if ((gButton.OKTimePressed != 0) && (millis() - gButton.OKTimePressed < gButton.shortPressedTime)) {

			gButton.OKTimePressed = 0;
		}
		else if (gButton.OKTimePressed != 0 && gButton.OKPressedLong) {
			gButton.OKTimePressed = 0;
			gButton.OKPressed = false;
			gButton.OKPressedLong = false;
			gButton.OKPressedLongDone = true;
		}
		else if (gButton.OKTimePressed != 0 && gButton.OKPressed) {
			gButton.OKTimePressed = 0;
			gButton.OKPressed = false;
			gButton.OKPressedLong = false;
			gButton.OKPressedDone = true;
		}
	}
	if (!digitalRead(gDevicePin.btnCancel)) {

		if (gButton.cancelTimePressed == 0) {

			gButton.cancelTimePressed = millis();
		}
		else if (!gButton.cancelPressed && (millis() - gButton.cancelTimePressed > gButton.shortPressedTime)) {
			gButton.cancelPressed = true;
			fBtnPressed();
		}
		else if (!gButton.cancelPressedLong && (millis() - gButton.cancelTimePressed > gButton.longPressedTime)) {
			gButton.cancelPressedLong = true;
			fBtnPressed();
		}
	}

	else {

		if ((gButton.cancelTimePressed != 0) && (millis() - gButton.cancelTimePressed < gButton.shortPressedTime)) {
			gButton.cancelTimePressed = 0;
		}
		else if (gButton.cancelTimePressed != 0 && gButton.cancelPressedLong) {
			gButton.cancelTimePressed = 0;
			gButton.cancelPressed = false;
			gButton.cancelPressedLong = false;
			gButton.cancelPressedLongDone = true;
		}
		else if (gButton.cancelTimePressed != 0 && gButton.cancelPressed) {
			gButton.cancelTimePressed = 0;
			gButton.cancelPressed = false;
			gButton.cancelPressedLong = false;
			gButton.cancelPressedDone = true;
		}
	}
}