#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "defines.h"
#include "json.hpp"
#include "mbed.h"
#include "menu.h"
#include "tinyxml2.h"
#include "wifi.h"
#include <cstdio>
#include <cstring>
#include <iostream>

PwmOut buzzer(D6);

DFRobot_RGBLCD lcd(16, 2, D14, D15);

DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);

void getTempHum(data *datainstance) {

  sensor.init(NULL);
  sensor.enable();

  while (true) {
    datainstance->mutex.lock();
    sensor.get_humidity(&datainstance->hum);
    sensor.get_temperature(&datainstance->temp);
    datainstance->mutex.unlock();
    ThisThread::sleep_for(5s);
  }
}

int main() {

  data datainstance;

  datainstance.buttonState = 1;
  datainstance.lcd = &lcd;
  datainstance.temp = 0;
  datainstance.hum = 0;
  datainstance.tempMet = 0;
  datainstance.weather = nullptr;
  datainstance.news1 = nullptr;
  datainstance.news2 = nullptr;
  datainstance.news3 = nullptr;

  lcd.init();
  lcd.clear();
  lcd.printf("00:00");

  NetworkInterface *network = NetworkInterface::get_default_instance();

  if (!network) {
    printf("Failed to get default network interface!\n");
    while (true)
      ;
  }

  nsapi_size_or_error_t result;

  do {
    printf("Connecting to network...\n");
    result = network->connect();
    if (result != 0) {
      printf("Failed to connect to network!\n");
    }
  } while (result != 0);

  printf("Connected to network!\n");

  SocketAddress address;

  result = network->get_ip_address(&address);

  if (result != 0) {
    printf("Failed to get local IP address!\n");
    while (true)
      ;
  }

  std::string unixString = getRequest(network, unixRequest, address, unixHost);

  int positionUnix = unixString.find_first_of("\"");
  std::string epochTime;
  epochTime.assign(unixString, positionUnix + 1, 10);

  lcd.clear();
  lcd.printf("%s", epochTime.c_str());
  ThisThread::sleep_for(5s);

  std::string weatherString =
      getRequest(network, weatherRequest, address, weatherHost);

  int weatherSLenght = weatherString.length();
  char weatherChar[weatherSLenght + 1];
  strcpy(weatherChar, weatherString.c_str());
  char *ptr = strchr(weatherChar, '{');

  nlohmann::json j_object = nlohmann::json::parse(ptr, nullptr, false);

  datainstance.weather = j_object["weather"][0]["description"];
  datainstance.tempMet = j_object["main"]["temp"];

  std::string newsString = getRequest(network, newsRequest, address, newsHost);

  static const char *xml = strchr(newsString.c_str(), '<');

  tinyxml2::XMLDocument doc;

  doc.Parse(xml);

  tinyxml2::XMLNode *pRSS = doc.RootElement();
  tinyxml2::XMLNode *pChannel = pRSS->FirstChildElement("channel");
  tinyxml2::XMLElement *pListElement = pChannel->FirstChildElement("item");
  std::vector<std::string> vecList;

  while (pListElement != nullptr) {

    std::string title;

    title = pListElement->FirstChildElement("title")->GetText();

    vecList.push_back(title);

    pListElement = pListElement->NextSiblingElement("item");
  }

  for (int i = 0; i < vecList.size(); i++) {
    std::cout << vecList.at(i) << std::endl << std::endl;
  }

  Thread menufunc;
  Thread menuswitch;
  Thread getTempHumThread;

  menufunc.start(callback(menuFunc, &datainstance));
  menuswitch.start(callback(menuSwitch, &datainstance));
  getTempHumThread.start(callback(getTempHum, &datainstance));

  while (true) {
    ThisThread::sleep_for(1s);

    // insert counter :D
  }
}