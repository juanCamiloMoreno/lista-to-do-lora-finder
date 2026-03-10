#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <QMC5883LCompass.h>
#include <math.h>

// --- Heltec WiFi LoRa 32 V4 ---
static const int VEXT_ENABLE = 36; // LOW = ON

// I2C real en Heltec V4 (ESP32-S3)
static const int I2C_SDA = 41;
static const int I2C_SCL = 42;

static const int OLED_RST = 21;

// OLED SSD1306 128x64
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_RST);

// QMC5883L
QMC5883LCompass compass;

// --- Brújula UI ---
const int cx = 96;
const int cy = 32;
const int radio = 30;

void dibujarFlecha(int cx, int cy, float anguloDeg, int longitud);

void setup() {
  Serial.begin(115200);
  delay(300);

  // VEXT ON
  pinMode(VEXT_ENABLE, OUTPUT);
  digitalWrite(VEXT_ENABLE, LOW);
  delay(100);

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000); // opcional, 400k

  // OLED
  u8g2.begin();
  u8g2.setContrast(255);

  // Compass
  compass.init(); // usa Wire por defecto

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(0, 14, "QMC5883L listo");
  u8g2.drawStr(0, 30, "Giralo en 8 :)");
  u8g2.sendBuffer();

  Serial.println("QMC5883L listo. Mueve el sensor en forma de 8 para calibrar.");
}

void loop() {
  // Leer QMC5883L
  compass.read();
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();

  // Heading usando atan2(Y, X)
  float heading = atan2((float)y, (float)x);

  // Normalizar 0..2PI
  if (heading < 0) heading += 2.0f * PI;
  if (heading >= 2.0f * PI) heading -= 2.0f * PI;

  float gradosReales = heading * 180.0f / M_PI;

  // --- Dibujo ---
  u8g2.clearBuffer();

  // círculo y marcas cada 30°
  u8g2.drawCircle(cx, cy, radio);
  for (int a = 0; a < 360; a += 30) {
    float rad = a * PI / 180.0f;
    int x1 = cx + (int)((radio - 4) * cos(rad));
    int y1 = cy + (int)((radio - 4) * sin(rad));
    int x2 = cx + (int)((radio - 1) * cos(rad));
    int y2 = cy + (int)((radio - 1) * sin(rad));
    u8g2.drawLine(x1, y1, x2, y2);
  }

  // Flecha: norte fijo en pantalla
  dibujarFlecha(cx, cy, 360.0f - gradosReales, radio - 6);

  // Texto
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(2, 10, "NORTE MAG.");

  char buf1[24];
  snprintf(buf1, sizeof(buf1), "%3d deg", (int)gradosReales);

  u8g2.setFont(u8g2_font_7x14_tf);
  u8g2.drawStr(2, 35, buf1);

  // opcional: imprimir valores crudos
  u8g2.setFont(u8g2_font_5x7_tr);
  char buf2[24];
  snprintf(buf2, sizeof(buf2), "X:%d Y:%d", x, y);
  u8g2.drawStr(2, 60, buf2);

  u8g2.sendBuffer();

  delay(60);
}

void dibujarFlecha(int cx, int cy, float anguloDeg, int longitud) {
  float rad = anguloDeg * PI / 180.0f;

  int x_punta = cx + (int)round(longitud * cos(rad));
  int y_punta = cy + (int)round(longitud * sin(rad));
  int x_cola  = cx - (int)round(longitud * cos(rad));
  int y_cola  = cy - (int)round(longitud * sin(rad));

  u8g2.drawLine(x_cola, y_cola, x_punta, y_punta);

  int tam = 6;
  float angIzq = (anguloDeg + 150.0f) * PI / 180.0f;
  float angDer = (anguloDeg - 150.0f) * PI / 180.0f;

  u8g2.drawLine(x_punta, y_punta,
                x_punta + (int)round(tam * cos(angIzq)),
                y_punta + (int)round(tam * sin(angIzq)));

  u8g2.drawLine(x_punta, y_punta,
                x_punta + (int)round(tam * cos(angDer)),
                y_punta + (int)round(tam * sin(angDer)));

  u8g2.drawPixel(cx, cy);
}
