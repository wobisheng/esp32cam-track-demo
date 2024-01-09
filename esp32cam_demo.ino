#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
int now_angle = 90;
WebServer server(80);

#define CAMERA_MODEL_AI_THINKER

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
static camera_config_t camera_config = {
  .pin_pwdn = PWDN_GPIO_NUM,
  .pin_reset = RESET_GPIO_NUM,
  .pin_xclk = XCLK_GPIO_NUM, 
  .pin_sscb_sda = SIOD_GPIO_NUM,  
  .pin_sscb_scl = SIOC_GPIO_NUM,
  .pin_d7 = Y9_GPIO_NUM,
  .pin_d6 = Y8_GPIO_NUM,
  .pin_d5 = Y7_GPIO_NUM, 
  .pin_d4 = Y6_GPIO_NUM,
  .pin_d3 = Y5_GPIO_NUM,
  .pin_d2 = Y4_GPIO_NUM,  
  .pin_d1 = Y3_GPIO_NUM,
  .pin_d0 = Y2_GPIO_NUM,
  .pin_vsync = VSYNC_GPIO_NUM,
  .pin_href = HREF_GPIO_NUM,
  .pin_pclk = PCLK_GPIO_NUM,
  
  .xclk_freq_hz = 20000000,
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,
  .pixel_format = PIXFORMAT_JPEG,
  .frame_size = FRAMESIZE_QQVGA,
  .jpeg_quality = 18,
  .fb_count = 1,
};

esp_err_t camera_init()
{
  //initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) 
  {
    Serial.print("Camera Init Failed");\
    return err;
  }
  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV2640_PID) 
  {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_contrast(s, 1);
  }
  Serial.print("Camera Init OK");
  return ESP_OK;
}

void set_angle(int num)
{
  int one_angle = 100/9;
  int zero = 500;
  int sum = 20000;
  for (int i = 0; i <= 2 ; i ++)
  {
  digitalWrite(12,HIGH);
  delayMicroseconds(one_angle*num+zero);
  digitalWrite(12,LOW);
  delayMicroseconds(sum-one_angle*num-zero);
  }
}

void handleRoot()
{
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  String web = "";
  for (int i = 0; i < fb->len; i ++)
  {
    web += char(fb->buf[i]);
  }
  server.send(200, "image/jpg", web);
  esp_camera_fb_return(fb);
}

void setup() 
{
  Serial.begin(115200);
  WiFi.softAP("Remote","asdfghjkl");
  pinMode(12,OUTPUT);
  camera_init();
  server.on("/",handleRoot);
  server.begin();
}

void loop() 
{
  server.handleClient();
}
