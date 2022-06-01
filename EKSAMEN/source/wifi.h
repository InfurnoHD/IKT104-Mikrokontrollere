#pragma once

#include "ISM43362.h"
#include <string>

std::string getRequest(NetworkInterface *network, const char *request,
                       SocketAddress address, const char *host);
