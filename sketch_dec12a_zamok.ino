
#include <EEPROM.h>
#include <OneWire.h>


 
const byte saveKey = 2; // вход для кнопки обнуления
const byte statusLed = 13; 
const byte doorPin = 3;
OneWire  ds(8); 

byte addr[8];
byte allKey; // всего ключей
 
// функция сверяет два адреса (два массива)
boolean addrTest(byte addr1[8], byte addr2[8]){
    for(int i = 0; i < 8; i++) {
      if (addr1[i] != addr2[i]) return 0;
  }
  return 1;
}

void setup() { 
  pinMode(doorPin, OUTPUT);
  digitalWrite(doorPin, HIGH);
  pinMode(statusLed, OUTPUT);  
  pinMode(saveKey, INPUT_PULLUP);
  // если при включении нажата кнопка, сбрасываем ключи на 0 
  if (!digitalRead(saveKey)) EEPROM.write(511, 0);
  allKey = EEPROM.read(511); // читаем количество ключей
}


void error(){
  while(1){
    digitalWrite(statusLed, !digitalRead(statusLed));
    delay(300);
  }
}//

boolean keyTest(){ // возвращает 1 если ключ есть в еепром
  byte addrTemp[8];  
  for (int i = 0; i < allKey; i++){  
    for(int y = 0; y < 8; y++) addrTemp[y] = EEPROM.read((i << 3) + y );   
    if (addrTest(addrTemp, addr)) return 1;
  }     
  return 0;
}//

void save(){ // сохраняет ключ в еепром
   digitalWrite(statusLed, HIGH);    
   if (allKey >= 63) error(); // если места нет 
 
   while (!ds.search(addr)) ds.reset_search(); // ждем ключ 
    if ( OneWire::crc8( addr, 7) != addr[7]) error();
    if (keyTest()) error(); // если нашли ключ в еепром.
  
   for(int i = 0; i < 8; i++) EEPROM.write((allKey << 3) + i, addr[i]);    
      
   allKey++; // прибавляем единицу к количеству ключей 
   EEPROM.write(511, allKey); 
  
  digitalWrite(statusLed, LOW);
  digitalWrite(doorPin, HIGH);
}

void openDoor(){ // тут включаем\выключаем выход
    delay(500);
    if (digitalRead(doorPin) == HIGH)
    {
    digitalWrite(doorPin, LOW);
    }
   else
    {
    digitalWrite(doorPin, HIGH);
    }
    
    //digitalWrite(doorPin, LOW);
    //delay(200);
    //digitalWrite(doorPin, HIGH);  
}



void loop(){    
  ds.reset_search();
  if (!digitalRead(saveKey)) save(); // если нажали кнопку  
  // сканируем шину, если нет устройств выходим из loop
  if (!ds.search(addr)) return; 
  if ( OneWire::crc8( addr, 7) != addr[7]) return;
  if (keyTest()) openDoor(); // если нашли ключ в еепром, открываем дверь 
}

