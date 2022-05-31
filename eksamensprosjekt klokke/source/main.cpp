#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "HTS221_driver.h"
#include "Thread.h"
#include "defines.h"
#include "humidtemp.h"
#include "json.hpp"
#include "mbed.h"
#include "symbol_codes_weather.h"
#include "wifi.h"

DFRobot_RGBLCD lcd(16, 2, D14, D15);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);
DigitalIn button(D0, PullUp);

void parseJsonData(char chunks[]) {

  using json = nlohmann::json;

  char *ptr = strchr(chunks, '{');

  size_t index = (chunks - ptr) + 1;

  json j_object = json::parse(ptr, nullptr, false);

  if (j_object.is_discarded()) {
    printf("\nThe input is invalid JSON\n");
    return;
  } else {
    printf("\nThe input is valid JSON\n\n");
  }
  if (j_object["date_time_unix"].is_number_float()) {
    int dir1 = j_object["date_time_unix"];
    printf("unix time: %i\n", dir1);
  }

  if (j_object["type"].is_string()) {
    std::string dir2 = j_object["properties"]["timeseries"][0]["data"]
                               ["next_1_hours"]["summary"]["symbol_code"];
    for (int i = 0; i < symbolCodes.size(); i++) {
      if (dir2 == symbolCodes.at(i)) {
        dir2 = symbolCorrections.at(i);
      }
    }
    lcd.printf("%s", dir2.c_str());
    lcd.setCursor(0, 1);
    float dir3 = j_object["properties"]["timeseries"][0]["data"]["instant"]
                         ["details"]["air_temperature"];
    lcd.printf("%.1f degrees", dir3);
  }
  chunks = 0;
}

int main() {

  nsapi_size_or_error_t result;

  TLSSocket weatherSocket;

  TLSSocket unixSocket;

  TCPSocket newsSocket;

  NetworkInterface *network = NetworkInterface::get_default_instance();

  SocketAddress address;

  lcd.on();
  lcd.init();
  lcd.clear();

  int count = 0;
  float temp = 0;
  float hum = 0;

  while (true) {
    gethum(sensor, button, lcd, hum);
    gettemp(sensor, button, lcd, temp);
    printtemphum(count, temp, hum);
  }

  while (true) {

    if (!network) {
      printf("Failed to get network configuration!\n");
      break;
    }

    printf("Connecting to the network...\n");
    result = network->connect();

    if (result != 0) {
      printf("Failed to connect to the network!\n");
      return 0;
    } else {
      printf("Connected to the network!\n");
    }

    result = network->get_ip_address(&address);
    if (result != 0) {
      printf("Failed to get local IP address\n");
      break;
    } else {
      printf("Connected to IP: %s\n", address.get_ip_address());
    }
    break;
  }

  setSocket(weatherSocket, result, network, address, metcer, metrologaddress);

  getRequest(weatherSocket, weatherGET, result);

  result = weatherSocket.close();
  printf("\n\n%i\n\n", result);

  setSocket(unixSocket, result, network, address, unixcer, ipgeoaddress);

  getRequest(unixSocket, unixGET, result);

  unixSocket.close();

  setSocketHTTP(newsSocket, result, network, address, newsaddress);

  getRequestHTTP(newsSocket, newsGET, result);

  newsSocket.close();

  return 0;
}
