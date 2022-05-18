#include "DFRobot_RGBLCD.h"
#include "HTS221Sensor.h"
#include "HTS221_driver.h"
#include "Thread.h"
#include "defines.h"
#include "json.hpp"
#include "mbed.h"
#include "symbol_codes_weather.h"
#include <cstring>
#include <string>

#define WAIT_TIME_MS 500

DFRobot_RGBLCD lcd(16, 2, D14, D15);
DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);

void initialConnectWIFI(TLSSocket *socket, nsapi_size_or_error_t *result,
                        NetworkInterface *network, SocketAddress *address) {

  while (true) {

    if (!network) {
      printf("Failed to get network configuration!\n");
      break;
    }

    // Kobler til nettverket og printer hvordan det gikk
    printf("Connecting to the network...\n");
    *result = network->connect();

    if (*result != 0) {
      printf("Failed to connect to the network!\n");
      break;
    } else {
      printf("Connected to the network!\n");
    }

    // Skaffer IP adresse
    *result = network->get_ip_address(address);
    if (*result != 0) {
      printf("Failed to get local IP address\n");
      break;
    } else {
      printf("Connected to IP: %s\n", address->get_ip_address());
    }
    break;
  }
  ThisThread::sleep_for(3s);
}

void setSocket(TLSSocket *socket, nsapi_size_or_error_t *result,
               NetworkInterface *network, SocketAddress *address,
               const char certificate[], const char host[]) {

  while (true) {

    // Åpner socket
    *result = socket->open(network);

    if (*result != 0) {
      printf("Failed to open TLSSocket\n");
      break;
    } else {
      printf("Opened socket!\n");
    }

    *result = network->gethostbyname(host, address);

    if (*result != 0) {
      printf("Failed to get IP address of host: %d\n", *result);
      break;
    }

    printf("IP address of server %s is %s\n", host, address->get_ip_address());

    // Setter porten
    address->set_port(443);

    *result = socket->set_root_ca_cert(certificate);

    if (*result != 0) {
      printf("Failed to set root certificate of the web site: %d\n", *result);
      break;
    }

    // Kobler til server og printer hvordan det gikk
    *result = socket->connect(*address);

    if (*result != 0) {
      printf("Failed to connect to server: %d\n", *result);
      socket->close();
      break;
    } else {
      printf("Connected to server!\n");
    }
    break;
  }
}

void setSocketHTTP(TCPSocket *socket, nsapi_size_or_error_t *result,
                   NetworkInterface *network, SocketAddress *address,
                   const char host[]) {

  while (true) {

    // Åpner socket
    *result = socket->open(network);

    if (*result != 0) {
      printf("Failed to open TLSSocket\n");
      break;
    } else {
      printf("Opened socket!\n");
    }

    *result = network->gethostbyname(host, address);

    if (*result != 0) {
      printf("Failed to get IP address of host: %d\n", *result);
      break;
    }

    printf("IP address of server %s is %s\n", host, address->get_ip_address());

    // Setter porten
    address->set_port(80);


    // Kobler til server og printer hvordan det gikk
    *result = socket->connect(*address);

    if (*result != 0) {
      printf("Failed to connect to server: %d\n", *result);
      socket->close();
      break;
    } else {
      printf("Connected to server!\n");
    }
    break;
  }
}

void parseJsonData(char *chunks) {

  using json = nlohmann::json;

  // fjerner metadata fra responsen
  char *ptr = strchr(chunks, '{');

  size_t index = (chunks - ptr) + 1;

  // parser
  json j_object = json::parse(ptr, nullptr, false);

  // printer data som oppgaven etterspurte
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



void getRequest(TLSSocket *socket, NetworkInterface *network,
                const char getreq[]) {

  nsapi_size_or_error_t result;

  static char buffer[3500];

  result = socket->send(getreq, strlen(getreq));

  if (result < 0) {
    printf("Failed to send request: %d\n", result);
    return;
  }

  memset(buffer, 0, 3500);
  socket->recv(buffer, 3500);
  printf("%s", buffer);

  parseJsonData(buffer);
}

void getRequestHTTP(TCPSocket *socket, NetworkInterface *network,
                const char getreq[]) {

  nsapi_size_or_error_t result;

  static char buffer[10000];

  result = socket->send(getreq, strlen(getreq));

  if (result < 0) {
    printf("Failed to send request: %d\n", result);
    return;
  }

  memset(buffer, 0, 10000);
  socket->recv(buffer, 10000);
  printf("%s", buffer);

  parseJsonData(buffer);
}

int main() {

  nsapi_size_or_error_t *result = new nsapi_size_or_error_t;
  TLSSocket *weatherSocket = new TLSSocket;
  TLSSocket *unixSocket = new TLSSocket;
  TCPSocket *newsSocket = new TCPSocket;
  NetworkInterface *network = NetworkInterface::get_default_instance();
  SocketAddress *address = new SocketAddress;

  lcd.on();
  lcd.init();
  lcd.clear();

  initialConnectWIFI(weatherSocket, result, network, address);
  setSocket(weatherSocket, result, network, address, metcer, metrologaddress);
  getRequest(weatherSocket, network, weatherGET);

  delete weatherSocket;

  setSocket(unixSocket, result, network, address, unixcer, ipgeoaddress);
  getRequest(unixSocket, network, unixGET);

  delete unixSocket;

  setSocketHTTP(newsSocket, result, network, address, newsaddress);
  getRequestHTTP(newsSocket, network, newsGET);
}
