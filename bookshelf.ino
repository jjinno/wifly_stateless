#include "SPI.h"
#include <WiFly.h>

#define HTTP_UNKNOWN 0
#define HTTP_GET     1
#define HTTP_POST    2
#define HTTP_PUT     3
#define HTTP_DELETE  4

/****************************************************************/

char ssid[] = "XVAdHoc";
WiFlyServer server(80);

String line = String("");
String url = String("");

/*============================================================================*/

void
http_sendOK(WiFlyClient &client)
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
}

void
http_parseUrl()
{
  if (line.indexOf("DELETE") >= 0)
    url = line.substring(line.indexOf('/'), line.lastIndexOf(' '));
  else if (line.indexOf("GET") >= 0)
    url = line.substring(line.indexOf('/'), line.lastIndexOf(' '));
  else if (line.indexOf("PUT") >= 0)
    url = line.substring(line.indexOf('/'), line.lastIndexOf(' '));
  else if (line.indexOf("POST") >= 0)
    url = line.substring(line.indexOf('/'), line.lastIndexOf(' '));
}

void
http_parseHeader(WiFlyClient &client)
{
  boolean eol=true, doBreak=false;
  char c;
  
  while (client.connected()) {
    if (doBreak == true) break;
    if (client.available()) {
      c = client.read();
      switch (c) {
        case '\n':
          if (eol) {
            // END OF HEADER
            http_sendOK(client);
            delay(250);
            
            client.stop();
            client.flush();
            doBreak = true;
            break;
          }
          eol = true;

          /* HANDLE HTTP VERBS */
          if ((line.indexOf("GET") >= 0)||
              (line.indexOf("PUT") >= 0)||
              (line.indexOf("POST") >= 0)||
              (line.indexOf("DELETE") >= 0))
            http_parseUrl();
          
          line = "";
          break;
        case '\r':
          break;
        default:
          if (eol) eol=false;
          line.concat(c);
          break;
      } // switch (c)
    } // if (client.available())
  } // while (client.connected())
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
}



/*============================================================================*/

void
loop() {
  WiFlyClient client = server.available();
  if (client) {
    http_parseHeader(client);
  } // if (client)
  
  Serial.println(url);
  delay(1000);
}
