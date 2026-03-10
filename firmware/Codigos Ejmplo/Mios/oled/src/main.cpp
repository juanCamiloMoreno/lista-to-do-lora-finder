#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>

// Heltec WiFi LoRa 32 V4 (ESP32-S3)
static const int VEXT_ENABLE = 36; // activo LOW
static const int OLED_SDA    = 17;
static const int OLED_SCL    = 18;
static const int OLED_RST    = 21;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_RST);

// --- Brújula ---
float angulo = 0;

// Centro y radio para 128x64 (radio máximo sin cortar en Y es 31 si cy=32)
const int cx = 96;
const int cy = 32;
const int radio = 31;

void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud);
void dibujarFlecha(int cx, int cy, float angulo, int longitud);

void setup() {
  Serial.begin(115200);
  delay(800);

  // Power OLED (VEXT)
  pinMode(VEXT_ENABLE, OUTPUT);
  digitalWrite(VEXT_ENABLE, LOW);
  delay(100);

  // I2C OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  // OLED
  u8g2.begin();
  u8g2.setContrast(255);

  Serial.println("\n=== BRUJULA 128x64 ===");
}

void loop() {
  u8g2.clearBuffer();

  // CIRCULO EXTERIOR (doble para grosor)
  u8g2.drawCircle(cx, cy, radio);
  u8g2.drawCircle(cx, cy, radio - 1);


  /*
  // MARCAS N, S, E, O
  u8g2.setFont(u8g2_font_6x12_tf);
  // Ajustes finos para que queden centradas respecto a la letra
  u8g2.drawStr(cx - 4, 10, "N");        // arriba
  u8g2.drawStr(cx - 4, 63, "S");        // abajo (baseline ~63)
  u8g2.drawStr(120, cy + 4, "E");       // derecha
  u8g2.drawStr(4,   cy + 4, "O");       // izquierda (en español)
*/
  // MARCAS CADA 30°
  for (int a = 0; a < 360; a += 30) {
    float rad = a * PI / 180.0;
    int x1 = cx + (int)((radio - 4) * cos(rad));
    int y1 = cy + (int)((radio - 4) * sin(rad));
    int x2 = cx + (int)((radio - 1) * cos(rad));
    int y2 = cy + (int)((radio - 1) * sin(rad));
    u8g2.drawLine(x1, y1, x2, y2);
  }

  // FLECHA GRUESA
  dibujarFlecha(cx, cy, angulo, radio - 6);

  // ANGULO (arriba-izquierda)
  u8g2.setFont(u8g2_font_5x7_tr);
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%3d deg", (int)angulo);
  u8g2.drawStr(2, 8, buffer);

  u8g2.sendBuffer();

  // Animación
  angulo += 2;
  if (angulo >= 360) angulo = 0;

  delay(30);
}

// ========================================
// FLECHA GRUESA (3 px ancho) + punta
// ========================================
void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud) {
  float rad = angulo * PI / 180.0;

  // PUNTA
  int x_punta = cx + (int)(longitud * cos(rad));
  int y_punta = cy + (int)(longitud * sin(rad));

  // COLA
  int x_cola = cx - (int)(longitud * cos(rad));
  int y_cola = cy - (int)(longitud * sin(rad));

  // Vector perpendicular (para grosor)
  float perpRad = rad + PI / 2;
  int dx = (int)round(cos(perpRad));
  int dy = (int)round(sin(perpRad));

  // Línea gruesa (3 líneas paralelas)
  u8g2.drawLine(x_cola, y_cola, x_punta, y_punta);
  u8g2.drawLine(x_cola + dx, y_cola + dy, x_punta + dx, y_punta + dy);
  u8g2.drawLine(x_cola - dx, y_cola - dy, x_punta - dx, y_punta - dy);

  // Punta (triángulo)
  int tamanoPunta = 7;
  float angIzq = (angulo + 150) * PI / 180.0;
  float angDer = (angulo - 150) * PI / 180.0;

  int x_izq = x_punta + (int)(tamanoPunta * cos(angIzq));
  int y_izq = y_punta + (int)(tamanoPunta * sin(angIzq));
  int x_der = x_punta + (int)(tamanoPunta * cos(angDer));
  int y_der = y_punta + (int)(tamanoPunta * sin(angDer));

  u8g2.drawTriangle(x_punta, y_punta, x_izq, y_izq, x_der, y_der);

  // Cola (dos “bigotes”)
  int tamanoCola = 5;
  float colaIzq = (angulo + 180 + 45) * PI / 180.0;
  float colaDer = (angulo + 180 - 45) * PI / 180.0;

  int xc_izq = x_cola + (int)(tamanoCola * cos(colaIzq));
  int yc_izq = y_cola + (int)(tamanoCola * sin(colaIzq));
  int xc_der = x_cola + (int)(tamanoCola * cos(colaDer));
  int yc_der = y_cola + (int)(tamanoCola * sin(colaDer));

  u8g2.drawLine(x_cola, y_cola, xc_izq, yc_izq);
  u8g2.drawLine(x_cola, y_cola, xc_der, yc_der);

  // Punto central
  u8g2.drawDisc(cx, cy, 2); // más visible que un pixel
}



// ========================================
// FLECHA  (1 px ancho) + punta
// ========================================

void dibujarFlecha(int cx, int cy, float angulo, int longitud) {
  float rad = angulo * PI / 180.0;

  // Punta y cola
  int x_punta = cx + (int)round(longitud * cos(rad));
  int y_punta = cy + (int)round(longitud * sin(rad));
  int x_cola  = cx - (int)round(longitud * cos(rad));
  int y_cola  = cy - (int)round(longitud * sin(rad));

  // Línea central (1 px)
  u8g2.drawLine(x_cola, y_cola, x_punta, y_punta);

  // Punta de flecha (triángulo fino)
  int tamanoPunta = 6;
  float angIzq = (angulo + 150) * PI / 180.0;
  float angDer = (angulo - 150) * PI / 180.0;

  int x_izq = x_punta + (int)round(tamanoPunta * cos(angIzq));
  int y_izq = y_punta + (int)round(tamanoPunta * sin(angIzq));
  int x_der = x_punta + (int)round(tamanoPunta * cos(angDer));
  int y_der = y_punta + (int)round(tamanoPunta * sin(angDer));

  u8g2.drawLine(x_punta, y_punta, x_izq, y_izq);
  u8g2.drawLine(x_punta, y_punta, x_der, y_der);

  // Punto central sutil
  u8g2.drawPixel(cx, cy);
}

