/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

// Update these with values suitable for your network.

const char* ssid = "HUAWEI-73ED";
const char* password = "04592945";
const char* mqtt_server = "192.168.8.103";
int port =8883;
const char* ca_cert = \ 
"-----BEGIN CERTIFICATE-----\n" \
"MIIDpzCCAo+gAwIBAgIJALtWSGCSZIPMMA0GCSqGSIb3DQEBDQUAMGoxFzAVBgNV\n" \
"BAMMDkFuIE1RVFQgYnJva2VyMRYwFAYDVQQKDA1Pd25UcmFja3Mub3JnMRQwEgYD\n" \
"VQQLDAtnZW5lcmF0ZS1DQTEhMB8GCSqGSIb3DQEJARYSbm9ib2R5QGV4YW1wbGUu\n" \
"bmV0MB4XDTE4MTIwOTA3MjgyNFoXDTMyMTIwNTA3MjgyNFowajEXMBUGA1UEAwwO\n" \
"QW4gTVFUVCBicm9rZXIxFjAUBgNVBAoMDU93blRyYWNrcy5vcmcxFDASBgNVBAsM\n" \
"C2dlbmVyYXRlLUNBMSEwHwYJKoZIhvcNAQkBFhJub2JvZHlAZXhhbXBsZS5uZXQw\n" \
"ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDVvENqtS3P9fTTZBu6adUk\n" \
"3Xin4AftqyS3Xx45qaUcywXCgsCdFdzCZIp7xVC+5Q7N4xi0hgzncnrW0RMVG5O6\n" \
"7LEFEUN6iALMSdt81InlV4EfDjIAfulLyCVpWzXpd41kbwv/mtfp5IGxg+omzXh9\n" \
"ufy6th3coefSnZxw5YMhJMvWCJvUW2pAOfWaQq2BU1yR6QPG+ZMgDtowfvLxylRF\n" \
"/SPOvNgDKU+k6CWUkMZlDXnewVOAD5GjfBULPfzpJw6s2eHRQ/jjTSG9qEbCwHnh\n" \
"mA6TA5m5V1eQIxwYqGNYMiMh3T5/WS4a0juNVnbw61IoZ0yhUEwvRELwAGw6VNl9\n" \
"AgMBAAGjUDBOMB0GA1UdDgQWBBQYuqG6sec9S32FcjS4fFh/hL8xKDAfBgNVHSME\n" \
"GDAWgBQYuqG6sec9S32FcjS4fFh/hL8xKDAMBgNVHRMEBTADAQH/MA0GCSqGSIb3\n" \
"DQEBDQUAA4IBAQDF6qqViEjR81RnMiJ26xsJJ0f4qrO74rMO9STiJc0o4I27MCVd\n" \
"ZzqUZA0aSlF4CZgsyyuhNpfHtkVYUwxGSzKvLEMuMcEMW0xO8kLl5nJclQkQChqd\n" \
"/gnjSDZZRhruNTho+Y1VpVeNbElXlCWqLJ8H95V1GdTdpjLKzuTTkoMNpO6Jwnow\n" \
"pI2y494Z0BnvrS2HxC5mioRLhoHVCl8M5V3BdUzn9GDUc3jx0u0e/syz5a+8hCP2\n" \
"c2NCc+dpuARmY++q5J84+p2PYhPcQEKbtqDIi0aCQ0LuVXnUkEMNCWZ2mOVTsxDm\n" \
"0QwX/xO9DY3Bb211XBshuvao/onAaMIHXpWw\n" \
"-----END CERTIFICATE-----\n" ;

WiFiClientSecure espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  //espClient.setCACert(ca_cert);
  client.setServer(mqtt_server, port);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
