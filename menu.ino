int16_t fReturnDigit(int16_t aValue, const int16_t aMinValue, const int16_t aMaxValue, const bool aForward = true, int16_t aMultiplicity = 1) {
	if (aForward) {
		aValue = aValue + aMultiplicity;
		if (aValue > aMaxValue) {
			aValue = aMinValue;
		}
	}
	else {
		aValue = aValue - aMultiplicity;
		if (aValue < aMinValue) {
			aValue = aMaxValue;
		}
	}
	return aValue;
} // fReturnDigit(int16_t aValue, const int16_t aMinValue, const int16_t aMaxValue, const bool aForward = true, int16_t aMultiplicity = 1)

void fMenuExit() {
	//Serial.print("ML OUT - "); Serial.println(gMenu.menuLevel);
	gMenu.menuPos = gMenu.menuLevel % 10;
	//Serial.print("MP OUT - "); Serial.println(gMenu.menuLevel);
	if (gMenu.menuPos > 0) {
		gMenu.menuPos--;
	}
	//Serial.print("MP OUT1 - "); Serial.println(gMenu.menuLevel);
	gMenu.menuLevel = gMenu.menuLevel / 10;
	//Serial.print("ML OUT1 - "); Serial.println(gMenu.menuLevel);

	gMenu.menuDisplayed = false;
	gMenu.menuMaxPos = 0;
}// fMenuExit()

void fMenuListDraw(const char** aMenuList, uint8_t aMenuSize) {
	uint8_t lMenuSize = aMenuSize;
	for (uint8_t i = 0; i < lMenuSize; i++)
	{
		gDisplay.print(aMenuList[i], 9, i * 2 + 1, 2);
	}

	gDisplay.print(">", 0, gMenu.menuPos * 2 + 1, 2);
} // fMenuListDraw(const char** aMenuList, uint8_t aMenuSize)

void fMenuListAction(const char** aMenuList, uint8_t aMenuSize) {
	if (gButton.upPressedDone) {
		gButton.upPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == 0) {
			gMenu.menuPos = aMenuSize - 1;
		}
		else {
			gMenu.menuPos--;
		}
		gDisplay.print(" ", 0, gMenu.menuPrevPos * 2 + 1, 2);
		gDisplay.print(">", 0, gMenu.menuPos * 2 + 1, 2);
	}
	else if (gButton.downPressedDone) {
		gButton.downPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == aMenuSize - 1) {
			gMenu.menuPos = 0;
		}
		else {
			gMenu.menuPos++;
		}
		gDisplay.print(" ", 0, gMenu.menuPrevPos * 2 + 1, 2);
		gDisplay.print(">", 0, gMenu.menuPos * 2 + 1, 2);
	}
	else if (gButton.OKPressedDone) {
		gButton.OKPressedDone = false;
		// войдем в выбранное подменю
		gMenu.menuLevel = gMenu.menuLevel * 10 + (gMenu.menuPos + 1);
		//Serial.print("ML IN - "); Serial.println(gMenu.menuLevel);

		gMenu.menuPos = 0;

		gMenu.menuDisplayed = false;
	}
	else if (gButton.cancelPressedDone) {
		gButton.cancelPressedDone = false;
		fMenuExit();
	}
} // fMenuListAction(const char** aMenuList, uint8_t aMenuSize)

void fMenuRun() {

	if (gButton.upPressedDone) {
		gButton.upPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;

		if (gMenu.menuPos == 0) {
			gMenu.menuPos = 6;
		}
		else {
			gMenu.menuPos--;
		}
	}
	else if (gButton.downPressedDone) {
		gButton.downPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;

		if (gMenu.menuPos == 6) {
			gMenu.menuPos = 0;
		}
		else {
			gMenu.menuPos++;
		}
	}
	else if (gButton.OKPressedDone) {
		gButton.OKPressedDone = false;

		switch (gMenu.menuPos)
		{
		case 0:  
			gMenu.menuRunDay = fReturnDigit(gMenu.menuRunDay, 1, gMenu.menuMaxRunDay);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
			gDisplay.print(gDisplayBuff, 36, 1, 2, 1);
			break;
		case 1:  
			gSetupDate.endDay = fReturnDigit(gSetupDate.endDay, 1, gMenu.menuMaxRunDay);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
			gDisplay.print(gDisplayBuff, 90, 1, 2, 1);
			break;
		case 2:  
			gSetupDate.tempStart = fReturnDigit(gSetupDate.tempStart, 200, 3800, true, 100);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
			gDisplay.print(gDisplayBuff, 36, 3, 2, 1);
			break;
		case 3:  
			gSetupDate.tempEnd = fReturnDigit(gSetupDate.tempEnd, 100, 3700, true, 100);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
			gDisplay.print(gDisplayBuff, 90, 3, 2, 1);
			break;
		case 4:  
			gSetupDate.lightLevel = fReturnDigit(gSetupDate.lightLevel, 10, 100, true, 10);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
			gDisplay.print(gDisplayBuff, 27, 5, 2, 1);
			gState.lightPWM = uint8_t(pow((gSetupDate.lightLevel * 255)/100, 2.35) / 1773);
			analogWrite(gDevicePin.lightPower, gState.lightPWM);
			break;
		case 5:  
			gDisplay.clear();
			gSetupDate.systemMode = gMenu.menuLevel;
			gSetupDate.startMin = gState.minuteFrom2018 - (uint32_t)((gMenu.menuRunDay - 1) * 1440UL);
			gSetupDate.DTRestart = gDT;
			gSetupDate.restartCount = 0;

			EEPROM.put(0, gSetupDate);
			fControlUpdate();
			gMenu.menuDisplayed = false;

			gAlarm.disableAlarm = false;
			gAlarm.errorAlarm = false;
			gAlarm.infoAlarm = false;
			break;
		case 6: 
			fMenuExit();
			break;
		default:
			break;
		}
	}
	else if (gButton.cancelPressedDone) {
		gButton.cancelPressedDone = false;

		switch (gMenu.menuPos)
		{
		case 0:  
			gMenu.menuRunDay = fReturnDigit(gMenu.menuRunDay, 1, gMenu.menuMaxRunDay, false);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
			gDisplay.print(gDisplayBuff, 36, 1, 2, 1);
			break;
		case 1:  
			gSetupDate.endDay = fReturnDigit(gSetupDate.endDay, 1, gMenu.menuMaxRunDay, false);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
			gDisplay.print(gDisplayBuff, 90, 1, 2, 1);
			break;
		case 2:  
			gSetupDate.tempStart = fReturnDigit(gSetupDate.tempStart, 200, 3800, false, 100);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
			gDisplay.print(gDisplayBuff, 36, 3, 2, 1);
			break;
		case 3:  
			gSetupDate.tempEnd = fReturnDigit(gSetupDate.tempEnd, 100, 3700, false, 100);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
			gDisplay.print(gDisplayBuff, 90, 3, 2, 1);
			break;
		case 4:  
			gSetupDate.lightLevel = fReturnDigit(gSetupDate.lightLevel, 10, 100, false, 10);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
			gDisplay.print(gDisplayBuff, 27, 5, 2, 1);
			gState.lightPWM = uint8_t(pow((gSetupDate.lightLevel * 255) / 100, 2.35) / 1773);
			analogWrite(gDevicePin.lightPower, gState.lightPWM);
			break;
		case 5:  
			break;
		case 6: 
			fMenuExit();
			break;
		default:
			break;
		}
	}
	if (gMenu.menuPrevPos != gMenu.menuPos) {
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
		switch (gMenu.menuPos)
		{
		case 0: 

			sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
			gDisplay.print(gDisplayBuff, 36, 1, 2, 1);

			sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
			gDisplay.print(gDisplayBuff, 90, 1, 2);
			gDisplay.print(" ", 90, 6, 3);
			break;
		case 1: 

			sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
			gDisplay.print(gDisplayBuff, 90, 1, 2, 1);

			sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
			gDisplay.print(gDisplayBuff, 36, 1, 2);
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
			gDisplay.print(gDisplayBuff, 36, 3, 2);
			break;
		case 2: 

			sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
			gDisplay.print(gDisplayBuff, 36, 3, 2, 1);

			sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
			gDisplay.print(gDisplayBuff, 90, 1, 2);
			sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
			gDisplay.print(gDisplayBuff, 90, 3, 2);
			break;
		case 3: 

			sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
			gDisplay.print(gDisplayBuff, 90, 3, 2, 1);

			sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
			gDisplay.print(gDisplayBuff, 36, 3, 2);
			sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
			gDisplay.print(gDisplayBuff, 27, 5, 2);
			break;
		case 4: 

			sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
			gDisplay.print(gDisplayBuff, 27, 5, 2, 1);

			sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
			gDisplay.print(gDisplayBuff, 90, 3, 2);
			gDisplay.print(" ", 90, 6, 3);
			break;
		case 5: 

			gDisplay.print(" ", 90, 6, 3, 1);

			sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
			gDisplay.print(gDisplayBuff, 27, 5, 2);
			gDisplay.print("!", 114, 6, 3);
			break;
		case 6:  

			gDisplay.print("!", 114, 6, 3, 1);

			gDisplay.print(" ", 90, 6, 3);
			sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
			gDisplay.print(gDisplayBuff, 36, 1, 2);
		break;
		default:
			gMenu.menuPos = 0;
			break;
		}
		gMenu.menuPrevPos = gMenu.menuPos;
	}
} // fMenuRun() 

void fMenuSetupDateTime() {

	if (gButton.upPressedDone) {
		gButton.upPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == 0) {
			gMenu.menuPos = 7;
		}
		else {
			gMenu.menuPos--;
		}
	}
	else if (gButton.downPressedDone) {
		gButton.downPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == 7) {
			gMenu.menuPos = 0;
		}
		else {
			gMenu.menuPos++;
		}
	}
	else if (gButton.OKPressedDone) {
		gButton.OKPressedDone = false;
		gDisplayBuff[0] = '\0';

		switch (gMenu.menuPos)
		{
		case 0:

			gSetupDT.day = fReturnDigit(gSetupDT.day, 1, 31);
			sprintf(gDisplayBuff, "%02d", gSetupDT.day);
			gDisplay.print(gDisplayBuff, 54, 1, 2, 1);
			break;
		case 1:

			gSetupDT.month = fReturnDigit(gSetupDT.month, 1, 12);
			sprintf(gDisplayBuff, "%02d", gSetupDT.month);
			gDisplay.print(gDisplayBuff, 78, 1, 2, 1);
			break;
		case 2:

			gSetupDT.year = fReturnDigit(gSetupDT.year, 2018, 2099);
			sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
			gDisplay.print(gDisplayBuff, 102, 1, 2, 1);
			break;
		case 3:

			gSetupDT.hour = fReturnDigit(gSetupDT.hour, 0, 23);
			sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
			gDisplay.print(gDisplayBuff, 54, 3, 2, 1);
			break;
		case 4:

			gSetupDT.minute = fReturnDigit(gSetupDT.minute, 0, 59);
			sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
			gDisplay.print(gDisplayBuff, 78, 3, 2, 1);
			break;
		case 5:

			gSetupDT.second = fReturnDigit(gSetupDT.second, 0, 59);
			sprintf(gDisplayBuff, "%02d", gSetupDT.second);
			gDisplay.print(gDisplayBuff, 78, 3, 2, 1);
			break;
		case 6:

			gRTC.setDateTime(gSetupDT);

			gState.secondFrom2018 = gRTC.getSecondFrom18Year();
			gState.displaySleepTime = gState.secondFrom2018 + gState.displayWorkTime;
			gState.displaySleep = false;
			fMenuExit();
			break;
		case 7:

			fMenuExit();
			break;
		default:
			break;
		}
	}
	else if (gButton.cancelPressedDone) {
		gButton.cancelPressedDone = false;
		gDisplayBuff[0] = '\0';

		switch (gMenu.menuPos)
		{
		case 0:

			gSetupDT.day = fReturnDigit(gSetupDT.day, 1, 31, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.day);
			gDisplay.print(gDisplayBuff, 54, 1, 2, 1);
			break;
		case 1:

			gSetupDT.month = fReturnDigit(gSetupDT.month, 1, 12, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.month);
			gDisplay.print(gDisplayBuff, 78, 1, 2, 1);
			break;
		case 2:

			gSetupDT.year = fReturnDigit(gSetupDT.year, 2018, 2099, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
			gDisplay.print(gDisplayBuff, 102, 1, 2, 1);
			break;
		case 3:

			gSetupDT.hour = fReturnDigit(gSetupDT.hour, 0, 23, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
			gDisplay.print(gDisplayBuff, 54, 3, 2, 1);
			break;
		case 4:

			gSetupDT.minute = fReturnDigit(gSetupDT.minute, 0, 59, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
			gDisplay.print(gDisplayBuff, 78, 3, 2, 1);
			break;
		case 5:

			gSetupDT.second = fReturnDigit(gSetupDT.second, 0, 59, false);
			sprintf(gDisplayBuff, "%02d", gSetupDT.second);
			gDisplay.print(gDisplayBuff, 78, 3, 2, 1);
			break;
		case 6:

			break;
		case 7:

			fMenuExit();
			break;
		default:
			break;
		}
	}
	if (gMenu.menuPrevPos != gMenu.menuPos) {
		gDisplayBuff[0] = '\0';
		switch (gMenu.menuPos)
		{
		case 0:

			gDisplay.print("!", 112, 5, 3);
			sprintf(gDisplayBuff, "%02d", gSetupDT.day);
			gDisplay.print(gDisplayBuff, 54, 1, 2, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.month);
			gDisplay.print(gDisplayBuff, 78, 1, 2);
			break;
		case 1:

			sprintf(gDisplayBuff, "%02d", gSetupDT.day);
			gDisplay.print(gDisplayBuff, 54, 1, 2);
			sprintf(gDisplayBuff, "%02d", gSetupDT.month);
			gDisplay.print(gDisplayBuff, 78, 1, 2, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
			gDisplay.print(gDisplayBuff, 102, 1, 2);
			break;
		case 2:

			sprintf(gDisplayBuff, "%02d", gSetupDT.month);
			gDisplay.print(gDisplayBuff, 78, 1, 2);
			sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
			gDisplay.print(gDisplayBuff, 102, 1, 2, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
			gDisplay.print(gDisplayBuff, 54, 3, 2);
			break;
		case 3:

			sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
			gDisplay.print(gDisplayBuff, 102, 1, 2);
			sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
			gDisplay.print(gDisplayBuff, 54, 3, 2, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
			gDisplay.print(gDisplayBuff, 78, 3, 2);
			break;
		case 4:

			sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
			gDisplay.print(gDisplayBuff, 54, 3, 2);
			sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
			gDisplay.print(gDisplayBuff, 78, 3, 2, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.second);
			gDisplay.print(gDisplayBuff, 102, 3, 2);
			break;
		case 5:

			sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
			gDisplay.print(gDisplayBuff, 78, 3, 2);
			sprintf(gDisplayBuff, "%02d", gSetupDT.second);
			gDisplay.print(gDisplayBuff, 102, 3, 2, 1);
			gDisplay.print(" ", 88, 5, 3);
			break;
		case 6:

			sprintf(gDisplayBuff, "%02d", gSetupDT.second);
			gDisplay.print(gDisplayBuff, 102, 3, 2);
			gDisplay.print(" ", 88, 5, 3, 1);
			gDisplay.print("!", 112, 5, 3);
			break;
		case 7:

			gDisplay.print(" ", 88, 5, 3);
			gDisplay.print("!", 112, 5, 3, 1);
			sprintf(gDisplayBuff, "%02d", gSetupDT.day);
			gDisplay.print(gDisplayBuff, 54, 1, 2);
			break;
		default:
			gMenu.menuPos = 0;
			break;
		}
		gMenu.menuPrevPos = gMenu.menuPos;
	}
}

void fMenuSetupDS() {

	if (gButton.upPressedDone) {
		gButton.upPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == 0) {
			gMenu.menuPos = 4;
		}
		else {
			gMenu.menuPos--;
		}
	}
	else if (gButton.downPressedDone) {
		gButton.downPressedDone = false;
		gMenu.menuPrevPos = gMenu.menuPos;
		// перемещаем курсор
		if (gMenu.menuPos == 4) {
			gMenu.menuPos = 0;
		}
		else {
			gMenu.menuPos++;
		}
	}
	else if (gButton.OKPressedDone) {
		gButton.OKPressedDone = false;

		switch (gMenu.menuPos)
		{
		case 0:  
			gSetupDate.numSensor[0] = fReturnDigit(gSetupDate.numSensor[0], 1, 2);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
			gDisplay.print(gDisplayBuff, 64, 1, 2, 1);
			fDisplayTemperature();
			break;
		case 1:  
			gSetupDate.numSensor[1] = fReturnDigit(gSetupDate.numSensor[1], 1, 2);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
			gDisplay.print(gDisplayBuff, 64, 3, 2, 1);
			fDisplayTemperature();
			break;
		case 2: 

			fControlResetDS();
			gMenu.menuLevel = 323;
			break;
		case 3: 

			EEPROM.put(0, gSetupDate);
			fMenuExit();
			break;
		case 4: 

			EEPROM.get(0, gSetupDate);
			fMenuExit();
			break;
		default:
			break;
		}
	}
	else if (gButton.cancelPressedDone) {
		gButton.cancelPressedDone = false;

		switch (gMenu.menuPos)
		{
		case 0:  
			gSetupDate.numSensor[0] = fReturnDigit(gSetupDate.numSensor[0], 1, 2, false);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
			gDisplay.print(gDisplayBuff, 64, 1, 2, 1);
			fDisplayTemperature();
			break;
		case 1:  
			gSetupDate.numSensor[1] = fReturnDigit(gSetupDate.numSensor[1], 1, 2, false);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
			gDisplay.print(gDisplayBuff, 64, 3, 2, 1);
			fDisplayTemperature();
			break;
		case 2: 
			break;
		case 3: 
			break;
		case 4: 

			EEPROM.get(0, gSetupDate);
			fMenuExit();
			break;
		default:
			break;
		}
	}
	if (gMenu.menuPrevPos != gMenu.menuPos) {
		gDisplayBuff[0] = '\0';
		switch (gMenu.menuPos)
		{
		case 0:
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
			gDisplay.print(gDisplayBuff, 64, 1, 2, 1);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
			gDisplay.print(gDisplayBuff, 64, 3, 2);
			gDisplay.print("!", 112, 5, 3);
			break;
		case 1:
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
			gDisplay.print(gDisplayBuff, 64, 1, 2);
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
			gDisplay.print(gDisplayBuff, 64, 3, 2, 1);
			gDisplay.print("СБРОС", 0, 5, 2);
			break;
		case 2:
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
			gDisplay.print(gDisplayBuff, 64, 3, 2);
			gDisplay.print("СБРОС", 0, 5, 2, 1);
			gDisplay.print(" ", 88, 5, 3);
			break;
		case 3:
			gDisplay.print("СБРОС", 0, 5, 2);
			gDisplay.print(" ", 88, 5, 3, 1);
			gDisplay.print("!", 112, 5, 3);
			break;
		case 4:
			sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
			gDisplay.print(gDisplayBuff, 64, 1, 2);
			gDisplay.print(" ", 88, 5, 3);
			gDisplay.print("!", 112, 5, 3, 1);
			break;
		default:
			gMenu.menuPos = 0;
			break;
		}
		gMenu.menuPrevPos = gMenu.menuPos;
	}
}

void fMenuResetDS() {
	if (gButton.upPressedDone) {
		gButton.upPressedDone = false;
	}
	else if (gButton.downPressedDone) {
		gButton.downPressedDone = false;
	}
	else if (gButton.OKPressedDone) {
		gButton.OKPressedDone = false;

		gMenu.menuLevel = 32;
		gMenu.menuPos = 0;
		gMenu.menuDisplayed = false;
	}
	else if (gButton.cancelPressedDone) {
		gButton.cancelPressedDone = false;
	}
}

void fMenuDraw() {
	if (gMenu.menuDisplayed) {

		return;
	}

	gDisplay.clear();

	if (gMenu.menuLevel == 0) {
		fMenuListDraw(gMenu.menuItem_0, charArraySize(gMenu.menuItem_0));
	}
	if (gMenu.menuLevel == 1 || gMenu.menuLevel == 2) {
		gDisplay.print("'", 0, 1, 3	);
		gDisplay.print("С    ПО", 18, 1, 2);
		gDisplay.print("*", 0, 3, 3);
		gDisplay.print("С    ПО", 18, 3, 2);
		gDisplay.print(")", 0, 5, 3);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", gMenu.menuRunDay);
		gDisplay.print(gDisplayBuff, 36, 1, 2, 1);

		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", gSetupDate.endDay);
		gDisplay.print(gDisplayBuff, 90, 1, 2);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", gSetupDate.tempStart/100);
		gDisplay.print(gDisplayBuff, 36, 3, 2);

		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", gSetupDate.tempEnd/100);
		gDisplay.print(gDisplayBuff, 90, 3, 2);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
		gDisplay.print(gDisplayBuff, 27, 5, 2);
		gDisplay.print(" ", 90, 6, 3);
		gDisplay.print("!", 114, 6, 3);
	}
	if (gMenu.menuLevel == 3) {
		fMenuListDraw(gMenu.menuItem_3, charArraySize(gMenu.menuItem_3));
	}
	if (gMenu.menuLevel == 31) {
		gSetupDT = gRTC.getDateTime();
		gDisplay.print("ДАТА", 0, 1, 2);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%02d", gSetupDT.day);
		gDisplay.print(gDisplayBuff, 54, 1, 2, 1);
		gDisplay.print(".", 74, 1, 2);
		sprintf(gDisplayBuff, "%02d", gSetupDT.month);
		gDisplay.print(gDisplayBuff, 78, 1, 2);
		gDisplay.print(".", 98, 1, 2);
		sprintf(gDisplayBuff, "%02d", gSetupDT.year - 2000);
		gDisplay.print(gDisplayBuff, 102, 1, 2);
		gDisplay.print("ВРЕМЯ", 0, 3, 2);
		sprintf(gDisplayBuff, "%02d", gSetupDT.hour);
		gDisplay.print(gDisplayBuff, 54, 3, 2);
		gDisplay.print(":", 74, 3, 2);
		sprintf(gDisplayBuff, "%02d", gSetupDT.minute);
		gDisplay.print(gDisplayBuff, 78, 3, 2);
		gDisplay.print(":", 98, 3, 2);
		sprintf(gDisplayBuff, "%02d", gSetupDT.second);
		gDisplay.print(gDisplayBuff, 102, 3, 2);
		// рисуем кнопку ОК и отмена
		gDisplay.print(" ", 88, 5, 3);
		gDisplay.print("!", 112, 5, 3);
	}

	if (gMenu.menuLevel == 32) {

		gDisplay.print("НАГРЕВ", 0, 1, 2);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[0]);
		gDisplay.print(gDisplayBuff, 64, 1, 2, 1);

		gDisplay.print("ВОЗДУХ", 0, 3, 2);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%1d", gSetupDate.numSensor[1]);
		gDisplay.print(gDisplayBuff, 64, 3, 2);
		gDisplay.print("СБРОС", 0, 5, 2);
		// рисуем кнопку ОК и отмена
		gDisplay.print(" ", 88, 5, 3);
		gDisplay.print("!", 112, 5, 3);
		fDisplayTemperature();

	}
	fControlUpdate();
	gMenu.menuDisplayed = true;
} // fMenuDraw()

void fMenuAction() {
	if (!(gButton.upPressedDone || gButton.downPressedDone || gButton.OKPressedDone || gButton.cancelPressedDone)) {

		return;
	}

//	gState.displayWakeUpSecond = gSystemState.lCurrentSecondFrom2018;
	if (gMenu.menuLevel == 0) {
		fMenuListAction(gMenu.menuItem_0, charArraySize(gMenu.menuItem_0));
	}

	if (gMenu.menuLevel == 1 || gMenu.menuLevel == 2) {
		fMenuRun();
	}

	if (gMenu.menuLevel == 3) {
		fMenuListAction(gMenu.menuItem_3, charArraySize(gMenu.menuItem_3));
	}

	if (gMenu.menuLevel == 31) {
		fMenuSetupDateTime();
	}
	if (gMenu.menuLevel == 32) {
		fMenuSetupDS();
	}
	if (gMenu.menuLevel == 323) {
		fMenuResetDS();
	}

} // fMenuAction()

void fRunMenu() {

	fMenuDraw();

} // fRunMenu()
