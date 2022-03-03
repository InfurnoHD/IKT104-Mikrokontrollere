/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

// Blinking rate in milliseconds
#define BLINKING_RATE 5000ms

nsapi_size_or_error_t send_request(Socket &socket, const char *request) {
  // The request might not be fully sent in one go,
  // so keep track of how much we have sent
  nsapi_size_t bytes_to_send = strlen(request);
  nsapi_size_or_error_t bytes_sent = 0;

  printf("Sending message: \n%s", request);

  // Loop as long as there are more data to send
  while (bytes_to_send) {
    // Try to send the remaining data, send()
    // returns how many bytes were actually sent
    bytes_sent = socket.send(request + bytes_sent, bytes_to_send);

    if (bytes_sent < 0) {
      // Negative return values from send() are errors
      return bytes_sent;
    } else {
      printf("Sent %d bytes\n", bytes_sent);
    }

    bytes_to_send -= bytes_sent;
  }

  printf("Complete message sent\n");
  return 0;
}

nsapi_size_or_error_t read_response(Socket &socket, char *buffer,
                                    int buffer_length) {
  int remaining_bytes = buffer_length;
  int received_bytes = 0;

  // Loop as long as there are more data to read,
  // we might not read all in one call to recv()
  while (remaining_bytes > 0) {
    nsapi_size_or_error_t result =
        socket.recv(buffer + received_bytes, remaining_bytes);

    // If the result is 0 there are no more bytes to read
    if (result == 0) {
      break;
    }

    // Negative return values from recv() are errors
    if (result < 0) {
      return result;
    }

    received_bytes += result;
    remaining_bytes -= result;
  }

  // Print out first line in response
  printf("\nReceived %d bytes:\n%.*s\n", received_bytes,
         strstr(buffer, "\n") - buffer, buffer);

  return received_bytes;
}

int main() {
  // Initialise the digital pin LED1 as an output
  DigitalOut led(LED1);

  // Get pointer to default network interface
  NetworkInterface *network = NetworkInterface::get_default_instance();

  if (!network) {
    printf("Failed to get default network interface\n");
    while (1);
  }

  nsapi_size_or_error_t result;

  do {
    printf("Connecting to the network...\n");
    result = network->connect();

    if (result != 0) {
      printf("Failed to connect to network: %d\n", result);
    }
  } while (result != 0);

  SocketAddress address;
  network->get_ip_address(&address);

  printf("Connected to WLAN and got IP address %s\n", address.get_ip_address());

  while (true) {
    led = !led;
    ThisThread::sleep_for(BLINKING_RATE);

    TCPSocket socket;
    socket.open(network);

    const char *host = "www.mocky.io";
    result = network->gethostbyname(host, &address);

    if (result != 0) {
      printf("Failed to get IP address of host %s: %d\n", host, result);
      socket.close();
      continue;
    }

    printf("IP address of server %s is %s\n", host, address.get_ip_address());

    // Set server TCP port number
    address.set_port(80);

    // Connect to server at the given address
    result = socket.connect(address);

    // Check result
    if (result != 0) {
      printf("Failed to connect to server at %s: %d\n", host, result);
      socket.close();
      continue;
    }

    printf("Successfully connected to server %s\n", host);

    // Create HTTP request
    const char request[] = "GET /v2/5e37e64b3100004c00d37d03 HTTP/1.1\r\n"
                           "Host: www.mocky.io\r\n"
                           "Connection: close\r\n"
                           "\r\n";

    // Send request
    result = send_request(socket, request);

    // Check result
    if (result != 0) {
      printf("Failed to send request: %d\n", result);
      socket.close();
      continue;
    }

    // Buffer for HTTP responses
    static constexpr size_t HTTP_RESPONSE_BUF_SIZE = 545;
    // Use static keyword to move response[] from stack to bss
    static char response[HTTP_RESPONSE_BUF_SIZE];

    // Read response
    result = read_response(socket, response, HTTP_RESPONSE_BUF_SIZE);

    // Check result
    if (result < 0) {
      printf("Failed to read response: %d\n", result);
      socket.close();
      continue;
    }

    socket.close();

    // Take a look at the response, but before doing
    // that make sure we have a null terminated string
    response[result] = '\0';
    printf("\nThe HTTP GET response:\n%s\n", response);
  }
}