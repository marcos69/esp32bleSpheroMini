/**
 * A BLE client example that is rich in capabilities.
 * 
 * 
 *          ON CHINAfun  BLACK  usbserial-SLAB
 * 
 */

#include "BLEDevice.h"
#include "BLEUtils.h"
//#include "BLEScan.h"
//#include <Sphero.h>
//Sphero sphero;

std::string sendCommand(char DID, char CID, char SEQ, char DLEN, ...){
    short i=0;
    char sum=0, data;
    va_list args;
    
    // Calculate checksum
    sum += DID + CID + SEQ + DLEN;

    int cnter=6;
    char yyy[cnter+DLEN];
    yyy[0]= char(0xFF);
    yyy[1]= char(0xFE);
    yyy[2]= char(DID);
    yyy[3]= char(CID);
    yyy[4]= char(SEQ);
    yyy[5]= char(DLEN);
   
    va_start(args, DLEN);
    for(; i<DLEN-1; i++){
        data = va_arg(args, int);
        //Serial1.write(data);
        yyy[cnter]=data;
        cnter+=1;
        sum += data;
    }
    va_end(args);
    
    yyy[cnter]= char(~sum) ;

    return std::string(yyy);
  
    // Wait for Simple Response
    //return readSimplePacket();
} 

// The remote service we wish to connect to.
static BLEUUID service1UUID("00010001-574f-4f20-5370-6865726f2121");
// The characteristic of the remote service we are interested in.
static BLEUUID   char12UUID("00010002-574f-4f20-5370-6865726f2121");
static BLEUUID   char13UUID("00010003-574f-4f20-5370-6865726f2121");
static BLEUUID  BatteryUUID(BLEUUID((uint16_t)0x180f));
static BLEUUID  charbatUUID(BLEUUID((uint16_t)0x2a19));

static BLEUUID service2UUID("00020001-574f-4f20-5370-6865726f2121");
// The characteristic of the remote service we are interested in.
static BLEUUID   char22UUID("00020002-574f-4f20-5370-6865726f2121");
static BLEUUID   char23UUID("00020003-574f-4f20-5370-6865726f2121");
static BLEUUID   char24UUID("00020004-574f-4f20-5370-6865726f2121");
static BLEUUID   char25UUID("00020005-574f-4f20-5370-6865726f2121");

static BLEAddress *pServerAddress; // d4:9e:cd:b0:77:30
static boolean doConnect = false;
static boolean connected = false;
static BLERemoteCharacteristic* pRemote12Characteristic;
static BLERemoteCharacteristic* pRemote13Characteristic;
static BLERemoteCharacteristic* pBatteryCharacteristic;
static BLERemoteCharacteristic* pTXpowerCharacteristic;

static BLERemoteCharacteristic* pRemote22Characteristic;
static BLERemoteCharacteristic* pRemote23Characteristic;
static BLERemoteCharacteristic* pRemote24Characteristic;
static BLERemoteCharacteristic* pRemote25Characteristic;
static int incnt=0;
static int respcode=0;

// This is the Arduino main loop function.
const uint8_t notificationOff[] = {0x0, 0x0};
const uint8_t notificationOn[] = {0x1, 0x0};
bool onoff = true;
std::string newValue;
int lopcnter=0;

std::string nvalue;
int oldValue[]={0,0,0,0,0,0};

void vallog(std::string value) {
 //       Serial.println("vallog=");
        for (int i = 0; i < value.length(); i++) {
        if (value[i] <16) Serial.print("0");
          Serial.print(value[i] ,HEX);
          Serial.print(" ");
          };
        Serial.println(); 
};

void writeCharaValue (std::string value, int selectedChara) {
  Serial.print("Setting new value on ");
  Serial.print(selectedChara);
  Serial.print(" characteristic -> ");
 
  for (int i = 0; i < value.length(); i++) {
        if (value[i] <16) Serial.print("0");
        Serial.print(value[i] ,HEX);
        Serial.print(" ");
        }
   /*
        if (selectedChara=12) pRemote12Characteristic->writeValue((uint8_t) value[i],true);
        if (selectedChara=13) pRemote13Characteristic->writeValue((uint8_t) value[i],true);
        if (selectedChara=22) pRemote22Characteristic->writeValue((uint8_t) value[i],true);
        if (selectedChara=23) pRemote23Characteristic->writeValue((uint8_t) value[i],true);
        if (selectedChara=24) pRemote24Characteristic->writeValue((uint8_t) value[i],true);
        if (selectedChara=25) pRemote25Characteristic->writeValue((uint8_t) value[i],true);
        delay(100);
        }*/
 if (selectedChara==25) Serial.println(value.c_str());
 
        if (selectedChara=12) pRemote12Characteristic->writeValue(value.c_str(), newValue.length()); 
        if (selectedChara=13) pRemote13Characteristic->writeValue(value.c_str(), newValue.length()); 
        if (selectedChara=22) pRemote22Characteristic->writeValue(value.c_str(), newValue.length()); 
        if (selectedChara=23) pRemote23Characteristic->writeValue(value.c_str(), newValue.length()); 
        if (selectedChara=24) pRemote24Characteristic->writeValue(value.c_str(), newValue.length()); 
        if (selectedChara=25) pRemote25Characteristic->writeValue(value.c_str(), newValue.length()); 

 Serial.println();
 delay(300);
 return;
};

void loopCountDisp(int loopcnter) { Serial.print("Loop counter =");
                                    Serial.println(loopcnter);
                                     return;
};
                        
void vallogChanged (std::string value, int CharaIndex) {
  if (nvalue.length() > 0) {
      int ivalue;
      ivalue=atoi(nvalue.c_str());
      if ( ivalue != oldValue[CharaIndex]){
            Serial.print("*Changed Charateristic  ");
            Serial.print(CharaIndex);
            Serial.print(" value: ");
            oldValue[CharaIndex]= ivalue;
            vallog(nvalue);
      }
    } 
 return;
};
 

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.println();
    Serial.print("**Notify callback for characteristic ");
    Serial.println(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
       
    if (length > 0)   {
        Serial.print("Value chr: ");
        for (int i = 0; i < length; i++){
            if (isPrintable((char) pData[i])==false) { Serial.print("*");}
             else {Serial.print(pData[i]);}
          Serial.print(" ");
          };
          
        Serial.println();
        Serial.print("Value hex: ");
        for (int i = 0; i < length; i++){
          if (pData[i] <16) Serial.print("0");
          Serial.print(pData[i] ,HEX);
          Serial.print(" ");
          };
        Serial.println("**");
      }
};

static void notifyCallback12(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    
   if (length == 1 ) {
      if (respcode==0 ) {
          uint8_t testVal[8]   ={0x8D ,0x09 ,0x13 ,0x0d ,0x00 ,0x00 ,0xd6 ,0xD8};
          if(  pData[0] == testVal[incnt] ) { 
                         incnt+=1;
                         if (incnt == 8 ){
                                    Serial.println("Received code1");
                                    incnt=0;
                                    respcode=1;
                                    }
                         }       
          }
      if (respcode==1 ) {
          uint8_t testVal[8]   ={0x8D ,0x09 ,0x13 ,0x0d ,0x00 ,0x00 ,0xd6 ,0xD8};
          // Serial.print(pData[0] ,HEX);
          if(  pData[0] == testVal[incnt] ) { 
                         // Serial.print("ok");
                         incnt+=1;
                         if (incnt == 8 ){
                                    Serial.println("Received code1 again");
                                    incnt=0;
                                    respcode=1;
                                    }
                         }       
          }
      
     };
     
    if (length > 1)   {
        Serial.println();      
        Serial.print("***Notify callback for characteristic 12 only ");
        Serial.println(pBLERemoteCharacteristic->getUUID().toString().c_str());
        Serial.print(" of data length ");
        Serial.println(length);
        Serial.print("CB Value chr: ");
        for (int i = 0; i < length; i++){
           if (isPrintable((char) pData[i])==false) { Serial.print("*");}
           else {Serial.print(pData[i]);}
          Serial.print(" ");
          };
          
        Serial.println();
        Serial.print("CB Value hex: ");
        for (int i = 0; i < length; i++){
             if (pData[i] <16) Serial.print("0");
             Serial.print(pData[i] ,HEX);
          Serial.print(" ");
          };
        Serial.println("***");
      }
};

bool connectToServer(BLEAddress pAddress) {
    std::string value;
    Serial.print("Forming a connection to ");
    Serial.println(pAddress.toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");
    delay(200);
    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println(" - Connected to server");

    Serial.print("rssi = ");
    Serial.print(pClient->getRssi());
    
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemote1Service = pClient->getService(service1UUID);
if (pRemote1Service == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(service1UUID.toString().c_str());
      return false;
 }
  else
 {
    Serial.println(" - Found our 10001 service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote12Characteristic = pRemote1Service->getCharacteristic(char12UUID);
    if (pRemote12Characteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(char12UUID.toString().c_str());
      return false;
      }
    Serial.println(" - Found our 10002 characteristic");
    // Read the value of the characteristic.
 //   value = pRemote12Characteristic->readValue();
 //   Serial.print("The 10002 characteristic value was: ");
 //   vallog(value);
       
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote13Characteristic = pRemote1Service->getCharacteristic(char13UUID);
    if (pRemote13Characteristic == nullptr) {
      Serial.print("Failed to find our 10003 characteristic UUID: ");
      Serial.println(char13UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our 10003 characteristic");

    // Read the value of the characteristic.
//    value = pRemote13Characteristic->readValue();
//    Serial.print("The 10003 characteristic value was: ");
//    vallog(value);
  
    
    BLERemoteService* pBatteryService = pClient->getService(BatteryUUID);
    if (pBatteryService == nullptr) {
      Serial.print("Failed to find our Battery UUID: ");
      Serial.println(BatteryUUID.toString().c_str());
      return false;
    }else{
   
    Serial.println(" - Found our Battery service");

    pBatteryCharacteristic = pBatteryService->getCharacteristic(charbatUUID);
    if (pBatteryCharacteristic == nullptr) {
      Serial.print("Failed to find our Battery characteristic UUID: ");
      Serial.println(charbatUUID.toString().c_str());
      return false;
      }
    Serial.println(" - Found our Bat. characteristic");
    
    // Read the value of the characteristic.
    value = pBatteryCharacteristic->readValue();
    Serial.print("The characteristic Bat. value was: ");
    vallog(value);
        
    }
    
   // Obtain a reference to the characteristic in the service of the remote BLE server.
 /*   pTXpowerCharacteristic = pRemoteService->getCharacteristic(TXpowerUUID);
    if (pTXpowerCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic TXpower UUID: ");
      Serial.println(TXpowerUUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our TXpower characteristic");

    // Read the value of the characteristic.
    value = pTXpowerCharacteristic->readValue();
    Serial.print("The characteristic TXpower value was: ");
    Serial.println(value.c_str());

        for (int i = 0; i < value.length(); i++) {
             if (value[i] <16) Serial.print("0");
             Serial.print(value[i] ,HEX);
        }
    pTXpowerCharacteristic->registerForNotify(notifyCallback);*/
   // Obtain a reference to the characteristic in the service of the remote BLE server.

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemote2Service = pClient->getService(service2UUID);
    if (pRemote2Service == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(service2UUID.toString().c_str());
      return false;
      }
    Serial.println(" - Found our 20001 service");
    
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote22Characteristic = pRemote2Service->getCharacteristic(char22UUID);
    if (pRemote22Characteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(char22UUID.toString().c_str());
      return false;
      }
    Serial.println(" - Found our 20002 characteristic");
    // Read the value of the characteristic.
//    value = pRemote22Characteristic->readValue();   
//    Serial.print("The 20002 characteristic value was: ");
//    vallog(value);
   
    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote23Characteristic = pRemote2Service->getCharacteristic(char23UUID);
    if (pRemote23Characteristic == nullptr) {
      Serial.print("Failed to find our 20003 characteristic UUID: ");
      Serial.println(char23UUID.toString().c_str());
      return false;
      }
    Serial.println(" - Found our 20003 characteristic");
    // Read the value of the characteristic.
//    value = pRemote23Characteristic->readValue();
//    Serial.print("The 20003 characteristic value was: ");
//    vallog(value);
 
    pRemote24Characteristic = pRemote2Service->getCharacteristic(char24UUID);
    if (pRemote24Characteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(char22UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our 20004 characteristic");

    // Read the value of the characteristic.
//    value = pRemote24Characteristic->readValue();   
//    Serial.print("The 20004 characteristic value was: ");
//    vallog(value);
 

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemote25Characteristic = pRemote2Service->getCharacteristic(char25UUID);
    if (pRemote25Characteristic == nullptr) {
      Serial.print("Failed to find our 20003 characteristic UUID: ");
      Serial.println(char25UUID.toString().c_str());
      return false;
    }
    Serial.println(" - Found our 20005 characteristic");

    // Read the value of the characteristic.
    value = pRemote25Characteristic->readValue();
    Serial.print("The 20005 characteristic value was: ");
    vallog(value);

    pRemote12Characteristic->registerForNotify(notifyCallback12);
    pRemote13Characteristic->registerForNotify(notifyCallback);
    pRemote22Characteristic->registerForNotify(notifyCallback);
//    pRemote23Characteristic->registerForNotify(notifyCallback);    
//    pRemote24Characteristic->registerForNotify(notifyCallback);    
//    pRemote25Characteristic->registerForNotify(notifyCallback);
    pBatteryCharacteristic->registerForNotify(notifyCallback);


  return true;
 }
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    //if (advertisedDevice.haveServiceUUID() && advertisedDevice.getServiceUUID().equals(serviceUUID)) {
    if (advertisedDevice.haveServiceUUID()) {
    Serial.print("Got Manufacturer Data: ");
    std::string manuData=advertisedDevice.getManufacturerData();
    Serial.println(manuData.c_str());
    for (int i = 0; i < manuData.length(); i++){
          if (manuData[i] <16) Serial.print("0");
          Serial.print(manuData[i] ,HEX);}
    Serial.println();
    Serial.print("Got Name: ");
    std::string nameData=advertisedDevice.getName();
    Serial.println(nameData.c_str());
    Serial.print("Got Rssi ");
    Serial.println(advertisedDevice.getRSSI());
    Serial.print("Got TXPower ");
    Serial.println(advertisedDevice.getTXPower());
//    Serial.print("Got Appearance ");
//    Serial.println(advertisedDevice.getAppearance());
   
      // 
      Serial.print("Found our device!  address: "); 
      advertisedDevice.getScan()->stop();

      //pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      pServerAddress =  new BLEAddress("d4:9e:cd:b0:77:30");
      doConnect = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
} // End of setup.


void loop() {

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      connected = true;
      
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
 if (connected) {
    
  if (lopcnter == 55){// newValue= sendCommand( 0x02 , 0x20 , 0x01 , 0x05 , 0xff,0xff,0xff, 0xff);
                         // vallog(newValue);
                         };
  if (lopcnter == 9) {   //newValue= {0x8d, 0x08, 0x13, 0x1a, 0xff, 0xcb, 0xd8 }; 
                         //vallog(newValue);
                         //writeCharaValue(newValue,12);
                         loopCountDisp(lopcnter);
                         delay(300); 
                         lopcnter =10;
                         };
  if (lopcnter == 8) {   newValue= {0x01};
                         writeCharaValue(newValue,13);
                         loopCountDisp(lopcnter);delay(500);                  
                         lopcnter =10;   }
  if (lopcnter == 7) {    
      if (respcode == 1)  { Serial.println("Confirmed code1");
                           loopCountDisp(lopcnter);
                           lopcnter =8;
                           }
                          delay(500);
                           lopcnter =8;
                         }                  
  if (lopcnter == 6) {   newValue= {0x8d, 0x0a, 0x13, 0x0d, 0x00, 0xd5, 0xd8};
                         //vallog(newValue);
                         //writeCharaValue(newValue,12);
                         loopCountDisp(lopcnter);
                         delay(300); 
                         lopcnter =7;
                         };
  if (lopcnter == 5) {   delay(500); 
                         lopcnter =6;   }  
  if (lopcnter == 4) {  delay(500); 
                         lopcnter =5;   }
  if (lopcnter == 3) {   newValue= {0x00,0x00, 0x08, 0x00, 0x0c, 0x00, 0x02, 0x02, 0x53};
                         vallog(newValue);
                         writeCharaValue(newValue,24);
                         loopCountDisp(lopcnter);
                         delay(300); 
                         lopcnter =4;
                         }
  if (lopcnter == 2) {   newValue= {0x10,0x8d,0x03};
                         //writeCharaValue(newValue,22);
                         loopCountDisp(lopcnter);
                         delay(300); 
                         lopcnter =3; 
                         }
  if (lopcnter == 1) {   newValue="usetheforce...band";
                         writeCharaValue(newValue,25);
                         loopCountDisp(lopcnter);
                         lopcnter =2;
                         }
  if (lopcnter == 0 ) {  Serial.println("Notifications turned on");
                         pRemote12Characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
                         pRemote13Characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
                         pRemote22Characteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
                         lopcnter =1;
                         }
  if (lopcnter > 48)  {  
                         nvalue = pRemote12Characteristic->readValue();
                         vallogChanged(nvalue,1); // index1 =12          
                         nvalue = pRemote13Characteristic->readValue();
                         vallogChanged(nvalue,2); // index2 =13 
                         nvalue = pRemote22Characteristic->readValue();
                         vallogChanged(nvalue,3); // index3 =22    
                         nvalue = pRemote23Characteristic->readValue();
                         vallogChanged(nvalue,4); // index4 =23 
                         nvalue = pRemote24Characteristic->readValue();
                         vallogChanged(nvalue,5); // index5 =24  
                         nvalue = pRemote25Characteristic->readValue();
                         vallogChanged(nvalue,6); // index6 =25 
                         } 
 }

  delay(200); // Delay a second between loops.
} // End of loop
