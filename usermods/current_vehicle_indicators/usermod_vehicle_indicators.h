#pragma once

#include "wled.h"p

char* _name = "Current Vehicle Indicators";

typedef struct PinState {
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


    void setupPins() {
      Serial.println("Setting up pins for CurrentVehicleIndicatorUserMod");
      if (this->drivePin != -1) {
        Serial.print("Drive pin: "); Serial.println(this->drivePin);
        pinMode(this->drivePin, INPUT);
      }
      if (this->backupPin != -1) {
        Serial.print("Backup pin: "); Serial.println(this->backupPin);
        pinMode(this->backupPin, INPUT);
      }
      if (this->brakePin != -1) {
        Serial.print("Braje pin: "); Serial.println(this->brakePin);
        pinMode(this->brakePin, INPUT);
      }
      if (this->leftBlinkerPin != -1) {
        Serial.print("Left blinker pin: "); Serial.println(this->leftBlinkerPin);
        pinMode(this->leftBlinkerPin, INPUT);
      }
      if (this->rightBlinkerPin != -1) {
        Serial.print("Right blinker pin: "); Serial.println(this->rightBlinkerPin);
        pinMode(this->rightBlinkerPin, INPUT);
      }
    }

    PinState readState() {
      PinState ret;
      ret.drive = this->drivePin != -1 ? digitalRead(this->drivePin) : false;
      ret.backup = this->backupPin != -1 ? digitalRead(this->backupPin) : false;
      ret.brake = this->brakePin != -1 ? digitalRead(this->brakePin) : false;
      ret.leftBlinker = this->leftBlinkerPin != -1 ? digitalRead(this->leftBlinkerPin) : false;
      ret.rightBlinker = this->rightBlinkerPin != -1 ? digitalRead(this->rightBlinkerPin) : false;
      return ret;
    }

  public:

    inline void enable(bool enable) { enabled = enable; }

    inline bool isEnabled() { return true; }

    void setup() {
      setupPins();
    }

    void loop() {

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

    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      Serial.println("Connected to WiFi!");
    }

    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    void addToJsonInfo(JsonObject& root)
    {
      // if "u" object does not exist yet wee need to create it
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      //this code adds "u":{"ExampleUsermod":[20," lux"]} to the info object
      //int reading = 20;
      //JsonArray lightArr = user.createNestedArray(FPSTR(_name))); //name
      //lightArr.add(reading); //value
      //lightArr.add(F(" lux")); //unit

      // if you are implementing a sensor usermod, you may publish sensor data
      //JsonObject sensor = root[F("sensor")];
      //if (sensor.isNull()) sensor = root.createNestedObject(F("sensor"));
      //temp = sensor.createNestedArray(F("light"));
      //temp.add(reading);
      //temp.add(F("lux"));
    }


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      // if (!initDone || !enabled) return;  // prevent crash on boot applyPreset()

      // JsonObject usermod = root[FPSTR(_name)];
      // if (usermod.isNull()) usermod = root.createNestedObject(FPSTR(_name));

      //usermod["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      // if (!initDone) return;  // prevent crash on boot applyPreset()

      // JsonObject usermod = root[FPSTR(_name)];
      // if (!usermod.isNull()) {
      //   // expect JSON usermod data in usermod name object: {"ExampleUsermod:{"user0":10}"}
      //   userVar0 = usermod["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
      // }
      // // you can as well check WLED state JSON keys
      // //if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject(FPSTR(_name));
      top[FPSTR("enabled")] = enabled;
      // //save these vars persistently whenever settings are saved
      // top["great"] = userVar0;
      // top["testBool"] = testBool;
      // top["testInt"] = testInt;
      // top["testLong"] = testLong;
      // top["testULong"] = testULong;
      top["testFloat"] = 420.69;
      top["testString"] = "string goes here";
      // JsonArray pinArray = top.createNestedArray("pin");
      // pinArray.add(testPins[0]);
      // pinArray.add(testPins[1]); 
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      // JsonObject top = root[FPSTR(_name)];

      // bool configComplete = !top.isNull();

      // configComplete &= getJsonValue(top["great"], userVar0);
      // configComplete &= getJsonValue(top["testBool"], testBool);
      // configComplete &= getJsonValue(top["testULong"], testULong);
      // configComplete &= getJsonValue(top["testFloat"], testFloat);
      // configComplete &= getJsonValue(top["testString"], testString);

      // // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      // configComplete &= getJsonValue(top["testInt"], testInt, 42);  
      // configComplete &= getJsonValue(top["testLong"], testLong, -42424242);

      // // "pin" fields have special handling in settings page (or some_pin as well)
      // // configComplete &= getJsonValue(top["pin"][0], testPins[0], -1);
      // // configComplete &= getJsonValue(top["pin"][1], testPins[1], -1);

      return true;
    }


    /*
     * appendConfigData() is called when user enters usermod settings page
     * it may add additional metadata for certain entry fields (adding drop down is possible)
     * be careful not to add too much as oappend() buffer is limited to 3k
     */
    void appendConfigData()
    {
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":great")); oappend(SET_F("',1,'<i>(this is a great config value)</i>');"));
      oappend(SET_F("addInfo('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F(":testString")); oappend(SET_F("',1,'enter any string you want');"));
      oappend(SET_F("dd=addDropdown('")); oappend(String(FPSTR(_name)).c_str()); oappend(SET_F("','testInt');"));
      oappend(SET_F("addOption(dd,'Nothing',0);"));
      oappend(SET_F("addOption(dd,'Everything',42);"));
    }


    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      //strip.setPixelColor(0, RGBW32(0,0,0,0)) // set the first pixel to black
    }


    /**
     * handleButton() can be used to override default button behaviour. Returning true
     * will prevent button working in a default way.
     * Replicating button.cpp
     */
    bool handleButton(uint8_t b) {
      yield();
      return true;
    }
};

