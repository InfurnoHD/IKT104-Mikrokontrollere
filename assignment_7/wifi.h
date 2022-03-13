#pragma once
#include "mbed.h"

// Deklarerer funksjoner
void initialConnectWIFI(TCPSocket *socket, nsapi_size_or_error_t result,
                        NetworkInterface *network);
void reconnectWIFI(TCPSocket *socket, nsapi_size_or_error_t result,
                   NetworkInterface *network);