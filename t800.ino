
#include <Wire.h>
#define LED_PIN 3 // Пин пары светодиодов
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out, XX = 1000, YY = 1000, ZZ = 1000;  // Outputs
uint32_t tmr = 0, tmr1 = 0;
bool iswaswake = true;
int sens = 8;//Чувствительность
long rndtime;
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  wake_t800(10);
}
void loop() {
 if (millis() - tmr1 >= 150000) {//Каждые 150 сек. сброс iswaswake в ложь.
    tmr1=millis();
    iswaswake=false;
  } 
  if (!iswaswake) {
    Wire.beginTransmission(ADXL345);
    Wire.write(0x32);
    Wire.endTransmission(false);
    Wire.requestFrom(ADXL345, 6, true);
    X_out = ( Wire.read()| Wire.read() << 8);
    X_out = fabs(X_out);
    Y_out = ( Wire.read()| Wire.read() << 8);
    Y_out = fabs(Y_out);
    Z_out = ( Wire.read()| Wire.read() << 8);
    Z_out = fabs(Z_out);
  }
  if (millis() - tmr >= 10000 || XX == 1000) {//Каждые 10 сек. сохраняем показания по осям.
    tmr=millis();
    XX=X_out;
    YY=Y_out;
    ZZ=Z_out;
  } 
  if ( fabs(XX-X_out) > sens || fabs(YY-Y_out) > sens || fabs(ZZ-Z_out) > sens) {//Небрежно определяем удар и пробуждаемся, если долго спали
    XX = X_out;
    YY = Y_out;
    ZZ = Z_out;
    if (iswaswake == false) {
      //rndtime=random(30,60);
      wake_t800(20);
    }
  }
}
void wake_t800(int waketime) {
  iswaswake=true;//Устанавливаем признак пробуждения
  for(int i=0;i<=255;i++) {
    analogWrite(LED_PIN, i);
    if(i>=10 && 1==random(11)) {
      i=i-10;
    }
    delay(1);
  } 
  delay(waketime*1000);
  for(int i=255;i>=0;i--) {
    analogWrite(LED_PIN, i);
    delay(5);
  } 
  for(int i=0;i<=255;i++) {
    analogWrite(LED_PIN, i);
  }
  for(int i=255;i>=0;i--) {
    analogWrite(LED_PIN, i);
  }
}
