#include "mbed.h"

void initialConnectWIFI(TCPSocket *socket, nsapi_size_or_error_t result,
                        NetworkInterface *network) {

  while (true) {

    if (!network) {
      printf("Failed to get network configuration!\n");
      break;
    }

    // Kobler til nettverket og printer hvordan det gikk
    printf("Connecting to the network...\n");
    result = network->connect();

    if (result != 0) {
      printf("Failed to connect to the network!\n");
    } else {
      printf("Connected to the network!\n");
    }

    // Skaffer IP adresse
    SocketAddress address;
    network->get_ip_address(&address);
    printf("Connected to IP: %s\n", address.get_ip_address());

    // Åpner socket
    socket->open(network);

    // Setter IP adressen til serveren
    result = network->gethostbyname("192.168.11.66", &address);

    if (result != 0) {
      printf("Failed to set IP address of HTTP server: %d\n", result);
      break;
    }

    // Setter porten
    address.set_port(9090);

    // Kobler til server og printer hvordan det gikk
    result = socket->connect(address);

    if (result != 0) {
      printf("Failed to connect to server: %d\n", result);
      socket->close();
      break;
    } else {
      printf("Connected to server!\n");
    }
    break;
  }
  ThisThread::sleep_for(3s);
}

// Samme funksjon som over men gjelder da bare tilkoblingen til serveren, ikke
// nettverket
void reconnectWIFI(TCPSocket *socket, nsapi_size_or_error_t result,
                   NetworkInterface *network) {
  while (true) {

    SocketAddress address;
    network->get_ip_address(&address);
    socket->open(network);
    network->gethostbyname("192.168.11.66", &address);
    address.set_port(9090);
    socket->connect(address);

    if (result != 0) {
      printf("Failed to reconnect to server: %d\n", result);
      socket->close();
      break;
    } else {
      printf("Closed socket and reconnected to server!\n\n");
    }
    break;
  }
  ThisThread::sleep_for(3s);
}