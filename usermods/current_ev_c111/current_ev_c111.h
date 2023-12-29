#pragma once

#include <Arduino.h>
#include <LED_Controller.h>

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
  float psu1Current = 0.0f;
  float psu2Current = 0.0f;
};

#define USER_PIN_1 C111_USER_INPUT_1
#define USER_PIN_2 C111_USER_INPUT_2
#define USER_PIN_3 C111_USER_INPUT_3
#define USER_PIN_4 C111_USER_INPUT_4
#define USER_PIN_5 C111_USER_INPUT_5
#define USER_PIN_6 C111_USER_INPUT_6
#define USER_PIN_7 C111_USER_INPUT_7
#define USER_PIN_8 C111_USER_INPUT_8

#define DRIVE_PIN_DEFAULT       USER_PIN_1
#define BACKUP_PIN_DEFAULT      USER_PIN_2
#define BRAKE_PIN_DEFAULT       USER_PIN_3
#define LEFT_PIN_DEFAULT        USER_PIN_4
#define RIGHT_PIN_DEFAULT       USER_PIN_5
#define IGNITION_PIN_DEFAULT    USER_PIN_6
#define ACCESSORY_PIN_DEFAULT   USER_PIN_7
#define AUX1_PIN_DEFAULT        USER_PIN_8

#define CHATTER_FREQUENCY 2000

#define OVERHEATED_TEMP_CELCIUS 80.0

class CurrentVehicleIndicatorUserMod : public Usermod {

  public:
    CurrentVehicleIndicatorUserMod() :
      c111() {}

  private:
    LED_Controller c111;

    bool enabled = true;
    bool overheated = false;
    int chatterGuard = 0;

    // states available here: https://docs.google.com/document/d/1xnQHJQ36Hoe6f8d2YZyPMUZjU54ghxtrWkC_2V9NMOE/edit#heading=h.xf94nigg76ec
    PinState state;

    bool psu1Enable = false;
    bool psu2Enable = false;
    bool differentialDataEnabled = false;
    bool canTerminated = false;

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
      
      c111.setDifferentialDataEnabled(this->differentialDataEnabled);
      c111.setCanTerminated(this->canTerminated);
    }

    void updateLightPowerSettings() {
      c111.enablePSU1(this->psu1Enable && !c111.isOverHeated());
      c111.enablePSU2(this->psu2Enable && !c111.isOverHeated());
    }

    PinState readState() {

      PinState ret;
      ret.drive = this->drivePin != -1 ? c111.getUserInputState(this->drivePin) == this->driveOn : false;
      ret.backup = this->backupPin != -1 ? c111.getUserInputState(this->backupPin) == this->backupOn : false;
      ret.brake = this->brakePin != -1 ? c111.getUserInputState(this->brakePin) == this->brakeOn : false;
      ret.leftBlinker = this->leftBlinkerPin != -1 ? c111.getUserInputState(this->leftBlinkerPin) == this->leftBlinkerOn : false;
      ret.rightBlinker = this->rightBlinkerPin != -1 ? c111.getUserInputState(this->rightBlinkerPin) == this->rightBlinkerOn : false;
      ret.ignition = this->ignitionPin != -1 ? c111.getUserInputState(this->ignitionPin) == this->ignitionOn : false;
      ret.accessory = this->accessoryPin != -1 ? c111.getUserInputState(this->accessoryPin) == this->accessoryOn : false;
      ret.aux1 = this->aux1Pin != -1 ? c111.getUserInputState(this->aux1Pin) == this->aux1On : false;

      ret.psu1Current = c111.getPSUCurrent(C111_SENSOR_PSU1_CURRENT);
      ret.psu2Current = c111.getPSUCurrent(C111_SENSOR_PSU2_CURRENT);

      if (this->canChatter()) {
        Serial.println("========== c111:");
        Serial.print("canTerminated: "); Serial.println(this->canTerminated);
        Serial.print("digitalRead(this->drivePin): "); Serial.println(c111.getUserInputState(this->drivePin));
        Serial.print("digitalRead(this->backupPin): "); Serial.println(c111.getUserInputState(this->backupPin));
        Serial.print("digitalRead(this->brakePin): "); Serial.println(c111.getUserInputState(this->brakePin));
        Serial.print("digitalRead(this->leftBlinkerPin): "); Serial.println(c111.getUserInputState(this->leftBlinkerPin));
        Serial.print("digitalRead(this->rightBlinkerPin): "); Serial.println(c111.getUserInputState(this->rightBlinkerPin));
        Serial.print("digitalRead(this->ignitionPin): "); Serial.println(c111.getUserInputState(this->ignitionPin));
        Serial.print("digitalRead(this->accessoryPin): "); Serial.println(c111.getUserInputState(this->accessoryPin));
        Serial.print("digitalRead(this->aux1Pin): "); Serial.println(c111.getUserInputState(this->aux1Pin));
        Serial.print("c111.isPSU1Enabled(): "); Serial.println(c111.isPSU1Enabled());
        Serial.print("c111.isPSU2Enabled(): "); Serial.println(c111.isPSU2Enabled());
        Serial.print("C111_SENSOR_PSU1_CURRENT: "); Serial.println(ret.psu1Current);
        Serial.print("C111_SENSOR_PSU2_CURRENT: "); Serial.println(ret.psu2Current);
        Serial.print("c111.getTemperatureCelcius(): "); Serial.println(c111.getTemperatureCelcius());
        Serial.print("c111.isDifferentialDataEnabled()"); Serial.println(c111.isDifferentialDataEnabled());
      }

      return ret;
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

      if (c111.isOverHeated()) {
        this->overheated = true;
        // handle overheating
        if (this->canChatter()) {
          Serial.print("WARNING!!! System is overheated, disabling power to lights: ");
          Serial.print(c111.getTemperatureCelcius()); Serial.println(" celcius");
        }
        this->updateLightPowerSettings();
        return;

      } else if (!c111.isOverHeated() && this->overheated) {
        this->overheated = false;
        // handle coming out of overheating
        Serial.print("No longer overheated, enabling power to lights: ");
        Serial.print(c111.getTemperatureCelcius()); Serial.println(" celcius");
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
      c111.initialize();
      c111.setOverheatTempCelcius(OVERHEATED_TEMP_CELCIUS);
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

      top[F("psu1Enable")] = this->psu1Enable;
      top[F("psu2Enable")] = this->psu2Enable;
      top[F("differentialDataEnabled")] = this->differentialDataEnabled;
      top[F("canTerminated")] = this->canTerminated;
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

      getJsonValue(top[F("psu1Enable")], this->psu1Enable, false);
      getJsonValue(top[F("psu2Enable")], this->psu2Enable, false);
      getJsonValue(top[F("differentialDataEnabled")], this->differentialDataEnabled, false);
      getJsonValue(top[F("canTerminated")], this->canTerminated, false);

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

      oappend(SET_F("addBooleanConfig('psu1Enable');"));
      oappend(SET_F("addBooleanConfig('psu2Enable');"));
      oappend(SET_F("addBooleanConfig('canTerminated');"));
      oappend(SET_F("addBooleanConfig('differentialDataEnabled');"));
    }
};
