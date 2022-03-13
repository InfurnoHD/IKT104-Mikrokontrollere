#include "HTS221Sensor.h"
#include "HTS221_driver.h"
#include "mbed.h"
#include "wifi.h"
#include <cstdio>
#include <cstring>

DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);

constexpr uint32_t HTTP_REQUEST_BUFFER_SIZE = 400;
constexpr uint32_t JSON_CONTENT_BUFFER_SIZE = 300;
constexpr uint32_t HTTP_RESPONSE_BUFFER_SIZE = 400;

// Funksjon for å sende HTTP POST
nsapi_size_or_error_t sendData(float temp, float hum, Socket *socket,
                               char *httpRequest, char *jsonContent) {

  // Variabler
  nsapi_size_t bytes_to_send = strlen(httpRequest);
  nsapi_size_or_error_t bytes_sent = 0;

  // Lager jsonContent
  std::snprintf(jsonContent, JSON_CONTENT_BUFFER_SIZE,
                "{\"temperature\": %.2f, \"humidity\": %.2f}", temp, hum);

  // Lager httpRequest
  std::snprintf(httpRequest, JSON_CONTENT_BUFFER_SIZE,
                "POST /api/v1/dJwwWNFhpTdNy8qjCZUz/telemetry HTTP/1.1\r\n"
                "Host: 192.168.11.66:9090\r\n"
                "Connection: keep-alive\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %u\r\n"
                "\r\n",
                strlen(jsonContent));

  // Kombinerer jsonContent og httpRequest
  strcat(httpRequest, jsonContent);

  printf("\nSending message: \n%s\n\n", httpRequest);

  // Loop for å sjekke om alt blir sendt
  while (bytes_to_send) {

    bytes_sent = socket->send(httpRequest + bytes_sent, bytes_to_send);

    if (bytes_sent < 0) {

      return bytes_sent;
    } else {
      printf("Sent %d bytes\n", bytes_sent);
    }

    bytes_to_send -= bytes_sent;
  }

  printf("Complete message sent\n\n");
  return bytes_to_send;
}

int main() {

  // Deklarerer variabler
  nsapi_size_or_error_t result;
  TCPSocket *socket = new TCPSocket;
  NetworkInterface *network = NetworkInterface::get_default_instance();

  static char httpRequest[HTTP_REQUEST_BUFFER_SIZE];
  static char jsonContent[JSON_CONTENT_BUFFER_SIZE];

  // Kobler til nettverk og server
  initialConnectWIFI(socket, result, network);

  float temp = 0;
  float hum = 0;

  // Initialliserer sensorer
  sensor.init(NULL);
  sensor.enable();

  while (true) {

    // Leser data fra sensorer
    sensor.get_humidity(&hum);

    sensor.get_temperature(&temp);

    // Kaller på funksjon for å sende HTTP POST
    result = sendData(temp, hum, socket, httpRequest, jsonContent);

    if (result != 0) {
      printf("Sending HTTP POST failed with error code %d\n", result);
      socket->close();
      break;
    }

    // Lukker socket og kobler til server på ny
    // Måtte gjøre dette for at ikke brannmuren kappa hode av koblingen etter å
    // ha sendt et par HTTP POSTer
    socket->close();
    reconnectWIFI(socket, result, network);

    ThisThread::sleep_for(3s);
  }
}
