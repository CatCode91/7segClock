#include <Wire.h>

int counter = 7;

const byte Sseg_pins[3] = {8,9,10}; 
const byte Matrix_pins[3] = {17,16,15}; 

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

byte Matrix_2[3]= {
  B0110000,
  B0000110, 
  B0110110,
 
};

byte Matrix_0[3]= {
  B1111111, 
  B1111111,
  B1111111,
};

byte Matrix_1[3]= {
  B0000000, 
  B1011111,
  B1101111,
};

byte Matrix_t[3]= {
  B1110110, 
  B1100000,
  B1110111,
};


void ShowTemperature(int t1,int t2)
{
  int temp;
  for(char i=0; i<3; i++){ 
      switch(i){
        case 0:
        temp = t2;
        break;
        case 1:
        temp = t1;
        break;
        case 2:
        temp = 11;
        break;
    }
       digitalWrite(Sseg_pins[i], HIGH);
       fill7seg(temp);
       delay(3);
       digitalWrite(Sseg_pins[i], LOW);
    }
    
  MH(Matrix_t);
  counter--;
  
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


void Display()
{ 
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);

  if(second%10 == 0)
  {
      counter = 17;
  }
 
  
  int m_1 = minute/10;
  int m_2 = minute%10;
  int h_1 = hour/10;
  int h_2 = hour%10;

  Display7seg(m_1,m_2,h_2);
  DisplayMatrix(h_1);
  counter--;
}


void setup() 
{
  Wire.begin(); 

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

void loop()
{
    Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
    Wire.write(0);                                // Send register address
    Wire.endTransmission(false);                  // I2C restart
    Wire.requestFrom(0x68, 4);                    // Request 4 bytes from DS3231 and release I2C bus at end of reading
    second = Wire.read();                         // Read seconds from register 0
    minute = Wire.read();                         // Read minuts from register 1
    hour   = Wire.read();    
    // Read hour from register 2
    if(counter > 10)
    {
      ShowTemperature(2,0);
    }
    else
    {
       Display();
    } 
}
