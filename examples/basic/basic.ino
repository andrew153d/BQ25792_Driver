#include <BQ25792_Driver.h>
#include <string.h>

#define SDA_PIN 32
#define SCL_PIN 33
#define STAT_LED 15

BQ25792 charger(0, 0);
void setup() {

  Serial.begin(9600);
  pinMode(STAT_LED, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  // start Charger
  charger.reset();
  delay(500);  // give the charger time to reboot
  charger.flashChargeLevel(STAT_LED);
}

void loop() {
  Serial.printf("Status: %s     Cells: %.1d     Max Voltage: %.1f     MinVoltage: %.1f     VBat: %.1f\n", charger.getChargeStatus(), charger.getCellCount(), charger.getChargeVoltageLimit(), charger.getVSYSMIN(), charger.getVBAT());
  delay(1000);
}
