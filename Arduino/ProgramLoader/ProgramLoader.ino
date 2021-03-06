/*
 * Troy's 8-bit computer - esp8266 Wi-Fi program loader
 * 
 * Copyright (c) 2019 Troy Schrapel
 * 
 * This code is licensed under the MIT license
 * 
 * Needs to control
 *  Bus
 *  Clock 
 *  Reset (hold during programming)
 *  _MAW
 *  _MEMW
 *  
 *  1 x 595 for Bus + a 245 - 3 pins
 *  1 x 157 for switching control - 4 pins?
 *    - Clock, Reset?, _MAW, _MEMW
 *  1 pin to enable/disable programmer mode 
 *    - Disables the BUS_W 3->8
 *    - Enables PGM
 *    - Sets the mode of the 157
 * 
 * https://github.com/visrealm/vrcpu
 *
 */


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library

#define CPU_EN    D0
#define CPU_RST   D1

#define SHIFT_DATA D2 // data to shift into shift register
#define SHIFT_CLK  D4 // pos-edge clock for shift register
#define DFF_CLK    D3 // pos-edge clock for DFF to set after filling shift register

#define CPU_MA_W  D5
#define CPU_MEM_W D6
#define CPU_CLK   D7
#define CPU_PGM   D8

#define SSID1 "YOURSSID1"
#define PASS1 "YOURPW1"

#define SSID2 "YOURSSID2"
#define PASS2 "YOURPW2"


// parameters for the aseembler web host (HTTPS)
#define PROGRAM_HOST     "cpu.visualrealmsoftware.com"
#define PROGRAM_HOST_KEY "83f67255cc18cb313e2a2c2981c21c01816f3e85"
#define PROGRAM_PATH     "/asm/current/"

// seconds between program poll requests
#define POLL_SECONDS 5
/************************* WiFi Access Point *********************************/

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'


void setup() {
  pinMode(CPU_EN, OUTPUT);
  digitalWrite(CPU_EN, HIGH);
  
  pinMode(CPU_PGM, OUTPUT);
  digitalWrite(CPU_PGM, LOW);
  
  pinMode(CPU_RST, INPUT);
  
  pinMode(SHIFT_DATA, OUTPUT);
  digitalWrite(SHIFT_DATA, LOW);
  
  pinMode(SHIFT_CLK, OUTPUT);
  digitalWrite(SHIFT_CLK, LOW);
  
  pinMode(DFF_CLK, OUTPUT);
  digitalWrite(DFF_CLK, LOW);
  
  pinMode(CPU_CLK, OUTPUT);
  digitalWrite(CPU_CLK, LOW);
  
  pinMode(CPU_RST, OUTPUT);
  digitalWrite(CPU_RST, LOW);
  
  pinMode(CPU_MA_W, OUTPUT);
  digitalWrite(CPU_MA_W, HIGH);
  
  pinMode(CPU_MEM_W, OUTPUT);
  digitalWrite(CPU_MEM_W, HIGH);
  

  wifiMulti.addAP(SSID1, PASS1);
  wifiMulti.addAP(SSID2, PASS2);
  //wifiMulti.addAP(SSID3, PASS3);

  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  Serial.print("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP ad
}

bool programLoaded = false;
String lastProgram = "";

// load the program. First 256 bytes is PGM memory, the remainder goes to runtime RAM
void loadProgram(String hex)
{
  if (hex.length() > 1024)
    return;

  // convert the hex program to binary
  size_t bytes = hex.length() / 2;
  Serial.print("Bytes: "); 
  Serial.println(bytes);  

  // take control back from the CPU
  pinMode(CPU_RST, OUTPUT);
  digitalWrite(CPU_RST, LOW);
  digitalWrite(CPU_EN, LOW);
  digitalWrite(CPU_PGM, LOW);

  // cycle through the program's bytes
  byte tmp = 0;
  for (size_t i = 0; i < bytes; ++i)
  {
    sscanf(hex.c_str() + (i * 2), "%2x", &tmp);
    
    // output the address to the bus
    digitalWrite(DFF_CLK, LOW);
    shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, i);
    digitalWrite(DFF_CLK, HIGH);

    // enable MAW
    delayMicroseconds(100);
    digitalWrite(CPU_MA_W, LOW);

    // cycle clock
    delayMicroseconds(100);
    digitalWrite(CPU_CLK, HIGH);
    delayMicroseconds(100);
    digitalWrite(CPU_CLK, LOW);
    delayMicroseconds(100);

    // disable MAW
    digitalWrite(CPU_MA_W, HIGH);

   // output the data (code) to bus
    digitalWrite(DFF_CLK, LOW);
    shiftOut(SHIFT_DATA, SHIFT_CLK, LSBFIRST, tmp);
    digitalWrite(DFF_CLK, HIGH);

    // set PGM
    digitalWrite(CPU_PGM, i < 256 ? HIGH : LOW);

    // enable MEM write
    delayMicroseconds(100);
    digitalWrite(CPU_MEM_W, LOW);
    
    // cycle clock
    delayMicroseconds(100);
    digitalWrite(CPU_CLK, HIGH);
    delayMicroseconds(100);
    digitalWrite(CPU_CLK, LOW);
    delayMicroseconds(100);
    
    digitalWrite(CPU_MEM_W, HIGH);
    digitalWrite(CPU_PGM, LOW);    
  }

  // give control back to the CPU
  digitalWrite(CPU_MEM_W, HIGH);
  digitalWrite(CPU_MA_W, HIGH);
  digitalWrite(CPU_PGM, LOW);
  digitalWrite(CPU_EN, HIGH);  
  delay(5);    
  pinMode(CPU_RST, INPUT);  

  programLoaded = true;
  lastProgram = hex;
}


void loop() {

  String host = PROGRAM_HOST;

  WiFiClientSecure httpsClient;
  httpsClient.setFingerprint(PROGRAM_HOST_KEY);
  httpsClient.setTimeout(15000); // 15 Seconds
  Serial.print("HTTPS Connecting");
  
  int r = 0; //retry counter
  while((!httpsClient.connect(host, 443)) && (r < 30)){
      delay(100);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }

  String link = PROGRAM_PATH;
  
  Serial.print("requesting URL: ");
  Serial.println(host + link);

  httpsClient.print(String("GET ") + link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String program;
  while(httpsClient.available()){        
    program = httpsClient.readStringUntil('\n');  //Read Line by Line
    if (program.length() > 3)
    {
      Serial.println("Program received: " + program); //Print response
      break;
    }
  }
  Serial.println("==========");
  Serial.println("closing connection");

  // program has changed. load it!
  if (program.length() > 0 && program != lastProgram)
  {
    loadProgram(program);
  }
    
  delay(POLL_SECONDS * 1000);  //GET Data at every 5 seconds
}
