#include "TCPSocket.h"
#include "json.hpp"
#include "mbed.h"

void parseJsonData(char *chunks) {

  using json = nlohmann::json;

//fjerner metadata fra responsen
  char *ptr = strchr(chunks, '{');
  size_t index = (chunks - ptr) + 1;

//parser
  json j_object = json::parse(ptr, nullptr, false);

//printer data som oppgaven etterspurte
  if (j_object.is_discarded()) {
    printf("\nThe input is invalid JSON\n");
    return;
  } else {
    printf("\nThe input is valid JSON\n\n");
  }
  if (j_object["first name"].is_string()) {
    std::string dir1 = j_object["first name"];
    printf("First name: %s\n", dir1.c_str());
  }
  if (j_object["last name"].is_string()) {
    std::string dir1 = j_object["last name"];
    printf("Last name: %s\n", dir1.c_str());
  }
  if (j_object["age"].is_number()) {
    int dir1 = j_object["age"];
    printf("Age: %i\n", dir1);
  }
}

nsapi_size_or_error_t sendRequest(Socket *socket, const char *request) {

  nsapi_size_t bytes_to_send = strlen(request);
  nsapi_size_or_error_t bytes_sent = 0;

  printf("Sending message: \n%s", request);

  while (bytes_to_send) {

    bytes_sent = socket->send(request + bytes_sent, bytes_to_send);

    if (bytes_sent < 0) {
      return bytes_sent;
    } else {
      printf("Sent %d bytes\n", bytes_sent);
    }

    bytes_to_send -= bytes_sent;
  }

  printf("Complete message sent\n");

  return 0;
}

nsapi_size_or_error_t readResponse(Socket &socket, char *buffer,
                                   int buffer_length, char chunks[2000]) {

  int remaining_bytes = buffer_length;
  int received_bytes = 0, counter = 0;

//lager chunk med størrelse på 100 byte
  char chunk[100] = {0};
  nsapi_size_or_error_t result = 1;

//while loop som henter data om det er mer igjen
  while (result > 0) {
    result = socket.recv(chunk, 100);

    for (int i = 0; i < 100; i++, counter++) {
      if (chunk[i] == '\0') {
        printf("\n\n");
        return received_bytes;
      }

      //printer chunks på 100 byte og ... mellom hver print
      printf("%c", chunk[i]);
      chunks[counter] = chunk[i];
      chunk[i] = 0;
    }
    printf("...");
  }

  return received_bytes;
}

int main() {

//henter config frå json filaog kobler til nettet
  NetworkInterface *network = NetworkInterface::get_default_instance();

  if (!network) {
    printf("Failed to get network configuration!\n");
    while (1)
      ;
  }

  nsapi_size_or_error_t result;

  do {
    printf("Connecting to the network...\n");
    result = network->connect();

    if (result != 0) {
      printf("Failed to connect to the network!\n");
    }
  } while (result != 0);

  printf("Connected to the network!\n");

  //setter socket
  SocketAddress address;
  network->get_ip_address(&address);
  TCPSocket *socket = new TCPSocket;

  if (socket == nullptr) {
    printf("Failed to allocate socket instance\n");
  }
  socket->open(network);

  const char *host = "www.mocky.io";
  result = network->gethostbyname(host, &address);

  if (result != 0) {
    printf("Failed to get IP address of host %s: %d\n", host, result);
    socket->close();
  }

//kobler til mocky.io
  address.set_port(80);
  result = socket->connect(address);

  if (result != 0) {
    printf("Failed to connect to server at %s: %d\n", host, result);
    socket->close();
  } else {
    printf("Connected to server!\n");
  }

//lager og sender GET requesten
  const char request[] = "GET /v2/5e37e64b3100004c00d37d03 HTTP/1.1\r\n"
                         "Host: www.mocky.io\r\n"
                         "Connection: close\r\n"
                         "\r\n";

  result = sendRequest(socket, request);

  if (result != 0) {
    printf("Failed to send request: %d\n", result);
    socket->close();
  }

  //definerer varibler og leser responsen
  static constexpr size_t HTTP_RESPONSE_BUF_SIZE = 100;
  static char response[HTTP_RESPONSE_BUF_SIZE];
  static char chunks[2000];

  result = readResponse(*socket, response, HTTP_RESPONSE_BUF_SIZE, chunks);

//printer heile responsen
  printf("%s", chunks);

  if (result != 0) {
    printf("Failed to read response: %d\n", result);
    socket->close();
  }

//parser data og printer
  parseJsonData(chunks);
}
