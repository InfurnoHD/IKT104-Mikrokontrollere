#include "wifi.h"
#include "mbed.h"
#include "string"
#include "iostream"

void setSocket(TLSSocket &socket, nsapi_size_or_error_t &result,
               NetworkInterface *network, SocketAddress &address,
               const char certificate[], const char host[]) {

  while (true) {

    result = socket.open(network);

    if (result != 0) {
      printf("Failed to open TLSSocket\n");
      break;
    } else {
      printf("Opened socket!\n");
    }

    result = network->gethostbyname(host, &address);

    if (result != 0) {
      printf("Failed to get IP address of host: %d\n", result);
      break;
    }

    printf("IP address of server %s is %s\n", host, address.get_ip_address());

    address.set_port(443);

    result = socket.set_root_ca_cert(certificate);

    if (result != 0) {
      printf("Failed to set root certificate of the web site: %d\n", result);
      break;
    }

    result = socket.connect(address);

    if (result != 0) {
      printf("Failed to connect to server: %d\n", result);
      socket.close();
      break;
    } else {
      printf("Connected to server!\n");
    }
    break;
  }
}

void setSocketHTTP(TCPSocket &socket, nsapi_size_or_error_t &result,
                   NetworkInterface *network, SocketAddress &address,
                   const char host[]) {

  while (true) {

    result = socket.open(network);

    if (result != 0) {
      printf("Failed to open TLSSocket\n");
      break;
    } else {
      printf("Opened socket!\n");
    }

    result = network->gethostbyname(host, &address);

    if (result != 0) {
      printf("Failed to get IP address of host: %d\n", result);
      break;
    }

    printf("IP address of server %s is %s\n", host, address.get_ip_address());

    address.set_port(80);

    result = socket.connect(address);

    if (result != 0) {
      printf("Failed to connect to server: %d\n", result);
      socket.close();
      break;
    } else {
      printf("Connected to server!\n");
    }
    break;
  }
}

void getRequest(TLSSocket &socket, const char getreq[],
                nsapi_size_or_error_t &result) {

  static char buffer[3500];

  result = socket.send(getreq, strlen(getreq));

  if (result < 0) {
    printf("Failed to send request: %d\n", result);
    return;
  }

  memset(buffer, 0, sizeof(buffer));
  socket.recv(buffer, sizeof(buffer));
  printf("%s", buffer);

  parseJsonData(buffer);
}

void getRequestHTTP(TCPSocket &socket, const char getreq[],
                    nsapi_size_or_error_t &result) {


  static char buffer[15000];

  memset(buffer, 0, sizeof(buffer));

  int bytes_to_send = strlen(getreq);
  int bytes_sent = 0;

  int remaining_bytes = sizeof(buffer);
  int recieved_bytes = 0;

  printf("Sending get request: \n%s", getreq);

  while (bytes_to_send){
      bytes_sent = socket.send(getreq + bytes_sent, bytes_to_send);

      printf("Sent %d bytes\n", bytes_sent);

      bytes_to_send -= bytes_sent;
  }


  if (result < 0) {
    printf("Failed to send request: %d\n", result);
    return;
  }

  while (remaining_bytes > 0) {
      result = socket.recv(buffer, sizeof(buffer));

      if (result == 0){
          break;
      }
      recieved_bytes += result;
      remaining_bytes -= result;
  }

  printf("\nRecieved %d bytes\n", recieved_bytes);

  std::string str = buffer;
  unsigned startfirst = str.find("<title><![CDATA[");
  unsigned endfirst = str.find("]]></title>");
  std::string title = str.substr(startfirst + 16, endfirst - (startfirst + 16));
  std::cout << title << std::endl;

  unsigned start2 = str.find_last_of("<title><![CDATA[");
  unsigned end2 = str.find_last_of("]]></title>");
  std::string title1 = str.substr(start2 + 16, end2 - (start2 + 16));
  std::cout << title1 << std::endl;

  //printf("%s", buffer);
}