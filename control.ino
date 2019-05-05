void fControlState() {

	if (gSetupDate.systemMode == 0) {
	}

	else if (gSetupDate.systemMode > 0) {

		uint8_t lRunDay = (gState.minuteFrom2018 - gSetupDate.startMin) / 1440UL + 1UL;

		gState.tempRequired = gSetupDate.tempEnd + (gSetupDate.tempStart - gSetupDate.tempEnd) / 100 * (int16_t)max((10000L - ((lRunDay - 1) * 100 / (gSetupDate.endDay - 1) * 100)) / 100, 0);
		if (lRunDay >= gSetupDate.lightOffDay) {
			gState.lightOffHour = ((uint16_t)gSetupDate.lightMaxHour * 100 / (gSetupDate.endDay - gSetupDate.lightOffDay)*(lRunDay + 1 - gSetupDate.lightOffDay) / 100) + 1;
			if (gState.lightOffHour > gSetupDate.lightMaxHour) {
				gState.lightOffHour = gSetupDate.lightMaxHour;
			}
		}
		else {
			gState.lightOffHour = 0;
		}
		fDisplayState(lRunDay);
	}
}
void fControlTemperature() {

	if (gSetupDate.systemMode == 0) {

		if (digitalRead(gDevicePin.heaterPower)) {

			digitalWrite(gDevicePin.heaterPower, LOW);
		}
	}

	else if (gSetupDate.systemMode > 0) {

		if (gHeaterTemperature.sensorFixValue >= (gAlarm.heaterMax * 100) && digitalRead(gDevicePin.heaterPower)) {
			digitalWrite(gDevicePin.heaterPower, LOW);
			gAlarm.heaterAlarm = true;
		}
		else if (gHeaterTemperature.sensorFixValue < (gAlarm.heaterMax * 100)) {

			gAlarm.heaterAlarm = false;
		}

		if ((abs(gTemperature.sensorFixValue - gState.tempRequired) >= (gAlarm.tempDeviation * 100))) {

			gAlarm.tempAlarm = true;
		}
		else {

			gAlarm.tempAlarm = false;
		}
		if (!gAlarm.heaterAlarm) {

			if (!digitalRead(gDevicePin.heaterPower) && gTemperature.sensorFixValue < gState.tempRequired) {

				digitalWrite(gDevicePin.heaterPower, HIGH);
			}
			else if (digitalRead(gDevicePin.heaterPower) && gTemperature.sensorFixValue >= gState.tempRequired) {

				digitalWrite(gDevicePin.heaterPower, LOW);

				if (gSetupDate.systemMode == 1) {

					gAlarm.tempDone = true;
				}
			}
		}
	}
	fDisplayTemperature();
}
void fControlLight() {
	const uint8_t lLighhtStepInterval = 1;
	static uint8_t lCurrentSecond = 0;

	if (gState.secondFrom2018 - lCurrentSecond > lLighhtStepInterval) {
		lCurrentSecond = gState.secondFrom2018;

		uint8_t lCurrenHour = gRTC.getHour();

		if ((lCurrenHour >= gState.lightOffHour || gSetupDate.systemMode == 0) && gState.lightPWM <= (255UL * gSetupDate.lightLevel) / 100) {

			gState.lightPWM++;

			uint8_t lightPWMExp = uint8_t(pow(gState.lightPWM, 2.35) / 1773);
			analogWrite(gDevicePin.lightPower, lightPWMExp);
		}

		else if (lCurrenHour < gState.lightOffHour && gState.lightPWM > 0) {

			gState.lightPWM--;

			uint8_t lightPWMExp = uint8_t(pow(gState.lightPWM, 2.35) / 1773);
			analogWrite(gDevicePin.lightPower, lightPWMExp);
		}
		fDisplayLight();
	}
}

void fControlResetDS() {
	gDisplay.clear(1, 7);

	gSensorDS.reset_search();

	for (uint8_t i = 0; i <= 1; i++) {

		gDisplayBuff[0] = '\0';
		sprintf(gDisplayBuff, "СЕНСОР - %1d", i + 1);
		gDisplay.print(gDisplayBuff, 0, (i * 2) + 1, 1);
		gSetupDate.numSensor[i] = 0;
		if (gSensorDS.search(gSetupDate.addrDS[i])) {

			if (OneWire::crc8(gSetupDate.addrDS[i], 7) != gSetupDate.addrDS[i][7]) {

				gSetupDate.addrDS[i][0] = 0;

				gDisplay.print("CRC ERR", 54, (i * 2) + 1, 1);
				continue;
			}
			gDisplay.print("CRC OK", 54, (i * 2) + 1, 1);
			if (gSetupDate.addrDS[i][0] != 40) {

				gSetupDate.addrDS[i][0] = 0;

				gDisplay.print("TYPE ERR", 88, (i * 2) + 1, 1);
				continue;
			}
			gDisplay.print("TYPE OK", 88, (i * 2) + 1, 1);
		}
		else {

			gSetupDate.addrDS[i][0] = 0;
			gDisplay.print("НЕ ОБНАРУЖЕН", 54, (i * 2) + 1, 1);
		}
	}

	for (uint8_t i = 0; i <= 1; i++) {

		if (gSetupDate.addrDS[i][0] == 40) {
			gSensorDS.reset(); 
			gSensorDS.select(gSetupDate.addrDS[i]); 
			gSensorDS.write(0x4E); 
			gSensorDS.write(0x7F); 
			gSensorDS.write(0xFF); 
			gSensorDS.write(0x60); 

			gDisplayBuff[0] = '\0';
			sprintf(gDisplayBuff, "ID - %02X.%02X.%02X.%02X.%02X.%02X.%02X.%02X", gSetupDate.addrDS[i][0], gSetupDate.addrDS[i][1], gSetupDate.addrDS[i][2], gSetupDate.addrDS[i][3], gSetupDate.addrDS[i][4], gSetupDate.addrDS[i][5], gSetupDate.addrDS[i][6], gSetupDate.addrDS[i][7]);
			gDisplay.print(gDisplayBuff, 0, (i * 2) + 2, 1);
			gSetupDate.numSensor[i] = i + 1;
		}
	}

	EEPROM.put(0, gSetupDate);

	gDisplay.print("OK", 55, 5, 2, 1);
}
void fControlUpdate() {
	fDisplayDateTime();
	fControlState();
	fControlTemperature();
	fControlLight();
} 
void fControlAbortMode() {
	gMenu.menuPos = 0;
	gMenu.menuLevel = 0;
	gMenu.menuDisplayed = false;
	gMenu.menuMaxPos = 0;
	gSetupDate.systemMode = 0;
	EEPROM.put(0, gSetupDate);

	gAlarm.disableAlarm = false;
	gAlarm.errorAlarm = false;
	gAlarm.infoAlarm = false;
}
