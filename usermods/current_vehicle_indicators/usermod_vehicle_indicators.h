#pragma once

#include <Arduino.h>
#include "wled.h"

const char* _name = "CurrentVehicleIndicators";

struct PinState {
  bool drive = false; 
  bool backup = false;
  bool brake = false;
  bool leftBlinker = false;
  bool rightBlinker = false;
};

class CurrentVehicleIndicatorUserMod : public Usermod {

  private:

    bool enabled = true;

    // states available here: https://docs.google.com/document/d/1xnQHJQ36Hoe6f8d2YZyPMUZjU54ghxtrWkC_2V9NMOE/edit#heading=h.xf94nigg76ec
    PinState state;

    int drivePin = -1; 
    int backupPin = -1;
    int brakePin = -1;
    int leftBlinkerPin = -1;
    int rightBlinkerPin = -1;

    int driveOn = 1; 
    int backupOn = 1;
    int brakeOn = 1;
    int leftBlinkerOn = 1;
    int rightBlinkerOn = 1;

    void setupPins() {
      Serial.println("Setting up pins for CurrentVehicleIndicatorUserMod");
      if (this->drivePin != -1) {
        Serial.print("Drive pin: "); Serial.print(this->drivePin); Serial.print(" on: "); Serial.println(this->driveOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->drivePin, INPUT);
      }
      if (this->backupPin != -1) {
        Serial.print("Backup pin: "); Serial.print(this->backupPin); Serial.print(" on: "); Serial.println(this->backupOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->backupPin, INPUT);
      }
      if (this->brakePin != -1) {
        Serial.print("Brake pin: "); Serial.print(this->brakePin); Serial.print(" on: "); Serial.println(this->brakeOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->brakePin, INPUT);
      }
      if (this->leftBlinkerPin != -1) {
        Serial.print("Left blinker pin: "); Serial.print(this->leftBlinkerPin); Serial.print(" on: "); Serial.println(this->leftBlinkerOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->leftBlinkerPin, INPUT);
      }
      if (this->rightBlinkerPin != -1) {
        Serial.print("Right blinker pin: "); Serial.print(this->rightBlinkerPin); Serial.print(" on: "); Serial.println(this->rightBlinkerOn == HIGH ? "HIGH" : "LOW");
        pinMode(this->rightBlinkerPin, INPUT);
      }
    }


    int lobster = 0;

    PinState readState() {
      if (++lobster % 10000 == 0) {
        Serial.println("========== io:");
        Serial.print("digitalRead(this->drivePin): "); Serial.println(digitalRead(this->drivePin));
        Serial.print("digitalRead(this->backupPin): "); Serial.println(digitalRead(this->backupPin));
        Serial.print("digitalRead(this->brakePin): "); Serial.println(digitalRead(this->brakePin));
        Serial.print("digitalRead(this->leftBlinkerPin): "); Serial.println(digitalRead(this->leftBlinkerPin));
        Serial.print("digitalRead(this->rightBlinkerPin): "); Serial.println(digitalRead(this->rightBlinkerPin));
      }

      PinState ret;
      ret.drive = this->drivePin != -1 ? digitalRead(this->drivePin) == this->driveOn : false;
      ret.backup = this->backupPin != -1 ? digitalRead(this->backupPin) == this->backupOn : false;
      ret.brake = this->brakePin != -1 ? digitalRead(this->brakePin) == this->brakeOn : false;
      ret.leftBlinker = this->leftBlinkerPin != -1 ? digitalRead(this->leftBlinkerPin) == this->leftBlinkerOn : false;
      ret.rightBlinker = this->rightBlinkerPin != -1 ? digitalRead(this->rightBlinkerPin) == this->rightBlinkerOn : false;
      return ret;
    }

    void appendPinConfig(const char* pinField, const char* onField) {
      oappend(SET_F("dd=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("', '")); oappend(SET_F(pinField)); oappend(SET_F("');"));
      oappend(SET_F("addOption(dd,'Input 1', 27);"));
      oappend(SET_F("addOption(dd,'Input 2', 28);"));
      oappend(SET_F("addOption(dd,'Input 3', 29);"));
      oappend(SET_F("addOption(dd,'Input 4', 30);"));
      oappend(SET_F("addOption(dd,'Input 5', 36);"));
      oappend(SET_F("addOption(dd,'Input 6', 37);"));
      oappend(SET_F("addOption(dd,'Input 7', 31);"));
      oappend(SET_F("addOption(dd,'Input 8', 33);"));
      
      oappend(SET_F("dd=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("', '")); oappend(SET_F(onField)); oappend(SET_F("');"));
      oappend(SET_F("addOption(dd,'HIGH', 1);"));
      oappend(SET_F("addOption(dd,'LOW', 0);"));
    }

  public:

    void loop() {
      if (!this->enabled) {
        return;
      }

      PinState newState = this->readState();
      bool driveChanged = newState.drive != this->state.drive;
      bool backupChanged = newState.backup != this->state.backup;
      bool brakeChanged = newState.brake != this->state.brake;
      bool leftBlinkerChanged = newState.leftBlinker != this->state.leftBlinker;
      bool rightBlinkerChanged = newState.rightBlinker != this->state.rightBlinker;

      bool changed = driveChanged || backupChanged || brakeChanged || leftBlinkerChanged || rightBlinkerChanged;
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

      if (driveChanged && this->state.drive) {
        // turn on drive lights
      } else if (driveChanged && !this->state.drive) {
        // turn off drive lights
      }

      if (backupChanged && this->state.backup) {
        // turn on backup lights
      } else if (backupChanged && !this->state.backup) {
        // turn off backup lights
      }

      if (brakeChanged && this->state.brake) {
        // turn on brake lights
      } else if (brakeChanged && !this->state.brake) {
        // turn off brake lights
      }

      if (leftBlinkerChanged && this->state.leftBlinker) {
        // turn on left blinker lights
      } else if (leftBlinkerChanged && !this->state.leftBlinker) {
        // turn off left blinker lights
      }

      if (rightBlinkerChanged && this->state.rightBlinker) {
        // turn on right blinker lights
      } else if (rightBlinkerChanged && !this->state.rightBlinker) {
        // turn off right blinker lights
      }
    }

    void enable(bool enable) {
      this->enabled = enable;
      setupPins();
    }

    void setup() {
      if (!this->enabled) {
        return;
      }
      setupPins();
    }

    void addToConfig(JsonObject& root) {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top.clear();

      top[FPSTR("enabled")] = enabled;
      top[F("drivePin")] = this->drivePin;
      top[F("backupPin")] = this->backupPin;
      top[F("brakePin")] = this->brakePin;
      top[F("leftBlinkerPin")] = this->leftBlinkerPin;
      top[F("rightBlinkerPin")] = this->rightBlinkerPin;

      top[F("driveOn")] = this->driveOn;
      top[F("backupOn")] = this->backupOn;
      top[F("brakeOn")] = this->brakeOn;
      top[F("leftBlinkerOn")] = this->leftBlinkerOn;
      top[F("rightBlinkerOn")] = this->rightBlinkerOn;
    }

    bool readFromConfig(JsonObject& root) {
      JsonObject top = root[FPSTR(_name)];
      getJsonValue(top[FPSTR("enabled")], this->enabled, false);
      getJsonValue(top[F("drivePin")], this->drivePin, -1);
      getJsonValue(top[F("backupPin")], this->backupPin, -1);
      getJsonValue(top[F("brakePin")], this->brakePin, -1);
      getJsonValue(top[F("leftBlinkerPin")], this->leftBlinkerPin, -1);
      getJsonValue(top[F("rightBlinkerPin")], this->rightBlinkerPin, -1);

      getJsonValue(top[F("driveOn")], this->driveOn, 1);
      getJsonValue(top[F("backupOn")], this->backupOn, 1);
      getJsonValue(top[F("brakeOn")], this->brakeOn, 1);
      getJsonValue(top[F("leftBlinkerOn")], this->leftBlinkerOn, 1);
      getJsonValue(top[F("rightBlinkerOn")], this->rightBlinkerOn, 1);

      this->drivePin = (int)this->drivePin;
      this->backupPin = (int)this->backupPin;
      this->brakePin = (int)this->brakePin;
      this->leftBlinkerPin = (int)this->leftBlinkerPin;
      this->rightBlinkerPin = (int)this->rightBlinkerPin;

      this->driveOn = (int)this->driveOn;
      this->backupOn = (int)this->backupOn;
      this->brakeOn = (int)this->brakeOn;
      this->leftBlinkerOn = (int)this->leftBlinkerOn;
      this->rightBlinkerOn = (int)this->rightBlinkerOn;

      this->enable(this->enabled);
      this->setupPins();

      return true;
    }

    void appendConfigData() {
      this->appendPinConfig("drivePin", "driveOn");
      this->appendPinConfig("backupPin", "backupOn");
      this->appendPinConfig("brakePin", "brakeOn");
      this->appendPinConfig("leftBlinkerPin", "leftBlinkerOn");
      this->appendPinConfig("rightBlinkerPin", "rightBlinkerOn");
    }
};
