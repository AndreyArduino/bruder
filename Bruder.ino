#define charArraySize(charArray) (sizeof(charArray)/sizeof(charArray[0]))

#include <avr/wdt.h>

#include <OneWire.h>
// библиотека для EEPROM
#include <EEPROM.h>

#include <Wire.h>

#include "ask_ds3231.h"
ask_ds3231 gRTC;

RTCDateTime gDT;

RTCDateTime gSetupDT;

#include "ask_ssd1306.h"
ask_ssd1306 gDisplay;

struct {
	const uint8_t heaterPower = 4;  
	const uint8_t lightPower = 5;  
	const uint8_t wireData = 7;
	const uint8_t alarmBeeper = 8;  
	const uint8_t btnUp = A3;
	const uint8_t btnDown = A2;
	const uint8_t btnOK = A1;
	const uint8_t btnCancel = A0;
} gDevicePin;

OneWire gSensorDS(gDevicePin.wireData);

struct {
	uint8_t	 systemMode = 0;
	uint32_t startMin = 0;
	uint8_t	 endDay = 21;
	RTCDateTime DTRestart;
	uint8_t	 restartCount = 0;
	int16_t  tempStart = 3600;
	int16_t  tempEnd = 2500;
	uint8_t  lightOffDay = 7;
	uint8_t  lightMaxHour = 4;
	uint8_t  lightLevel = 50;   
	uint8_t  numSensor[2];
	uint8_t  addrDS[2][8];
} gSetupDate;

struct {
	bool	 abortMode = false;

	int16_t  tempRequired = 0;
	uint8_t	 lightOffHour = 0;
	uint8_t	 lightPWM = 0;

	bool	 displaySleep = false;
	uint32_t displaySleepTime = 0;
	uint8_t  displayWorkTime = 180;

	uint32_t secondFrom2018 = 0;
	uint32_t minuteFrom2018 = 0;
} gState;

struct {
	bool upPressed = false;
	bool upPressedLong = false;
	bool upPressedDone = false;
	bool upPressedLongDone = false;
	uint32_t upTimePressed = 0;
	bool downPressed = false;       
	bool downPressedLong = false;
	bool downPressedDone = false;   
	bool downPressedLongDone = false;   
	uint32_t downTimePressed = 0;   
	bool OKPressed = false;         
	bool OKPressedLong = false;        
	bool OKPressedDone = false;     
	bool OKPressedLongDone = false;    
	uint32_t OKTimePressed = 0;     
	bool cancelPressed = false;     
	bool cancelPressedLong = false;     
	bool cancelPressedDone = false; 
	bool cancelPressedLongDone = false; 
	uint32_t cancelTimePressed = 0; 
	uint8_t shortPressedTime = 60; 
	uint16_t longPressedTime = 3000;  
} gButton;

struct {
	uint16_t menuLevel = 0;
	uint8_t	 menuPrevPos = 0;
	uint8_t  menuPos = 0;
	uint8_t  menuMaxPos = 0;
	bool	 menuDisplayed = false;
	const char* menuItem_0[3] = { "1. ПОДГОТОВКА", "2. БРУДЕР", "3. НАСТРОЙКИ" };
	const char* menuItem_3[2] = { "1. ДАТА/ВРЕМЯ", "2. DS18B20" };
	RTCDateTime menuItem_14;
	uint8_t  menuRunDay = 1;
	const uint8_t  menuMaxRunDay = 21;
} gMenu;

struct {
	bool errorAlarm = false;
	bool infoAlarm = false;
	uint8_t heaterMax = 63;
	uint8_t tempDeviation = 3;
	bool tempAlarm = false;
	bool heaterAlarm = false;
	bool tempDone = false;

	bool disableAlarm = false;
	uint8_t disableAlarmTime = 180; 
} gAlarm;

typedef struct {
	int32_t sensorNowValue;
	int16_t sensorFixValue;
	int16_t deviationValue;
	uint8_t currentIteration;
	uint8_t numberIterations;

	bool updateValue(int32_t aSensorNowValue) {

		currentIteration++;
		sensorNowValue += aSensorNowValue;
		if (currentIteration >= numberIterations) {

			sensorNowValue = sensorNowValue / currentIteration;

			currentIteration = 0;

			if (abs(sensorNowValue - sensorFixValue) >= deviationValue) {
				sensorFixValue = sensorNowValue;

				sensorNowValue = 0;
				return true;
			}
			sensorNowValue = 0;
		}
		return false;
	}
} sensorData;

sensorData gTemperature{ 0, 0, 50, 0, 5 };

sensorData gHeaterTemperature{ 0, 0, 50, 0, 5 };

char gDisplayBuff[32];

void setup() {
	// запустим Serial для отладки
	Serial.begin(115200);

	// установка назначения выводов
	pinMode(gDevicePin.heaterPower, OUTPUT);
	pinMode(gDevicePin.lightPower, OUTPUT);
	pinMode(gDevicePin.alarmBeeper, OUTPUT);
	// установим низкий уровень
	digitalWrite(gDevicePin.heaterPower, LOW);
	digitalWrite(gDevicePin.lightPower, LOW);
	digitalWrite(gDevicePin.alarmBeeper, LOW);
	// настройка выводов для кнопок
	pinMode(gDevicePin.btnUp, INPUT);
	pinMode(gDevicePin.btnDown, INPUT);
	pinMode(gDevicePin.btnOK, INPUT);
	pinMode(gDevicePin.btnCancel, INPUT);

	// стартуем wire, она нужна для всех библиотек I2C
	Wire.begin();

	gRTC.begin();

	if (!gRTC.sensorActivated()) {
		gAlarm.errorAlarm = true;
		while (true){

			fInkBeep();
		}
	}

	gDisplay.begin();

	if (!gDisplay.displayActivated()) {
		gAlarm.errorAlarm = true;
		while (true){

			fInkBeep();
		}
	}
	else {
		gDisplay.clear();
		gDisplay.print("1. ЧАСЫ    - OK", 0, 0, 1);
		gDisplay.print("2. ДИСПЛЕЙ - OK", 0, 1, 1);
	}

	/* ЧИТАЕМ EEPROM */
	EEPROM.get(0, gSetupDate);

	gDT = gRTC.getDateTime();

	gState.secondFrom2018 = gRTC.getSecondFrom18Year(gDT);
	gState.minuteFrom2018 = gRTC.getMinuteFrom18Year(gDT);

	if (gSetupDate.systemMode >= 1) {

		gDisplay.clear();

		gSetupDate.DTRestart = gDT;
		gSetupDate.restartCount++;

		EEPROM.put(0, gSetupDate);
	}
	else {

		if (gSetupDate.systemMode != 0) {

			gSetupDate.systemMode = 0;
			EEPROM.put(0, gSetupDate);
		}

		delay(1000);
	}

	fBtnBeep();
	// очистим дисплей
	gDisplay.clear();

	gState.secondFrom2018 = gRTC.getSecondFrom18Year();
	gState.displaySleepTime = gState.secondFrom2018 + gState.displayWorkTime;
	gState.displaySleep = false;
	fControlUpdate();

	// запустим сторожевой таймер, значение 8 секунд
	wdt_enable(WDTO_8S);
	wdt_reset();
}

void loop() {
	// переменные loop
	static uint8_t lPrewSecond = 0;
	uint8_t lCurrSecond = 0;
	static uint8_t lPrewMinute = 0;
	uint8_t lCurrMinute = 0;
	wdt_reset();
	fInkBeep();
	fBtnCheckPress();

	if (!gState.displaySleep && gState.secondFrom2018 >= gState.displaySleepTime) {
		gState.displaySleep = true;

		gDisplay.sleep();
	}
	else if (gState.displaySleep) {

		if (gButton.upPressedDone || gButton.downPressedDone || gButton.OKPressedDone || gButton.cancelPressedDone) {

			fBtnPressedReset();

			gDisplay.wakeUp();

			gState.displaySleep = false;

			gState.displaySleepTime = gState.secondFrom2018 + gState.displayWorkTime;
		}
	}
	if (fTemperatureDS()) {

		fControlTemperature();
	}
	lCurrSecond = gRTC.getSecond();
	if (lPrewSecond != lCurrSecond) {

		lPrewSecond = lCurrSecond;
		gDT = gRTC.getDateTime();
		gState.secondFrom2018 = gRTC.getSecondFrom18Year(gDT);
		fDisplayDateTime();
		fControlLight();
		//fControlLightFadeTest();
	}
	lCurrMinute = gRTC.getMinute();
	if (lPrewMinute != lCurrMinute) {

		lPrewMinute = lCurrMinute;
		gState.minuteFrom2018 = gRTC.getMinuteFrom18Year(gDT);

		fControlState();
	}
	if (gSetupDate.systemMode == 0) {

		fRunMenu();

	}

	else if (gSetupDate.systemMode > 0) {

		/*if (!gAlarm.errorAlarm && (gAlarm.heaterAlarm || gAlarm.tempAlarm)) {
			gAlarm.errorAlarm = true;
			gDisplay.print("/", 116, 0, 3);
		}*/

		if (!gAlarm.infoAlarm && gAlarm.tempDone) {
			gAlarm.infoAlarm = true;
			gDisplay.print("/", 116, 0, 3);
		}
		if (gButton.cancelPressedLongDone && !gState.abortMode) {
			fBtnPressedReset();

			if (gAlarm.errorAlarm && !gAlarm.disableAlarm) {

				gAlarm.errorAlarm = false;

				gAlarm.disableAlarm = true;
			}

			else if (gAlarm.infoAlarm && !gAlarm.disableAlarm) {

				gAlarm.infoAlarm = false;

				gAlarm.disableAlarm = true;
			}

			else {
				gState.abortMode = true;
				gDisplay.clear(6, 8);
				gDisplay.print("ПРЕРВАТЬ?", 23, 5, 2, 1);
			}
		}

		else if (gButton.cancelPressedDone && gState.abortMode) {
			fBtnPressedReset();
			gState.abortMode = false;
			gDisplay.clear(6, 8);
		}

		else if (gButton.OKPressedDone && gState.abortMode) {
			fBtnPressedReset();
			fControlAbortMode();
		}
	}
}

bool fTemperatureDS() {
	static bool lReadSensor = false;
	int32_t lTemperature = 0;
	bool lTemperatureUpdate = false;
	static uint32_t lPreviousMillis = 0;

	if (millis() - lPreviousMillis > 1000) {
		lPreviousMillis = millis();
		if (lReadSensor) {
			gSensorDS.reset();  
			gSensorDS.write(0xCC);
			gSensorDS.write(0x44);
		}
		else {
			if (gSetupDate.numSensor[0] > 0 && gSetupDate.addrDS[gSetupDate.numSensor[0]-1][0] == 40) {
				gSensorDS.reset();
				gSensorDS.select(gSetupDate.addrDS[gSetupDate.numSensor[0]-1]);
				gSensorDS.write(0xBE); 

				lTemperature = (float)(gSensorDS.read() | (gSensorDS.read() << 8)) * 6.25;
				if (gHeaterTemperature.updateValue(lTemperature)) {
					lTemperatureUpdate = true;
				}
			}
			else {
				gHeaterTemperature.sensorFixValue = 9990;
			}

			if (gSetupDate.numSensor[1] > 0 && gSetupDate.addrDS[gSetupDate.numSensor[1]-1][0] == 40) {
				gSensorDS.reset();
				gSensorDS.select(gSetupDate.addrDS[gSetupDate.numSensor[1]-1]);
				gSensorDS.write(0xBE); 

				lTemperature = (float)(gSensorDS.read() | (gSensorDS.read() << 8)) * 6.25;
				if (gTemperature.updateValue(lTemperature)) {
					lTemperatureUpdate = true;
				}
			}
			else {
				gTemperature.sensorFixValue = 9990;
			}
		}
		lReadSensor = !lReadSensor;
	}
	return lTemperatureUpdate;
}

void fBtnBeep() {
	digitalWrite(gDevicePin.alarmBeeper, HIGH);
	delay(6);
	digitalWrite(gDevicePin.alarmBeeper, LOW);
}

void fInkBeep() {

	uint32_t lCurrentMillis = 0;
	static uint32_t lPreviousMillis = 0;
	static uint32_t lPrevMillisDisable = 0;
	const uint16_t lAlarmTone[][9] = { { 0, 200, 100, 200, 100, 200, 100, 700, 1000 },{ 0, 200, 100, 200, 1000, 200, 100, 200, 1000 } };
	static uint8_t lToneType = 0;
	static uint8_t lTonePos = 0;

	lCurrentMillis = millis();

	if (!gAlarm.errorAlarm && !gAlarm.infoAlarm) {

		if (digitalRead(gDevicePin.alarmBeeper)) {
			lTonePos = 0;
			digitalWrite(gDevicePin.alarmBeeper, LOW);
		}

		return;
	}
	if (gAlarm.disableAlarm) {
		if (lCurrentMillis - lPrevMillisDisable > ((uint32_t)(gAlarm.disableAlarmTime) * 10000UL)) {

			gAlarm.errorAlarm = false;
			gAlarm.infoAlarm = false;

			gAlarm.disableAlarm = false;

			gDisplay.print(".", 116, 0, 3);

			lTonePos = 0;
		}
		return;
	}

	lPrevMillisDisable = millis();
	if (gAlarm.errorAlarm) { lToneType = 0; }
	if (lCurrentMillis - lPreviousMillis > lAlarmTone[lToneType][lTonePos]) {
		lPreviousMillis = millis();
		digitalWrite(gDevicePin.alarmBeeper, !(lTonePos % 2));
		lTonePos++;
	}

	if (lTonePos > 8) {
		lTonePos = 0;
	}
}