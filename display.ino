void fDisplayState(const uint8_t aRunDay) {

	if (gSetupDate.systemMode == 0) {

		gDisplayBuff[0] = '\0';
	}

	else if (gSetupDate.systemMode > 0) {

		gDisplay.print("'", 0, 0, 3);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d", aRunDay);
		gDisplay.print(gDisplayBuff, 16, 0, 2);
	}
}

void fDisplayTemperature() {

	if (gSetupDate.systemMode == 0) {

		if(gMenu.menuLevel == 32){
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d.%1d", gHeaterTemperature.sensorFixValue / 100, (gHeaterTemperature.sensorFixValue % 100) / 10);
			gDisplay.print(gDisplayBuff, 82, 1, 2);

			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "%2d.%1d", gTemperature.sensorFixValue / 100, (gTemperature.sensorFixValue % 100) / 10);
			gDisplay.print(gDisplayBuff, 82, 3, 2);
		}
		else{
			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "Т%2d.%1d ТН%2d.%1d", gTemperature.sensorFixValue / 100, (gTemperature.sensorFixValue % 100) / 10, gHeaterTemperature.sensorFixValue / 100, (gHeaterTemperature.sensorFixValue % 100) / 10);
			gDisplay.print(gDisplayBuff, 0, 0, 1);
		}
	}

	else if (gSetupDate.systemMode > 0) {
		gDisplay.print("*", 0, 2, 3);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d.%1d", gTemperature.sensorFixValue / 100, (gTemperature.sensorFixValue % 100) / 10);
		gDisplay.print(gDisplayBuff, 16, 2, 2, gAlarm.tempAlarm);
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "(%2d.%1d)", gState.tempRequired / 100, (gState.tempRequired % 100) / 10);
		gDisplay.print(gDisplayBuff, 16, 4, 1);

		gDisplay.print("+", 58, 2, 3, digitalRead(gDevicePin.heaterPower));
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%2d.%1d", gHeaterTemperature.sensorFixValue / 100, (gHeaterTemperature.sensorFixValue % 100) / 10);
		gDisplay.print(gDisplayBuff, 76, 2, 2, gAlarm.heaterAlarm);
		sprintf(gDisplayBuff, "(%2d)", gAlarm.heaterMax);
		gDisplay.print(gDisplayBuff, 80, 4, 1);
	}
}

void fDisplayLight() {

	if (gSetupDate.systemMode == 0) {

		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
		gDisplay.print(gDisplayBuff, 0, 7, 1);

	}

	else if (gSetupDate.systemMode > 0) {

		if (gState.lightPWM == 0) {
			gDisplay.print("(", 114, 2, 3);
		}
		else {
			gDisplay.print(")", 114, 2, 3);
		}
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%3d%%", gSetupDate.lightLevel);
		gDisplay.print(gDisplayBuff, 106, 4, 1);
	}
}

void fDisplayDateTime() {
	gDisplayBuff[0] = '\0';

	if (gSetupDate.systemMode == 0) {

		sprintf(gDisplayBuff, "%02d.%02d %02d:%02d:%02d", gDT.day, gDT.month, gDT.hour, gDT.minute, gDT.second);
		gDisplay.print(gDisplayBuff, 64, 0, 1);
	}

	else if (gSetupDate.systemMode > 0) {

		gDisplay.print(gDisplayBuff, 38, 0, 1);
		//дата и время последней загрузки
		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "%02d.%02d %02d:%02d:%02d %02d", gSetupDate.DTRestart.day, gSetupDate.DTRestart.month, gSetupDate.DTRestart.hour, gSetupDate.DTRestart.minute, gSetupDate.DTRestart.second, gSetupDate.restartCount);
		gDisplay.print(gDisplayBuff, 38, 1, 1);
	}
}

