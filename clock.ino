//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>

const int LCD_CHARACTERS_LEN = 16;

LiquidCrystal_I2C lcd(0x27, LCD_CHARACTERS_LEN, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

// clock
ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

// bof: menu related types
typedef void (*IncDecFunc)();

typedef String (*ValStrFormatFunc)();

enum ItemType { NAVIGATION, INT_SETTING };

struct MenuItem {
  const char* label;
  const char* secondLabel;
  ItemType itemType;
  int* intValuePtr;
  ValStrFormatFunc valStrFornatFunc;
  IncDecFunc incValFunc;
  IncDecFunc decValFunc;

  MenuItem* parent;
  MenuItem* firstChild;
  MenuItem* next;
  MenuItem* prev;
};

MenuItem mainMenu = {"Settings", "Exit       Enter", NAVIGATION, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

int lightsTurnOnHour = 9;
void incrementTurnOnHour() {
  if (lightsTurnOnHour==24) { 
    lightsTurnOnHour = 0;
    return;
  }
  lightsTurnOnHour++;
  return;
}
String formatlightsTurnOnHour() {
  char buffer[5];

  sprintf(buffer, "%02d H", lightsTurnOnHour);
  return String(buffer);
}
void decrementTurnOnHour() {
  if (lightsTurnOnHour==0) {
    lightsTurnOnHour = 24;
    return;
  }
  lightsTurnOnHour--;
  return;
}
MenuItem lightsTurnOnHourNav = {"Lamp Turn On HH", NULL, NAVIGATION, NULL, NULL, NULL, NULL, &mainMenu, NULL, NULL, NULL};
MenuItem lightsTurnOnHourSettings = {"Lamp Turn On HH", NULL, INT_SETTING, &lightsTurnOnHour, &formatlightsTurnOnHour, &incrementTurnOnHour, &decrementTurnOnHour, &lightsTurnOnHourNav, NULL, NULL, NULL};

int lightsTurnOnMins = 0;
void incrementTurnOnMins() {
  if (lightsTurnOnMins==60) { 
    lightsTurnOnMins = 0;
    return;
  }
  lightsTurnOnMins++;
  return;
}
String formatlightsTurnOMins() {
  char buffer[5];

  sprintf(buffer, "%02d m", lightsTurnOnMins);
  return String(buffer);
}
void decrementTurnOnMins() {
  if (lightsTurnOnMins==0) {
    lightsTurnOnMins = 60;
    return;
  }
  lightsTurnOnMins--;
  return;
}
MenuItem lightsTurnOnMinsNav = {"Lamp Turn On MM", NULL, NAVIGATION, NULL, NULL, NULL, NULL, &mainMenu, NULL, NULL, NULL};
MenuItem lightsTurnOnMinsSettings = {"Lamp Turn On MM", NULL, INT_SETTING, &lightsTurnOnMins, &formatlightsTurnOMins, &incrementTurnOnMins, &decrementTurnOnMins, &lightsTurnOnMinsNav, NULL, NULL, NULL};

int lightsTurnOffHour = 18;
void incrementTurnOffHour() {
  if (lightsTurnOffHour==24) { 
    lightsTurnOffHour = 0;
    return;
  }
  lightsTurnOffHour++;
  return;
}
String formatlightsTurnOffHour() {
  char buffer[5];

  sprintf(buffer, "%02d H", lightsTurnOffHour);
  return String(buffer);
}
void decrementTurnOffHour() {
  if (lightsTurnOffHour==0) {
    lightsTurnOffHour = 24;
    return;
  }
  lightsTurnOffHour--;
  return;
}
MenuItem lightsTurnOffHourNav = {"Lamp Turn Off HH", NULL, NAVIGATION, NULL, NULL, NULL, NULL, &mainMenu, NULL, NULL, NULL};
MenuItem lightsTurnOffHourSettings = {"Lamp Turn Off HH", NULL, INT_SETTING, &lightsTurnOffHour, &formatlightsTurnOffHour, &incrementTurnOffHour, &decrementTurnOffHour, &lightsTurnOffHourNav, NULL, NULL, NULL};

int lightsTurnOffMins = 0;
void incrementTurnOffMins() {
  if (lightsTurnOffMins==60) { 
    lightsTurnOffMins = 0;
    return;
  }
  lightsTurnOffMins++;
  return;
}
String formatlightsTurnOffMins() {
  char buffer[5];

  sprintf(buffer, "%02d m", lightsTurnOffMins);
  return String(buffer);
}
void decrementTurnOffMins() {
  if (lightsTurnOffMins==0) {
    lightsTurnOffMins = 60;
    return;
  }
  lightsTurnOffMins--;
  return;
}
MenuItem lightsTurnOffMinsNav = {"Lamp Turn Off MM", NULL, NAVIGATION, NULL, NULL, NULL, NULL, &mainMenu, NULL, NULL, NULL};
MenuItem lightsTurnOffMinsSettings = {"Lamp Turn Off MM", NULL, INT_SETTING, &lightsTurnOffMins, &formatlightsTurnOffMins, &incrementTurnOffMins, &decrementTurnOffMins, &lightsTurnOffMinsNav, NULL, NULL, NULL};
// eof: menu related types

const int DEBOUNCE_BTN_TIME = 400;
unsigned long lastBtnPressedTimestamp = 0;
MenuItem* activeMenu = NULL;
const unsigned long EXIT_MENU_TIMEOUT = 20UL*1000UL;
bool lampIsTurnedOn = false;
bool isLcdBacklightOn = false;
const unsigned long LCD_BACKLIGHT_OFF_TIMOUT_MS = 5UL*60UL*1000UL;

void setup()
{
  Serial.begin(9600);

  Serial.println("setup is called");

  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);

  pinMode(13, OUTPUT);

  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  isLcdBacklightOn = true;
  lcd.setCursor(6,0);
  lcd.print("CLOCK");
  lcd.setCursor(2,1);
  lcd.print("Zdravei, Viki!");

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);

  // uncomment the bellow, at the very first start, to ensure the clock is up to date
  // Serial.println("Overrite the current clock time!");
  // Rtc.SetIsWriteProtected(false);
  // Rtc.SetDateTime(compiled);
  
  Rtc.SetIsWriteProtected(true);
  
  // restore the current lamp state, according to the current time and the turn on and off settings
  lampIsTurnedOn = calculateLigthState();
  if (lampIsTurnedOn) {
    digitalWrite(13, HIGH);
  }


  lcd.clear();

  // bof: menu tree wiring:
  mainMenu.firstChild = &lightsTurnOnHourNav;
  lightsTurnOnHourNav.parent = &mainMenu;
  lightsTurnOnHourNav.next = &lightsTurnOnMinsNav;
  lightsTurnOnHourNav.prev = &lightsTurnOffMinsNav;
  lightsTurnOnHourNav.firstChild = &lightsTurnOnHourSettings;

  lightsTurnOnMinsNav.parent = &mainMenu;
  lightsTurnOnMinsNav.next = &lightsTurnOffHourNav;
  lightsTurnOnMinsNav.prev = &lightsTurnOnHourNav;
  lightsTurnOnMinsNav.firstChild = &lightsTurnOnMinsSettings;

  lightsTurnOffHourNav.parent = &mainMenu;
  lightsTurnOffHourNav.next = &lightsTurnOffMinsNav;
  lightsTurnOffHourNav.prev = &lightsTurnOnMinsNav;
  lightsTurnOffHourNav.firstChild = &lightsTurnOffHourSettings;

  lightsTurnOffMinsNav.parent = &mainMenu;
  lightsTurnOffMinsNav.next = &lightsTurnOnHourNav;
  lightsTurnOffMinsNav.prev = &lightsTurnOffHourNav;
  lightsTurnOffMinsNav.firstChild = &lightsTurnOffMinsSettings;
  // eof: menu tree wiring

  
}


void loop()
{
  int leftBtnVal = !digitalRead(8);
  int rightBtnVal = !digitalRead(9);
  int upBtnVal = !digitalRead(10);
  int downBtnVal = !digitalRead(11);

  if (leftBtnVal || rightBtnVal || upBtnVal || downBtnVal) {
    if (millis() - lastBtnPressedTimestamp > DEBOUNCE_BTN_TIME) {
      lastBtnPressedTimestamp = millis();
      Serial.println("button clicked");
      if (!isLcdBacklightOn) {
        Serial.println("enable lcd backlight");
        lcd.backlight();
        isLcdBacklightOn = true;
      }

      if (activeMenu == NULL) {
        activeMenu = &mainMenu;
      } else {
        handleButtonClickeddWithinMenu(leftBtnVal, rightBtnVal, upBtnVal, downBtnVal);
      }

      showMenuItem(activeMenu);
      delay(10);
    }
  }
  if (millis() - lastBtnPressedTimestamp > EXIT_MENU_TIMEOUT) {
    activeMenu = NULL;
  }
  // Serial.print("elapsed millis since btnClick: ");
  // Serial.println(millis() - lastBtnPressedTimestamp);
  // Serial.println(LCD_BACKLIGHT_OFF_TIMOUT_MS);
  if (isLcdBacklightOn && (millis() - lastBtnPressedTimestamp > LCD_BACKLIGHT_OFF_TIMOUT_MS)) {
    Serial.println("disable lcd backlight");
    lcd.noBacklight();
    isLcdBacklightOn = false;
  }

  if (lampIsTurnedOn != calculateLigthState()) {
    if (!lampIsTurnedOn) {
      digitalWrite(13, HIGH);
      lampIsTurnedOn = true;
    } else {
      digitalWrite(13, LOW);
      lampIsTurnedOn = false;
    }
  }
  
  
  if (activeMenu == NULL) {
    showClock();
    delay(10);
  }

  // Serial.print(leftBtnVal);
  // Serial.print(" ");
  // Serial.print(rightBtnVal);
  // Serial.print(" ");
  // Serial.print(upBtnVal);
  // Serial.print(" ");
  // Serial.println(downBtnVal);
}

void handleButtonClickeddWithinMenu(bool leftBtnVal, bool rightBtnVal, bool upBtnVal, bool downBtnVal) {
  if (activeMenu != NULL) {
    if (leftBtnVal) {
      if (activeMenu->itemType == NAVIGATION) {
        // navigation item, so -> navgate
        if (activeMenu->prev != NULL) {
          activeMenu = activeMenu->prev;
        }
      } else {
        // settting menu, so -> change values instead of navigating
        activeMenu->decValFunc();
      }
      return;
    }

    if (rightBtnVal) {
      if (activeMenu->itemType == NAVIGATION) {
        // navigation item, so -> navgate
        if (activeMenu->next != NULL) {
          activeMenu = activeMenu->next;
        }
      } else {
        // settting menu, so -> change values instead of navigating
        activeMenu->incValFunc();
      }
      return;
    }

    if (upBtnVal) {
      // if parent is NULL, then this will exit from the menu and it will get back go the clock
      activeMenu = activeMenu->parent;
      return;
    }

    if (downBtnVal) {
      if (activeMenu->firstChild != NULL) {
        activeMenu = activeMenu->firstChild;
      } else {
        if (activeMenu->itemType == INT_SETTING) {
          activeMenu = activeMenu->parent;
          return;
        }
      }
      return;
    }
  }
  return;
}

void showClock() {
  RtcDateTime now = Rtc.GetDateTime();
  String currentDate = str_Date(now);
  lcd.setCursor(0,0);
  lcd.print(stringCharacterCentered(currentDate));
  String currentTime = str_Time(now);
  lcd.setCursor(0,1);
  lcd.print(stringCharacterCentered(currentTime));
}

void showMenuItem(MenuItem* menuItem) {
  if (menuItem != NULL) {
    lcd.setCursor(0,0);
    String lbl = stringCharacterCentered(menuItem->label);
    Serial.println(lbl);
    lcd.print(lbl);
    lcd.setCursor(0,1);
    if (menuItem->secondLabel != NULL) {
      lcd.print(stringCharacterCentered(menuItem->secondLabel));
    } else {
      if (menuItem->itemType == NAVIGATION) {
        if (menuItem->firstChild == NULL) {
          lcd.print(stringCharacterCentered("<  >  Back -"));
        } else {
          if (menuItem->firstChild->itemType == NAVIGATION) {
            lcd.print(stringCharacterCentered("<  >  Back Enter"));
          } else {
            lcd.print(stringCharacterCentered("<  >  Back Edit"));
          }
        }
      } else {
        lcd.print(settingsCentered(menuItem->valStrFornatFunc()));
      }
      lcd.print(stringCharacterCentered(""));
    }
  }
}

String stringCharacterCentered(String str) {
  if (str.length() > LCD_CHARACTERS_LEN) {
    return str;
  }
  char centeredstr[LCD_CHARACTERS_LEN + 1];
  for (int i = 0; i < LCD_CHARACTERS_LEN; i++) {
    centeredstr[i] = ' ';
  }
  centeredstr[LCD_CHARACTERS_LEN] = '\0';

  int strLen = str.length();
  if (strLen > LCD_CHARACTERS_LEN) strLen = LCD_CHARACTERS_LEN;
  int startPos = (LCD_CHARACTERS_LEN - strLen) / 2;
  for (int i = 0; i < strLen; i++) {
    centeredstr[startPos + i] = str[i];
  }

  return String(centeredstr);
}

String settingsCentered(String str) {
  char centeredstr[LCD_CHARACTERS_LEN + 1];
  centeredstr[0] = '<';
  centeredstr[1] = ' ';
  centeredstr[2] = '>';
  for (int i = 3; i < LCD_CHARACTERS_LEN-2; i++) {
    centeredstr[i] = ' ';
  }
  centeredstr[LCD_CHARACTERS_LEN-2] = 'O';
  centeredstr[LCD_CHARACTERS_LEN-1] = 'k';
  centeredstr[LCD_CHARACTERS_LEN] = '\0';

  int strLen = str.length();
  if (strLen > LCD_CHARACTERS_LEN) strLen = LCD_CHARACTERS_LEN;
  int startPos = (LCD_CHARACTERS_LEN - strLen) / 2;
  for (int i = 0; i < strLen; i++) {
    centeredstr[startPos + i] = str[i];
  }

  return String(centeredstr);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

String str_Date(const RtcDateTime& dt) {
  char datestring[11];

  snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u"),
            dt.Day(),
            dt.Month(),
            dt.Year() );
  return String(datestring);
}

String str_Time(const RtcDateTime& dt) {
  char timetring[9];

  snprintf_P(timetring, 
            countof(timetring),
            PSTR("%02u:%02u:%02u"),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
  return String(timetring);
}

String printDateTime(const RtcDateTime& dt)
{
    char datestimetring[26];

    snprintf_P(datestimetring, 
            countof(datestimetring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    return String(datestimetring);
}

/*
Calculates what should be the current state, based on the current time and the lights turn on and turn off settings.

returns true if lights should be on and false otherwise
*/
bool calculateLigthState() {
  RtcDateTime now = Rtc.GetDateTime();
  if (lightsTurnOffHour == lightsTurnOnHour) {
    if (lightsTurnOffMins > lightsTurnOnMins) {
      if (now.Minute() >= lightsTurnOnMins && now.Minute() < lightsTurnOffMins) {
        return true;
      } else {
        return false;
      }
    } else {
      // lightsTurnOffMins < lightsTurnOnMins case
      if (now.Minute() >= lightsTurnOffMins && now.Minute() < lightsTurnOnMins) {
        return false;
      } else {
        return true;
      }
    }
  } else if (lightsTurnOffHour > lightsTurnOnHour) {
    if (now.Hour() == lightsTurnOnHour) {
      if (now.Minute() >= lightsTurnOnMins) {
        return true;
      } else {
        return false;
      }
    } else if (now.Hour() == lightsTurnOffHour) {
      if (now.Minute() < lightsTurnOffMins) {
        return true;
      } else {
        return false;
      }
    } else if (now.Hour() > lightsTurnOnHour && now.Hour() < lightsTurnOffHour) {
      return true;
    } else {
      return false;
    }
  } else {
    // lightsTurnOffHour < lightsTurnOnHour case
    if (now.Hour() == lightsTurnOffHour) {
      if (now.Minute() >= lightsTurnOffMins) {
        return false;
      } else {
        return true;
      }
    } else if (now.Hour() == lightsTurnOnHour) {
      if (now.Minute() < lightsTurnOnMins) {
        return false;
      } else {
        return true;
      }
    } else if (now.Hour() > lightsTurnOffHour && now.Hour() < lightsTurnOnHour) {
      return false;
    } else {
      return true;
    }
  }
}
