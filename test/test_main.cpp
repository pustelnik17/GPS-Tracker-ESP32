#include <unity.h>
#include <GpsTracker.hpp>
#define SSID "Ekoenergetyka"
#define PASS "%ekoenergetyka$"
// #define SSID "12345678"
// #define PASS "12345678"

// server
// #define SERVER_IP "192.168.43.68"
#define SERVER_IP "10.103.29.107"

#pragma region GetDistanceOnASphereTest
void GetDistanceOnASphereTest1() {
    double expected = 34.89;
    double actual = Functionality::GetDistanceOnASphere(51.925964, 15.617121, 51.925756, 15.616740);
    TEST_ASSERT_EQUAL(expected, actual);
}
void GetDistanceOnASphereTest2() {
    double expected = 6545311;
    double actual = Functionality::GetDistanceOnASphere(51.925964, 15.617121, 40.748600, -73.986400);
    TEST_ASSERT_EQUAL(expected, actual);
}
void GetDistanceOnASphereTest3() {
    double expected = 2154293;
    double actual = Functionality::GetDistanceOnASphere(51.925964, 15.617121, 51.748600, -15.986400);
    TEST_ASSERT_EQUAL(expected, actual);
}
void GetDistanceOnASphereTest4() {
    double expected = 5814502;
    double actual = Functionality::GetDistanceOnASphere(0.925964, 15.617121, 51.748600, 0.986400);
    TEST_ASSERT_EQUAL(expected, actual);
}
void GetDistanceOnASphereTest5() {
    double expected = 157225;
    double actual = Functionality::GetDistanceOnASphere(1, 3, 2, 4);
    TEST_ASSERT_EQUAL(expected, actual);
}
void GetDistanceOnASphereTest6() {
    double expected = 5800931;
    double actual = Functionality::GetDistanceOnASphere(0.5, 15.1, 51.1, 0.1);
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region WaitTest
void WaitTest1() {
    bool expected = true;
    unsigned long previousTime = 0;
    bool actual = Functionality::Wait(previousTime, 10, 10);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest2() {
    bool expected = true;
    unsigned long previousTime = 0;
    bool actual = Functionality::Wait(previousTime, 10, 11);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest3() {
    bool expected = false;
    unsigned long previousTime = 10;
    bool actual = Functionality::Wait(previousTime, 10, 11);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest4() {
    bool expected = true;
    unsigned long previousTime = 100;
    bool actual = Functionality::Wait(previousTime, 1, 101);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest5() {
    bool expected = true;
    unsigned long previousTime = -1;
    bool actual = Functionality::Wait(previousTime, 10, 10);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest6() {
    bool expected = false;
    unsigned long previousTime = -1;
    bool actual = Functionality::Wait(previousTime, -2, 10);
    TEST_ASSERT_EQUAL(expected, actual);
}
void WaitTest7() {
    bool expected = false;
    unsigned long previousTime = 60;
    bool actual = Functionality::Wait(previousTime, 10, 61);
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region ConnectionTest
void ConnectionTest1() {
    bool expected = true;
    Connection connection(SSID, PASS, SERVER_IP);
    connection.RefreshWiFiConnection();
    bool actual = WiFi.status();
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region UpdateStatusTest
void UpdateStatusTest1() {
    Status expected = NO_ALARM;
    Status actual = NO_ALARM;
    Functionality::UpdateStatus(actual, 100, 10, true);
    TEST_ASSERT_EQUAL(expected, actual);
}
void UpdateStatusTest2() {
    Status expected = FENCING_ALARM;
    Status actual = NO_ALARM;
    Functionality::UpdateStatus(actual, 100, 101, true);
    TEST_ASSERT_EQUAL(expected, actual);
}
void UpdateStatusTest3() {
    Status expected = NO_GPS_CONNECTION;
    Status actual = NO_ALARM;
    Functionality::UpdateStatus(actual, 100, 101, false);
    TEST_ASSERT_EQUAL(expected, actual);
}
void UpdateStatusTest4() {
    Status expected = NO_GPS_CONNECTION;
    Status actual = NO_ALARM;
    Functionality::UpdateStatus(actual, 100, 10, false);
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region ReactToFencingButtonHitTest
void ReactToFencingButtonHitTest1() {
    volatile boolean fencingButtonHitFlag = false;
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    Functionality::ReactToFencingButtonHit(fencingButtonHitFlag, fencing, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = fencing.isLocked;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToFencingButtonHitTest2() {
    volatile boolean fencingButtonHitFlag = true;
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    Functionality::ReactToFencingButtonHit(fencingButtonHitFlag, fencing, localization, postingHandler, device, connection);
    bool expected = true;
    bool actual = fencing.isLocked;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToFencingButtonHitTest3() {
    volatile boolean fencingButtonHitFlag = true;
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    fencing.isLocked = true;
    Functionality::ReactToFencingButtonHit(fencingButtonHitFlag, fencing, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = fencing.isLocked;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToFencingButtonHitTest4() {
    volatile boolean fencingButtonHitFlag = true;
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    Functionality::ReactToFencingButtonHit(fencingButtonHitFlag, fencing, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = fencingButtonHitFlag;
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region ReactToMotionSensorTest
void ReactToMotionSensorTest1(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);

    Functionality::ReactToMotionSensor(false, fencing, localization, postingHandler, device, connection);
    bool expected = true;
    bool actual = device.motionDetectorRising;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToMotionSensorTest2(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);

    fencing.isLocked = true;
    device.motionDetectorRising = true;
    Functionality::ReactToMotionSensor(true, fencing, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = device.motionDetectorRising;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToMotionSensorTest3(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);

    fencing.isLocked = false;
    Functionality::ReactToMotionSensor(true, fencing, localization, postingHandler, device, connection);
    bool expected = true;
    bool actual = device.motionDetectorRising;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToMotionSensorTest4(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);

    fencing.isLocked = true;
    device.motionDetectorRising = false;
    Functionality::ReactToMotionSensor(true, fencing, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = device.motionDetectorRising;
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region ReactToAlarmButtonTest
void ReactToAlarmButtonTest1(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    volatile boolean alarmButtonHitFlag = false;
    Functionality::ReactToAlarmButton(alarmButtonHitFlag, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = alarmButtonHitFlag;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToAlarmButtonTest2(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    volatile boolean alarmButtonHitFlag = true;
    Functionality::ReactToAlarmButton(alarmButtonHitFlag, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = alarmButtonHitFlag;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToAlarmButtonTest3(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    volatile boolean alarmButtonHitFlag = false;
    device.alarm = true;
    Functionality::ReactToAlarmButton(alarmButtonHitFlag, localization, postingHandler, device, connection);
    bool expected = true;
    bool actual = device.alarm;
    TEST_ASSERT_EQUAL(expected, actual);
}
void ReactToAlarmButtonTest4(){
    Fence fencing;
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    volatile boolean alarmButtonHitFlag = true;
    device.alarm = true;
    Functionality::ReactToAlarmButton(alarmButtonHitFlag, localization, postingHandler, device, connection);
    bool expected = false;
    bool actual = device.alarm;
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region HeartbeatTest
void HeartbeatTest1(){
    Fence fencing;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    
    bool expected = true;
    bool actual = postingHandler.Heartbeat(device, connection, fencing);
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region LocalizationTest
void LocalizationTest1(){
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    
    bool expected = true;
    bool actual = postingHandler.Localization(device, connection, localization);
    TEST_ASSERT_EQUAL(expected, actual);
}
void LocalizationTest2(){
    Position localization;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, "");
    
    bool expected = false;
    bool actual = postingHandler.Localization(device, connection, localization);
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion
#pragma region FencingTest
void FencingTest1(){
    Fence fencing;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, SERVER_IP);
    
    bool expected = true;
    bool actual = postingHandler.Fencing(device, connection, fencing);;
    TEST_ASSERT_EQUAL(expected, actual);
}
void FencingTest2(){
    Fence fencing;
    PostingHandler postingHandler;
    Device device(1);
    Connection connection(SSID, PASS, "");
    
    bool expected = false;
    bool actual = postingHandler.Fencing(device, connection, fencing);;
    TEST_ASSERT_EQUAL(expected, actual);
}
#pragma endregion

void setup(){
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(GetDistanceOnASphereTest1);
    RUN_TEST(GetDistanceOnASphereTest2);
    RUN_TEST(GetDistanceOnASphereTest3);
    RUN_TEST(GetDistanceOnASphereTest4);
    RUN_TEST(GetDistanceOnASphereTest5);
    RUN_TEST(GetDistanceOnASphereTest6);
    RUN_TEST(WaitTest1);
    RUN_TEST(WaitTest2);
    RUN_TEST(WaitTest3);
    RUN_TEST(WaitTest4);
    RUN_TEST(WaitTest5);
    RUN_TEST(WaitTest6);
    RUN_TEST(WaitTest7);
    RUN_TEST(ConnectionTest1);
    RUN_TEST(UpdateStatusTest1);
    RUN_TEST(UpdateStatusTest2);
    RUN_TEST(UpdateStatusTest3);
    RUN_TEST(UpdateStatusTest4);
    RUN_TEST(ReactToFencingButtonHitTest1);
    RUN_TEST(ReactToFencingButtonHitTest2);
    RUN_TEST(ReactToFencingButtonHitTest3);
    RUN_TEST(ReactToFencingButtonHitTest4);
    RUN_TEST(ReactToMotionSensorTest1);
    RUN_TEST(ReactToMotionSensorTest2);
    RUN_TEST(ReactToMotionSensorTest3);
    RUN_TEST(ReactToMotionSensorTest4);
    RUN_TEST(ReactToAlarmButtonTest1);
    RUN_TEST(ReactToAlarmButtonTest2);
    RUN_TEST(ReactToAlarmButtonTest3);
    RUN_TEST(ReactToAlarmButtonTest4);
    RUN_TEST(LocalizationTest1);
    RUN_TEST(LocalizationTest2);
    RUN_TEST(FencingTest1);
    RUN_TEST(FencingTest2);
    RUN_TEST(HeartbeatTest1);
    UNITY_END();
}
void loop(){
}