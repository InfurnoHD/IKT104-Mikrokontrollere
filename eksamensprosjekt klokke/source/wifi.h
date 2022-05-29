#pragma once
#include "mbed.h"

void setSocket(TLSSocket &socket, nsapi_size_or_error_t &result,
               NetworkInterface *network, SocketAddress &address,
               const char certificate[], const char host[]);

void setSocketHTTP(TCPSocket &socket, nsapi_size_or_error_t &result,
                   NetworkInterface *network, SocketAddress &address,
                   const char host[]);

void getRequest(TLSSocket &socket, const char getreq[],
                nsapi_size_or_error_t &result);

void getRequestHTTP(TCPSocket &socket, const char getreq[],
                    nsapi_size_or_error_t &result);

void parseJsonData(char chunks[]);