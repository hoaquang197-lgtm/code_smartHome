/*
  Chỉ log 6 ký tự hex cuối của UID thẻ MFRC522
  - In dạng: A1B2C3
*/

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Thư viện ArduinoJson
#include <WiFi.h>

//--------------------------------------MQTT--------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

// MQTT Broker
const char* mqtt_broker = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic_pub = "smartHome/25/esp32/haui/";
const char* mqtt_topic_sub = "smartHome/25/esp32/haui/";

// ===== MQTT topics =====
#define USE_WILDCARD 1   // =1: dùng wildcard "haui/+" ; =0: subscribe từng topic

const char* MQTT_SUB_BASE   = "smartHome/25/esp32/haui";
const char* MQTT_SUB_ALL    = "smartHome/25/esp32/haui/+";

// ===== SENSOR PUBLISH TOPICS =====
const char* TOPIC_TEMP  = "smartHome/25/esp32/haui/temp";
const char* TOPIC_HUM   = "smartHome/25/esp32/haui/hum";
const char* TOPIC_LIGHT = "smartHome/25/esp32/haui/light";
const char* TOPIC_GAS   = "smartHome/25/esp32/haui/gas";

const char* TOPIC_LAMP_GUEST = "smartHome/25/esp32/haui/lampGuest";
const char* TOPIC_LAMP_SLEEP = "smartHome/25/esp32/haui/lampSleep";
const char* TOPIC_FAN_SLEEP  = "smartHome/25/esp32/haui/fanSleep";
const char* TOPIC_LAMP_WC    = "smartHome/25/esp32/haui/lampWC";
const char* TOPIC_FAN_WC     = "smartHome/25/esp32/haui/fanWC";
const char* TOPIC_LAMP_COOK  = "smartHome/25/esp32/haui/lampCook";
const char* TOPIC_FAN_COOK   = "smartHome/25/esp32/haui/fanCook";
const char* TOPIC_LOCK       = "smartHome/25/esp32/haui/lock";
static const size_t MSG_BUF_SZ = 256;

// Tốc độ nháy
static const uint32_t BLINK_FAST_MS = 150;  // khi CHƯA kết nối MQTT
static const uint32_t BLINK_SLOW_MS = 800;  // khi ĐÃ kết nối MQTT

// Biến lưu giá trị đã parse
uint16_t gas_ppm = 0;
uint16_t light   = 0;
uint8_t  hum     = 0;
uint8_t  tempC   = 0;
// Buffer đọc từng dòng
static char lineBuf[200];   // đủ cho JSON ngắn
static size_t lineLen = 0;

/* ====== Cấu hình UART2 ====== */
constexpr int UART2_TX_PIN   = 17;        // ESP32-WROOM: TX2
constexpr int UART2_RX_PIN   = 16;        // ESP32-WROOM: RX2
constexpr unsigned long UART2_BAUD = 115200;
constexpr uint32_t UART2_TIMEOUT_MS = 20;

//--------------------------------------RFID--------------------------------------

MFRC522DriverPinSimple ss_pin(5);
MFRC522DriverSPI driver{ss_pin};
MFRC522 mfrc522{driver};

#define START_HEADER '@'
#define STOP_HEADER '#'

#define ledSttPin 22
#define ledStt(stt) digitalWrite(ledSttPin, stt)

// Thời gian chặn trùng lặp khi cùng một thẻ vẫn để trên đầu đọc
static const uint32_t SUPPRESS_MS = 150;  // chỉnh nhỏ/lớn tùy thực tế

// Wi-Fi credentials
const char* ssid = "BaFpga"; // Thay bằng tên Wi-Fi của bạn
const char* password = "88888889"; // Thay bằng mật khẩu Wi-Fi của bạn

char esp_data_rec[500];
bool en_read_data=false;
char count_data_rec=0;
bool data_done_frame=false;

void setup() 
{
  pinMode(ledSttPin, OUTPUT);
  ledStt(LOW);                  // mặc định tắt
  Serial.begin(115200);
  uart2Init();
  while (!Serial) delay(1);

  mfrc522.PCD_Init();
  setupWiFi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(mqttCallback);
  Serial.println(F("Ready. Tap a card..."));
}


void loop() 
{
  userUartHandle();
  // 1) Luôn phục vụ Wi-Fi/MQTT trước để giảm latency nhận gói
  checkWiFi();
  if (!client.connected()) 
  {
    reconnectMQTT();
  }
  client.loop();

  // Cập nhật LED trạng thái MQTT (nháy nhanh/chậm)
  updateMqttLed();

  // 2) Đọc thẻ (non-blocking)
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial())   return;

  // 3) Lấy 6 ký tự hex (3 byte cuối)
  char uid6[7];
  getUidLast6Hex(mfrc522, uid6);

  // 4) Chống trùng lặp bằng millis()
  static char lastUid6[7] = "";
  static uint32_t lastUidMs = 0;
  uint32_t now = millis();

  bool sameUid = (strncmp(uid6, lastUid6, 6) == 0);
  bool stillSuppressed = (now - lastUidMs < SUPPRESS_MS);

  if (!(sameUid && stillSuppressed)) 
  {
      // Có UID mới hoặc đã hết thời gian chặn trùng lặp → xử lý
      // Nếu UID thuộc danh sách cho phép → gửi lệnh lockOn sang STM32
      if (strcmp(uid6, "3C6D9F") == 0 || strcmp(uid6, "C83F02") == 0) 
      {
        sendToStm32("lockOn");
      }

    // Ví dụ publish MQTT (tuỳ topic/payload của bạn):
    // client.publish("rfid/uid6", uid6, true);

    strncpy(lastUid6, uid6, sizeof(lastUid6));
    lastUidMs = now;
  }

  // 5) Kết thúc phiên làm việc với thẻ
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}






