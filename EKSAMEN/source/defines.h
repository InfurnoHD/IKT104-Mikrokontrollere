#pragma once

const char weatherHost[] = "api.openweathermap.org";

const char weatherRequest[] =
    "GET "
    "/data/2.5/"
    "weather?lat=58.33&lon=8.57&appid=8a086b7fc3cd8a1f084e5c5d3a1f4cf6&units="
    "metric\r\n"
    "Host: api.openweathermap.org\r\n"
    "Connection: close\r\n"
    "\r\n";

const char unixHost[] = "showcase.api.linx.twenty57.net";

const char unixRequest[] = "GET /UnixTime/tounix?date=now HTTP/1.1\r\n"
                           "Host: showcase.api.linx.twenty57.net\r\n"
                           "Connection: close\r\n"
                           "\r\n";

const char newsHost[] = "feeds.bbci.co.uk";

const char newsRequest[] = "GET /news/world/europe/rss.xml HTTP/1.1\r\n"
                           "Host: feeds.bbci.co.uk\r\n"
                           "Connection: close\r\n"
                           "\r\n";
