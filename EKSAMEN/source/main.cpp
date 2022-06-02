#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "clock.h"
#include "defines.h"
#include "functions.h"
#include "json.hpp"
#include "mbed.h"
#include "menu.h"
#include "wifi.h"
#include <cstdio>
#include <cstring>

// Declaring struct and LCD
data datainstance;
DFRobot_RGBLCD lcd(16, 2, D14, D15);

int main() {

  // Variable initialization
  datainstance.buttonState = 1;
  datainstance.lcd = &lcd;
  datainstance.temp = 0;
  datainstance.hum = 0;
  datainstance.tempMet = 0;
  datainstance.weather = nullptr;
  datainstance.news1 = nullptr;
  datainstance.news2 = nullptr;
  datainstance.news3 = nullptr;

  // Initializing LCD and clearing it
  lcd.init();
  lcd.clear();

  // Initializing clock (to get 00:00 on screen at boot)
  setInitialClock(&datainstance);
  ThisThread::sleep_for(2s);

  // Getting default network interface with SSID and PASSWORD from mbed_app.json
  NetworkInterface *network = NetworkInterface::get_default_instance();
  if (!network) {
    printf("Failed to get default network interface!\n");
    while (true)
      ;
  }

  // Error variable
  nsapi_size_or_error_t result;

  // Connecting to network
  do {
    printf("Connecting to network...\n");
    result = network->connect();
    if (result != 0) {
      printf("Failed to connect to network!\n");
    }
  } while (result != 0);
  printf("Connected to network!\n");

  // Setting address and giving it an IP address
  SocketAddress address;
  result = network->get_ip_address(&address);
  if (result != 0) {
    printf("Failed to get local IP address!\n");
    while (true)
      ;
  }

  // Sending HTTP GET request to get unix epoch time
  // Adding 7200 (2 hours) to the epoch time value to give it the correct
  // timezone and account for daylight saving time
  std::string unixString = getRequest(network, unixRequest, address, unixHost);
  int positionUnix = unixString.find_first_of("\"");
  std::string epochTimeString;
  epochTimeString.assign(unixString, positionUnix + 1, 10);
  int epochTime = stoi(epochTimeString);
  epochTime += 7200;
  datainstance.epochTime = (time_t)epochTime;

  // Printing UNIX epoch time to screen and leaving it there for 5 seconds
  lcd.clear();
  lcd.printf("Unix epoch time:");
  lcd.setCursor(0, 1);
  lcd.printf("%i", datainstance.epochTime);
  ThisThread::sleep_for(5s);

  // Starting clock thread
  Thread clockThread;
  clockThread.start(callback(setClockAndAlarm, &datainstance));

  // Sending HTTP GET request to get weather
  std::string weatherString =
      getRequest(network, weatherRequest, address, weatherHost);
  int weatherSLenght = weatherString.length();
  char weatherChar[weatherSLenght + 1];
  strcpy(weatherChar, weatherString.c_str());
  char *ptr = strchr(weatherChar, '{');

  // Parsing the weather HTTP response
  nlohmann::json j_object = nlohmann::json::parse(ptr, nullptr, false);
  datainstance.weather = j_object["weather"][0]["description"];
  datainstance.tempMet = j_object["main"]["temp"];

  // Sending HTTP GET request to get news
  std::string newsString = getRequest(network, newsRequest, address, newsHost);
  static const char *xml = strchr(newsString.c_str(), '<');

  // Calling function to parse XML data to extract news titles
  parseXML(xml, &datainstance);

  // Declaring threads and starting them
  Thread menufunc;
  Thread getTempHumThread;
  menufunc.start(callback(menuFunc, &datainstance));
  datainstance.menuswitch.start(callback(menuSwitch, &datainstance));
  getTempHumThread.start(callback(getTempHum, &datainstance));
  menufunc.set_priority(osPriorityAboveNormal);
  datainstance.menuswitch.set_priority(osPriorityAboveNormal);

  // Endless loop to keep main() running
  while (true) {
    ThisThread::sleep_for(1s);
  }
}