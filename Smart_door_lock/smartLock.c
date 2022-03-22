// Smart door lock

#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#include "camera_pins.h"

#define PHOTO 14
#define LED 4
#define LOCK 12
#define IN_BUTTON 15
#define IRSensor 2
#define BELL 13




BLYNK_WRITE(V3)
{ int pinValue = param.asInt();
  digitalWrite(14,pinValue); 
 }

BLYNK_WRITE(V2)
{ int pinValue = param.asInt();
  digitalWrite(12,pinValue); 
 }

const char* ssid = "realme3pro";       //wifi name
const char* password = "12345678";       //password
char auth[] = "MaLRygihTn3j3ZfdL4dI0Wqvk8_cVpkr";          //sent by Blynk

String local_IP;
int count = 0;
void startCameraServer();

void takePhoto()
{

  uint32_t randomNum = random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+ (String)randomNum);
  Blynk.setProperty(V1, "urls", "http://"+local_IP+"/capture?_cb="+(String)randomNum);
  delay(1000);
}

void setup() {
 
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  pinMode(BELL,OUTPUT);
  pinMode(LOCK,OUTPUT);
  pinMode(IRSensor,INPUT);
  pinMode(IN_BUTTON,INPUT);
  pinMode(PHOTO,OUTPUT);
  digitalWrite(BELL,LOW);
  digitalWrite(LOCK,LOW);
  Serial.setDebugOutput(true);
  Serial.println();
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
  Blynk.begin(auth, ssid, password);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  digitalWrite(BELL,HIGH);
//  digitalWrite(LOCK,HIGH);
  if(digitalRead(PHOTO) == HIGH)
  {
    takePhoto();
  }
  if(digitalRead(IN_BUTTON) == LOW){
    digitalWrite(LOCK,LOW);
    delay(5000);
    digitalWrite(LOCK,HIGH);
    }  
  
  if(digitalRead(IRSensor) == LOW ){
//    count = 1;
    digitalWrite(BELL,LOW);
    delay(1000);
    digitalWrite(BELL,HIGH);
    Blynk.notify("Someone Arrived");
    takePhoto();
    delay(3000);
//    count = 0;
  }
//
//  if(digitalRead(LOCK) == LOW){
//    digitalWrite(LED,HIGH);}
//  else
//   {
//    digitalWrite(LED,LOW);
//    }
  
}
