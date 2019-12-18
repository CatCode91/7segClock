#include <Wire.h>

const byte Sseg_pins[3] = {8,9,10}; 
const byte matrix_pins[5] = {11,14,15,16,17}; 

const byte Sdata_pin = 3;
const byte Ssh_pin = 4;
const byte Sst_pin = 5;

//const byte btn_1 = 13;
const byte btn_2 = 19;

const byte Mdata_pin = 0;
const byte Msh_pin = 1;
const byte Mst_pin = 2;

byte i, second, minute, hour, date, month, year;

//Temp C - D6, Time - SCL - D11, SCA - D12

const byte digits[10] = {
    B11101110, 
    B10000010, 
    B11011100, 
    B11010110,
    B10110010, 
    B01110110, 
    B01111110, 
    B11000010, 
    B11111110, 
    B11110110 
};

byte customChar[7]= {
  B01110,
  B01010,
  B00010,
  B00100,
  B01000,
  B01110,
  B00000,
};

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

void fillMatrix( ){
    for(char i=0; i<7; i++)
    {
        digitalWrite(Msh_pin, LOW);
        digitalWrite(Mdata_pin, customChar[i] & (1<<i));
        digitalWrite(Msh_pin, HIGH);
    }
    
    digitalWrite(Mst_pin, HIGH); 
    digitalWrite(Mst_pin, LOW);
}

void DS3231_display()
{
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);

  int m_1 = minute/10;
  int m_2 = minute%10;
  int h_1 = hour/10;
  int h_2 = hour%10;

  digitalWrite(Sseg_pins[0], HIGH);
  fill7seg(m_2);
  delay(3);
  digitalWrite(Sseg_pins[0], LOW);
  digitalWrite(Sseg_pins[1], HIGH);
  fill7seg(m_1);
  delay(3);
  digitalWrite(Sseg_pins[1], LOW);
  
  digitalWrite(Sseg_pins[2], HIGH);
  fill7seg(h_2);
  delay(3);
  digitalWrite(Sseg_pins[2], LOW); 
}


void setup() 
{
  Wire.begin(); 

    for(int i=0; i<3; i++)
    {
        pinMode(Sseg_pins[i], OUTPUT);
    }

    for(int i=0; i<7; i++)
    {
        pinMode(matrix_pins[i], OUTPUT);
    }
    
    pinMode(Sdata_pin, OUTPUT);
    pinMode(Ssh_pin, OUTPUT);
    pinMode(Sst_pin, OUTPUT);
    
    pinMode(Mdata_pin, OUTPUT);
    pinMode(Msh_pin, OUTPUT);
    pinMode(Mst_pin, OUTPUT);
       
}

void loop()
{
    Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
    Wire.write(0);                                // Send register address
    Wire.endTransmission(false);                  // I2C restart
    Wire.requestFrom(0x68, 4);                    // Request 4 bytes from DS3231 and release I2C bus at end of reading
    second = Wire.read();                         // Read seconds from register 0
    minute = Wire.read();                         // Read minuts from register 1
    hour   = Wire.read();                         // Read hour from register 2
    DS3231_display();
}


