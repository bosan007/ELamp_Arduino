#include <ESP8266WiFi.h>;
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// these constants won't change. They are the lowest and highest readings you
// get from your sensor:
const int sensorMin = 0;      // sensor minimum, discovered through experiment
const int sensorMax = 600;    // sensor maximum, discovered through experiment

// Instantiate the ESP8266WebServer class, passing the argument 80 for the
// port that the server will be listening.
ESP8266WebServer server(80);

void handleRoot() {
  // Simply sends an 'OK' (200) response to the client, and a plain text
  // string with usage.
  server.send(200, "text/plain", String("Hello from esp8266! Usage: navigate to") +
              String(" /setleds?led1=XX&led2=YY changing XX/YY to ON or OFF."));
}

void handleSetLeds() {
  // We can read the desired status of the LEDs from the expected parameters that
  // should be passed in the URL.  We expect two parameters "led1" and "led2".
  String led1_status = server.arg("led1");
  
  // Check if the URL include a change of the LED status
  bool url_check = false;
  if((led1_status == "ON")||(led1_status == "OFF"))
    url_check = true;

  // It's not required to pass them both, so we check that they're exactly equal to
  // the strings ON or OFF by our design choice (this can be changed if you prefer
  // a different behavior)
  if (led1_status == "ON")
    digitalWrite(led1_pin, HIGH);
  else if (led1_status == "OFF")
    digitalWrite(led1_pin, LOW);
  if (url_check)
    // If we've set the LEDs to the requested status, we have the webserver
    // return an "OK" (200) response.  We also include the number of milliseconds
    // since the program started running.
    // Note: This number will overflow (go back to zero), after approximately 50 days.
    server.send(200, "text/plain", "LEDs' status changed! (" + String(millis()) + ")");
  else
    server.send(200, "text/plain", "LEDs' status unchanged! (" + String(millis()) + ")");
}

// If the client requests any other URL than the root directory or the /setled path:
void handleNotFound() {
  // We construct a message to be returned to the client
  String message = "File Not Found\n\n";
  // which includes what URI was requested
  message += "URI: ";
  message += server.uri();
  // what method was used
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  // and what parameters were passed
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  // the response, as expected, is a "Not Found" (404) error
  server.send(404, "text/plain", message);
}

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_AP_STA);
  delay(500);

  WiFi.beginSmartConfig();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    Serial.println(WiFi.smartConfigDone());
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Associate the URLs with the functions that will be handling the requests
  server.on("/", HTTP_GET, handleRoot);
  server.on("/setleds", HTTP_GET, handleSetLeds);
  server.onNotFound(handleNotFound);
}

void loop(void) {
  server.handleClient();
}
