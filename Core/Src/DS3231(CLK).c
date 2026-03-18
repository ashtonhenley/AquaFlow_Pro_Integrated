/*
 * DS3231(CLK).c
 *
 *  Created on: Nov 15, 2025
 *      Author: ashtonhenley
 */
#include "DS3231(CLK).h"
#include "main.h"


I2C_HandleTypeDef *_ds3231_ui2c;

DateTimeStruct curr_date_time = {0};

// Set scheduled date, will later be set by Interface.
ScheduledWaterChange sched_date_time = {
		.day = 17,
		.month = 3,
		.year = 26,
		.minutes = 58,
		.hours = 15,
};


bool timer_expired(uint32_t start_sod, uint32_t runtime_s, uint8_t curr_hour, uint8_t curr_min, uint8_t curr_sec)
	{
    uint32_t now_sod =
        (uint32_t)curr_hour * 3600u +
        (uint32_t)curr_min  * 60u +
        (uint32_t)curr_sec;

    uint32_t elapsed =
        (now_sod + 86400u - start_sod) % 86400u;

    return (elapsed >= runtime_s);
	}
void update_date_time(){
	uint16_t year = DS3231_GetYear();

	curr_date_time.day = DS3231_GetDate();
	curr_date_time.month = DS3231_GetMonth() ;
	curr_date_time.year = (uint8_t)(year % 100);

	// Now, handle time

	curr_date_time.seconds = DS3231_GetSecond();
	curr_date_time.minutes = DS3231_GetMinute();
	curr_date_time.hours = DS3231_GetHour();

}



void DS3231_Init(I2C_HandleTypeDef *hi2c) {
	_ds3231_ui2c = hi2c;
	DS3231_EnableAlarm1(DS3231_DISABLED);
	DS3231_EnableAlarm2(DS3231_DISABLED);
	DS3231_ClearAlarm1Flag();
	DS3231_ClearAlarm2Flag();
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}


void DS3231_SetRegByte(uint8_t regAddr, uint8_t val) {
	uint8_t bytes[2] = { regAddr, val };
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, bytes, 2, DS3231_TIMEOUT);
}


uint8_t DS3231_GetRegByte(uint8_t regAddr) {
	uint8_t val;
	HAL_I2C_Master_Transmit(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &regAddr, 1, DS3231_TIMEOUT);
	HAL_I2C_Master_Receive(_ds3231_ui2c, DS3231_I2C_ADDR << 1, &val, 1, DS3231_TIMEOUT);
	return val;
}


void DS3231_EnableBatterySquareWave(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xbf) | ((enable & 0x01) << DS3231_BBSQW));
}


void DS3231_SetInterruptMode(DS3231_InterruptMode mode){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfb) | ((mode & 0x01) << DS3231_INTCN));
}


void DS3231_SetRateSelect(DS3231_Rate rate){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xe7) | ((rate & 0x03) << DS3231_RS1));
}


void DS3231_EnableOscillator(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0x7f) | ((!enable & 0x01) << DS3231_EOSC));
}


void DS3231_EnableAlarm2(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfd) | ((enable & 0x01) << DS3231_A2IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}


void DS3231_ClearAlarm2Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfd;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A2F));
}


void DS3231_SetAlarm2Minute(uint8_t minute){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_MINUTE) & 0x80;
	uint8_t a2m2 = temp | (DS3231_EncodeBCD(minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_MINUTE, a2m2);
}


void DS3231_SetAlarm2Hour(uint8_t hour_24mode){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_HOUR) & 0x80;
	uint8_t a2m3 = temp | (DS3231_EncodeBCD(hour_24mode) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_HOUR, a2m3);
}


void DS3231_SetAlarm2Date(uint8_t date){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	uint8_t a2m4 = temp | (DS3231_EncodeBCD(date) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2m4);
}


void DS3231_SetAlarm2Day(uint8_t day){
	uint8_t temp = DS3231_GetRegByte(DS3231_A2_DATE) & 0x80;
	uint8_t a2m4 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(day) & 0x3f);
	DS3231_SetRegByte(DS3231_A2_DATE, a2m4);
}


void DS3231_SetAlarm2Mode(DS3231_Alarm2Mode alarmMode){
	uint8_t temp;
	temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_MINUTE, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_HOUR, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;
	DS3231_SetRegByte(DS3231_A2_DATE, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}


void DS3231_EnableAlarm1(DS3231_State enable){
	uint8_t control = DS3231_GetRegByte(DS3231_REG_CONTROL);
	DS3231_SetRegByte(DS3231_REG_CONTROL, (control & 0xfe) | ((enable & 0x01) << DS3231_A1IE));
	DS3231_SetInterruptMode(DS3231_ALARM_INTERRUPT);
}


void DS3231_ClearAlarm1Flag(){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfe;
	DS3231_SetRegByte(DS3231_REG_STATUS, status & ~(0x01 << DS3231_A1F));
}


void DS3231_SetAlarm1Second(uint8_t second){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x80;
	uint8_t a1m1 = temp | (DS3231_EncodeBCD(second) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_SECOND, a1m1);
}


void DS3231_SetAlarm1Minute(uint8_t minute){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x80;
	uint8_t a1m2 = temp | (DS3231_EncodeBCD(minute) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_MINUTE, a1m2);
}


void DS3231_SetAlarm1Hour(uint8_t hour_24mode){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x80;
	uint8_t a1m3 = temp | (DS3231_EncodeBCD(hour_24mode) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_HOUR, a1m3);
}


void DS3231_SetAlarm1Date(uint8_t date){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	uint8_t a1m4 = temp | (DS3231_EncodeBCD(date) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1m4);
}


void DS3231_SetAlarm1Day(uint8_t day){
	uint8_t temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x80;
	uint8_t a1m4 = temp | (0x01 << DS3231_DYDT) | (DS3231_EncodeBCD(day) & 0x3f);
	DS3231_SetRegByte(DS3231_A1_DATE, a1m4);
}


void DS3231_SetAlarm1Mode(DS3231_Alarm1Mode alarmMode){
	uint8_t temp;
	temp = DS3231_GetRegByte(DS3231_A1_SECOND) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_SECOND, temp | (((alarmMode >> 0) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_MINUTE) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_MINUTE, temp | (((alarmMode >> 1) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_HOUR) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_HOUR, temp | (((alarmMode >> 2) & 0x01) << DS3231_AXMY));
	temp = DS3231_GetRegByte(DS3231_A1_DATE) & 0x7f;
	DS3231_SetRegByte(DS3231_A1_DATE, temp | (((alarmMode >> 3) & 0x01) << DS3231_AXMY) | (alarmMode & 0x80));
}


uint8_t DS3231_IsOscillatorStopped(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_OSF) & 0x01;
}


uint8_t DS3231_Is32kHzEnabled(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_EN32KHZ) & 0x01;
}


uint8_t DS3231_IsAlarm1Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A1F) & 0x01;
}


uint8_t DS3231_IsAlarm2Triggered(){
	return (DS3231_GetRegByte(DS3231_REG_STATUS) >> DS3231_A2F) & 0x01;
}


uint8_t DS3231_GetDayOfWeek(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DOW));
}


uint8_t DS3231_GetDate(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_DATE));
}


uint8_t DS3231_GetMonth(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f);
}


uint16_t DS3231_GetYear(void) {
	uint8_t decYear = DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_YEAR));
	uint16_t century = (DS3231_GetRegByte(DS3231_REG_MONTH) >> DS3231_CENTURY) * 100 + 2000;
	return century + decYear;
}


uint8_t DS3231_GetHour(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_HOUR));
}


uint8_t DS3231_GetMinute(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_MINUTE));
}


uint8_t DS3231_GetSecond(void) {
	return DS3231_DecodeBCD(DS3231_GetRegByte(DS3231_REG_SECOND));
}


void DS3231_SetDayOfWeek(uint8_t dayOfWeek) {
	DS3231_SetRegByte(DS3231_REG_DOW, DS3231_EncodeBCD(dayOfWeek));
}


void DS3231_SetDate(uint8_t date) {
	DS3231_SetRegByte(DS3231_REG_DATE, DS3231_EncodeBCD(date));
}


void DS3231_SetMonth(uint8_t month) {
	uint8_t century = DS3231_GetRegByte(DS3231_REG_MONTH) & 0x80;
	DS3231_SetRegByte(DS3231_REG_MONTH, DS3231_EncodeBCD(month) | century);
}


void DS3231_SetYear(uint16_t year) {
	uint8_t century = (year / 100) % 20;
	uint8_t monthReg = (DS3231_GetRegByte(DS3231_REG_MONTH) & 0x7f) | (century << DS3231_CENTURY);
	DS3231_SetRegByte(DS3231_REG_MONTH, monthReg);
	DS3231_SetRegByte(DS3231_REG_YEAR, DS3231_EncodeBCD(year % 100));
}


void DS3231_SetHour(uint8_t hour_24mode) {
	DS3231_SetRegByte(DS3231_REG_HOUR, DS3231_EncodeBCD(hour_24mode & 0x3f));
}


void DS3231_SetMinute(uint8_t minute) {
	DS3231_SetRegByte(DS3231_REG_MINUTE, DS3231_EncodeBCD(minute));
}


void DS3231_SetSecond(uint8_t second) {
	DS3231_SetRegByte(DS3231_REG_SECOND, DS3231_EncodeBCD(second));
}


void DS3231_SetFullTime(uint8_t  hour_24mode, uint8_t minute, uint8_t second){
	DS3231_SetHour(hour_24mode);
	DS3231_SetMinute(minute);
	DS3231_SetSecond(second);
}


void DS3231_SetFullDate(uint8_t date, uint8_t month, uint8_t dow, uint16_t year){
	DS3231_SetDate(date);
	DS3231_SetMonth(month);
	DS3231_SetDayOfWeek(dow);
	DS3231_SetYear(year);
}


uint8_t DS3231_DecodeBCD(uint8_t bin) {
	return (((bin & 0xf0) >> 4) * 10) + (bin & 0x0f);
}


uint8_t DS3231_EncodeBCD(uint8_t dec) {
	return (dec % 10 + ((dec / 10) << 4));
}


void DS3231_Enable32kHzOutput(DS3231_State enable){
	uint8_t status = DS3231_GetRegByte(DS3231_REG_STATUS) & 0xfb;
	DS3231_SetRegByte(DS3231_REG_STATUS, status | (enable << DS3231_EN32KHZ));
}


int8_t DS3231_GetTemperatureInteger(){
	return DS3231_GetRegByte(DS3231_TEMP_MSB);
}


uint8_t DS3231_GetTemperatureFraction(){
	return (DS3231_GetRegByte(DS3231_TEMP_LSB) >> 6) * 25;
}
