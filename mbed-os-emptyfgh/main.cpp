#include "mbed.h"
#include <DFRobot_RGBLCD.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <json.hpp>

using json = nlohmann::json;

DigitalInOut led(LED1);
DFRobot_RGBLCD lcd(16, 2, D14, D15);
NetworkInterface *network = NetworkInterface::get_default_instance();
TCPSocket socket;
SocketAddress address;

nsapi_size_or_error_t sendRequest(Socket &socket, const char *request) {
  nsapi_size_t bytesToSend = strlen(request);
  nsapi_size_or_error_t bytesSent = 0;

  printf("Sending message: \n%s", request);

  while (bytesToSend) {
    bytesSent = socket.send(request + bytesSent, bytesToSend);

    if (bytesSent < 0) {
      return bytesSent;
    } else {
      printf("Sent %d bytes\n", bytesSent);
    }

    bytesToSend -= bytesSent;
  }

  printf("Complete message sent\n");
  return 0;
}

nsapi_size_or_error_t readResponse(Socket &socket, char *buffer,
                                   int bufferLength) {
  int remainingBytes = bufferLength;
  int recievedBytes = 0;

  while (remainingBytes > 0) {
    nsapi_size_or_error_t result =
        socket.recv(buffer + recievedBytes, remainingBytes);

    if (result == 0) {
      break;
    }

    if (result < 0) {
      return result;
    }

    recievedBytes += result;
    remainingBytes -= result;
  }

  printf("\nRecieved %d bytes:\n%.*s\n", recievedBytes,
         strstr(buffer, "\n") - buffer, buffer);

  return recievedBytes;
}

void parse_json(char *input) {
  json json_data = json::parse(input, nullptr, false);

  if (json_data.is_discarded()) {
    printf("The input JSON data is not valid!\n");
    return;
  } else
    printf("The input JSON data is valid!\n");

  if (json_data["current"]["temp_c"].is_number_float()) {
    float temperature = json_data["current"]["temp_c"];
    lcd.printf("%.2f degrees", temperature);
    lcd.setCursor(0, 1);
  }

  if (json_data["current"]["condition"]["text"].is_string()) {
    std::string cloud = json_data["current"]["condition"]["text"];
    lcd.printf("%s", cloud.c_str());
  }
}

char *get_weather()

{
  static constexpr size_t HTTP_RESPONSE_BUF_SIZE = 2000;
  static char response[HTTP_RESPONSE_BUF_SIZE];
  static char jsonSource[HTTP_RESPONSE_BUF_SIZE] = {0};

  lcd.on();
  lcd.init();
  lcd.clear();

  if (!network) {
    printf("Failed to get the default network interface!\n");
    while (1)
      ;
  }

  nsapi_size_or_error_t result;

  while (result != 0) {
    printf("Connecting to the network...\n");
    result = network->connect();

    if (result != 0) {
      printf("Failed to connect to network: %d\n", result);
    }
  }

  network->get_ip_address(&address);
  printf("Connected to the network and got IP address %s\n",
         address.get_ip_address());

  while (true) {
    led = !led;
    ThisThread::sleep_for(5s);

    socket.open(network);

    const char *host = "api.weatherapi.com";
    result = network->gethostbyname(host, &address);

    if (result != 0) {
      printf("Failed to get IP address of host: %s %d\n", host, result);
      socket.close();
      continue;
    }
    printf("IP address of server %s is %s\n", host, address.get_ip_address());

    address.set_port(80);

    result = socket.connect(address);

    if (result != 0) {
      printf("Failed to connect to server at %s: %d\n", host, result);
      socket.close();
      continue;
    }

    printf("Sucessfully connected to server %s\n", host);

    const char request[] = "GET "
                           "/v1/"
                           "current.json?key=5699082827554b858a1184745222005&q="
                           "Grimstad&aqi=yes HTTP/1.1\r\n"
                           "Host: api.weatherapi.com\r\n"
                           "Connection: close\r\n"
                           "\r\n";

    result = sendRequest(socket, request);

    if (result != 0) {
      printf("Failed to send request: %d\n", result);
      socket.close();
      continue;
    }

    result = readResponse(socket, response, HTTP_RESPONSE_BUF_SIZE);

    if (result < 0) {
      printf("Failed to read response: %d\n", result);
      socket.close();
      continue;
    }

    response[result] = '\0';
    printf("\nThe HHTP repsonse:\n%s\n", response);

    socket.close();

    strcpy(jsonSource, response);

    char *jsonSourceStart = strchr(jsonSource, '{');

    if (jsonSourceStart == nullptr) {
      printf("Could not find JSON data!\n");
      continue;
    }
    // Ensures final character is '}'
    int jsonSourceSize = strlen(jsonSource);
    while (jsonSourceSize > 0) {
      if (jsonSource[jsonSourceSize] == '}')
        break;
      else
        jsonSource[jsonSourceSize] = '\0';

      jsonSourceSize -= 1;
    }
    if ((jsonSourceSize == 0) ||
        (jsonSource + jsonSourceSize < jsonSourceStart)) {
      printf("No ending \"}\" found, invalid JSON\n");
      continue;
    }
    return jsonSourceStart;
  }
}

int main() {

  parse_json(get_weather());

  return 0;

  std:string hello;
  std::string onlytite = hello.substr(startpostiion, endpostion) 
}