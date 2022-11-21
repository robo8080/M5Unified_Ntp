#include <Arduino.h>
#include <M5Unified.h>
#include <time.h>
#include <WiFi.h>

// for WiFi
char ssid[] = "---------------"; // your SSID
char pass[] = "---------------"; // your Pass word

// for Time
const char* ntpServer = "ntp.jst.mfeed.ad.jp"; // NTP server
const long  gmtOffset_sec = 9 * 3600;          // Time offset 9hr
const int   daylightOffset_sec = 0;            // No summer time
static m5::rtc_date_t RTC_DateStruct; // Data
static m5::rtc_time_t RTC_TimeStruct; // Time
static const char *wd[7] = {"Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
struct tm timeinfo;
String dateStr;
String timeStr;
void getTimeFromNTP(){
  // To get Time from NTP server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  while (!getLocalTime(&timeinfo)) {
    delay(1000);
  }
}

void setNTP2RTC(){
    // timeSet
    getTimeFromNTP();
    getLocalTime(&timeinfo);
    // read RTC
    M5.Rtc.getTime(&RTC_TimeStruct);
    M5.Rtc.getDate(&RTC_DateStruct);
    // --- to over write date&time
    RTC_DateStruct.year = timeinfo.tm_year + 1900;
    RTC_DateStruct.month = timeinfo.tm_mon + 1;
    RTC_DateStruct.date = timeinfo.tm_mday;
    RTC_DateStruct.weekDay = timeinfo.tm_wday;
    M5.Rtc.setDate(&RTC_DateStruct);
    RTC_TimeStruct.hours = timeinfo.tm_hour;
    RTC_TimeStruct.minutes = timeinfo.tm_min;
    RTC_TimeStruct.seconds = timeinfo.tm_sec;
    M5.Rtc.setTime(&RTC_TimeStruct);
}

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE,BLACK);
  //
  int con_count = 0;
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
    con_count++;
    if(con_count >= 20){
      break;
    }
  }
  if(con_count < 20){
    M5.Lcd.print("\nWiFi connected.");
    setNTP2RTC();
  }else{
    M5.Lcd.print("\nWiFi did not connect.");
  }
  M5.Lcd.print("\nIP=");
  M5.Lcd.print(WiFi.localIP());  
}

void loop() {
  M5.Lcd.setCursor(0, 80);
  M5.Rtc.getDate(&RTC_DateStruct);
  M5.Rtc.getTime(&RTC_TimeStruct);
  M5.Lcd.printf("%04d.%02d.%02d %s\n", RTC_DateStruct.year, RTC_DateStruct.month, RTC_DateStruct.date, wd[RTC_DateStruct.weekDay]);
  M5.Lcd.printf("%02d:%02d:%02d", RTC_TimeStruct.hours, RTC_TimeStruct.minutes, RTC_TimeStruct.seconds);
  delay(500);
}
