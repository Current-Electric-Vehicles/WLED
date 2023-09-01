#pragma once

#include <Arduino.h>
#include <TMP1075.h>

#include "wled.h"

struct PinState {
  bool drive = false; 
  bool backup = false;
  bool brake = false;
  bool leftBlinker = false;
  bool rightBlinker = false;
  bool ignition = false;
  bool accessory = false;
  bool aux1 = false;
  float monitorCurrent = 0.0f;
};

#define LED_POWER1_ENABLE_PIN     GPIO_NUM_22
#define LED_MONITOR_SEL1          GPIO_NUM_2
#define LED_PSU1_ENABLE_PIN       GPIO_NUM_25

#define LED_POWER2_ENABLE_PIN     GPIO_NUM_23
#define LED_MONITOR_SEL2          GPIO_NUM_0
#define LED_PSU2_ENABLE_PIN       GPIO_NUM_27

#define LED_POWER_DIAG_PIN        GPIO_NUM_36

#define USER_PIN_1 GPIO_NUM_14
#define USER_PIN_2 GPIO_NUM_17
#define USER_PIN_3 GPIO_NUM_5
#define USER_PIN_4 GPIO_NUM_18
#define USER_PIN_5 GPIO_NUM_39
#define USER_PIN_6 GPIO_NUM_4
#define USER_PIN_7 GPIO_NUM_35
#define USER_PIN_8 GPIO_NUM_34

#define DRIVE_PIN_DEFAULT       USER_PIN_1
#define BACKUP_PIN_DEFAULT      USER_PIN_2
#define BRAKE_PIN_DEFAULT       USER_PIN_3
#define LEFT_PIN_DEFAULT        USER_PIN_4
#define RIGHT_PIN_DEFAULT       USER_PIN_5
#define IGNITION_PIN_DEFAULT    USER_PIN_6
#define ACCESSORY_PIN_DEFAULT   USER_PIN_7
#define AUX1_PIN_DEFAULT        USER_PIN_8

#define I2C_SDA GPIO_NUM_14
#define I2C_SCL GPIO_NUM_26
#define I2C_FREQUENCY 100000
#define TEMP_SENSOR_I2C_ADDR 0b1001000

#define ADC_RESOLUTION (3.3f / 4096.0f)
#define ADC_SCALE_FACTOR 2.0f

#define CHATTER_FREQUENCY 10000

#define OVERHEATED_TEMP_CELCIUS 80.0

class CurrentVehicleIndicatorUserMod : public Usermod {

  public:
    CurrentVehicleIndicatorUserMod() :
      wire(0),
      tmp1075(wire, TEMP_SENSOR_I2C_ADDR) {}

  private:
    TwoWire wire;
    TMP1075::TMP1075 tmp1075;

    bool enabled = true;
    float temp = 0.0f;
    bool overhated = false;
    int chatterGuard = 0;

    // states available here: https://docs.google.com/document/d/1xnQHJQ36Hoe6f8d2YZyPMUZjU54ghxtrWkC_2V9NMOE/edit#heading=h.xf94nigg76ec
    PinState state;

    bool psu1Enable = false;
    bool psu2Enable = false;
    bool ledPower1Enable = false;
    bool ledPower2Enable = false;
    int diagnosticMode = -1;

    int drivePin        = DRIVE_PIN_DEFAULT; 
    int backupPin       = BACKUP_PIN_DEFAULT;
    int brakePin        = BRAKE_PIN_DEFAULT;
    int leftBlinkerPin  = LEFT_PIN_DEFAULT;
    int rightBlinkerPin = RIGHT_PIN_DEFAULT;
    int ignitionPin     = IGNITION_PIN_DEFAULT;
    int accessoryPin    = ACCESSORY_PIN_DEFAULT;
    int aux1Pin         = AUX1_PIN_DEFAULT;

    int driveOn = 1; 
    int backupOn = 1;
    int brakeOn = 1;
    int leftBlinkerOn = 1;
    int rightBlinkerOn = 1;
    int ignitionOn = 1;
    int accessoryOn = 1;
    int aux1On = 1;

    void setupPins() {
      Serial.println("Setting up pins for CurrentVehicleIndicatorUserMod");

      pinMode(LED_MONITOR_SEL1, OUTPUT);
      pinMode(LED_MONITOR_SEL2, OUTPUT);
      pinMode(LED_PSU1_ENABLE_PIN, OUTPUT);
      pinMode(LED_PSU2_ENABLE_PIN, OUTPUT);
      pinMode(LED_POWER1_ENABLE_PIN, OUTPUT);
      pinMode(LED_POWER2_ENABLE_PIN, OUTPUT);
    
      switch (this->diagnosticMode) {
        case 100:
          digitalWrite(LED_MONITOR_SEL1, LOW);
          digitalWrite(LED_MONITOR_SEL2, LOW);
          break;
        case 200:
          digitalWrite(LED_MONITOR_SEL1, LOW);
          digitalWrite(LED_MONITOR_SEL2, HIGH);
          break;
        case 300:
          digitalWrite(LED_MONITOR_SEL1, HIGH);
          digitalWrite(LED_MONITOR_SEL2, LOW);
          break;
        default: // -1
          digitalWrite(LED_MONITOR_SEL1, HIGH);
          digitalWrite(LED_MONITOR_SEL2, HIGH);
          break;
      }

      if (this->drivePin != -1) {
        Serial.print("Drive pin: "); Serial.print(this->drivePin, 10); Serial.print(" on: "); Serial.println(this->driveOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->drivePin, INPUT);
      }
      if (this->backupPin != -1) {
        Serial.print("Backup pin: "); Serial.print(this->backupPin, 10); Serial.print(" on: "); Serial.println(this->backupOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->backupPin, INPUT);
      }
      if (this->brakePin != -1) {
        Serial.print("Brake pin: "); Serial.print(this->brakePin, 10); Serial.print(" on: "); Serial.println(this->brakeOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->brakePin, INPUT);
      }
      if (this->leftBlinkerPin != -1) {
        Serial.print("Left blinker pin: "); Serial.print(this->leftBlinkerPin, 10); Serial.print(" on: "); Serial.println(this->leftBlinkerOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->leftBlinkerPin, INPUT);
      }
      if (this->rightBlinkerPin != -1) {
        Serial.print("Right blinker pin: "); Serial.print(this->rightBlinkerPin, 10); Serial.print(" on: "); Serial.println(this->rightBlinkerOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->rightBlinkerPin, INPUT);
      }
      if (this->ignitionPin != -1) {
        Serial.print("Ignition pin: "); Serial.print(this->ignitionPin, 10); Serial.print(" on: "); Serial.println(this->ignitionOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->ignitionPin, INPUT);
      }
      if (this->accessoryPin != -1) {
        Serial.print("Accessory pin: "); Serial.print(this->accessoryPin, 10); Serial.print(" on: "); Serial.println(this->accessoryOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->accessoryPin, INPUT);
      }
      if (this->aux1Pin != -1) {
        Serial.print("AUX1 pin: "); Serial.print(this->aux1Pin, 10); Serial.print(" on: "); Serial.println(this->aux1On == HIGH ? "HIGH" : "LOW");
        pinMode(this->aux1Pin, INPUT);
      }
    }

    void updateLightPowerSettings() {
        this->setPUS1Enabled(this->psu1Enable && !this->overhated);
        this->setPUS2Enabled(this->psu2Enable && !this->overhated);
        this->setLED1PowerEnabled(this->ledPower1Enable && !this->overhated);
        this->setLED2PowerEnabled(this->ledPower2Enable && !this->overhated);
    }

    void setPUS1Enabled(bool enabled) {
      digitalWrite(LED_PSU1_ENABLE_PIN, enabled ? HIGH : LOW);
    }

    void setPUS2Enabled(bool enabled) {
      digitalWrite(LED_PSU2_ENABLE_PIN, enabled ? HIGH : LOW);
    }

    void setLED1PowerEnabled(bool enabled) {
      digitalWrite(LED_POWER1_ENABLE_PIN, enabled ? HIGH : LOW);
    }

    void setLED2PowerEnabled(bool enabled) {
      digitalWrite(LED_POWER2_ENABLE_PIN, enabled ? HIGH : LOW);
    }

    PinState readState() {

      PinState ret;
      ret.drive = this->drivePin != -1 ? digitalRead(this->drivePin) == this->driveOn : false;
      ret.backup = this->backupPin != -1 ? digitalRead(this->backupPin) == this->backupOn : false;
      ret.brake = this->brakePin != -1 ? digitalRead(this->brakePin) == this->brakeOn : false;
      ret.leftBlinker = this->leftBlinkerPin != -1 ? digitalRead(this->leftBlinkerPin) == this->leftBlinkerOn : false;
      ret.rightBlinker = this->rightBlinkerPin != -1 ? digitalRead(this->rightBlinkerPin) == this->rightBlinkerOn : false;
      ret.ignition = this->ignitionPin != -1 ? digitalRead(this->ignitionPin) == this->ignitionOn : false;
      ret.accessory = this->accessoryPin != -1 ? digitalRead(this->accessoryPin) == this->accessoryOn : false;
      ret.aux1 = this->aux1Pin != -1 ? digitalRead(this->aux1Pin) == this->aux1On : false;

      if (this->diagnosticMode != -1) {
        analogReadResolution(12);
        ret.monitorCurrent = (ADC_RESOLUTION * (float)analogRead(LED_POWER_DIAG_PIN)) * ADC_SCALE_FACTOR;
        ret.monitorCurrent *= 2.0f;
      } else {
        ret.monitorCurrent = 0.0f;
      }

      if (this->canChatter()) {
        Serial.println("========== io:");
        Serial.print("getTemperatureCelsius(): "); Serial.println(this->temp);
        Serial.print("digitalRead(this->drivePin): "); Serial.println(digitalRead(this->drivePin));
        Serial.print("digitalRead(this->backupPin): "); Serial.println(digitalRead(this->backupPin));
        Serial.print("digitalRead(this->brakePin): "); Serial.println(digitalRead(this->brakePin));
        Serial.print("digitalRead(this->leftBlinkerPin): "); Serial.println(digitalRead(this->leftBlinkerPin));
        Serial.print("digitalRead(this->rightBlinkerPin): "); Serial.println(digitalRead(this->rightBlinkerPin));
        Serial.print("digitalRead(this->ignitionPin): "); Serial.println(digitalRead(this->ignitionPin));
        Serial.print("digitalRead(this->accessoryPin): "); Serial.println(digitalRead(this->accessoryPin));
        Serial.print("digitalRead(this->aux1Pin): "); Serial.println(digitalRead(this->aux1Pin));
        Serial.print("digitalRead(LED_POWER1_ENABLE_PIN): "); Serial.println(digitalRead(LED_POWER1_ENABLE_PIN));
        Serial.print("digitalRead(LED_POWER2_ENABLE_PIN): "); Serial.println(digitalRead(LED_POWER2_ENABLE_PIN));
        Serial.print("digitalRead(LED_PSU1_ENABLE_PIN): "); Serial.println(digitalRead(LED_PSU1_ENABLE_PIN));
        Serial.print("digitalRead(LED_PSU2_ENABLE_PIN): "); Serial.println(digitalRead(LED_PSU2_ENABLE_PIN));
        Serial.print("digitalRead(LED_MONITOR_SEL1): "); Serial.println(digitalRead(LED_MONITOR_SEL1));
        Serial.print("digitalRead(LED_MONITOR_SEL2): "); Serial.println(digitalRead(LED_MONITOR_SEL2));
        Serial.print("analogRead(LED_POWER_DIAG_PIN): "); Serial.println(analogRead(LED_POWER_DIAG_PIN));
        Serial.print("digitalRead(this->diagnosticMode): "); Serial.println(this->diagnosticMode);
        Serial.print("digitalRead(this->ledPower1Enable): "); Serial.println(this->ledPower1Enable);
        Serial.print("digitalRead(this->ledPower2Enable): "); Serial.println(this->ledPower2Enable);
        Serial.print("digitalRead(this->psu1Enable): "); Serial.println(this->psu1Enable);
        Serial.print("digitalRead(this->psu2Enable): "); Serial.println(this->psu2Enable);
        Serial.print("monitorCurrent: "); Serial.println(ret.monitorCurrent);
      }

      return ret;
    }

    bool isOverHeated() {
      return this->temp >= OVERHEATED_TEMP_CELCIUS;
    }
    
    bool canChatter() {
      return (chatterGuard % CHATTER_FREQUENCY) == 0;
    }

    /**
     * This method is where the logic goes for changing the lights
     * based on inputs and other events.
     */
    void handleStateChanges() {

      PinState newState = this->readState();

      bool isSomethingOn = newState.drive || newState.backup || newState.brake || newState.leftBlinker || newState.rightBlinker || newState.ignition || newState.accessory || newState.aux1;
      if (isSomethingOn) {
        this->updateLightPowerSettings();
      }

      bool driveChanged = newState.drive != this->state.drive;
      bool backupChanged = newState.backup != this->state.backup;
      bool brakeChanged = newState.brake != this->state.brake;
      bool leftBlinkerChanged = newState.leftBlinker != this->state.leftBlinker;
      bool rightBlinkerChanged = newState.rightBlinker != this->state.rightBlinker;
      bool ignitionChanged = newState.ignition != this->state.ignition;
      bool accessoryChanged = newState.accessory != this->state.accessory;
      bool aux1Changed = newState.aux1 != this->state.aux1;

      bool changed = driveChanged || backupChanged || brakeChanged || leftBlinkerChanged || rightBlinkerChanged || ignitionChanged || accessoryChanged || aux1Changed;
      if (!changed) {
        return;
      }

      this->state = newState;

      if (driveChanged) {
        Serial.print("drive: "); Serial.println(this->state.drive);
      }
      if (backupChanged) {
        Serial.print("backup: "); Serial.println(this->state.backup);
      }
      if (brakeChanged) {
        Serial.print("brake: "); Serial.println(this->state.brake);
      }
      if (leftBlinkerChanged) {
        Serial.print("leftBlinker: "); Serial.println(this->state.leftBlinker);
      }
      if (rightBlinkerChanged) {
        Serial.print("rightBlinker: "); Serial.println(this->state.rightBlinker);
      }
      if (ignitionChanged) {
        Serial.print("ignition: "); Serial.println(this->state.ignition);
      }
      if (accessoryChanged) {
        Serial.print("accessory`: "); Serial.println(this->state.accessory);
      }
      if (aux1Changed) {
        Serial.print("aux1`: "); Serial.println(this->state.aux1);
      }

      // rear driver outer
      // rear driver inner
      // rear driver panel
      // rear passenger outer
      // rear passenger inner
      // rear passenger panel
      // front driver grill
      // front driver panel
      // front passenger grill
      // front passenger panel

      /**
       * IGNITION
       */
      if (ignitionChanged && this->state.ignition) {

      } else if (ignitionChanged && !this->state.ignition) {

      }

      /**
       * ACCESSORY
       */
      if (accessoryChanged && this->state.accessory) {

      } else if (accessoryChanged && !this->state.accessory) {

      }

      /**
       * DRIVE
       */
      if (driveChanged && this->state.drive) {
        // turn on drive lights
      } else if (driveChanged && !this->state.drive) {
        // turn off drive lights
      }

      /**
       * BACKUP
       */
      if (backupChanged && this->state.backup) {
        // turn on backup lights
      } else if (backupChanged && !this->state.backup) {
        // turn off backup lights
      }

      /**
       * BRAKE
       */
      if (brakeChanged && this->state.brake) {
        // turn on brake lights
      } else if (brakeChanged && !this->state.brake) {
        // turn off brake lights
      }

      /**
       * LEFT
       */
      if (leftBlinkerChanged && this->state.leftBlinker) {
        // turn on left blinker lights
      } else if (leftBlinkerChanged && !this->state.leftBlinker) {
        // turn off left blinker lights
      }

      /**
       * RIGHT
       */
      if (rightBlinkerChanged && this->state.rightBlinker) {
        // turn on right blinker lights
      } else if (rightBlinkerChanged && !this->state.rightBlinker) {
        // turn off right blinker lights
      }

      /**
       * AUX1
       */
      if (aux1Changed && this->state.aux1) {
        
      } else if (aux1Changed && !this->state.aux1) {

      }
    }

  public:

    void loop() {

      // get the temperature
      chatterGuard++;
      tmp1075.setConversionTime(TMP1075::ConversionTime220ms);
      this->temp = this->tmp1075.getTemperatureCelsius();

      if (this->isOverHeated()) {
        // handle overheating
        if (this->canChatter()) {
          Serial.print("WARNING!!! System is overheated, disabling power to lights: ");
          Serial.print(this->temp); Serial.println(" celcius");
        }
        this->overhated = true;
        this->updateLightPowerSettings();
        return;

      } else if (!this->isOverHeated() && this->overhated) {
        // handle coming out of overheating
        Serial.print("No longer overheated, enabling power to lights: ");
        Serial.print(this->temp); Serial.println(" celcius");
        this->overhated = false;
        this->updateLightPowerSettings();
      }

      if (!this->enabled) {
        return;
      }

      this->handleStateChanges();
    }

    void connected() {
      Serial.println("CurrentVehicleIndicators connected!");
    }

    void enable(bool enable) {
      this->enabled = enable;
      setupPins();
      this->updateLightPowerSettings();
    }

    void setup() {
      if (!this->enabled) {
        return;
      }
      if (!wire.begin(I2C_SDA, I2C_SCL)) {
        Serial.println("wire.begin(I2C_SDA, I2C_SCL) failed!");
      }
    }

    void addToConfig(JsonObject& root) {
      JsonObject top = root.createNestedObject(FPSTR("CurrentVehicleIndicators"));
      top.clear();

      top[FPSTR("enabled")] = enabled;
      top[F("drivePin")] = this->drivePin;
      top[F("backupPin")] = this->backupPin;
      top[F("brakePin")] = this->brakePin;
      top[F("leftBlinkerPin")] = this->leftBlinkerPin;
      top[F("rightBlinkerPin")] = this->rightBlinkerPin;
      top[F("ignitionPin")] = this->ignitionPin;
      top[F("accessoryPin")] = this->accessoryPin;
      top[F("aux1Pin")] = this->aux1Pin;

      top[F("driveOn")] = this->driveOn;
      top[F("backupOn")] = this->backupOn;
      top[F("brakeOn")] = this->brakeOn;
      top[F("leftBlinkerOn")] = this->leftBlinkerOn;
      top[F("rightBlinkerOn")] = this->rightBlinkerOn;
      top[F("ignitionOn")] = this->ignitionOn;
      top[F("accessoryOn")] = this->accessoryOn;
      top[F("aux1On")] = this->aux1On;

      top[F("ledPower1Enable")] = this->ledPower1Enable;
      top[F("ledPower2Enable")] = this->ledPower2Enable;
      top[F("psu1Enable")] = this->psu1Enable;
      top[F("psu2Enable")] = this->psu2Enable;
      top[F("diagnosticMode")] = this->diagnosticMode;
    }

    bool readFromConfig(JsonObject& root) {
      JsonObject top = root[FPSTR("CurrentVehicleIndicators")];
      getJsonValue(top[FPSTR("enabled")], this->enabled, false);
      getJsonValue(top[F("drivePin")], this->drivePin, DRIVE_PIN_DEFAULT);
      getJsonValue(top[F("backupPin")], this->backupPin, BACKUP_PIN_DEFAULT);
      getJsonValue(top[F("brakePin")], this->brakePin, BRAKE_PIN_DEFAULT);
      getJsonValue(top[F("leftBlinkerPin")], this->leftBlinkerPin, LEFT_PIN_DEFAULT);
      getJsonValue(top[F("rightBlinkerPin")], this->rightBlinkerPin, RIGHT_PIN_DEFAULT);
      getJsonValue(top[F("ignitionPin")], this->ignitionPin, IGNITION_PIN_DEFAULT);
      getJsonValue(top[F("accessoryPin")], this->accessoryPin, ACCESSORY_PIN_DEFAULT);
      getJsonValue(top[F("aux1Pin")], this->aux1Pin, AUX1_PIN_DEFAULT);

      getJsonValue(top[F("driveOn")],         this->driveOn, 1);
      getJsonValue(top[F("backupOn")],        this->backupOn, 1);
      getJsonValue(top[F("brakeOn")],         this->brakeOn, 1);
      getJsonValue(top[F("leftBlinkerOn")],   this->leftBlinkerOn, 1);
      getJsonValue(top[F("rightBlinkerOn")],  this->rightBlinkerOn, 1);
      getJsonValue(top[F("ignitionOn")],      this->ignitionOn, 1);
      getJsonValue(top[F("accessoryOn")],     this->accessoryOn, 1);
      getJsonValue(top[F("aux1On")],          this->aux1On, 1);

      getJsonValue(top[F("ledPower1Enable")], this->ledPower1Enable, false);
      getJsonValue(top[F("ledPower2Enable")], this->ledPower2Enable, false);
      getJsonValue(top[F("psu1Enable")], this->psu1Enable, false);
      getJsonValue(top[F("psu2Enable")], this->psu2Enable, false);

      getJsonValue(top[F("diagnosticMode")], this->diagnosticMode, -1);

      this->drivePin = (int)this->drivePin;
      this->backupPin = (int)this->backupPin;
      this->brakePin = (int)this->brakePin;
      this->leftBlinkerPin = (int)this->leftBlinkerPin;
      this->rightBlinkerPin = (int)this->rightBlinkerPin;
      this->ignitionPin = (int)this->ignitionPin;
      this->accessoryPin = (int)this->accessoryPin;
      this->aux1Pin = (int)this->aux1Pin;

      this->driveOn = (int)this->driveOn;
      this->backupOn = (int)this->backupOn;
      this->brakeOn = (int)this->brakeOn;
      this->leftBlinkerOn = (int)this->leftBlinkerOn;
      this->rightBlinkerOn = (int)this->rightBlinkerOn;
      this->ignitionOn = (int)this->ignitionOn;
      this->accessoryOn = (int)this->accessoryOn;
      this->aux1On = (int)this->aux1On;

      this->enable(this->enabled);

      return true;
    }

    void appendConfigData() {
      oappend(SET_F("function addPinConfig(p,o){"));
      oappend(SET_F("i=addDropdown('CurrentVehicleIndicators',p);"));
      oappend(SET_F("addOption(i,'Unused',-1);"));
      oappend(SET_F("addOption(i,'Input 1',")); oappendi(USER_PIN_1); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 2',")); oappendi(USER_PIN_2); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 3',")); oappendi(USER_PIN_3); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 4',")); oappendi(USER_PIN_4); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 5',")); oappendi(USER_PIN_5); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 6',")); oappendi(USER_PIN_6); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 7',")); oappendi(USER_PIN_7); oappend(SET_F(");"));
      oappend(SET_F("addOption(i,'Input 8',")); oappendi(USER_PIN_8); oappend(SET_F(");"));
      oappend(SET_F("i=addDropdown('CurrentVehicleIndicators',o);"));
      oappend(SET_F("addOption(i,'HIGH',1);"));
      oappend(SET_F("addOption(i,'LOW',0);"));
      oappend(SET_F("}"));
      oappend(SET_F("function addBooleanConfig(n){"));
      oappend(SET_F("i=addDropdown('CurrentVehicleIndicators',n);"));
      oappend(SET_F("addOption(i,'on',true);"));
      oappend(SET_F("addOption(i,'off',false);"));
      oappend(SET_F("}"));

      oappend(SET_F("addPinConfig('drivePin','driveOn');"));
      oappend(SET_F("addPinConfig('backupPin','backupOn');"));
      oappend(SET_F("addPinConfig('brakePin','brakeOn');"));
      oappend(SET_F("addPinConfig('leftBlinkerPin','leftBlinkerOn');"));
      oappend(SET_F("addPinConfig('rightBlinkerPin','rightBlinkerOn');"));
      oappend(SET_F("addPinConfig('ignitionPin','ignitionOn');"));
      oappend(SET_F("addPinConfig('accessoryPin','accessoryOn');"));
      oappend(SET_F("addPinConfig('aux1Pin','aux1On');"));

      oappend(SET_F("addBooleanConfig('ledPower1Enable');"));
      oappend(SET_F("addBooleanConfig('ledPower2Enable');"));

      oappend(SET_F("addBooleanConfig('psu1Enable');"));
      oappend(SET_F("addBooleanConfig('psu2Enable');"));

      oappend(SET_F("dd=addDropdown('CurrentVehicleIndicators','diagnosticMode');"));
      oappend(SET_F("addOption(dd,'none',-1);"));
      oappend(SET_F("addOption(dd,'LED1 Current',100);"));
      oappend(SET_F("addOption(dd,'LED2 Current',200);"));
      oappend(SET_F("addOption(dd,'Temp',300);"));
    }
};
