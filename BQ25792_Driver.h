#ifndef BQ25792_H
#define BQ25792_H
#include <Arduino.h>
#include <Wire.h>
#include <string>

#define DEVICEADDRESS 0x6B

struct __attribute__((__packed__)) precharge_control
{
    uint8_t Vbat_lowV;
    float Iprechrg;
};

typedef union
{
    uint8_t raw;
    struct
    {
        bool IINDPM_STAT : 1;
        bool VINDPM_STAT : 1;
        bool WD_STAT : 1;
        bool POORSRC_STAT : 1;
        bool PG_STAT : 1;
        bool AC2_PRESENT_STAT : 1;
        bool AC1_PRESENT_STAT : 1;
        bool VBUS_PRESENT_STAT : 1;
    };
} charger_status_0;

enum class CHG_STAT : uint8_t
{
    NOT_CHARGING = 0x0,
    TRICKLE_CHARGE = 0x1,
    PRECHARGE = 0x2,
    FAST_CHARGE = 0x3,
    TAPER_CHARGE = 0x4,
    TOP_OFF = 0x6,
    CHARGEDONE = 0x7
};


enum class VBUS_STAT : uint8_t
{
    NOINPUT = 0x0,
    USB_SDP_500MA = 0x1,
    USB_CDP_1500MA = 0x2,
    USB_DCP_3250MA = 0x3,
    ADJUSTABLE_HIGH = 0x4,
    UNKNOWN_ADAPTER = 0x5,
    NON_STANDARD_ADAPTER = 0x6,
    OTG_MODE = 0x7,
    NOT_QUALIFIED_ADAPTER = 0x8,
    POWERED_FROM_VBUS = 0xB
};

#define REG00_Minimal_System_Voltage 0x00
#define REG01_Charge_Voltage_Limit 0x01
#define REG03_Charge_Current_Limit 0x03
#define REG05_Input_Voltage_Limit 0x05
#define REG06_Input_Current_Limit 0x06
#define REG08_Precharge_Control 0x08
#define REG09_Termination_Control 0x09
#define REG0A_Recharge_Control 0x0A
#define REG0B_VOTG_regulation 0x0B
#define REG0D_IOTG_regulation 0x0D
#define REG0E_Timer_Control 0x0E
#define REG0F_Charger_Control_0 0x0F
#define REG10_Charger_Control_1 0x10
#define REG11_Charger_Control_2 0x11
#define REG12_Charger_Control_3 0x12
#define REG13_Charger_Control_4 0x13
#define REG14_Charger_Control_5 0x14
#define REG15_Reserved 0x15
#define REG16_Temperature_Control 0x16
#define REG17_NTC_Control_0 0x17
#define REG18_NTC_Control_1 0x18
#define REG19_ICO_Current_Limit 0x19
#define REG1B_Charger_Status_0 0x1B
#define REG1C_Charger_Status_1 0x1C
#define REG1D_Charger_Status_2 0x1D
#define REG1E_Charger_Status_3 0x1E
#define REG1F_Charger_Status_4 0x1F
#define REG20_FAULT_Status_0 0x20
#define REG21_FAULT_Status_1 0x21
#define REG22_Charger_Flag_0 0x22
#define REG23_Charger_Flag_1 0x23
#define REG24_Charger_Flag_2 0x24
#define REG25_Charger_Flag_3 0x25
#define REG26_FAULT_Flag_0 0x26
#define REG27_FAULT_Flag_1 0x27
#define REG28_Charger_Mask_0 0x28
#define REG29_Charger_Mask_1 0x29
#define REG2A_Charger_Mask_2 0x2A
#define REG2B_Charger_Mask_3 0x2B
#define REG2C_FAULT_Mask_0 0x2C
#define REG2D_FAULT_Mask_1 0x2D
#define REG2E_ADC_Control 0x2E
#define REG2F_ADC_Function_Disable_0 0x2F
#define REG30_ADC_Function_Disable_1 0x30
#define REG31_IBUS_ADC 0x31
#define REG33_IBAT_ADC 0x33
#define REG35_VBUS_ADC 0x35
#define REG37_VAC1_ADC 0x37
#define REG39_VAC2_ADC 0x39
#define REG3B_VBAT_ADC 0x3B
#define REG3D_VSYS_ADC 0x3D
#define REG3F_TS_ADC 0x3F
#define REG41_TDIE_ADC 0x41
#define REG43_D +_ADC 0x43
#define REG45_D -_ADC 0x45
#define REG47_DPDM_Driver 0x47
#define REG48_Part_Information 0x48

#define VSYS_MIN_FIXED_OFFSET 2500
#define VSYS_MIN_STEP_SIZE 250

#define CHARGEVOLTAGELIMIT_STEP_SIZE 10

class BQ25792
{
private:
    int BCIN_Pin;
    int QON_Pin;

public:
    BQ25792(int _BCIN, int _QON);

    // wrapper functions
    void begin();
    bool flashChargeLevel(uint16_t pinToFlash, int totalDuration = 500, uint16_t cycles = 4);
    
    String getChargeStatus();

    // Register Access Functions
    float getVSYSMIN();
    void setVSYSMIN(uint8_t vsys);

    uint8_t getCellCount();
    void setCellCount2(uint8_t cells);

    float getChargeVoltageLimit();
    void setChargeVoltageLimit(float limit);

    float getChargeCurrentLimit();
    void setChargeCurrentLimit(float limit);

    float getInputVoltageLimit();
    void setInputVoltageLimit(float limit);

    float getInputCurrentLimit();
    void setInputCurrentLimit(float limit);

    precharge_control getPrechargeControl();
    void setPreChargeControl(precharge_control *cntrl);

    void getVBATReadDone();

    bool isPluggedIn();

    CHG_STAT getChargeStatus0();
    VBUS_STAT getVBUStatus();

    bool isBatteryPresent();

    bool isErrorPresent();

    void setCellCount(uint8_t cells);

    float getVBAT();
    float getIBUS();

    void resetPower();

    uint8_t getDeviceInfo();

    void reset();

private:
    void readBytes(uint8_t addr, uint8_t *data, uint8_t size);
    uint16_t readWord(uint8_t addr);
    uint8_t readByte(uint8_t addr);
    void writeBytes(uint8_t addr, uint8_t *data, uint8_t size);
    void writeWord(uint8_t addr, uint16_t data);
    void writeByte(uint8_t addr, uint8_t data);
};

#endif