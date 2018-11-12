#include <SoftwareSerial.h>//Library esp8266
#include <DHT.h> //Library DHT11

//inisialisasi pin digital
#define RX 2 //pin arduino D2 sebagai RX
#define TX 3 //pin arduino D3 sebagai TX

String AP = "********"; //pada string AP silahkan isikan nama WiFi yang adakan digunakan
String PASS = "********"; //pada string PASS silahkan isikan password WiFi
String API = "CD9A8TNHK4MTVII6";   //Api key yang disediakan oleh thingspeak
String HOST = "api.thingspeak.com"; //Website Tingspeak
String PORT = "80"; //PORT thingspeak

//inisialisasi pengiriman data
String field1 = "field1";  //alamat pengiriman data Suhu DH11
String field2 = "field2"; //alamat pengiriman data kelembaban DH11

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 

//deklarasi variabel suatu nilai
int valSensor1 = 1;
int valSensor2 = 1;

SoftwareSerial esp8266(RX,TX); //pemanggilan pin RX dan TX

#define DHTPIN 4 //input sensor DHT
#define DHTTYPE DHT11 //jenis tipe DHT yang digunakan
DHT dht(DHTPIN, DHTTYPE); //pemanggilan dht (pin D4,tipe DHT)

void setup() 
{
  Serial.begin(9600);     //9600 merupakan alamat serial untuk Arduino Nano
  esp8266.begin(115200);  //115200 merupakan alamat serial untuk ESP8266-01
  
  sendCommand("AT",5,"OK"); //printah AT untuk komunikasi arduino dengan ESP8266
  sendCommand("AT+CWMODE=1",5,"OK"); //printah AT+CWMODE = 1 pada ESP berfungsi sebagai penerima WiFi
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK"); //AT+CWJAP = "AP","PASS" agar modul ESP terhubung dengan jaringan WiFi yang ada
  //Jika ingin mengetahu jaringan WiFi yang dapat di deteksi oleh ESP
  //cukup dengan memberikan komentar AT+CWLAP
}

void loop() 
{
  //program upload setiap data
  valSensor1 = getSensorData1(); //pengiriman suhu DHT
  valSensor2 = getSensorData2(); //pengiriman kembaban DHT
  
  //perintah pengiriman ke akun website thingspeak
  String getData = "GET /update?api_key="+ API +
  "&"+ field1 +"="+String(valSensor1)+
  "&"+ field2 +"="+String(valSensor2);
   
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK"); //AT+CIPSTAR = 0 merupakan target data yang akan dikirimkan ke website Thingspeak
  sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">"); //AT+CIPSEND = 0 merupakan perintah pengiriman data pada website Thingseak
  esp8266.println(getData);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK"); 
}

//suhu DHT11
int getSensorData1()
{
  // Read sensor values
   float t = dht.readTemperature();
   //float h = dht.readHumidity();
   Serial.print(t);
   return (t);
}

//kelembaban DHT11
int getSensorData2()
{
   // Read sensor values
   //float t = dht.readTemperature();
   float h = dht.readHumidity();
   Serial.print(h); 
   return (h);
}

void sendCommand(String command, int maxTime, char readReplay[]) 
{
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("BERHASIL");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("GAGAL");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
}
