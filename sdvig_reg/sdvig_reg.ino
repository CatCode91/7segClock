#include "Wire.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "TimerOne.h"
#include "DS3232RTC.h"

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 13
DS3232RTC myRTC;

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

const byte Sseg_pins[3] = {16,15,14}; 
const byte Matrix_pins[3] = {2,3,4}; 

const byte Sdata_pin = 10;
const byte Ssh_pin = 11;
const byte Sst_pin = 12;

const byte Mdata_pin = 7;
const byte Msh_pin = 5;
const byte Mst_pin = 6;

//const byte btn_1 = 3;
//const byte btn_2 = 4;

//Temp C - D6, Time - SCL - D11, SCA - D12
//byte i, second, minute, hour, date, month, year;

//числа 1-0
const byte digits[12] = {
    B11101110, 
    B10000010, 
    B11011100, 
    B11010110,
    B10110010, 
    B01110110, 
    B01111110, 
    B11000010, 
    B11111110, 
    B11110110,
    B00000000,
    B00010000
};

//число 2 на матрице
byte Matrix_2[3]= {
  B0110000,
  B0000110, 
  B0110110,
 
};

//число 1 на матрице
byte Matrix_0[3]= {
  B1111111, 
  B1111111,
  B1111111,
};

//число 1 на матрице
byte Matrix_1[3]= {
  B0000000, 
  B1011111,
  B1101111,
};


//буква t на матрице
byte Matrix_t[3]= {
  B1110110, 
  B1100000,
  B1110111,
};


void ShowTemperature(int16_t rtcTemp)
{
  char outstr[15];
  dtostrf(rtcTemp,2,1, outstr);
  
  //float c = rtcTemp / 4.;
  int t_1 = 2;
  int t_2 = 5;
  
  int temp;
  for(char i=0; i<3; i++){ 
      switch(i){
        case 0:
        temp = t_2;
        break;
        case 1:
        temp = t_1;
        break;
        case 2:
        if(rtcTemp<0)
        {
        temp = 11;
        }
        else   
        {
           temp = 10;
        }
        break;
    }
       digitalWrite(Sseg_pins[i], HIGH);
       fill7seg(temp);
       delay(3);
       digitalWrite(Sseg_pins[i], LOW);
    }

  MH(Matrix_t);
}

void fill7seg( byte d ){
    for(char i=0; i<8; i++)
    {
        digitalWrite(Ssh_pin, LOW);
        digitalWrite(Sdata_pin, digits[d] & (1<<i));
        digitalWrite(Ssh_pin, HIGH);
    }
    digitalWrite(Sst_pin, HIGH);
 
    digitalWrite(Sst_pin, LOW);
}

void fillMatrix(byte d, byte Temp[3])
{
     for(char i=0; i<7; i++)
    {
       digitalWrite(Msh_pin, LOW);
       digitalWrite(Mdata_pin, Temp[d] & (1<<i));
       digitalWrite(Msh_pin, HIGH);

    }
     
    digitalWrite(Mst_pin, LOW);
    digitalWrite(Mst_pin, HIGH);
}

void Display7seg(int m1,int m2,int h2)
{
  int temp;

  for(char i=0; i<3; i++){ 
      switch(i){
        case 0:
        temp = m2;
        break;
        case 1:
        temp = m1;
        break;
        case 2:
        temp = h2;
        break;
    }
       digitalWrite(Sseg_pins[i], HIGH);
       fill7seg(temp);
       delay(3);
       digitalWrite(Sseg_pins[i], LOW);
    }
}

//h1 -  отображает нужный символ на матрице
void DisplayMatrix(int h1)
{  
  byte Temp[3];
      switch(h1){
        case 1:
        MH(Matrix_1);
        break;
        case 2:
        MH(Matrix_2);
        break;
        case 77:
        MH(Matrix_t);
        break;
        default:
        MH(Matrix_0);
        break;
    }
}

void MH(byte Temp[3])
{
  for(char i=0; i<3; i++)
    { 
       digitalWrite(Matrix_pins[i], HIGH);
       fillMatrix(i,Temp);
       delay(3);
       digitalWrite(Matrix_pins[i], LOW);
    } 
}

void Display(int t_min,int t_hour)
{ 
  int m_1 = t_min/10;
  int m_2 = t_min%10;
  int h_1 = t_hour/10;
  int h_2 = t_hour%10;

  Display7seg(m_1,m_2,h_2);
  DisplayMatrix(h_1);
}


void setup() 
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();
  myRTC.begin();    
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  
  Wire.begin(); 
  Timer1.initialize(1000000);         // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt

    for(int i=0; i<3; i++)
    {
        pinMode(Sseg_pins[i], OUTPUT);
    }

    for(int i=0; i<3; i++)
    {
        pinMode(Matrix_pins[i], OUTPUT);
    }
    
    pinMode(Sdata_pin, OUTPUT);
    pinMode(Ssh_pin, OUTPUT);
    pinMode(Sst_pin, OUTPUT);
    
    pinMode(Mdata_pin, OUTPUT);
    pinMode(Msh_pin, OUTPUT);
    pinMode(Mst_pin, OUTPUT);  
}

void callback()
{
   sensors.requestTemperatures();    
}

void loop()
{   
  time_t t = myRTC.get();    
  int t_sec = second(t);
  int t_min = minute(t);
  int t_hour = hour(t);

  int count = 0;

    if(t_sec%10 == 0)
    {
       sensors.requestTemperatures(); // Send the command to get temperatures
       float temperature = sensors.getTempCByIndex(0);
       count = 8;
        Serial.print("Temperature for Device 1 is: ");
        while(count != 0)
        {
          ShowTemperature(temperature);
          count--;
        } 
    }
      else
      {
       Display(t_min,t_hour);
       } 
}
