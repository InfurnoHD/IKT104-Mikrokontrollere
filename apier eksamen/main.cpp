#include "DFRobot_RGBLCD.h"
#include "json.hpp"
#include "mbed.h"

DFRobot_RGBLCD lcd(16, 2, D14, D15);

void getRequest(NetworkInterface *network, const char *request,
                SocketAddress address, const char *host) {
  nsapi_size_or_error_t result;
  TCPSocket socket;
  socket.set_timeout(500);
  result = socket.open(network);

  if (result != 0) {
    printf("Failed to open socket!\n");
  }

  result = network->gethostbyname(host, &address);

  if (result != 0) {
    printf("Failed to get IP address of host!\n");
  }

  address.set_port(80);

  result = socket.connect(address);

  if (result != 0) {
    printf("Failed to connect to server!\n");
  }
  printf("Connected to server!\n");

  nsapi_size_or_error_t bytes_to_send = strlen(request);
  nsapi_size_or_error_t bytes_sent = 0;

  while (bytes_to_send) {
    bytes_sent = socket.send(request + bytes_sent, bytes_to_send);

    if (bytes_sent < 0) {
      printf("Error sending request!\n");
      break;
    } else {
      printf("Sent %d bytes!\n", bytes_sent);
    }
    bytes_to_send -= bytes_sent;
  }
  printf("Complete message sent!\n");

  static constexpr size_t RESPONSE_BUFFER_SIZE = 15000;
  static char response[RESPONSE_BUFFER_SIZE];

  memset(response, 0, RESPONSE_BUFFER_SIZE);

  int remaining_bytes = RESPONSE_BUFFER_SIZE;
  int recieved_bytes = 0;

  while (remaining_bytes > 0) {
    result = socket.recv(response + recieved_bytes, remaining_bytes);
    if (result == 0) {
      break;
    }
    recieved_bytes += result;
    remaining_bytes -= result;
  }

  printf("Recieved %d bytes!\n", recieved_bytes);

  if (result < 0) {
    printf("Failed to read response!\n");
  }

  printf("\nThe HTTP GET response:\n%s\n", response);

  result = socket.close();

  if (result != 0) {
    printf("Failed to close socket: %d!\n", result);
  } else {
    printf("Closed socket!\n");
  }
}

int main() {

  lcd.init();
  lcd.clear();

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

  const char weatherHost[] = "api.openweathermap.org";

  const char weatherRequest[] =
      "GET "
      "/data/2.5/"
      "weather?lat=58.33&lon=8.57&appid=8a086b7fc3cd8a1f084e5c5d3a1f4cf6&units="
      "metric\r\n"
      "Host: api.openweathermap.org\r\n"
      "Connection: close\r\n"
      "\r\n";

  getRequest(network, weatherRequest, address, weatherHost);

  const char unixHost[] = "showcase.api.linx.twenty57.net";

  const char unixRequest[] = "GET /UnixTime/tounix?date=now HTTP/1.1\r\n"
                             "Host: showcase.api.linx.twenty57.net\r\n"
                             "Connection: close\r\n"
                             "\r\n";

  getRequest(network, unixRequest, address, unixHost);

  const char newsHost[] = "fetchrss.com";

  const char newsRequest[] =
      "GET /rss/6295444f152e0c3ff13552c362954474c3254205780471f2.xml "
      "HTTP/1.1\r\n"
      "Host: fetchrss.com\r\n"
      "Connection: close\r\n"
      "\r\n";

  getRequest(network, newsRequest, address, newsHost);
}
