#include "HTS221Sensor.h"
#include "HTS221_driver.h"
#include "mbed.h"

DevI2C i2c(PB_11, PB_10);
HTS221Sensor sensor(&i2c);


void temp() {
  const char request[] = "POST /api/v1/v7ItFiAuGIlPaYWXEGcA/telemetry HTTP/1.1\r\n"
                         "Host: 10.245.30.86:9090\r\n"
                         "Connection: close\r\n"
                         "Content-Type: application/json"
                         "Content-Length: 100"
                         "\r\n";
}

int main() {
    
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

  SocketAddress address;
  TCPSocket socket;

  socket.open(network);

address.set_ip_address("192.168.48.1");
address.set_port(9090);

result = socket.connect(address);

  if (result != 0) {
    printf("Failed to connect to server: %d\n", result);
    socket.close();
  } else {
    printf("Connected to server!\n");
  }

  float temp = 0;
  float hum = 0;

  sensor.init(NULL);
  sensor.enable();


  while (false) {
    sensor.get_humidity(&hum);
    printf("Humidity: %.2f\n", hum);

    sensor.get_temperature(&temp);
    printf("Temperature: %.2f\n\n", temp);

    ThisThread::sleep_for(1s);
  }
}
