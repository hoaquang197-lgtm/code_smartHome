
void reconnectMQTT() 
{
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");

    // Tạo clientId duy nhất để tránh đụng nhau trên broker public
    String clientId = "ESP32_Client-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");

#if USE_WILDCARD
      client.subscribe(MQTT_SUB_ALL);       // Nhận mọi topic con dưới ".../haui/"
      Serial.print("Subscribed: "); Serial.println(MQTT_SUB_ALL);
#else
      // Subscribe tường minh từng topic
      client.subscribe(TOPIC_LAMP_GUEST);
      client.subscribe(TOPIC_LAMP_SLEEP);
      client.subscribe(TOPIC_FAN_SLEEP);
      client.subscribe(TOPIC_LAMP_WC);
      client.subscribe(TOPIC_FAN_WC);
      client.subscribe(TOPIC_LAMP_COOK);
      client.subscribe(TOPIC_FAN_COOK);
      client.subscribe(TOPIC_LOCK);
      Serial.println("Subscribed 8 control topics.");
#endif

    } else {
      Serial.print("failed, rc="); Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

static const char* lastPath(const char* t) {
  const char* p = strrchr(t, '/');
  return p ? p + 1 : t;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("[MQTT] topic="); Serial.print(topic);
  Serial.print(" len="); Serial.println(length);

  if (length == 0) return;                 // không gửi khung rỗng (nếu bạn muốn)
  sendToStm32((const uint8_t*)payload, length);
}

// Nháy LED theo trạng thái client.connected()
void updateMqttLed() {
  static uint32_t lastToggle = 0;
  static bool ledLevel = false;
  static bool prevConnected = false;

  bool connected = client.connected();
  uint32_t interval = connected ? BLINK_SLOW_MS : BLINK_FAST_MS;

  // Khi trạng thái kết nối đổi, reset chu kỳ để phản hồi ngay
  if (connected != prevConnected) {
    prevConnected = connected;
    lastToggle = millis();
    ledLevel = false;
    ledStt(LOW);
  }

  uint32_t now = millis();
  if (now - lastToggle >= interval) {
    lastToggle = now;
    ledLevel = !ledLevel;
    ledStt(ledLevel ? HIGH : LOW);
  }
}


















