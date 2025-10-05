
// Tạo chuỗi 6 hex (3 byte cuối UID), luôn đủ 6 ký tự bằng cách pad 0 ở đầu nếu UID < 3 byte (hiếm)
static void getUidLast6Hex(const MFRC522 &r, char out6[7]) 
{
  uint8_t last3[3] = {0, 0, 0};
  uint8_t len = r.uid.size;

  // Copy 3 byte cuối của UID (nếu thiếu sẽ được 0 ở đầu)
  for (uint8_t i = 0; i < 3 && i < len; ++i) {
    last3[2 - i] = r.uid.uidByte[len - 1 - i];
  }
  snprintf(out6, 7, "%02X%02X%02X", last3[0], last3[1], last3[2]);
}

static inline void printHex2(uint8_t v) 
{
  if (v < 0x10) Serial.print('0');
  Serial.print(v, HEX); // HEX mặc định in hoa
}

bool trim(char *source,char *des,char key_start[30],char key_stop[30])
{
    unsigned int start_id=0,stop_id=0;
    if(strstr(source,key_start)==0) return false;
    if(strstr(source,key_stop)==0) return false;
    memset(des,0,strlen(des));
    start_id=strstr(source,key_start)+strlen(key_start)-source;
    stop_id=strstr(source+start_id,key_stop)-source;
    //printf("start_id: %d\n",start_id);
    //printf("stop_id: %d\n",stop_id);
    if(start_id>stop_id) return false;
    for(int i=start_id;i<stop_id;i++) des[i-start_id]=source[i];
    return true;
}

void setupWiFi() 
{
  delay(10);
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
}

void checkWiFi() 
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    setupWiFi();
  }
}
























