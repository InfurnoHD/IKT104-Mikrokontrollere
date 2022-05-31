#include "wifi.h"
#include <cstdio>
#include <string>
#include <iostream>

std::string getRequest(NetworkInterface *network, const char *request,
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

  static constexpr size_t RESPONSE_BUFFER_SIZE = 5000;
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

    std::string str(response);

    return str;

}
