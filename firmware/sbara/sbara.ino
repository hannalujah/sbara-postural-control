#include <math.h>
#include <Encoder.h>
#include "HX711.h"
#include <Wire.h>

#define DOUT  A1
#define CLK  A0
#define DOUT1  A3
#define CLK1  A2
#define DOUT2  A7
#define CLK2  A6
#define DOUT3  A5
#define CLK3  A4
#define SIGN1 5
#define SIGN2 6
#define MPU6050_config 26

HX711 BL;
HX711 BR;
HX711 TL;
HX711 TR;

float calibration_factor = 7050; //-7050 worked for my 440lb max scale setup


double Left; double Right; double Top; double Bottom;
double stiffness=1000, mass=75, gravity=9.81;
double TL1, TR1, BL1, BR1;
double Total;
volatile float COPx1, COPy1, COPx2, COPy2, dCOPx, dCOPy; double W; double L; double alpha;
unsigned long tt1;


const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t ax, ay, az, Tmp, gx, gy, gz;
int buffersize = 1;
int mean_ax, mean_ay;


Encoder myEnc2(7, 8); Encoder myEnc1(9, 10);
long X  = -999; long Y  = -999;
long X0  = -999; long Y0  = -999;
int n;
int Z = 0; int Z0 = 0;
double left; double right; double top; double bottom;
int j; int I;
double COPX; double COPY;
int i = 0;
int a = 0; int b = 0;
int f = 0; int F = 0; int f1 = 0; int F1 = 0; int f2 = 0; int F2 = 0;
int Gx; int Gy;
int t; int t0; int tt;
int d = 0; int d1 = 0; int d2 = 0;
double Kp = 20; double Ki = 0; double Kd = 0; double SF = 120; int OFF = 500;
double Kp1 = -10; double Kp2 = -10, Ki1=-1 , Ki2=-1 , Kd1=-1 , Kd2=-1 ;
double Ppid; double Ipid; double Dpid;
int Drive; double C;
double Ppid1; double Ipid1; double Dpid1;
int Drive1; double C1;
double Ppid2; double Ipid2; double Dpid2;
int Drive2; double C2;
int Err = 0; int lastErr = 0;
double Err1 = 0, lastErr1 = 0;
double Err2 = 0,integral = 0,DErr=0,DErr2=0,integral2=0,lastErr2 = 0;
String command = ""; String e;
int amp = 0; double freq = 0; int flag = 0; int flag1 = 0; int flag2 = 0;
int amp1 = 0; int amp2 = 0; double freq1 = 0; double freq2 = 0;
int iii = 0;
void setup() {

  Serial.begin(250000);
  Y0 = myEnc2.read();  X0 = myEnc1.read();
  BL.begin(DOUT, CLK);
  BR.begin(DOUT1, CLK1);
  TL.begin(DOUT2, CLK2);
  TR.begin(DOUT3, CLK3);
  

  //Motor Int
  PIO_Configure(g_APinDescription[34].pPort,
                PIO_PERIPH_B, //hack Arduino does not allow high PWM by default
                g_APinDescription[34].ulPin,
                g_APinDescription[34].ulPinConfiguration);
  PIO_Configure(g_APinDescription[36].pPort,
                PIO_PERIPH_B, //hack Arduino does not allow high PWM by default
                g_APinDescription[36].ulPin,
                g_APinDescription[36].ulPinConfiguration);
  PIO_Configure(g_APinDescription[38].pPort,
                PIO_PERIPH_B, //hack Arduino does not allow high PWM by default
                g_APinDescription[38].ulPin,
                g_APinDescription[38].ulPinConfiguration);
  PWMC_DisableChannel(PWM, 0);
  PWMC_DisableChannel(PWM, 1);
  PWMC_DisableChannel(PWM, 2);
  pmc_enable_periph_clk(ID_PWM);
  PWMC_ConfigureChannel(PWM_INTERFACE, 0, 3, 0, 0);
  PWMC_SetPeriod(PWM, 0, 65000);
  PWMC_SetDutyCycle(PWM, 0, 32500);
  PWMC_DisableChannel(PWM, 0);
  PWM->PWM_CH_NUM[0].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[0].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 0);
  PWMC_ConfigureChannel(PWM_INTERFACE, 1, 3, 0, 0);
  PWMC_SetPeriod(PWM, 1, 65000);
  PWMC_SetDutyCycle(PWM, 1, 32500);
  PWMC_DisableChannel(PWM, 1);
  PWMC_ConfigureChannel(PWM_INTERFACE, 2, 3, 0, 0);
  PWMC_SetPeriod(PWM, 2, 65000);
  PWMC_SetDutyCycle(PWM, 2, 32500);
  PWMC_DisableChannel(PWM, 2);


  pinMode(SIGN1, OUTPUT);
  pinMode(SIGN2, OUTPUT);
 // Home();
  BL.set_scale();
  BL.tare(); //Reset the scale to 0
  BR.set_scale();
  BR.tare(20); //Reset the scale to 0*/
  TL.set_scale();
  TL.tare(); //Reset the scale to 0
  TR.set_scale();
  TR.tare(); //Reset the scale to 0

  long zero_factor = BL.read_average(); //Get a baseline reading
  BL.set_scale(calibration_factor); //Adjust to this calibration factor
  BR.set_scale(calibration_factor); //Adjust to this calibration factor
  TL.set_scale(calibration_factor); //Adjust to this calibration factor
  TR.set_scale(calibration_factor);///Adjust to this calibration factor
  
  //Serial.println("Setup is complete!");
  //Home();
}

void loop() {
  
  analogReadResolution(12);    
  
  if (Serial.available()) {
    command = Serial.readStringUntil('?');

    if (command == "0") {
      e = Serial.readString();
      Home();
      
    }
    if (command == "1") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Disturbance1();
    }
    if (command == "2") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Disturbance2();
    }
    if (command == "3") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Disturbance3();
    }
    if (command == "4") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      Y0 = myEnc2.read();
      Disturbance4();
    }
    if (command == "5") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      Y0 = myEnc2.read();
      Disturbance5();
    }
    if (command == "6") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      Y0 = myEnc2.read();
      Disturbance6();
    }
    if (command == "7") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Y0 = myEnc2.read();
      Disturbance7();
    }
    if (command == "8") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Y0 = myEnc2.read();
      Disturbance8();
    }
    if (command == "9") {
      e = Serial.readString();
      a = 0;
      t0 = millis();
      X0 = myEnc1.read();
      Y0 = myEnc2.read();
      Disturbance9();
    }
  
  }
  //iii++;
  //Serial.println("Loop is running:");
  //Serial.println(iii);
  //GetCOP();
  Home();
  if (millis() - t0 < 5000)
  {
    GetCOP();
  }
}

void Home() {
  long i = 0, buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
  f = 50000;
  F = floor(10500000 / f);
  double AP_Err,ML_Err;
  AP_Err=AP_Err_Calc();
  ML_Err=ML_Err_Calc();
  PWMC_DisableChannel(PWM, 2);
  PWM->PWM_CH_NUM[2].PWM_CPRD = F;
  PWM->PWM_CH_NUM[2].PWM_CDTY = F / 2;
  PWMC_EnableChannel(PWM, 2);
  while(abs(AP_Err)>0.3){
    
    if(AP_Err>0.3){
      digitalWrite(SIGN2, HIGH);
    }
    if(AP_Err<-0.3){
      digitalWrite(SIGN2, LOW);
    }
    AP_Err=AP_Err_Calc();
  }
  PWMC_DisableChannel(PWM, 2);
  PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 2);
  
  PWMC_DisableChannel(PWM, 1);
  PWM->PWM_CH_NUM[1].PWM_CPRD = F;
  PWM->PWM_CH_NUM[1].PWM_CDTY = F / 2;
  PWMC_EnableChannel(PWM, 1);
  while(abs(ML_Err)>0.3){
        if(ML_Err>0.3){
      digitalWrite(SIGN1, LOW);
    }
    if(ML_Err<-0.3){
      digitalWrite(SIGN1, HIGH);
    }
    ML_Err=ML_Err_Calc();
  }
  PWMC_DisableChannel(PWM, 1);
  PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 1);

  PWMC_DisableChannel(PWM, 2);
  PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 2);
  PWMC_DisableChannel(PWM, 1);
  PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 1);
  
}


void GetCOP() {
  //Serial.print(tt);
    //Serial.print(",  BL:  ,");
    //Serial.print(BL.get_units(), 1);
    //Serial.print(",  BR:  ,");
    //erial.print(BR.get_units(), 1);
    //Serial.print(",  TL:  ,");
    //Serial.print(TL.get_units(), 1);
    //Serial.print(",  TR:  ,");
    //Serial.println(TR.get_units(), 1);

  TR1 = TR.get_units()  * .5 + .07;
  TL1 = TL.get_units()  * .5 + .07;
  BR1 = -1*(BR.get_units()  * .5 + .07);
  BL1 = BL.get_units()  * .5 + .07;
  //Serial.println(BR1);
  Left = TL1 + BL1; Right = TR1 + BR1;
  Top = TL1 + TR1; Bottom = BL1 + BR1;
  Total = -1 * (Top + Bottom);

 //Serial.print("Top: "); 
  //Serial.println(Top);
  //Serial.print("Left: "); 
  //Serial.println(Left);
  COPx1 = ((43.3 / 2) * ((abs(Right) - abs(Left)) / (abs(Left) + abs(Right))));
  COPy1 = ((23.8 / 2) * ((abs(Top) - abs(Bottom)) / (abs(Top) + abs(Bottom))));
  dCOPx = abs(COPx2 - COPx1); dCOPy = abs(COPy2 - COPy1);
  dCOPx = abs(COPx2 - COPx1); dCOPy = abs(COPy2 - COPy1);
  //L = sqrt((COPx1 * COPx1) + (COPy1 * COPy1));
  //alpha = atan2(COPy1 / 10, COPx1 / 10);
  //W = (Left + Right) / 32;
  //Serial.print(L);Serial.print(" ");Serial.print(TR1);Serial.print(" ");Serial.print(BR1);Serial.print(" ");Serial.print(TL1);Serial.print(" ");Serial.println(BL1);
  if (-1 * Total > 2) {
    if (dCOPx * 90 < 20 * 10 && dCOPy * 90 < 10 * 10) {
            Serial.print(COPx1, 1);
            Serial.print(","); Serial.print(COPy1, 1);
            Serial.print(","); Serial.println(millis());
    }
  }
  else
  {
    COPx1 = 0; COPy1 = 0;
        Serial.print(COPx1, 1);
        Serial.print(","); Serial.print(COPy1, 1);
        Serial.print(","); Serial.println(millis());
        
  }
  COPx2 = COPx1;  COPy2 = COPy1;

}

void input() {
  while (a == 0) {

    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      randomSeed(analogRead(A8));
      //amp = 1000 * random(5, 20);
      amp = 1000 * 10;
      flag = 1;
    }
    
    d = 10000;
    X = myEnc1.read() - X0;
    if ((amp - abs(X) < 1000)) {
      flag = 0;
    }
    Err = d - X;
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * Kp;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 100000) {
      Drive = 100000;
    }
    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if (Drive == 0) {
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      integral = 0;
      lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      e = Serial.readString();
    }
  }
  Home();
  
}


void Disturbance1() {
  int start = millis();
  
  while (millis() - start < 1001) {

    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      randomSeed(analogRead(A8));
      //amp = 1000 * random(5, 20);
      amp = 1000 * 10;
      flag = 1;
    }
    t = millis();
    tt = (t0 - t) * 6.28 / 360;
    d = amp * sin(tt * .05);
    X = myEnc1.read() - X0;
    if ((amp - abs(X) < 1000)) {
      flag = 0;
    }
    Err = d - X;
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * Kp;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 100000) {
      Drive = 100000;
    }
    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if (Drive == 0) {
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      integral = 0;
      lastErr = 0;
    }
    lastErr = Err;
    /*if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      e = Serial.readString();
    }*/
  }
  a = 1;
  PWMC_DisableChannel(PWM, 1);
  PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
  PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
  PWMC_EnableChannel(PWM, 1);
  e = Serial.readString();
  Home();
}

void Disturbance2() {
  int tnow = millis();
  //Serial.println(myEnc1.read());
  while (millis() - tnow < 950) {
  
    //Serial.println(ML_Ang_Calc());
    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      amp = 1000 * 30;
      flag = 1;
    }
    t = millis();
    tt = (t - tnow) * 6.28;
    d = 1 * amp * sin(tt * 1);
    X = myEnc1.read() - X0;
    //Serial.println(ML_theta1_Calc());
    if ((amp - abs(X) < 1000)) {
      flag = 0;
    }
    //Serial.println(myEnc1.read());
    //Serial.println(d);
    //Serial.println("###############");
    
    
    Err = d - X;
  
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * Kp;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 350000) {
      Drive = 350000;
    }
  

    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      //Serial.println(f);
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if ((X > -150)) {
      
        if (millis() - tnow > 100){
          PWMC_DisableChannel(PWM, 1);
          PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
          PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
          PWMC_EnableChannel(PWM, 1);
          //Home();
        }
      //PWMC_DisableChannel(PWM, 2);
      //PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      //PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      //PWMC_EnableChannel(PWM, 2);
      //integral = 0;
      //lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      e = Serial.readString();
    }
  }
  Home();
}
void Disturbance3() {
  int tnow = millis();
  //Serial.println(myEnc1.read());
  while (millis() - tnow < 900) {
  
    //Serial.println(ML_Ang_Calc());
    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      amp = 1000 * 50;
      flag = 1;
    }
    t = millis();
    tt = (t - tnow) * 6.28;
    d = -1 * amp * sin(tt * 1);
    X = myEnc1.read() - X0;
    if ((amp - abs(X) < 1000)) {
      flag = 0;
    }
    //Serial.println(myEnc1.read());
    //Serial.println(d);
    //Serial.println("###############");
    
    
    Err = d - X;
  
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * Kp;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 350000) {
      Drive = 350000;
    }
  

    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      //Serial.println(f);
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if (X < 150) {
        if (millis() - tnow > 100){
          PWMC_DisableChannel(PWM, 1);
        PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
        PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
        PWMC_EnableChannel(PWM, 1);
        }
      //PWMC_DisableChannel(PWM, 2);
      //PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      //PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      //PWMC_EnableChannel(PWM, 2);
      //integral = 0;
      //lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      e = Serial.readString();
    }
  }
  Home();
}

void Disturbance4() {
  int tnow = millis();
  //Serial.println(myEnc2.read());
  while (millis() - tnow < 900){
    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      amp = 1000 * 10;
      flag = 1;
    }
    t = millis();
    tt = (t0 - t) * 6.28;
    d = -1* amp * sin(tt * 1);
    Y = myEnc2.read() - Y0;
    /*Serial.println(myEnc2.read());
    Serial.println(d);
    Serial.println("###############");*/

    if ((amp - abs(Y) < 1000)) {
      flag = 0;
    }
    Err = d - Y;
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * 20;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 250000) {
      Drive = 250000;
    }
    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = F;
      PWM->PWM_CH_NUM[2].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 2);
      if (C < 0) {
        digitalWrite(SIGN2, LOW);
      }
      if (C > 0) {
        digitalWrite(SIGN2, HIGH);
      }
    }
    
    if ((myEnc2.read() > -250)) {
        if (millis() - tnow > 100){
          Home();
        }
      //PWMC_DisableChannel(PWM, 2);
      //PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      //PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      //PWMC_EnableChannel(PWM, 2);
      //integral = 0;
      //lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
      e = Serial.readString();
    }
  }
  Home();
}

void Disturbance5() {
  
  int tnow = millis();
  //Serial.println(myEnc2.read());
  while (millis() - tnow <1000){
    //GetCOP();
    if ((abs(d) < 500) && (flag == 0)) {
      amp = 1000 * 25;
      flag = 1;
    }
    t = millis();
    tt = (tnow - t) * 6.28;
    d = amp * sin(tt * 1);
    Y = myEnc2.read() - Y0;
    //Serial.println(myEnc2.read());
    //Serial.println(d);
    //Serial.println("###############");

    if ((amp - abs(Y) < 1000)) {
      flag = 0;
    }
    Err = d - Y;
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * 20;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 350000) {
      Drive = 350000;
    }
    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = F;
      PWM->PWM_CH_NUM[2].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 2);
      if (C < 0) {
        digitalWrite(SIGN2, LOW);
      }
      if (C > 0) {
        digitalWrite(SIGN2, HIGH);
      }
    }
    
    if ((myEnc2.read() < 250)) {
        if (millis() - tnow > 200){
          Home();
        }
      //PWMC_DisableChannel(PWM, 2);
      //PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      //PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      //PWMC_EnableChannel(PWM, 2);
      //integral = 0;
      //lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
      e = Serial.readString();
    }
  }
  Home();
}

void Disturbance6() {
  int tnow = millis();
  Serial.println(myEnc2.read());
  while (millis() - tnow <2000){
    if ((abs(d) < 500) && (flag == 0)) {
      randomSeed(analogRead(A8));
      amp = 1000 * 350;
      flag = 1;
    }
    t = millis();
    tt = (t0 - t) * 6.28;
    d = amp * sin(tt * 1);
    Y = myEnc2.read() - Y0;
    Serial.println(myEnc2.read());
    Serial.println(d);
    Serial.println("###############");

    if ((amp - abs(Y) < 1000)) {
      flag = 0;
    }
    Err = d - Y;
    integral = (integral + Err);
    DErr = (Err - lastErr);
    Ppid = Err * 20;
    Ipid = integral * Ki;
    Dpid = DErr * Kd;
    C = Ppid + Ipid + Dpid;
    Drive = floor( SF * abs(C) + OFF );
    if (Drive > 175000) {
      Drive = 175000;
    }
    if (abs(Err) < 50) {
      Drive = 0;
    }
    if (Drive != 0) {
      f = Drive;
      F = floor(10500000 / f);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = F;
      PWM->PWM_CH_NUM[2].PWM_CDTY = F / 2;
      PWMC_EnableChannel(PWM, 2);
      if (C < 0) {
        digitalWrite(SIGN2, LOW);
      }
      if (C > 0) {
        digitalWrite(SIGN2, HIGH);
      }
    }
    
    if ((myEnc2.read() < 250)) {
        if (millis() - tnow > 500){
          Home();
        }
      //PWMC_DisableChannel(PWM, 2);
      //PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      //PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      //PWMC_EnableChannel(PWM, 2);
      //integral = 0;
      //lastErr = 0;
    }
    lastErr = Err;
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
      e = Serial.readString();
    }
  }
  Home();
}

void Disturbance7() {
  Home();
}

void Disturbance8() {
  d1 = 0; d2 = 0;
  flag1 = 0;
  while (a == 0) {
    //GetCOP();
    if ((abs(d1) < 500) && (flag1 == 0)) {
      randomSeed(analogRead(A8));
      amp1 = 1000 * random(5, 20);
      flag1 = 1;
    }
    if ((abs(d2) < 500) && (flag2 == 0)) {
      randomSeed(analogRead(A8));
      amp2 = 1000 * random(5, 20);
      flag2 = 1;
    }
    t = millis();
    tt = (t0 - t) * 6.28 / 360;
    d1 = amp1 * sin(tt * .1);
    d2 = amp2 * sin(tt * .07);
    X = myEnc1.read() - X0;
    Y = myEnc2.read() - Y0;
    if ((amp1 - abs(X) < 1000)) {
      flag1 = 0;
    }
    if ((amp2 - abs(Y) < 1000)) {
      flag2 = 0;
    }
    Err1 = d1 - X;
    Err2 = d2 - Y;
    Ppid1 = Err1 * Kp;
    Ppid2 = Err2 * Kp;
    C1 = Ppid1;
    C2 = Ppid2;
    Drive1 = floor( SF * abs(C1) + OFF );
    Drive2 = floor( SF * abs(C2) + OFF );
    if (Drive1 > 100000) {
      Drive1 = 100000;
    }
    if (Drive2 > 100000) {
      Drive2 = 100000;
    }
    if (abs(Err1) < 50) {
      Drive1 = 0;
    }
    if (abs(Err2) < 50) {
      Drive2 = 0;
    }
    if (Drive1 != 0) {
      f1 = Drive1;
      F1 = floor(10500000 / f1);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F1;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F1 / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C1 < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C1 > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if (Drive2 != 0) {
      f2 = Drive2;
      F2 = floor(10500000 / f2);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = F2;
      PWM->PWM_CH_NUM[2].PWM_CDTY = F2 / 2;
      PWMC_EnableChannel(PWM, 2);
      if (C2 < 0) {
        digitalWrite(SIGN2, LOW);
      }
      if (C2 > 0) {
        digitalWrite(SIGN2, HIGH);
      }
    }
    if (Drive1 == 0) {
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
    }
    if (Drive2 == 0) {
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
    }
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
      e = Serial.readString();
    }
  }
  Home();
}

void Disturbance9() {
  d1 = 0; d2 = 0;
  flag1 = 0;
  while (a == 0) {
    //GetCOP();
    if ((abs(d1) < 500) && (flag1 == 0)) {
      randomSeed(analogRead(A8));
      amp1 = 1000 * random(5, 20);
      flag1 = 1;
    }
    if ((abs(d2) < 500) && (flag2 == 0)) {
      randomSeed(analogRead(A8));
      amp2 = 1000 * random(5, 20);
      flag2 = 1;
    }
    t = millis();
    tt = (t0 - t) * 6.28 / 360;
    d1 = amp1 * sin(tt * .13);
    d2 = amp2 * sin(tt * .1);
    X = myEnc1.read() - X0;
    Y = myEnc2.read() - Y0;
    if ((amp1 - abs(X) < 1000)) {
      flag1 = 0;
    }
    if ((amp2 - abs(Y) < 1000)) {
      flag2 = 0;
    }
    Err1 = d1 - X;
    Err2 = d2 - Y;
    Ppid1 = Err1 * Kp;
    Ppid2 = Err2 * Kp;
    C1 = Ppid1;
    C2 = Ppid2;
    Drive1 = floor( SF * abs(C1) + OFF );
    Drive2 = floor( SF * abs(C2) + OFF );
    if (Drive1 > 100000) {
      Drive1 = 100000;
    }
    if (Drive2 > 100000) {
      Drive2 = 100000;
    }
    if (abs(Err1) < 50) {
      Drive1 = 0;
    }
    if (abs(Err2) < 50) {
      Drive2 = 0;
    }
    if (Drive1 != 0) {
      f1 = Drive1;
      F1 = floor(10500000 / f1);
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = F1;
      PWM->PWM_CH_NUM[1].PWM_CDTY = F1 / 2;
      PWMC_EnableChannel(PWM, 1);
      if (C1 < 0) {
        digitalWrite(SIGN1, HIGH);
      }
      if (C1 > 0) {
        digitalWrite(SIGN1, LOW);
      }
    }
    if (Drive2 != 0) {
      f2 = Drive2;
      F2 = floor(10500000 / f2);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = F2;
      PWM->PWM_CH_NUM[2].PWM_CDTY = F2 / 2;
      PWMC_EnableChannel(PWM, 2);
      if (C2 < 0) {
        digitalWrite(SIGN2, LOW);
      }
      if (C2 > 0) {
        digitalWrite(SIGN2, HIGH);
      }
    }
    if (Drive1 == 0) {
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
    }
    if (Drive2 == 0) {
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
    }
    if (Serial.available()) {
      a = 1;
      PWMC_DisableChannel(PWM, 1);
      PWM->PWM_CH_NUM[1].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[1].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 1);
      PWMC_DisableChannel(PWM, 2);
      PWM->PWM_CH_NUM[2].PWM_CPRD = 0;
      PWM->PWM_CH_NUM[2].PWM_CDTY = 0;
      PWMC_EnableChannel(PWM, 2);
      e = Serial.readString();
    }
  }
  Home();
}

double AP_Err_Calc(){
  double AP_Err=(analogRead(A10)+analogRead(A10)+analogRead(A10)+analogRead(A10)+analogRead(A10)+analogRead(A10))/(2*3723.6)-2.36;
  return AP_Err;
}

double ML_Err_Calc(){
  double ML_Err=(analogRead(A9)+analogRead(A9)+analogRead(A9)+analogRead(A9)+analogRead(A9)+analogRead(A9))/(2*3723.6)-2.5;
  return ML_Err;
}
double AP_Ang_Calc(){
  double AP_Err=AP_Err_Calc();
  double theta2_AP=asin(AP_Err*0.21);
  return theta2_AP*180/3.1415;
}
double ML_Ang_Calc(){
  double theta2_ML=asin(ML_Err_Calc()*0.21)*180/3.1415;
  return theta2_ML;
}
double AP_theta1_Calc(){
  //GetCOP();
  double theta1_AP=180*mass*gravity*COPy1*0.01/(3.1415*stiffness)-AP_Ang_Calc();
  return theta1_AP;
}
double ML_theta1_Calc(){
  //GetCOP();
  double theta1_ML=180*mass*gravity*COPx1*0.01/(3.1415*stiffness)-ML_Ang_Calc();
  return theta1_ML;
}
