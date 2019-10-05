#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <time.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "Font5x3.h"

#define WIFI_SSID       "XXXXXXXXXXXXX"
#define WIFI_PASSWORD   "XXXXXXXXXXXXX"

const IPAddress gw(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);
const IPAddress dns(192, 168, 0, 1);
const IPAddress ip(192,168, 0, 61);

WebServer server(80);

// touch sensors
#define TOUCH_IN T4

// MD_Parola settings
#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
//#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 27      // or SCK
#define DATA_PIN 12     // or MOSI
#define CS_PIN 14       // or SS
#define INTENSITY   0   // LED INTENSITY 0-15

// Sprite Definitions
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
};

const uint8_t F_INVADER = 6;
const uint8_t W_INVADER = 12;
static const uint8_t PROGMEM invader[F_INVADER * W_INVADER] =
{
  0x00, 0x70, 0x10, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x10, 0x70, 0x00,
  0x00, 0x70, 0x10, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x10, 0x70, 0x00,
  0x00, 0x70, 0x10, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x10, 0x70, 0x00,
  0x00, 0x1c, 0x28, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x28, 0x1c, 0x00,
  0x00, 0x1c, 0x28, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x28, 0x1c, 0x00,
  0x00, 0x1c, 0x28, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x28, 0x1c, 0x00
};

const uint8_t F_INVADER1 = 6;
const uint8_t W_INVADER1 = 10;
static const uint8_t PROGMEM invader1[F_INVADER1 * W_INVADER1] =
{
  0x00, 0x98, 0x5c, 0xf6, 0x7f, 0x7f, 0xf6, 0x5c, 0x98, 0x00,
  0x00, 0x98, 0x5c, 0xf6, 0x7f, 0x7f, 0xf6, 0x5c, 0x98, 0x00,
  0x00, 0x98, 0x5c, 0xf6, 0x7f, 0x7f, 0xf6, 0x5c, 0x98, 0x00,
  0x00, 0x98, 0x5c, 0xfe, 0x7f, 0x7f, 0xfe, 0x5c, 0x98, 0x00,
  0x00, 0x98, 0x5c, 0xfe, 0x7f, 0x7f, 0xfe, 0x5c, 0x98, 0x00,
  0x00, 0x98, 0x5c, 0xfe, 0x7f, 0x7f, 0xfe, 0x5c, 0x98, 0x00,
};

// Hardware SPI connection
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
//MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// TimeZonw
#define JST 3600 * 9

// pushwheel
boolean displayValue(MD_MAX72XX &mx, char *msg);

// Temperature
bool getTemperature(float *temp, float *humi);


enum CLOCK_TYPE
{
  HHMMSS,
  MMSS
};

enum DATE_TYPE
{
  MMDD,
  DDMM,
  YYMMDD,
  MMDDYY,
  DDMMYY
};

enum state_t
{
  INITIALISE,
  CLOCK_JUST,

  CLOCK_INIT,
  CLOCK_OPEN,
  CLOCK_ANI,
  DATE_INIT,
  DATE_ANI,
  TEMPHUMI_INIT,
  TEMPHUMI_ANI
};

state_t state = INITIALISE;

typedef struct _SETTINGS {
  CLOCK_TYPE clockType;
  DATE_TYPE dateType;

} SETTINGS;

SETTINGS settings;

void setup()
{
  Serial.begin(115200);

  touchAttachInterrupt(TOUCH_IN, touched, 40);

  while (!biginWiFi())
  {
    delay(1000);
  }

  initClock();
}

void loop()
{
  static uint8_t j = 0;
  static char stemp[10], shumi[10];
  float temp, humi;
  static char buf[32];

  server.handleClient();

  switch (state)
  {
  case INITIALISE:
    mx.begin();
    P.begin();
    P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
    //P.setSpriteData(invader, W_INVADER, F_INVADER, invader1, W_INVADER1, F_INVADER1);
    mx.setFont(_Fixed_5x3_1);
    P.setFont(_Fixed_5x3_1);
    P.setIntensity(INTENSITY);

    state = CLOCK_INIT;
    break;

  case TEMPHUMI_INIT:
    if (!getTemperature(&temp, &humi))
    {
      state = CLOCK_INIT;
      return;
    }
    if (humi < 1.0f)
    {
      return;
    }
    sprintf(stemp, "%s%cC", String(temp, 1).c_str(), 128);
    Serial.println(stemp);
    sprintf(shumi, "%s%%", String(humi, 1).c_str());
    Serial.println(shumi);
    if (j == 0)
    {
      P.displayText(stemp, PA_CENTER, 20, 2000, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
    }
    else
    {
      P.displayText(shumi, PA_CENTER, 20, 2000, PA_WIPE_CURSOR, PA_WIPE_CURSOR);
    }
    state = TEMPHUMI_ANI;
    // fall through
    
  case TEMPHUMI_ANI:
    if (P.displayAnimate()) // animates and returns true when an animation is completed
    {
      if (j >= 1)
      {
        state = CLOCK_INIT;
        j = 0;
      }
      else
      {
        state = TEMPHUMI_INIT;
        j++;
      }
      delay(100); // wait a while to show the text ...
    }
    break;

  case DATE_INIT:
    getDateText(settings.dateType, buf);
    Serial.println(buf);
    P.displayText(buf, PA_CENTER, 40, 3000, PA_BLINDS, PA_BLINDS);
    state = DATE_ANI;
  case DATE_ANI:
    if (P.displayAnimate()) // animates and returns true when an animation is completed
    {
      state = CLOCK_INIT;
    }
    break;

  case CLOCK_INIT:
    getClockText(settings.clockType, buf);
    Serial.println(buf);
    P.displayText(buf, PA_CENTER, 20, 0, PA_OPENING_CURSOR, PA_PRINT);
    state = CLOCK_OPEN;
    // fall through

  case CLOCK_OPEN:
    if (P.displayAnimate()) // animates and returns true when an animation is completed
    {
      state = CLOCK_ANI;
    }
    break;

  case CLOCK_ANI:
    if (displayValue(mx, buf))
    {
      getClockText(settings.clockType, buf);
      if (isJustTime(buf))
      {
        state = CLOCK_JUST;
        P.displayText(buf, PA_CENTER, 40, 1000, PA_SPRITE, PA_SPRITE);
      }
    }
    break;

  case CLOCK_JUST:
    if (P.displayAnimate()) // animates and returns true when an animation is completed
    {
      state = CLOCK_INIT;
    }
    break;
  }
}

void getClockText(CLOCK_TYPE ctype, char *timestr)
{
  struct tm local;

  // now
  getLocalTime(&local);

  switch (ctype)
  {
    case HHMMSS:
      sprintf(timestr, "%02d:%02d:%02d", local.tm_hour, local.tm_min, local.tm_sec);
      break;
    case MMSS:
      sprintf(timestr, "%02d:%02d", local.tm_hour, local.tm_min);
      break;
  }
}

void getDateText(DATE_TYPE dtype, char *datestr)
{
  struct tm local;
  static const char *wd3[7] = {"Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"};
  static const char *wd[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  // now
  getLocalTime(&local);

  switch (dtype)
  {
    case MMDD:
      sprintf(datestr, "%02d/%02d%s", local.tm_mon + 1, local.tm_mday, wd3[local.tm_wday]);
      break;
    case DDMM:
      sprintf(datestr, "%02d/%02d%s", local.tm_mday, local.tm_mon + 1, wd3[local.tm_wday]);
      break;
    case YYMMDD:
      sprintf(datestr, "%02d/%02d/%02d %s", local.tm_year % 100, local.tm_mon + 1, local.tm_mday, wd[local.tm_wday]);
      break;
    case MMDDYY:
      sprintf(datestr, "%02d/%02d/%02d %s", local.tm_mon + 1, local.tm_mday, local.tm_year % 100, wd[local.tm_wday]);
      break;
    case DDMMYY:
      sprintf(datestr, "%02d/%02d/%02d %s", local.tm_mday, local.tm_mon + 1, local.tm_year % 100, wd[local.tm_wday]);
      break;
  }
}

bool biginWiFi()
{
  Serial.println("biginWiFi");

  // WiFi connect
  WiFi.config(ip, gw, subnet, dns);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  for (int i = 0; i < 100 && WiFi.status() != WL_CONNECTED; i++)
  {
    Serial.print('.');
    delay(100);
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.printf("Connect Error");
    return false;
  }
  Serial.printf("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/test", handle_Test);
  server.on("/submit", handle_Submit);
  server.begin();

  return true;
}

void handle_Test() {
  String path = server.arg("dir");

  server.send(200, "text/html", "handle_Test:" + path);
}
void handle_Submit() {
  String path = server.arg("dir");

  server.send(200, "text/html", "handle_Submit:" + path);
}

void initClock()
{
  // NTP start
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  
  time_t t;
  while ((t = time(NULL)) <= 100000)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.println("Connected, NTP.");
}


void touched()
{
  static uint32_t touchs = 0;
  static uint32_t before = 0;

  touchs++;
  Serial.printf("T0 touched:%d\n", touchs);

  if (millis() - before > 500)
  {
    if (state >= TEMPHUMI_ANI)
    {
      state = CLOCK_INIT;
    }
    else
    {
      state = (state_t)((int)state + 1);
    }
    Serial.println(state);
    before = millis();
  }
}

bool isJustTime(char *buf)
{
  if (strstr(buf, ":00:00"))
  {
    return true;
  }
  return false;
}
