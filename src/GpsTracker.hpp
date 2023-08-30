#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <TinyGPSPlus.h>

// mathematical constants
#define RADIAN 0.017453292519943295769236907684886
#define EARTH_DIAMETER_IN_METERS 12742000
// pinout
#define RXD2 16
#define TXD2 17
#define ledPin1 12
#define ledPin2 32
#define fencingButtonPin 13
#define alarmButtonPin 34
#define motionDetectorPin 35
// enumerators
enum Status{NO_ALARM, FENCING_ALARM, NO_GPS_CONNECTION};

struct Connection{
  const char* ssid;
  const char* pass;
  String ip;
  String port;

  Connection(const char*, const char*, String);
  void RefreshWiFiConnection();
};
struct Device{
  int uid;
  bool alarm;
  bool motionDetectorRising;
  
  Device(int);
};
struct Fence{
  bool isLocked; 
  int fencingRange;
  double fencingDistance;
  double fencingLon;
  double fencingLat;
  Status status;

  Fence();
};
struct Position{
  TinyGPSPlus gps;
  double lat;
  double lon;
  bool isConnectedToGps;

  Position();
  void GetPosition();
};
struct PostingHandler{
  HTTPClient httpClient;
  int fencingInterval;
  int heartbeatInterval;
  int postInterval;
  unsigned long heartbeatPreviousTime;
  unsigned long postPreviousTime;
  unsigned long fencingPreviousTime;

  PostingHandler();
  void Calibration(Device, Connection, Fence&);
  bool Heartbeat(Device, Connection, Fence&);
  bool Localization(Device, Connection, Position);
  bool Fencing(Device, Connection, Fence);
};
struct Functionality{
  static double GetDistanceOnASphere(double, double, double, double);
  static bool Wait(unsigned long&, int, int);
  static void UpdateStatus(Status&, int, int, bool);
  static void ReactToFencingButtonHit(volatile boolean&, Fence&, Position, PostingHandler&, Device, Connection);
  static void ReactToMotionSensor(bool, Fence&, Position, PostingHandler&, Device&, Connection);
  static void ReactToAlarmButton(volatile boolean&, Position, PostingHandler&, Device&, Connection);
};

#pragma region Connection
Connection::Connection(const char* tssid, const char* tpass, String tip){
  ssid = tssid;
  pass = tpass;
  ip = tip;
  port = "8080";
}
void Connection::RefreshWiFiConnection(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Connecting To WiFi...");
    WiFi.begin(ssid, pass);
    while(WiFi.status() != WL_CONNECTED)
      delay(1);
  }
}
#pragma endregion
#pragma region Device
Device::Device(int tuid){
  uid = tuid;
  alarm = false;
  motionDetectorRising = true;
}
#pragma endregion
#pragma region Fence
Fence::Fence(){
  isLocked = false;
  status = NO_ALARM;
}
#pragma endregion
#pragma region Position
Position::Position(){
  lat = 0;
  lon = 0;
  isConnectedToGps = false;
}
void Position::GetPosition(){
  while (Serial2.available()){
    gps.encode(Serial2.read());
    lat = gps.location.lat();
    lon = gps.location.lng();
    isConnectedToGps = gps.location.isValid();
  }
}
#pragma endregion
#pragma region PostingHandler
PostingHandler::PostingHandler(){
  fencingInterval = 10;
  heartbeatInterval = 2;
  postInterval = 100;
  heartbeatPreviousTime = 0;
  postPreviousTime = 0;
  fencingPreviousTime = 0;
}
void PostingHandler::Calibration(Device device, Connection connection, Fence& fencing){
  digitalWrite(ledPin2, HIGH);
  postInterval = 0;
  while (postInterval == 0){
    DynamicJsonDocument doc(1024);
    Serial.println("Reading From Database...");
    httpClient.begin("http://" + connection.ip + ":" + connection.port + "/arduino/getDeviceConfiguration/?uid=" + device.uid);
    int httpCode = httpClient.GET();
    deserializeJson(doc, httpClient.getString());
    heartbeatInterval = doc["heartbeat_rate"];
    postInterval = doc["interval"];
    String temp = doc["time"];
    setTime(temp.substring(0,10).toInt());
    fencing.fencingRange = doc["range"];
    delay(1000);
  }
  digitalWrite(ledPin2, LOW);
}
bool PostingHandler::Heartbeat(Device device, Connection connection, Fence& fencing){
  digitalWrite(ledPin2, HIGH);
  Calibration(device, connection, fencing);
  DynamicJsonDocument doc(1024);
  char jsonOutput[128];
  Serial.println("Posting To Heartbeat...");
  httpClient.begin("http://" + connection.ip + ":" + connection.port + "/arduino/heartbeat/?uid=" + String(device.uid));
  httpClient.addHeader("Content-Type", "application/json");
  serializeJson(doc, jsonOutput);
  int httpCode = httpClient.POST(String(jsonOutput));
  digitalWrite(ledPin2, LOW);
  return httpCode>0 ? true : false;
}
bool PostingHandler::Localization(Device device, Connection connection, Position localization){
  digitalWrite(ledPin2, HIGH);
  DynamicJsonDocument doc(1024); 
  char jsonOutput[128];
  Serial.println("Posting To Localization...");
  httpClient.begin("http://" + connection.ip + ":" + connection.port + "/arduino/localization/?uid=" + String(device.uid));
  httpClient.addHeader("Content-Type", "application/json");
  doc["status"] = localization.isConnectedToGps;
  doc["lon"] = localization.lon;
  doc["lat"] = localization.lat;
  doc["alarm"] = device.alarm;
  doc["arduino_time"] = (long)now();
  serializeJson(doc, jsonOutput);
  int httpCode = httpClient.POST(String(jsonOutput));
  digitalWrite(ledPin2, LOW);
  return httpCode>0 ? true : false;
}
bool PostingHandler::Fencing(Device device, Connection connection, Fence fencing){
  digitalWrite(ledPin2, HIGH);
  DynamicJsonDocument doc(1024);
  char jsonOutput[128];
  Serial.println("Posting To Fencing...");
  httpClient.begin("http://" + connection.ip + ":" + connection.port + "/arduino/fencing/?uid=" + String(device.uid));
  httpClient.addHeader("Content-Type", "application/json");
  doc["fencingStatus"] = fencing.isLocked;
  doc["alarmStatus"] = fencing.status;
  serializeJson(doc, jsonOutput);
  int httpCode = httpClient.POST(String(jsonOutput));
  digitalWrite(ledPin2, LOW);
  return httpCode>0 ? true : false;
}
#pragma endregion
#pragma region Functionality
double Functionality::GetDistanceOnASphere(double lat1, double lon1, double lat2, double lon2){
  lat1 *= RADIAN;
  lat2 *= RADIAN;
  lon1 *= RADIAN;
  lon2 *= RADIAN;
  // return transformed haversine value
  return EARTH_DIAMETER_IN_METERS * asin(sqrt(pow(sin((lat1 - lat2)/2), 2) + cos(lat1) * cos(lat2) * pow(sin((lon1 - lon2)/2), 2)));
}
bool Functionality::Wait(unsigned long& previousTime, int interval, int timeNow){
  if(timeNow - previousTime < interval)
    return false;
  previousTime = timeNow;
  return true;
}
void Functionality::UpdateStatus(Status& status, int fencingRange, int fencingDistance, bool isConnectedToGps){
  status = NO_ALARM;
  if(fencingDistance > fencingRange)
    status = FENCING_ALARM;
  if(!isConnectedToGps)
    status = NO_GPS_CONNECTION;
}
void Functionality::ReactToFencingButtonHit(volatile boolean& fencingButtonHitFlag, Fence& fencing, Position localization, PostingHandler& postingHandler, Device device, Connection connection){
  if(!fencingButtonHitFlag)
    return;
  fencing.isLocked = !fencing.isLocked;
  digitalWrite(ledPin1, fencing.isLocked);
  fencing.fencingLat = localization.lat;
  fencing.fencingLon = localization.lon;
  fencing.fencingDistance = Functionality::GetDistanceOnASphere(localization.lat, localization.lon, fencing.fencingLat, fencing.fencingLon);
  Functionality::UpdateStatus(fencing.status, fencing.fencingRange, fencing.fencingDistance, localization.isConnectedToGps);
  postingHandler.Fencing(device, connection, fencing);
  postingHandler.fencingPreviousTime = now();
  delay(100);
  fencingButtonHitFlag = false;
}
void Functionality::ReactToMotionSensor(bool isMovement, Fence& fencing, Position localization, PostingHandler& postingHandler, Device& device, Connection connection){
  if(fencing.isLocked && isMovement && device.motionDetectorRising){
    fencing.fencingDistance = Functionality::GetDistanceOnASphere(localization.lat, localization.lon, fencing.fencingLat, fencing.fencingLon);
    Functionality::UpdateStatus(fencing.status, fencing.fencingRange, fencing.fencingDistance, localization.isConnectedToGps);
    postingHandler.Fencing(device, connection, fencing);
    postingHandler.fencingPreviousTime = now();
    device.motionDetectorRising = false;
  }
  if(!isMovement)
    device.motionDetectorRising = true;
}
void Functionality::ReactToAlarmButton(volatile boolean& alarmButtonHitFlag, Position localization, PostingHandler& postingHandler, Device& device, Connection connection){
  if(alarmButtonHitFlag){
    device.alarm = true;
    postingHandler.Localization(device, connection, localization);
    device.alarm = false;
    delay(100);
    alarmButtonHitFlag = false;
  }
}
#pragma endregion
