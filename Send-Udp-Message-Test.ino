#include <EtherCard.h>
#include <IPAddress.h>

//The IP address to set the Arduino to
static byte myip[] = { 169,254,240,156 };

// gateway ip address
static byte gwip[] = { 192,168,178,1 };
static byte dnsip[] = { 84,116,46,21 };

unsigned int myPort = 6005; //The port to send the data from
unsigned int destPort = 6001; //The target port to receive the data on.
static byte destIP[] = { 255,255,255,255 }; //Broadcast to all

#define CS 8 //CS pin


// ethernet mac address to set the Arduino to - must be unique on your network
static byte mymac[] = { 0x70,0x69,0x69,0x2D,0x30,0x31 };

byte Ethernet::buffer[500]; // tcp/ip send and receive buffer

void setup(){
  Serial.begin(9600);
  Serial.println("\nInitializing...");

  if (ether.begin(sizeof Ethernet::buffer, mymac,CS) == 0)
    Serial.println( "Failed to access Ethernet controller");
  Serial.println("Connected to controller");

  Serial.println("Static IP Setup");
  ether.staticSetup(myip, gwip, dnsip);

  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

}

//Size of temp buffer to send message
#define MSG_SIZE 6 

//Sends the number 'num' over ethernet using UDP
void sendCountMsg(uint16_t num){
  static char msg[MSG_SIZE]; //Message buffer

  char* msgStart = &msg[MSG_SIZE]; //The start of the message. Moves as more digits are added.
  uint8_t len = 0; //The current length of the message, or the number of digits.

  do{
    *(--msgStart) = '0' + (num % 10); //Move the message pointer and set the new character to the right most digit in the number
    len++; //Increase the length since a new digit was added
    num /= 10; //Shift the number to the right by one, popping the right most digit from the number.
  }while((num > 0) && (len < MSG_SIZE)); //Repeat until no digits are remaining or the buffer is full.

  //Send the message using our handy-dandy library
  ether.sendUdp(msgStart, len, myPort, destIP, destPort);
}

uint16_t count = 0;

void loop(){
  delay(500);
  sendCountMsg(count++);
  
  Serial.println("Sent UDP!");
}
