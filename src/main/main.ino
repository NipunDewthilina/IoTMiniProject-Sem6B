#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

WiFiClient espClient;
PubSubClient client(espClient);

//Variables
int i = 0;
int j = 0;
int statusCode;
String st;
String content;
const char* mqtt_server = "test.mosquitto.org";
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (400)
#define MQTT_MAX_PACKET_SIZE 2048
char payload1[MQTT_MAX_PACKET_SIZE];
char payload2[MQTT_MAX_PACKET_SIZE];
char msg[MSG_BUFFER_SIZE];
int value = 0;
const char* ssid;
const char* password;
const char* APssid = "my-node-mcu";

StaticJsonDocument<2048> safetyJson;
StaticJsonDocument<2048> trendJson;

//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect(void);

//Establishing Local server at port 80 whenever required
ESP8266WebServer server(80);
 
void setup()
{
 
  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting previously connected WiFi");
  WiFi.disconnect();
  EEPROM.begin(512); //Initialasing EEPROM
  delay(10);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
 
  //---------------------------------------- Read EEPROM for SSID and pass
  Serial.println("Reading EEPROM ssid");
 
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.println();
  Serial.print("SSID: ");
  Serial.println(esid);
  ssid = esid.c_str();;
  Serial.println("Reading EEPROM pass");
 
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  password = epass.c_str();;
  Serial.println(epass);
 
 
  WiFi.begin(esid.c_str(), epass.c_str());
  if (testWifi())
  {
//    WiFi.softAP(APssid, "");
//    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println("Succesfully Connected!!!");
    Serial.print("Got IP: ");  Serial.println(WiFi.localIP());
    delay(1000);
    //Web interface servers
  
  server.on("/home", [](){
    String homePage = getPage("Colombo");
    server.send(200, "text/html", homePage);
  });
  
  server.on("/danger", handleDistrict);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  delay(1000);
     client.setServer(mqtt_server, 1883);
     client.setCallback(callback);
    return;
  }
  else
  {
    Serial.println("Turning the HotSpot On");
    launchWeb();
    setupAP();// Setup HotSpot
  }
 
  Serial.println();
  Serial.println("Waiting.");
  
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(100);
    server.handleClient();
  }
}
void loop() {
  server.handleClient();
  if ((WiFi.status() == WL_CONNECTED))
  {
 delay(1000);
    if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
//    snprintf (msg, MSG_BUFFER_SIZE, "hello world Group 8 #%ld", value);
//    Serial.print("Sending : ");
//    Serial.println(msg);
//    client.publish("outTopic_G8", msg);
  }
 
  }
  else
  {
  }
 
}
 
 
//-------- Fuctions used for WiFi credentials saving and connecting to it which you do not need to change 
bool testWifi(void)
{
  int c = 0;
  Serial.println("Waiting for Wifi to connect");
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
}
 
void launchWeb()
{
  Serial.println("");
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  // Start the server
  server.begin();
  Serial.println("Server started");
}
 
void setupAP(void)
{
  WiFi.mode(WIFI_AP);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);
 
    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP(APssid, "");
  Serial.println("softap");
  launchWeb();
  Serial.println("over");
}
 
void createWebServer()
{
 {
    server.on("/", []() {
 
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });
    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
 
      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });
 
    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0 ) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) {
          EEPROM.write(i, 0);
        }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
 
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
        {
          EEPROM.write(i, qsid[i]);
          Serial.print("Wrote: ");
          Serial.println(qsid[i]);
        }
        Serial.println("writing eeprom pass:");
        for (int i = 0; i < qpass.length(); ++i)
        {
          EEPROM.write(32 + i, qpass[i]);
          Serial.print("Wrote: ");
          Serial.println(qpass[i]);
        }
        EEPROM.commit();
 
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.reset();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
 
    });
    
//  //Web interface servers
//  
//  server.on("/home", [](){
//    String homePage = getPage("Colombo");
//    server.send(200, "text/html", homePage);
//  });
//  
//  server.on("/danger", handleDistrict);
//  server.onNotFound(handle_NotFound);
//  
//  server.begin();
//  Serial.println("HTTP server started");
    
  } 
}

//to handle ditrict data requests
void handleDistrict() { 
String message = "";
  if (server.arg("d")== ""){message = "Invalid! Parameter not found";}
  else{ message = getPage(server.arg("d"));}
  server.send(200, "text/html", message); 
}

//provides necessary page for the request
String getPage(String district){
  String pageTop ="<html>  <head>    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" />    <style>      .con1 {        background-color: #95ecd6;        max-width: 400px;        max-height: 500px;        min-width: 300px;        min-height: 500px;        margin: 100 auto;        padding: 10px;        border-radius: 15px;      }      .con2 {        background-color: #ffffff;        width: 290px;        height: 370px;        margin: 20 auto;        border-radius: 10px;        padding: 5px;      }      body {        background-color: #192466;      }      .textCen {        text-align: center;        color: #2e2e2e;        margin: 0;        padding: 0%;        font-family: Garamond, serif;      }      .dropbtn {        background-color: #04aa6d;        color: white;        padding: 10px;        font-size: 16px;        border: none;        cursor: pointer;        margin: 10 70;        border-radius: 5px;      }      .dropbtn:hover,      .dropbtn:focus {        background-color: #3e8e41;      }      #myInput {        box-sizing: border-box;        background-image: url(\"searchicon.png\");        background-position: 14px 12px;        background-repeat: no-repeat;        font-size: 16px;        padding: 14px 20px 12px 25px;        border: none;        border-bottom: 1px solid #ddd;        margin: 10 10;      }      #myInput:focus {        outline: 3px solid #ddd;      }      .dropdown {        position: relative;        display: inline-block;      }      .dropdown-content {        display: none;        position: absolute;        background-color: #f6f6f6;        min-width: 130px;        overflow: auto;        border: 1px solid #ddd;        z-index: 1;      }      .dropdown-content a {        color: black;        padding: 12px 16px;        text-decoration: none;        display: block;      }      .dropdown a:hover {        background-color: #ddd;      }      .show {        display: block;      }    </style>  </head>  <body>    <script>      function myFunction() {        document.getElementById(\"myDropdown\").classList.toggle(\"show\");      }      function filterFunction() {        var input, filter, ul, li, a, i;        input = document.getElementById(\"myInput\");        filter = input.value.toUpperCase();        div = document.getElementById(\"myDropdown\");        a = div.getElementsByTagName(\"a\");        for (i = 0; i < a.length; i++) {          txtValue = a[i].textContent || a[i].innerText;          if (txtValue.toUpperCase().indexOf(filter) > -1) {            a[i].style.display = \"\";          } else {            a[i].style.display = \"none\";          }        }      }    </script>    <div class=\"con1\">      <h2 class=\"textCen\">WELCOME</h2>      <h4 class=\"textCen\">to</h4>      <h2 class=\"textCen\">COVID Caution Platform</h2>      <div class=\"con2\">        <br />        <h4 class=\"textCen\">Where do you want to go?</h4>        <div class=\"dropdown\">          <div id=\"myDropdown\" class=\"dropdown-content\">            <input              type=\"text\"              placeholder=\"Search district\"              id=\"myInput\"              onkeyup=\"filterFunction()\"            />            <a href=\"/danger?d=Ampara\">Ampara</a>            <a href=\"/danger?d=Anuradhapura\">Anuradhapura</a>            <a href=\"/danger?d=Badulla\">Badulla</a>            <a href=\"/danger?d=Baticola\">Baticola</a>            <a href=\"/danger?d=Colombo\">Colombo</a>            <a href=\"/danger?d=Galle\">Galle</a>            <a href=\"/danger?d=Gampaha\">Gampaha</a>            <a href=\"/danger?d=Hambanthota\">Hambanthota</a>            <a href=\"/danger?d=Jaffna\">Jaffna</a>            <a href=\"/danger?d=Kalulthara\">Kalulthara</a>            <a href=\"/danger?d=Kandy\">Kandy</a>            <a href=\"/danger?d=Kegalle\">Kegalle</a>            <a href=\"/danger?d=Kilinochchi\">Kilinochchi</a>            <a href=\"/danger?d=Mannar\">Mannar</a>            <a href=\"/danger?d=Matale\">Matale</a>            <a href=\"/danger?d=Matara\">Matara</a>            <a href=\"/danger?d=Monaragala\">Monaragala</a>            <a href=\"/danger?d=Mulathivu\">Mulathivu</a>            <a href=\"/danger?d=Nuwara-Eliya\">Nuwara-Eliya</a>            <a href=\"/danger?d=Polonnaruwa\">Polonnaruwa</a>            <a href=\"/danger?d=Puttlam\">Puttlam</a>            <a href=\"/danger?d=Ratnapura\">Ratnapura</a>            <a href=\"/danger?d=Trinocmalee\">Trinocmalee</a>            <a href=\"/danger?d=Vavuniya\">Vavuniya</a>          </div>          <button onclick=\"myFunction()\" class=\"dropbtn\">            Search a district          </button> <br />          <hr style=\"width: 220\" />          <div class=\"container\">            <div class=\"child\">              <h3 class=\"textCen\">District</h3>";
  String pageBottom="</h2></div></div><hr style=\"width: 220\" /> </div> </div> </div>  </body></html>";
  String pageCenter = "<h1 class=\"textCen\">";
  pageCenter+= district;
  pageCenter+="</h1><hr style=\"width: 120\" /><h3 style=\"color: #e74124\" class=\"textCen\">Danger level</h3><h2 class=\"textCen\">";
  pageCenter+= getDangerLevel(district);
  pageCenter+="</h2> <hr style=\"width: 120\" /> <h3 style=\"color: #e74124\" class=\"textCen\">Danger trend</h3> <h2 class=\"textCen\">";
  pageCenter+= getDangerTrend(district);
  
  String dangerPage = pageTop+pageCenter+pageBottom;
  Serial.println(district);
  return dangerPage;
}

//obtain danger level from data stored
String getDangerLevel(String district){
  String defaultResponse = "N/A";
  if(safetyJson.isNull()){
    Serial.println("safety data not avaible");
  }
  else{
    bool isTrue =  safetyJson[district];
    defaultResponse= isTrue ? "Safe" : "Unsafe";
  }
  Serial.println("safety is : "+defaultResponse);
  return defaultResponse;
}

//obtain danger trend from data stored
String getDangerTrend(String district){
  String defaultResponse = "N/A";
  if(trendJson.isNull()){
    Serial.println("trend data not available");
  }
  else{
    if (trendJson[district] == 2){
      defaultResponse = "Not Enough Data";
    }
    else{
    bool isTrue =  trendJson[district];
     defaultResponse= isTrue ? "Increasing" : "Decreasing";}
  }
  
  Serial.println("trend is : "+defaultResponse);
  return defaultResponse;
}

//handle json objct and store in glbal variable
void trendToJson(byte* payload){
  
  String input = String((char*)payload);
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    trendJson = doc;
  }
}

void safetyToJson(byte* payload){
  
  String input = String((char*)payload);
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else{
    safetyJson = doc;
  }
}

//interrupt required when receiving data from the mqtt server
void callback(char* topic, byte* payload, unsigned int length) {
  char* topic1 = "G8/safety_check/in";
  char* topic2 = "G8/node_mcu/sleep";
  char* topic3 = "G8/trend_check/in";
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if(strcmp(topic1,topic) == 0  ){
      for (int i = 0; i < length; i++) {
    payload1[i] = (char)payload[i];
    Serial.print(payload1[i]);
  }
  
   //TODO handle if necessary
    if ((char*)payload1[0] == "true") 
    {Serial.println("value is true");} 
    else{Serial.println("value is false");}

    //cnverting to json and storing
//    storeJson(payload);
    
    //cnverting to json and storing//todo change to safety
    safetyToJson(payload);
    Serial.println("safety:");
    serializeJsonPretty(safetyJson, Serial);
  }
  if (strcmp(topic3,topic) == 0 ){
    for (int i = 0; i < length; i++) {
    payload2[i] = (char)payload[i];
    Serial.print(payload2[i]);
  }
    //cnverting to json and storing
//    storeJson(payload);
    trendToJson(payload);
    Serial.println("trend:");
    serializeJsonPretty(trendJson, Serial);
  }
  if(strcmp(topic2,topic) == 0 )
  {
    ESP.deepSleep(atoi((char *)payload)*(3.6e+9));//sleep for 6 hours
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
      j = 0;
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic_G8", "hello world, Nipun");
      // ... and resubscribe
      client.subscribe("G8/safety_check/in");
      client.subscribe("G8/trend_check/in");
      client.subscribe("G8/node_mcu/sleep");
      Serial.println("Subscribed to topics");
    } else {
      j++;
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      if (j == 5){
          ESP.reset();
          j = 0;
      }
    }
  }
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}
