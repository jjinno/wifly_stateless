#include "SPI.h"
#include <WiFly.h>
#include <WString.h>

#define DO_POC    0
#define DO_BLOCKY 1

#define HTTP_UNKNOWN 0
#define HTTP_GET     1
#define HTTP_POST    2
#define HTTP_PUT     3
#define HTTP_DELETE  4

/****************************************************************/

char ssid[] = "XVAdHoc";
WiFlyServer server(80);

String line = String();
String term = String();
String url = String();
int urlStart=0, urlEnd=0;

/*============================================================================*/

void
sendOK(WiFlyClient &client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
}

/*============================================================================*/

void
setup() {
  Serial.begin(19200);

  Serial.println("Staring WiFly");
  WiFly.begin(true);
  if(!WiFly.createAdHocNetwork(ssid)) {
    while(1) {
      Serial.println("Failed to create network, handle error properly?");
    }
  }

  Serial.println("Network Created");
  Serial.print("IP: ");
  Serial.println(WiFly.ip());
  
  Serial.println("Starting Server");
  server.begin();
  
  Serial.println("Server Started");
}

void
loop() {
  int verb = HTTP_UNKNOWN;
  
  WiFlyClient client = server.available();
  if (client) {

    // an http request ends with a blank line
    boolean eol = true;
  
    while (client.connected()) {
      verb = HTTP_UNKNOWN;
      if (client.available()) {
        char c = client.read();
        switch (c) {
          case '\n':
            if (eol) Serial.println("END OF HEADER");
            eol = true;
            
            // Only care about GET right now...
            term = "GET";
            if (line.indexOf(term) >= 0) {
              Serial.println(line);
            }
            
            line = "";
            break;
          case '\r':
            Serial.print("<CR>");
            // eol = true; // ???
            break;
          default:
            if (eol) eol=false;
            line += c;
            break;
        }
      } // if (client.available())
    } // while (client.connected())
  } // if (client)
}
