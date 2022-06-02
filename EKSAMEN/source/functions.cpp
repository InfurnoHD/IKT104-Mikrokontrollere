#include "functions.h"
#include "mbed.h"
#include "menu.h"
#include "tinyxml2.h"
#include <iostream>
#include <vector>

// Declaring sensors
DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);

void getTempHum(data *datainstance) {

  // Initializing sensors
  sensor.init(NULL);
  sensor.enable();

  // Endless loop to get temperature and humidity from sensors
  while (true) {
    datainstance->mutex.lock();
    sensor.get_humidity(&datainstance->hum);
    sensor.get_temperature(&datainstance->temp);
    datainstance->mutex.unlock();
    ThisThread::sleep_for(5s);
  }
}

void parseXML(const char *xml, data *datainstance) {
  // Declaring variables
  tinyxml2::XMLDocument doc;
  std::vector<std::string> vecList;

  // Parsing XML char array and getting errorID
  tinyxml2::XMLError errorID = doc.Parse(xml);

  if (errorID == tinyxml2::XML_SUCCESS) {

    // Navigating XML
    tinyxml2::XMLNode *pRSS = doc.RootElement();
    tinyxml2::XMLNode *pChannel = pRSS->FirstChildElement("channel");
    tinyxml2::XMLElement *pListElement = pChannel->FirstChildElement("item");

    // Getting all title elements from XML and pushing them into vector
    while (pListElement != nullptr) {
      std::string title;
      title = pListElement->FirstChildElement("title")->GetText();
      vecList.push_back(title);
      pListElement = pListElement->NextSiblingElement("item");
    }

    std::cout << "Parsed XML data!" << std::endl;

    // Getting first three news titles
    datainstance->news1 = vecList.at(0);
    datainstance->news2 = vecList.at(1);
    datainstance->news3 = vecList.at(2);

  }

  else {
    std::cout << "Unable to parse XML data: " << errorID << std::endl;
  }
}