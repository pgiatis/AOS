#include "Wifi_MQTT.h"
#include "globals.h"

/*
 Here are some notes:
 My tests hardware has the mac address of 24:0A:C4:05:68:0C

 When you type a serial command it fills a String variable called command_message with a echo of what was selected from 
 the serial menu it then publishes a message to the mqtt broker with what was selected.  From a mqtt client if you send 
 the follwoing command : 24:0A:C4:05:68:0C/feeds/get_info the embedded system will respond with a json line with data 
 created from the function SendInfoMQTT() .  If you send it 24:0A:C4:05:68:0C/feeds/commands you will see on the serial 
 montor that it says got something in commands.  This is a templete to build from.
*/

// Function to get the MAC address of the device
// Returns the MAC address as a String
String getMacAddress() 
{
  uint8_t mac[6];
  WiFi.macAddress(mac);
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macStr);
}

// Function to set up WiFi connection
// Connects to the WiFi network using the provided SSID and password
void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());

  Serial.println();
  Serial.print("WiFi connected to IP address:");
  Serial.println(WiFi.localIP());
}

// Callback function for when a message is received
// Processes the received message based on the topic
void messageReceived(String &topic, String &payload) 
{
  String macAddress = getMacAddress();
  //Serial.println("incoming: " + topic + " - " + payload); // For debugging

  if (topic == macAddress + "/feeds/get_info") // mqtt info
  { 
    SendInfoMQTT(); // Print back Json information data
  }

  if (topic == macAddress + "/feeds/commands") // mqtt info
  { 
    Serial.println("Got something in commands"); 
    reset_serial();
  }
}

// Function to connect to the MQTT server
// Attempts to connect to the MQTT server and subscribes to necessary topics
void mqttConnect() 
{
  String macAddress = getMacAddress();
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connecting to Cloud Server... ");                           // Print connected to Cloud (MQTT) to serial port

  uint8_t retries = 20;  
  
  while (!client.connect(macAddress.c_str(), "try", "try")) 
  {
    Serial.println("Cloud Connection Failed: " + client.state());                            // Serial print to display that it is trying to re-connect to the MQTT server
    Serial.println("Retrying Cloud Server connection in 5 seconds...");
     delay(5000);
    retries--;
    if (retries == 0) 
    {
      while (1); // basically die and wait for WDT to reset me
    }
  }

  Serial.println("Connected to Cloud!");                       
  Serial.println("Subscribing to feeds!");
  Serial.println((macAddress + "/feeds/get_info").c_str());
  Serial.println((macAddress + "/feeds/commands").c_str());

  client.subscribe((macAddress + "/feeds/get_info").c_str());
  client.subscribe((macAddress + "/feeds/commands").c_str());

  Serial.println("\nMQTT Connected!");
}

// Function to handle MQTT service
// Ensures the MQTT client stays connected and publishes data when changes occur
void mqttService() 
{
  client.loop();

  if (!client.connected()) 
  {
    mqttConnect();
  }

  if (change == true)// Now we can publish stuff! Need to update to only publish when data changes
  {
    String macAddress = getMacAddress();    
    Serial.print("Publishing MQTT Data:");
    Serial.print((macAddress + "/feeds/command:").c_str());
    Serial.println(command_message.c_str());
    reset_serial();
    if (! client.publish((macAddress + "/feeds/command:").c_str(), command_message.c_str())) {} //Serial.println(F("Failed"));}else{Serial.println(F("OK!"));}
    change = false;
    command_message = "";
  }
}

// Callback function to send information via MQTT
// Triggers the SendInfoMQTT function
void SendInfoCallback() 
{
  Serial.println("Callback...");
  SendInfoMQTT();
}

// Function to send information via MQTT
// Publishes a JSON object with device information to the MQTT server
void SendInfoMQTT() 
{ 
    StaticJsonDocument<1024> jsonBuffer; // Updated to use StaticJsonDocument with a size of 1024 bytes
    String macAddress = getMacAddress();
     
    JsonObject root = jsonBuffer.to<JsonObject>();
    root["client_id"] = macAddress;
    root["First:"] = (String)"Petros";
    root["Last:"] = (String)"Giatis";
    //root["power_toggle"] = (String)power_toggle; 
    //root["lux"] = (String)LUX;
    //root["ext_temp"] = (String)int_temp;
    //root["p_led"] = (String)power_toggle;
    //root["h_led"] = (String)heat;
    //root["control_mode_value"] = (String)control_mode;
    //root["current_temp"] = (String)temperature_read_average;
    //root["set_temp"] = (String)set_temperature;
    //root["current_system_time"] = (String)currentDateAndTimeString;
    //root["firmware_version"] = (String)firmware_version;
    //root["circuit_mode_value"] = (String)circuit_mode;
    //root["timer_enable_value"] = (String)timer_enable;
    //root["circuit_1_last_state"] = (String)circuit_1_last_state;
    //root["circuit_2_last_state"] = (String)circuit_2_last_state;
    //root["circuit_3_last_state"] = (String)circuit_3_last_state;
    String output;
    serializeJson(root, output);
    Serial.println(output);
    reset_serial();
    if (! client.publish((macAddress + "/feeds/info").c_str(), output.c_str())) 
    {
      Serial.println(client.state()); // TODO handle error scenarios
    } 
    else 
    {
      //Serial.println(F("OK!")); // Wast printing
    }
}

// Callback funtion for when a mqtt message is received
void callback(char* topic, byte* payload, unsigned int length) 
{
    //Serial.println("got something"); // For debugging
    String topicStr = String(topic);
    String payloadStr = String((char*)payload).substring(0, length);
    messageReceived(topicStr, payloadStr);
}