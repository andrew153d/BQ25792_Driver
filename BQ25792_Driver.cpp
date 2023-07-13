#include "BQ25792_Driver.h"

// #define BQ25792_DEBUG

#ifdef BQ25792_DEBUG
#define DEBUG_PRINTER Serial
#define DEBUG_PRINT(...)                  \
    {                                     \
        DEBUG_PRINTER.print(__VA_ARGS__); \
    }
#define DEBUG_PRINTLN(...)                  \
    {                                       \
        DEBUG_PRINTER.println(__VA_ARGS__); \
    }
#define DEBUG_PRINTF(...)                  \
    {                                      \
        DEBUG_PRINTER.printf(__VA_ARGS__); \
    }
#define DEBUG_BEGIN(...)                  \
    {                                     \
        DEBUG_PRINTER.begin(__VA_ARGS__); \
    }
#else
#define DEBUG_PRINT(...) \
    {                    \
    }
#define DEBUG_PRINTLN(...) \
    {                      \
    }
#define DEBUG_PRINTF(...) \
    {                     \
    }
#define DEBUG_BEGIN(...) \
    {                    \
    }
#endif

BQ25792::BQ25792(int _BCIN, int _QON)
{
    BCIN_Pin = _BCIN;
    QON_Pin = _QON;
}

void IRAM_ATTR chargerInturruptCallback()
{
    DEBUG_PRINTLN("Hello");
}

void BQ25792::begin()
{
    attachInterrupt(BCIN_Pin, chargerInturruptCallback, CHANGE);
}

bool BQ25792::flashChargeLevel(uint16_t pinToFlash, int totalDuration, uint16_t cycles)
{
    float vBat = getVBAT();
    float min = getVSYSMIN();
    float max = getChargeVoltageLimit();

    float onTime = -1000;
    float offTime = 2000;
    totalDuration - onTime;
    while (onTime < 0 || onTime > 1000)
    {
        vBat = getVBAT();
        onTime = map(vBat * 100, min * 100, max * 100, 0, totalDuration);
        DEBUG_PRINTLN("Trying");
        delay(1000);
    }
    offTime = totalDuration - onTime;
    DEBUG_PRINTF("Vbat: %.1f   Min: %.1f   Max: %.1f\n", vBat, min, max);
    DEBUG_PRINTF("ON: %.1f  OFF:%.1f\n", onTime, offTime);
    for (int i = 0; i < cycles; i++)
    {
        digitalWrite(pinToFlash, HIGH);
        delay(onTime);
        digitalWrite(pinToFlash, LOW);
        delay(offTime);
    }

    return true;
}

String BQ25792::getChargeStatus()
{
 

    switch((uint8_t)getChargeStatus0()){
        case 0x0:
            return String("Not Charging");
        break;
        case 0x1:
            return String("Trickle Charge");
        break;
        case 0x2:
            return String("Precharge");
        break;
        case 0x3:
            return String("Fast Charge");
        break;
        case 0x4:
            return String("Taper Charge");
        break;
        case 0x5:
            return String("Reserved");
        break;
        case 0x6:
            return String("Top Off");
        break;
        case 0x7:
            return String("Charging Done");
        break;
    }
   

   return String("noipe");
}

float BQ25792::getVSYSMIN()
{
    float val = ((readByte(REG00_Minimal_System_Voltage) & 0x3F) * VSYS_MIN_STEP_SIZE) + VSYS_MIN_FIXED_OFFSET;
    return val / 1000;
}

void BQ25792::setVSYSMIN(uint8_t vsys)
{
    uint8_t reg = (vsys * 1000 - VSYS_MIN_FIXED_OFFSET) / VSYS_MIN_STEP_SIZE;
    writeByte(REG00_Minimal_System_Voltage, reg);
}

uint8_t BQ25792::getCellCount()
{
    return (int)(getVSYSMIN() / 3);
}

void BQ25792::setCellCount2(uint8_t cells)
{
    uint8_t reg = 0;
    switch (cells)
    {
    case 1:
        //(3500-Offset)/StepSize
        reg = (3500 - VSYS_MIN_FIXED_OFFSET) / VSYS_MIN_STEP_SIZE;
        writeByte(REG00_Minimal_System_Voltage, reg);
        break;
    case 2:
        reg = (7000 - VSYS_MIN_FIXED_OFFSET) / VSYS_MIN_STEP_SIZE;
        writeByte(REG00_Minimal_System_Voltage, reg);
        break;
    case 3:
        reg = (9000 - VSYS_MIN_FIXED_OFFSET) / VSYS_MIN_STEP_SIZE;
        writeByte(REG00_Minimal_System_Voltage, reg);
        break;
    case 4:
        reg = (12000 - VSYS_MIN_FIXED_OFFSET) / VSYS_MIN_STEP_SIZE;
        writeByte(REG00_Minimal_System_Voltage, reg);
        break;
    }
}

float BQ25792::getChargeVoltageLimit()
{
    uint8_t buf[2];
    readBytes(REG01_Charge_Voltage_Limit, &buf[0], 2);

    return (float)(((buf[0] & 0x07) << 8) | buf[1]) / 100;
}

void BQ25792::setChargeVoltageLimit(float limit)
{
    uint16_t _limit = limit * 100;
    uint8_t buf[2];
    buf[0] = (uint8_t)(_limit >> 8);
    buf[1] = (uint8_t)(_limit);
    writeBytes(REG01_Charge_Voltage_Limit, &buf[0], 2);
}

float BQ25792::getChargeCurrentLimit()
{
    uint8_t buf[2];
    readBytes(REG03_Charge_Current_Limit, &buf[0], 2);

    return (float)(((buf[0] & 0x01) << 8) | buf[1]) / 100;
}

void BQ25792::setChargeCurrentLimit(float limit)
{
    uint16_t _limit = limit * 100;
    uint8_t buf[2];
    buf[0] = (uint8_t)(_limit >> 8);
    buf[1] = (uint8_t)(_limit);
    writeBytes(REG03_Charge_Current_Limit, &buf[0], 2);
}

float BQ25792::getInputVoltageLimit()
{
    uint8_t reg = readByte(REG05_Input_Voltage_Limit);
    return ((float)reg / 10);
}

void BQ25792::setInputVoltageLimit(float limit)
{
    uint8_t _limit = int(limit * 10);
    writeByte(REG05_Input_Voltage_Limit, _limit);
}

float BQ25792::getInputCurrentLimit()
{
    uint8_t buf[2];

    readBytes(REG06_Input_Current_Limit, &buf[0], 2);

    return (float)(((buf[0] & 0x01) << 8) | buf[1]) / 100;
}

void BQ25792::setInputCurrentLimit(float limit)
{
    uint16_t _limit = (int)(limit * 100);
    uint8_t buf[2];
    buf[0] = (uint8_t)(_limit >> 8);
    buf[1] = (uint8_t)(_limit);
    writeBytes(REG06_Input_Current_Limit, &buf[0], 2);
}

precharge_control BQ25792::getPrechargeControl()
{
    // Page 60
    // https://www.ti.com/lit/ds/symlink/bq25792.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1685315646335

    uint8_t data = readByte(REG08_Precharge_Control);
    precharge_control cntrl;
    cntrl.Vbat_lowV = (data & 0xC0) >> 6;
    cntrl.Iprechrg = ((float)(data & 0x3F) * 40) / 1000;
    return cntrl;
}

void BQ25792::setPreChargeControl(precharge_control *cntrl)
{
    uint8_t data = (cntrl->Vbat_lowV << 6) | ((uint8_t)(cntrl->Iprechrg * 1000 / 40) & 0x3F);
    writeByte(REG08_Precharge_Control, data);
}

bool BQ25792::isPluggedIn()
{
    charger_status_0 status;
    status.raw = readByte(REG1B_Charger_Status_0);
    return status.VBUS_PRESENT_STAT;
}

CHG_STAT BQ25792::getChargeStatus0()
{
    CHG_STAT stat;
    uint8_t data = readByte(REG1C_Charger_Status_1);
    data = (data >> 5) & 0x07;
    stat = static_cast<CHG_STAT>(data);
    return stat;
}

VBUS_STAT BQ25792::getVBUStatus()
{
    VBUS_STAT stat;
    uint8_t data = readByte(REG1C_Charger_Status_1);
    data = (data >> 1) & 0b00001111;
    stat = static_cast<VBUS_STAT>(data);
    return stat;
}

bool BQ25792::isBatteryPresent()
{
    return readByte(REG1D_Charger_Status_2) & 0x01;
}

bool BQ25792::isErrorPresent()
{
    uint8_t err = readByte(REG20_FAULT_Status_0) | readByte(REG21_FAULT_Status_1);
    return err > 0;
}

float BQ25792::getVBAT()
{
    writeByte(REG2F_ADC_Function_Disable_0, 0b10001111);
    writeByte(REG30_ADC_Function_Disable_1, 0b11111111);
    writeByte(REG2E_ADC_Control, 0b10001100);

    uint8_t buf[2];
    readBytes(REG3B_VBAT_ADC, &buf[0], 2);
    uint16_t v;

    return (float)(((buf[0]) << 8) | buf[1]) / 1000;
}

void BQ25792::setCellCount(uint8_t cells)
{
    uint8_t currentConfig = readByte(REG0A_Recharge_Control);
    currentConfig |= (cells << 6);
    writeByte(REG0A_Recharge_Control, currentConfig);
}

float twosComplementToFloat(int16_t value)
{
    int16_t mask = 0x8000; // Mask for the sign bit
    int16_t sign = value & mask;
    int16_t magnitude = value & ~mask;
    float result = static_cast<float>(magnitude);

    if (sign != 0)
    {
        // Value is negative, convert it to negative float
        result = -result;
    }

    return result;
}

float BQ25792::getIBUS()
{
    writeByte(REG2E_ADC_Control, 0b10001100);

    uint8_t buf[2];
    readBytes(REG31_IBUS_ADC, &buf[0], 2);
    int16_t val = (float)(((buf[0]) << 8) | buf[1]);
    return twosComplementToFloat(val);
}

void BQ25792::getVBATReadDone()
{
}

void BQ25792::resetPower()
{
    writeByte(REG11_Charger_Control_2, 0b01000111);
}

uint8_t BQ25792::getDeviceInfo()
{
    return readByte(REG48_Part_Information);
}

void BQ25792::reset()
{
    writeByte(REG09_Termination_Control, 0b1000000);
}

void BQ25792::readBytes(uint8_t addr, uint8_t *data, uint8_t size)
{
    Wire.beginTransmission(DEVICEADDRESS);
    DEBUG_PRINTF("readI2C reg 0x%02x\n", addr)
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom((uint16_t)DEVICEADDRESS, size);
    for (uint8_t i = 0; i < size; ++i)
    {
        data[i] = Wire.read();
        DEBUG_PRINTF(" <- data[%d]:0x%02x\n", i, data[i])
    }
}

uint8_t BQ25792::readByte(uint8_t addr)
{
    uint8_t data;
    readBytes(addr, &data, 1);
    DEBUG_PRINTF("read byte = %d\n", data)
    return data;
}

void BQ25792::writeBytes(uint8_t addr, uint8_t *data, uint8_t size)
{
    Wire.beginTransmission(DEVICEADDRESS);
    DEBUG_PRINTF("writeI2C reg 0x%02x\n", addr)
    Wire.write(addr);
    for (uint8_t i = 0; i < size; i++)
    {
        DEBUG_PRINTF(" -> data[%d]:0x%02x\n", i, data[i])
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}

void BQ25792::writeByte(uint8_t addr, uint8_t data)
{
    DEBUG_PRINTF("write byte = %d\n", data)
    writeBytes(addr, &data, 1);
}