#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <FS.h>
#include "SPI.h"
#include "SD.h"
#include <EEPROM.h>
#include "driver/rtc_io.h"
#include "ESP32_MailClient.h"

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#define ID_ADDRESS            0x00
#define COUNT_ADDRESS         0x01
#define ID_BYTE               0xAA
#define EEPROM_SIZE           0x0F

uint16_t nextImageNumber = 0;

#define WIFI_SSID             "WIFI"
#define WIFI_PASSWORD         "PASSWORD"
#define emailSenderAccount    "sosdata123@gmail.com"
#define emailSenderPassword   "quizniufgztnzfau"
#define emailRecipient        "sosdata123@gmail.com"

#define BUTTON_PIN_BITMASK 0x200000000

SMTPData smtpData;
void sendCallback(SendStatus info);

void setup(){
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting...");  
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(100);
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  void startCameraServer();
  
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
    
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
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }else{
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  #if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
  #endif

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK){
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_contrast(s, 2);
  s->set_brightness(s, 2);
  s->set_saturation(s, 2);
  delay(1000);
  
  Serial.println("Mounting SD Card...");
  MailClient.sdBegin(14,2,15,13);
  
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  
  if (!EEPROM.begin(EEPROM_SIZE)){
    Serial.println("Failed to initialise EEPROM"); 
    Serial.println("Exiting now"); 
    while(1);
  }
  
  if(EEPROM.read(ID_ADDRESS) != ID_BYTE){
    Serial.println("Initializing ID byte & restarting picture count");
    nextImageNumber = 0;
    EEPROM.write(ID_ADDRESS, ID_BYTE);  
    EEPROM.commit(); 
  }
  else{
    EEPROM.get(COUNT_ADDRESS, nextImageNumber);
    nextImageNumber +=  1;    
    Serial.print("Next image number:");
    Serial.println(nextImageNumber);
  }

  camera_fb_t * fb = NULL;  
  fb = esp_camera_fb_get();
  if (!fb){
    Serial.println("Camera capture failed");
    Serial.println("Exiting now"); 
    while(1);
  }

  String path = "/Surveillance_Footage" + String(nextImageNumber) + ".jpg";  
  fs::FS &fs = SD;
  File file = fs.open(path.c_str(), FILE_WRITE);
  
  if(!file){
    Serial.println("Failed to create file");
    Serial.println("Exiting now"); 
    while(1);    
  } 
  else{
    file.write(fb->buf, fb->len); 
    EEPROM.put(COUNT_ADDRESS, nextImageNumber);
    EEPROM.commit();
  }
  file.close();

  esp_camera_fb_return(fb);
  Serial.printf("Image saved: %s\n", path.c_str());

  Serial.println("Sending email...");
  smtpData.setLogin("smtp.gmail.com", 587, emailSenderAccount, emailSenderPassword);
  smtpData.setSender("R.S.S", emailSenderAccount);
  smtpData.setPriority("High");
  smtpData.setSubject("Intruder Alert !!");
  smtpData.setMessage("<div style=\"color:#003366;font-size:20px;\">Surveillance Footage Attached.</div>", true);
  smtpData.addRecipient(emailRecipient);
  smtpData.addAttachFile(path);
  smtpData.setFileStorageType(MailClientStorageType::SD);
  smtpData.setSendCallback(sendCallback);
  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  smtpData.empty();

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  Serial.println("Entering deep sleep mode");
  Serial.flush(); 
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1);
  delay(8000);
  esp_deep_sleep_start();
}

void loop(){;
}

void sendCallback(SendStatus msg){
  Serial.println(msg.info());
  if (msg.success()){
    Serial.println("----------------");
  }
}
