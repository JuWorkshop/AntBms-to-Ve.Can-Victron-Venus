/*
Copyright (C) 2020  "Ju@Workshop" <ju82@free.fr>
License: GPL Version 3
*/

//variables for VE can
IntervalTimer Refresh;
#include "Parameters.h"
#include <FlexCAN_T4.h>
FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> VeCan;
int  canSpeed = 500; // kbit/s
int  Rate_ms = 250; //milliS refrech CAN
uint16_t SOH = 100; // SOH place holder
unsigned char alarm[4] = {0, 0, 0, 0};
unsigned char warning[4] = {0, 0, 0, 0};
unsigned char mes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char bmsname[8] = { '@', 'A', 'n', 't', 'B', 'm', 's', '@'};

//variables for AntBms
byte cmd[] = {0x5A, 0x5A, 0x00, 0x00, 0x01, 0x01};
byte startMark [] = { 0xAA, 0x55, 0xAA, 0xFF};
#define DATA_LENGTH           140
byte incomingByte[DATA_LENGTH] = {0};
int nbr_cell,soc,cell_av,cell_min,cell_max,cell_1,cell_2,cell_3,cell_4,cell_5,cell_6,cell_7,cell_8,
      cell_9,cell_10,cell_11,cell_12,cell_13,cell_14,cell_15,cell_16,cell_17,cell_18,cell_19,cell_20,
      cell_21,cell_22,cell_23,cell_24,t_bal, t_fet, t_1, t_2, t_3, t_4, puiss, HighTemp, LowTemp;
float volt,amp, Ah_install, Ah_rest, Ah_Total ;
long int trame_ok, trame_nok;

void VEcan() 
{
CAN_message_t msg;
  msg.id  = 0x351;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(ChargeVsetpoint * nbr_cell)/100);
  msg.buf[1] = highByte(uint16_t(ChargeVsetpoint * nbr_cell)/100);
  msg.buf[2] = lowByte(chargecurrent);//  chargecurrent
  msg.buf[3] = highByte(chargecurrent);//  chargecurrent
  msg.buf[4] = lowByte(discurrent );//discurrent
  msg.buf[5] = highByte(discurrent);//discurrent
  msg.buf[6] = lowByte(uint16_t(DischVsetpoint * nbr_cell)/100);
  msg.buf[7] = highByte(uint16_t(DischVsetpoint * nbr_cell)/100);
  VeCan.write(msg);

  delay(2);
  msg.id  = 0x355;
  msg.len = 8;
  msg.buf[0] = lowByte(soc);//SOC
  msg.buf[1] = highByte(soc);//SOC
  msg.buf[2] = lowByte(SOH);//SOH
  msg.buf[3] = highByte(SOH);//SOH
  msg.buf[4] = lowByte(soc * 10);//SOC
  msg.buf[5] = highByte(soc * 10);//SOC
  msg.buf[6] = 0;
  msg.buf[7] = 0;
  VeCan.write(msg);
//
  delay(2);
  msg.id  = 0x356;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(volt * 10));
  msg.buf[1] = highByte(uint16_t(volt * 10));
  msg.buf[2] = lowByte(long(-amp ));
  msg.buf[3] = highByte(long(-amp ));
  msg.buf[4] = lowByte(int16_t(t_fet * 10));
  msg.buf[5] = highByte(int16_t(t_fet * 10));
  msg.buf[6] = 0;
  msg.buf[7] = 0;
  VeCan.write(msg);
//
  delay(2);
  msg.id  = 0x35A;
  msg.len = 8;
  msg.buf[0] = alarm[0];
  msg.buf[1] = alarm[1];
  msg.buf[2] = alarm[2];
  msg.buf[3] = alarm[3];
  msg.buf[4] = warning[0];
  msg.buf[5] = warning[1];
  msg.buf[6] = warning[2];
  msg.buf[7] = warning[3];
  VeCan.write(msg);

  delay(2);
  msg.id  = 0x35E;
  msg.len = 8;
  msg.buf[0] = bmsname[0];
  msg.buf[1] = bmsname[1];
  msg.buf[2] = bmsname[2];
  msg.buf[3] = bmsname[3];
  msg.buf[4] = bmsname[4];
  msg.buf[5] = bmsname[5];
  msg.buf[6] = bmsname[6];
  msg.buf[7] = bmsname[7];
  VeCan.write(msg);

//  delay(2);
//  msg.id  = 0x370;
//  msg.len = 8;
//  msg.buf[0] = bmsmanu[0];
//  msg.buf[1] = bmsmanu[1];
//  msg.buf[2] = bmsmanu[2];
//  msg.buf[3] = bmsmanu[3];
//  msg.buf[4] = bmsmanu[4];
//  msg.buf[5] = bmsmanu[5];
//  msg.buf[6] = bmsmanu[6];
//  msg.buf[7] = bmsmanu[7];
//  VeCan.write(msg);

  delay(2);
  msg.id  = 0x373;
  msg.len = 8;
  msg.buf[0] = lowByte(uint16_t(cell_min));
  msg.buf[1] = highByte(uint16_t(cell_min));
  msg.buf[2] = lowByte(uint16_t(cell_max));
  msg.buf[3] = highByte(uint16_t(cell_max));
  msg.buf[4] = lowByte(uint16_t(LowTemp + 273.15));
  msg.buf[5] = highByte(uint16_t(LowTemp + 273.15));
  msg.buf[6] = lowByte(uint16_t(HighTemp + 273.15));
  msg.buf[7] = highByte(uint16_t(HighTemp + 273.15));
  VeCan.write(msg);

  delay(2);
  msg.id  = 0x372;
  msg.len = 8;
  msg.buf[0] = lowByte(nbr_cell);//bms.getNumModules()
  msg.buf[1] = highByte(nbr_cell);//bms.getNumModules()
  msg.buf[2] = 0x00;
  msg.buf[3] = 0x00;
  msg.buf[4] = 0x00;
  msg.buf[5] = 0x00;
  msg.buf[6] = 0x00;
  msg.buf[7] = 0x00;
  VeCan.write(msg);
}

void read_bms(){
  Serial2.flush(); Serial2.write(cmd, sizeof(cmd)); // while (Serial2.available()) {  Serial.print(Serial2.read()); };
  for (int i = 0; i < DATA_LENGTH; i++) { while (!Serial2.available()); incomingByte[i] = Serial2.read();}
  
if (incomingByte[0] == startMark [0] && incomingByte[1] == startMark [1] && incomingByte[2] == startMark [2] && incomingByte[3] == startMark [3]){
trame_ok += 1;//  Serial.println("trame_ok :");
  extract_value();//  for(int i = 0; i < DATA_LENGTH; i++) { Serial.print(incomingByte[i], HEX);}
  print_value();
  VEcan();
  digitalToggle(13);          // added to show loop() activity 
 }
 else{trame_nok += 1;}
}

void extract_value(){
   uint32_t tmp = ((((uint8_t)incomingByte[70])<< 24) + (((uint8_t)incomingByte[71])<< 16)+ (((uint8_t)incomingByte[72])<< 8)+ ((uint8_t)incomingByte[73]));
    if (tmp > 2147483648){amp= (-(2*2147483648)+tmp);}
    else{ amp = tmp;}
   uint32_t tmp2 = ((((uint8_t)incomingByte[111])<< 24) + (((uint8_t)incomingByte[112])<< 16)+ (((uint8_t)incomingByte[113])<< 8)+ ((uint8_t)incomingByte[114]));
    if (tmp2 > 2147483648){puiss= (-(2*2147483648)+tmp2);}
    else{ puiss = tmp2;}
   nbr_cell = (uint8_t)incomingByte[123]; 
   soc = (uint8_t)incomingByte[74]; 
   volt = ((((uint8_t)incomingByte[4])<< 8) + (uint8_t)incomingByte[5]); 
   cell_av = ((((uint8_t)incomingByte[121])<< 8) + (uint8_t)incomingByte[122]); 
   cell_min = ((((uint8_t)incomingByte[119])<< 8) + (uint8_t)incomingByte[120]); 
   cell_max = ((((uint8_t)incomingByte[116])<< 8) + (uint8_t)incomingByte[117]); 
   t_fet = ((((uint8_t)incomingByte[91])<< 8) + (uint8_t)incomingByte[92]); 
   t_bal = ((((uint8_t)incomingByte[93])<< 8) + (uint8_t)incomingByte[94]); 
   t_1 = ((((uint8_t)incomingByte[95])<< 8) + (uint8_t)incomingByte[96]); 
   t_2 = ((((uint8_t)incomingByte[97])<< 8) + (uint8_t)incomingByte[98]); 
   t_3 = ((((uint8_t)incomingByte[99])<< 8) + (uint8_t)incomingByte[100]); 
   t_4 = ((((uint8_t)incomingByte[101])<< 8) + (uint8_t)incomingByte[102]); 
   cell_1 = ((((uint8_t)incomingByte[6])<< 8) + (uint8_t)incomingByte[7]); 
   cell_2 = ((((uint8_t)incomingByte[8])<< 8) + (uint8_t)incomingByte[9]); 
   cell_3 = ((((uint8_t)incomingByte[10])<< 8) + (uint8_t)incomingByte[11]); 
   cell_4 = ((((uint8_t)incomingByte[12])<< 8) + (uint8_t)incomingByte[13]); 
   cell_5 = ((((uint8_t)incomingByte[14])<< 8) + (uint8_t)incomingByte[15]); 
   cell_6 = ((((uint8_t)incomingByte[16])<< 8) + (uint8_t)incomingByte[17]); 
   cell_7 = ((((uint8_t)incomingByte[18])<< 8) + (uint8_t)incomingByte[19]); 
   cell_8 = ((((uint8_t)incomingByte[20])<< 8) + (uint8_t)incomingByte[21]); 
   cell_9 = ((((uint8_t)incomingByte[22])<< 8) + (uint8_t)incomingByte[23]); 
   cell_10 = ((((uint8_t)incomingByte[24])<< 8) + (uint8_t)incomingByte[25]); 
   cell_11 = ((((uint8_t)incomingByte[26])<< 8) + (uint8_t)incomingByte[27]); 
   cell_12 = ((((uint8_t)incomingByte[28])<< 8) + (uint8_t)incomingByte[29]); 
   cell_13 = ((((uint8_t)incomingByte[30])<< 8) + (uint8_t)incomingByte[31]); 
   cell_14 = ((((uint8_t)incomingByte[32])<< 8) + (uint8_t)incomingByte[33]); 
   cell_15 = ((((uint8_t)incomingByte[14])<< 8) + (uint8_t)incomingByte[35]); 
   cell_16 = ((((uint8_t)incomingByte[16])<< 8) + (uint8_t)incomingByte[37]); 
   cell_17 = ((((uint8_t)incomingByte[18])<< 8) + (uint8_t)incomingByte[39]); 
   cell_18 = ((((uint8_t)incomingByte[20])<< 8) + (uint8_t)incomingByte[41]); 
   cell_19 = ((((uint8_t)incomingByte[22])<< 8) + (uint8_t)incomingByte[43]); 
   cell_20 = ((((uint8_t)incomingByte[24])<< 8) + (uint8_t)incomingByte[45]); 
   cell_21 = ((((uint8_t)incomingByte[26])<< 8) + (uint8_t)incomingByte[47]); 
   cell_22 = ((((uint8_t)incomingByte[28])<< 8) + (uint8_t)incomingByte[49]); 
   cell_23 = ((((uint8_t)incomingByte[30])<< 8) + (uint8_t)incomingByte[51]); 
   cell_24 = ((((uint8_t)incomingByte[32])<< 8) + (uint8_t)incomingByte[53]); 
   Ah_install = ((((uint8_t)incomingByte[75])<< 24) + (((uint8_t)incomingByte[76])<< 16)+ (((uint8_t)incomingByte[77])<< 8)+ ((uint8_t)incomingByte[78]));
   Ah_rest = ((((uint8_t)incomingByte[79])<< 24) + (((uint8_t)incomingByte[80])<< 16)+ (((uint8_t)incomingByte[81])<< 8)+ ((uint8_t)incomingByte[82]));
   Ah_Total = ((((uint8_t)incomingByte[83])<< 24) + (((uint8_t)incomingByte[84])<< 16)+ (((uint8_t)incomingByte[85])<< 8)+ ((uint8_t)incomingByte[86]));
}

void   print_value(){
  long int milli_start = millis();
  Serial.print((nbr_cell), DEC); Serial.println(" cells");
  Serial.print("soc = ");Serial.print((soc), DEC);Serial.println("%");  
  Serial.print("Voltage = ");Serial.print((volt/10), DEC);Serial.println(" V");
  Serial.print("Amperes = ");Serial.print((amp/10), DEC);Serial.println(" A");
  Serial.print("Puissance = ");Serial.print((puiss), DEC);Serial.println(" Wh");
  Serial.print("Cell_av = ");Serial.print((cell_av), DEC);Serial.println(" mV");
  Serial.print("Cell_min = ");Serial.print((cell_min), DEC);Serial.println(" mV");
  Serial.print("Cell_max = ");Serial.print((cell_max), DEC);Serial.println(" mV");
  Serial.print("T_MOSFET = ");Serial.print((t_fet), DEC);Serial.println(" °C");
  Serial.print("T_BALANCE = ");Serial.print((t_bal), DEC);Serial.println(" °C");
  Serial.print("T_1 = ");Serial.print((t_1), DEC);Serial.println(" °C");
  Serial.print("T_2 = ");Serial.print((t_2), DEC);Serial.println(" °C");
  Serial.print("T_3 = ");Serial.print((t_3), DEC);Serial.println(" °C");
  Serial.print("T_4 = ");Serial.print((t_4), DEC);Serial.println(" °C");
  Serial.print("Cell_1 = ");Serial.print((cell_1), DEC);Serial.println(" mV");
  Serial.print("Cell_2 = ");Serial.print((cell_2), DEC);Serial.println(" mV");
  Serial.print("Cell_3 = ");Serial.print((cell_3), DEC);Serial.println(" mV");
  Serial.print("Cell_4 = ");Serial.print((cell_4), DEC);Serial.println(" mV");
  Serial.print("Cell_5 = ");Serial.print((cell_5), DEC);Serial.println(" mV");
  Serial.print("Cell_6 = ");Serial.print((cell_6), DEC);Serial.println(" mV");
  Serial.print("Cell_7 = ");Serial.print((cell_7), DEC);Serial.println(" mV");
  Serial.print("Cell_8 = ");Serial.print((cell_8), DEC);Serial.println(" mV");
  Serial.print("Cell_9 = ");Serial.print((cell_9), DEC);Serial.println(" mV");
  Serial.print("Cell_10 = ");Serial.print((cell_10), DEC);Serial.println(" mV");
  Serial.print("Cell_11 = ");Serial.print((cell_11), DEC);Serial.println(" mV");
  Serial.print("Cell_12 = ");Serial.print((cell_12), DEC);Serial.println(" mV");
  Serial.print("Cell_13 = ");Serial.print((cell_13), DEC);Serial.println(" mV");
  Serial.print("Cell_14 = ");Serial.print((cell_14), DEC);Serial.println(" mV");
  Serial.print("Cell_15 = ");Serial.print((cell_15), DEC);Serial.println(" mV");
  Serial.print("Cell_16 = ");Serial.print((cell_16), DEC);Serial.println(" mV");
  Serial.print("Cell_17 = ");Serial.print((cell_17), DEC);Serial.println(" mV");
  Serial.print("Cell_18 = ");Serial.print((cell_18), DEC);Serial.println(" mV");
  Serial.print("Cell_19 = ");Serial.print((cell_19), DEC);Serial.println(" mV");
  Serial.print("Cell_20 = ");Serial.print((cell_20), DEC);Serial.println(" mV");
  Serial.print("Cell_21 = ");Serial.print((cell_21), DEC);Serial.println(" mV");
  Serial.print("Cell_22 = ");Serial.print((cell_22), DEC);Serial.println(" mV");
  Serial.print("Cell_23 = ");Serial.print((cell_23), DEC);Serial.println(" mV");
  Serial.print("Cell_24 = ");Serial.print((cell_24), DEC);Serial.println(" mV");
  Serial.print("Ah_install = ");Serial.print((Ah_install/1000000), DEC);Serial.println(" Ah");
  Serial.print("Ah_rest = ");Serial.print((Ah_rest/1000000), DEC);Serial.println(" Ah");
  Serial.print("Ah_Total = ");Serial.print((Ah_Total/1000), DEC);Serial.println(" Ah");
  Serial.print("trame ok/nok: ");Serial.print(trame_ok);Serial.print(" // ");Serial.print(trame_nok);Serial.print(" Ms: ");Serial.println((millis())-(milli_start));
  Serial.println("_____________________________________________");
}

void setup() {
  pinMode(13, OUTPUT);          
  VeCan.begin();
  VeCan.setBaudRate(canSpeed * 1000);
  Refresh.begin(read_bms, (Rate_ms) * 1000);  // read_bms to run every 0.20 seconds
  Serial.begin(115200); Serial2.begin(19200);//(19200,SERIAL_8N1,RXD2,TXD2)
  delay (100);
}

void loop() {
  
  noInterrupts();

  interrupts();
  
}
