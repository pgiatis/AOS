#include "globals.h"


//WiFiClient client;                                                            // Create an ESP8266 WiFiClient class to connect to the MQTT server. or... use WiFiClientSecure for SSL
//MQTTClient mqttClient(512);

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

const char* ssid = "petropower";
const char* password = "";
const char* mqtt_server = "pgiatis.dyndns.org"; // This is the mqtt ip address.

unsigned long last_print_time = millis();

void (*softReset) (void) = 0; // Declare Software Reset Function @ address 0

int inByte = -1; // incoming byte from serial RX

String inputString = ""; // a string to hold incoming data
String prompt = "AOS>"; // String to hold prompt
String command_message = "Starting System";

boolean stringComplete = false; // whether the string is complete

boolean change = true;

byte flag = 0; // Declare Error handling flag
byte done = 0; // Declare done flag
long baud = 0; // Initialize Baud Rate Variable
byte valid = 0; // check to see if something is valid
