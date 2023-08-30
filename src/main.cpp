#include <GpsTracker.hpp>

volatile boolean fencingButtonHitFlag = false;
volatile boolean alarmButtonHitFlag = false;

// interrupts
void FencingButtonHit(){
  fencingButtonHitFlag = true;
}
void AlarmButtonHit(){
  alarmButtonHitFlag = true;
}

void setup(){
  delay(2000);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(fencingButtonPin, INPUT);
  pinMode(alarmButtonPin, INPUT);
  pinMode(motionDetectorPin, INPUT);
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  attachInterrupt(fencingButtonPin, FencingButtonHit, FALLING);
  attachInterrupt(alarmButtonPin, AlarmButtonHit, FALLING);
 
  Position localization;
  Fence fencing;
  PostingHandler postingHandler;
  Device device(1);
  // Connection connection("Ekoenergetyka", "%ekoenergetyka$", "10.103.29.107");
  Connection connection("Testgps", "12345678", "192.168.25.68");
  connection.RefreshWiFiConnection();
  postingHandler.Calibration(device, connection, fencing);
  while(true){
    connection.RefreshWiFiConnection();
    localization.GetPosition();

    Functionality::ReactToFencingButtonHit(fencingButtonHitFlag, fencing, localization, postingHandler, device, connection);
    Functionality::ReactToAlarmButton(alarmButtonHitFlag, localization, postingHandler, device, connection);
    Functionality::ReactToMotionSensor(digitalRead(motionDetectorPin) == HIGH ? true : false, fencing, localization, postingHandler, device, connection);
    
    // Heartbeat
    if(Functionality::Wait(postingHandler.heartbeatPreviousTime, postingHandler.heartbeatInterval, now()))
      postingHandler.Heartbeat(device, connection, fencing);

    // Localization
    if(Functionality::Wait(postingHandler.postPreviousTime, postingHandler.postInterval, now())){
      postingHandler.Localization(device, connection, localization);
    }
    // Fencing
    if(fencing.isLocked && Functionality::Wait(postingHandler.fencingPreviousTime, postingHandler.fencingInterval, now())){
      fencing.fencingDistance = Functionality::GetDistanceOnASphere(localization.lat, localization.lon, fencing.fencingLat, fencing.fencingLon);
      Functionality::UpdateStatus(fencing.status, fencing.fencingRange, fencing.fencingDistance, localization.isConnectedToGps);
      postingHandler.Fencing(device, connection, fencing);
    }
  }
}
void loop(){
}