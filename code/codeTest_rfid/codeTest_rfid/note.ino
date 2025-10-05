/*
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>

MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

void setup() 
{
  Serial.begin(115200);
  while (!Serial) { delay(1); }

  mfrc522.PCD_Init();
  // Có thể bật dòng dưới để xem version reader nếu muốn:
  // MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);

  Serial.println(F("Ready. Tap a card..."));
}

static inline void printHex2(uint8_t v) 
{
  if (v < 0x10) Serial.print('0');
  Serial.print(v, HEX); // HEX mặc định in hoa
}

void loop() 
{
  // Không có thẻ mới -> thoát sớm
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial())   return;

  // Lấy 3 byte cuối của UID và in liền nhau (6 ký tự hex)
  uint8_t len = mfrc522.uid.size;                 // độ dài UID (4/7/10 byte)
  uint8_t start = (len >= 3) ? (len - 3) : 0;     // đề phòng thẻ UID ngắn bất thường

  // In đúng 6 ký tự nếu đủ 3 byte, còn thiếu thì in những gì có
  for (uint8_t i = start; i < len; i++) {
    printHex2(mfrc522.uid.uidByte[i]);
  }
  Serial.println();

  // Dừng giao tiếp với thẻ hiện tại
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(200); // chống trùng lặp log khi thẻ còn để yên trên đầu đọc
}
*/

/*

ssid: BaFpga
pass: 88888889

Gas: smartHome/25/esp32/haui/gas
BH1750: smartHome/25/esp32/haui/light

DHT11:
  +temp: smartHome/25/esp32/haui/temp
  +humidity: smartHome/25/esp32/haui/hum

lampGuest: smartHome/25/esp32/haui/lampGuest

lampSleep: smartHome/25/esp32/haui/lampSleep
fanSleep: smartHome/25/esp32/haui/fanSleep

lampWC: smartHome/25/esp32/haui/lampWC
fanWC: smartHome/25/esp32/haui/fanWC

LampCook: smartHome/25/esp32/haui/lampCook
fanCook: smartHome/25/esp32/haui/fanCook

Lock: smartHome/25/esp32/haui/lock

//--------------------------------------------------
smartHome/25/esp32/haui/lampGuest
smartHome/25/esp32/haui/lampSleep
smartHome/25/esp32/haui/fanSleep
smartHome/25/esp32/haui/lampWC
smartHome/25/esp32/haui/fanWC
smartHome/25/esp32/haui/lampCook
smartHome/25/esp32/haui/fanCook
smartHome/25/esp32/haui/lock

//--------------------------------------------------

smartHome/25/esp32/haui/temp
smartHome/25/esp32/haui/hum
smartHome/25/esp32/haui/light
smartHome/25/esp32/haui/gas
*/






















































