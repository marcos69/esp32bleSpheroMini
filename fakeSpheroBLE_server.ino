/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
// xxxxxxxxxx         ON SPARKFUN RED usbserial-DN0280C9     xxxxxxxxxx                

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_1_UUID        "00010001-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_12UUID "00010002-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_13UUID "00010003-574f-4f20-5370-6865726f2121"
#define SERVICE_2_UUID        "00020001-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_22UUID "00020002-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_23UUID "00020003-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_24UUID "00020004-574f-4f20-5370-6865726f2121"
#define CHARACTERISTIC_25UUID "00020005-574f-4f20-5370-6865726f2121"
static BLEUUID  BatteryUUID(BLEUUID((uint16_t)0x180f));
static BLEUUID  charbatUUID(BLEUUID((uint16_t)0x2a19));
static BLEUUID  GenAttrUUID(BLEUUID((uint16_t)0x1801));
static BLEUUID  SerChanUUID(BLEUUID((uint16_t)0x2a05));
static BLEUUID  GeneralUUID(BLEUUID((uint16_t)0x1800));
static BLEUUID  prefComUUID(BLEUUID((uint16_t)0x2a04));


//static BLEUUID  adv_1service("00010001-574f-4f20-5370-6865726f2121");
static BLEUUID  adv_2service(BLEUUID("00010001-574f-4f20-5370-6865726f2121"));
BLECharacteristic Characteristic23( CHARACTERISTIC_23UUID, 
                                    BLECharacteristic::PROPERTY_WRITE_NR );;
BLECharacteristic Characteristic22( CHARACTERISTIC_22UUID,
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_NOTIFY );
BLECharacteristic Characteristic24( CHARACTERISTIC_24UUID,
                                    BLECharacteristic::PROPERTY_READ );
BLECharacteristic Characteristic25( CHARACTERISTIC_25UUID,
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_WRITE_NR );
BLECharacteristic Characteristic12( CHARACTERISTIC_12UUID,
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_WRITE_NR |
                                    BLECharacteristic::PROPERTY_NOTIFY );
BLECharacteristic Characteristic13( CHARACTERISTIC_13UUID,
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_WRITE_NR |
                                    BLECharacteristic::PROPERTY_NOTIFY );

  BLEDescriptor chara12_Descriptor(BLEUUID((uint16_t)0x2902));
  BLEDescriptor chara13_Descriptor(BLEUUID((uint16_t)0x2902));
  BLEDescriptor chara22_Descriptor(BLEUUID((uint16_t)0x2902));
  BLEDescriptor service1_Descriptor(SERVICE_1_UUID);


bool _BLEClientConnected = false;
class MyServerCallbacks : public BLEServerCallbacks {
void onConnect(BLEServer* pServer) {
      _BLEClientConnected = true;
    };
void onDisconnect(BLEServer* pServer) {
      _BLEClientConnected = false;
    }
};

class MyCharaCallback: public BLECharacteristicCallbacks {
   void onRead(BLECharacteristic* pCharacteristic) {
    // Do something before the read completes.
    Serial.print(">read-callback from");
      std::string tempUUID=pCharacteristic->getUUID().toString();
      Serial.print(tempUUID.c_str());      
    Serial.println();
    }
   void onWrite(BLECharacteristic* pCharacteristic) {
      // Do something because a new value was written.
       Serial.print(">write-callback");
       std::string tempUUID=pCharacteristic->getUUID().toString();
      Serial.print(tempUUID.c_str());      
    Serial.println();
    }
};

void InitBLE() {
  BLEDevice::init("SM-77");
 
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService5 = pServer->createService(GeneralUUID);
  BLECharacteristic *pprefCom = pService5->createCharacteristic(
                                         prefComUUID , 0);
  byte Buff5[]={0x06,0x00,0x18,0x00,0x00,0xc00,0x19,0x00}; 
  pprefCom->setValue(Buff5,8);
  
  BLEService *pService4 = pServer->createService(GenAttrUUID);
  BLECharacteristic *pserviceChange = pService4->createCharacteristic(
                                         SerChanUUID , 0);
  // 20001
  BLEService *pService2 = pServer->createService(SERVICE_2_UUID);
  
              pService2->addCharacteristic(&Characteristic23);
                                       
              pService2->addCharacteristic(&Characteristic22);
  Characteristic22.addDescriptor(new BLE2902());
                                     
              pService2->addCharacteristic(&Characteristic24);
  byte Buff24[]={0x00,0x00,0x08,0x00,0x0c,0xc00,0x02,0x02}; 
  Characteristic24.setValue(Buff24,9);
                                     
              pService2->addCharacteristic(&Characteristic25);
              Characteristic22.setCallbacks(new MyCharaCallback());
              Characteristic23.setCallbacks(new MyCharaCallback());
              Characteristic24.setCallbacks(new MyCharaCallback());
              Characteristic25.setCallbacks(new MyCharaCallback());
              
// 180f battery
  BLEService *pService3 = pServer->createService(BatteryUUID);
  BLECharacteristic *pCharaBattery = pService3->createCharacteristic(
                                         charbatUUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY 
                                       );

  byte BuffBat[]={0x64}; 
  pCharaBattery->setValue(BuffBat,1);  

// 10001
  BLEService *pService1 = pServer->createService(SERVICE_1_UUID);

              pService1->addCharacteristic(&Characteristic12);
  byte Buff12[]={0x49};                                       
  Characteristic12.setValue(Buff12,1);
  Characteristic12.addDescriptor(new BLE2902());
  Characteristic12.setCallbacks(new MyCharaCallback());

              pService1->addCharacteristic(&Characteristic13);
   byte Buff13[]={0x01}; 
   Characteristic13.setValue(Buff13,1);  
   Characteristic13.addDescriptor(new BLE2902());
   Characteristic13.setCallbacks(new MyCharaCallback());

  pService5->start();
  pService4->start();
  pService2->start();
  pService3->start();
  pService1->start();

  pServer->getAdvertising()->addServiceUUID(adv_2service);
  pServer->getAdvertising()->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting The FakeSphero worked!");
  InitBLE();
}

void mylog(std::string raData){
    Serial.print(">");
    Serial.print(raData.c_str());
    Serial.print("< ");
    for (int i = 0; i < raData.length(); i++)
         { if (raData[i] <16) Serial.print("0");
           Serial.print(raData[i] ,HEX);
           Serial.print(" ");
           };
    Serial.println();
}

int lopcnter=0;
std::string charaData;
std::string chara12MeData;
std::string chara13MeData;
std::string chara22MeData;
std::string chara23MeData;
std::string chara24MeData;
std::string chara25MeData;
int ilop=0;
//bool once25=false;
void tick() {
Serial.print("TIME:");
Serial.println(millis());
};

void loop() {

if ( _BLEClientConnected == true) {

 if (lopcnter==8) {
  uint8_t respnotifbuf[7]={0x8d, 0x08, 0x13, 0x1a, 0xff, 0xcb, 0xd8 };
  uint8_t val[1];
  val[1]=respnotifbuf[ilop];
  Characteristic12.setValue(val,1);
  Characteristic12.notify();
  Serial.print((int)val[1], HEX);
  Serial.print(" ");
  ilop+=1;
  if (ilop == 7) { ilop=0;
                   lopcnter=9;
                   tick();
                   Serial.println("loop8 ended");
                  }
  }

 if (lopcnter==7) {
   uint8_t respnotifbuf[3]={0x10,0x01,0x03};
   Characteristic22.setValue(respnotifbuf,3);
   Characteristic22.notify(); 
   tick();
   Serial.println("loop7 ended");
   lopcnter=9;
   delay(5000);
  }

 if (lopcnter==6) {
  uint8_t respnotifbuf[3]={0x10,0x10,0x03};
   //Characteristic22.setValue(respnotifbuf,3);
   //Characteristic22.notify();
   tick();
   Serial.println("loop6 ended");
   lopcnter=7;
   delay(200);                
  }

 if (lopcnter==5) {
   uint8_t respnotifbuf[3]={0x10,0x65,0x03}; //was 10 8d 03
   Characteristic22.setValue(respnotifbuf,3);
   Characteristic22.notify();
   lopcnter=6;
   tick();
   Serial.println("loop5 ended");
   delay(200);
  }

 if (lopcnter==4) {
//  uint8_t respnotifbuf[8]={0x8d, 0x09, 0x13, 0x19, 0x00, 0x00, 0xd6, 0xd8 };
//  uint8_t val[1];
//  val[1]=respnotifbuf[ilop];
//  Characteristic12.setValue(respnotifbuf,8);
//  ilop=7;
//  Characteristic12.setValue(val,1);
//  Characteristic12.notify();
//  Serial.print((int)val[1],HEX);
//  Serial.print(" ");
  ilop+=1;
  if (ilop == 8) { ilop=0;
                   lopcnter=5; 
                   //Characteristic12.notify();
                   tick();
                   Serial.println("loop4 ended");}
  }

 if (lopcnter==3) {
   lopcnter=5;
   tick();
   Serial.println("loop3 ended");
   delay(200);
  }

// if (chara12MeData[0]==0x1e && lopcnter==2) {
 if (lopcnter==2) {
   //uint8_t respnotifbuf[3]={0x10,0x8d,0x03};
// Characteristic22.setValue(respnotifbuf,3);
// Characteristic22.notify();
   lopcnter=3;
   tick();
   Serial.println("loop2 ended");
   delay(200);
  } 

 if (lopcnter ==1) {
//   uint8_t respnotifbuf[3]={0x10,0x1e,0x03};
// Characteristic22.setValue(respnotifbuf,3);
// Characteristic22.notify();
   lopcnter=2;
   tick();
   Serial.println("loop1 ended");
   delay(200);
 }

 if (chara25MeData[0]==0x75 && lopcnter==0) {
   lopcnter=1;
   tick();
   Serial.println("loop0 ended ");
   delay(200); 
  }
//
// getting the data back if changed
//
 charaData = Characteristic22.getValue();
  if(charaData != chara22MeData) { tick();
    Serial.print("Received 20002=");
    mylog(charaData);
    chara22MeData=charaData;
  }
 charaData = Characteristic23.getValue();
  if(charaData != chara23MeData) { tick();
    Serial.print("Received 20003=");
    mylog(charaData);
    chara23MeData=charaData;
  }
 charaData = Characteristic24.getValue();
  if(charaData != chara24MeData) { tick();
    Serial.print("Received 20004=");
    mylog(charaData);
    chara24MeData=charaData;
  }
 charaData = Characteristic25.getValue();
  if(charaData != chara25MeData) { tick();
  Serial.print("Received 20005=");
  mylog(charaData);
    chara25MeData=charaData;
  }
 charaData = Characteristic12.getValue();
  if(charaData != chara12MeData) { tick();
    Serial.print("Received 10002=");
    mylog(charaData);
    chara12MeData=charaData;
 }
 charaData = Characteristic13.getValue();
  if(charaData != chara13MeData) { tick();
    Serial.print("Received 10003=");
    mylog(charaData);
    chara13MeData=charaData;
  }   
 }else{
   // Serial.print("Not connected");
    delay(3000);
 }

 //if (lopcnter > 80 ) delay(5000);
  // put your main code here, to run repeatedly:
  delay(200);
}

// we perhaps need to send 8d 08 13 1a ff cb d8   before ending com
