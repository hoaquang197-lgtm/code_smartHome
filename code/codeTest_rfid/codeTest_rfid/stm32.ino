/* Khởi tạo UART2 */
void uart2Init()
 {
  Serial2.end();                                    // đảm bảo clean trước khi begin
  Serial2.setRxBufferSize(1024);                    // tăng buffer RX nếu nhận nhiều
  // begin(baud, config, rxPin, txPin, invert=false, rxfifo_full_thrhd=64..256)
  Serial2.begin(UART2_BAUD, SERIAL_8N1, UART2_RX_PIN, UART2_TX_PIN, false, 256);
  Serial2.setTimeout(UART2_TIMEOUT_MS);
}

void sendToStm32(const String& data)
{
     Serial.print(START_HEADER);
     Serial.print(data);
     Serial.print(STOP_HEADER);

    Serial2.print(START_HEADER);
    Serial2.print(data);
    Serial2.print(STOP_HEADER);
}

// Gửi đúng payload raw theo length (không phụ thuộc null-terminator)
void sendToStm32(const uint8_t* data, size_t len)
{
    // (tuỳ chọn) debug ra Serial PC
     Serial.print(START_HEADER);
     Serial.write(data, len);
     Serial.print(STOP_HEADER);

    Serial2.print(START_HEADER);
    Serial2.write(data, len);      // <-- quan trọng: gửi đúng số byte
    Serial2.print(STOP_HEADER);
}

bool parseJsonLine(const char* json) {
  // Dùng StaticJsonDocument để tránh cấp phát động
  StaticJsonDocument<128> doc; // đủ cho JSON ngắn như ví dụ
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.print(F("JSON parse error: "));
    Serial.println(err.f_str());
    return false;
  }

  // Lấy giá trị với kiểu mong muốn
  // Kiểm tra tồn tại & kiểu dữ liệu hợp lệ
  if (!doc.containsKey("gas")   || !doc["gas"].is<uint32_t>())  return false;
  if (!doc.containsKey("light") || !doc["light"].is<uint32_t>())return false;
  if (!doc.containsKey("hum")   || !doc["hum"].is<uint32_t>())  return false;
  if (!doc.containsKey("temp")  || !doc["temp"].is<uint32_t>()) return false;

  uint32_t gas32   = doc["gas"].as<uint32_t>();
  uint32_t light32 = doc["light"].as<uint32_t>();
  uint32_t hum32   = doc["hum"].as<uint32_t>();
  uint32_t temp32  = doc["temp"].as<uint32_t>();

  // Ép kiểu an toàn về đúng kích thước (có clamp để tránh tràn)
  gas_ppm = (gas32   > 0xFFFF) ? 0xFFFF : (uint16_t)gas32;
  light   = (light32 > 0xFFFF) ? 0xFFFF : (uint16_t)light32;
  hum     = (hum32   > 0xFF)   ? 0xFF   : (uint8_t)hum32;
  tempC   = (temp32  > 0xFF)   ? 0xFF   : (uint8_t)temp32;

  return true;
}

void userUartHandle()
{
    while (Serial2.available()) 
    {
      char inChar = (char)Serial2.read();
  
      if(inChar==STOP_HEADER) data_done_frame=true;
      if(en_read_data && data_done_frame==false)
      {
        esp_data_rec[count_data_rec]=inChar;
        count_data_rec++;
      }
      if(inChar==START_HEADER) en_read_data=true;
    }//while (Serial.available()) 
  
    if(data_done_frame)
    {
      //code here
      if (parseJsonLine(esp_data_rec)) 
      {
        // In ra để kiểm tra
        Serial.print(F("gas="));   Serial.print(gas_ppm);
        Serial.print(F(", light="));Serial.print(light);
        Serial.print(F(", hum="));  Serial.print(hum);
        Serial.print(F(", temp=")); Serial.println(tempC);
        Serial.println();

        publishSensorReadings();

      } 
      else 
      {
        Serial.println(F("Invalid JSON frame."));
      }

      en_read_data=false;
      data_done_frame=false;
      count_data_rec=0;
      memset(esp_data_rec,0,strlen(esp_data_rec));
    }
}

static void publishSensorReadings() 
{
  if (!client.connected()) {
    Serial.println(F("MQTT not connected -> skip publish"));
    return;
  }

  char payload[50]; // đủ cho "65535" hoặc "255"
  // temp
  snprintf(payload, sizeof(payload), "%u", (unsigned)tempC);
  client.publish(TOPIC_TEMP,  payload, true);
  // hum
  snprintf(payload, sizeof(payload), "%u", (unsigned)hum);
  client.publish(TOPIC_HUM,   payload, true);
  // light
  snprintf(payload, sizeof(payload), "%u", (unsigned)light);
  client.publish(TOPIC_LIGHT, payload, true);
  // gas
  snprintf(payload, sizeof(payload), "%u", (unsigned)gas_ppm);
  client.publish(TOPIC_GAS,   payload, true);

}























