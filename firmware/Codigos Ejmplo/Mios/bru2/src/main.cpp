#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <math.h>

// Heltec WiFi LoRa 32 V4 (ESP32-S3)
static const int VEXT_ENABLE = 36; // activo LOW
static const int OLED_SDA    = 17;
static const int OLED_SCL    = 18;
static const int OLED_RST    = 21;

// Brújula en otro I2C
static const int MAG_SDA = 5;
static const int MAG_SCL = 6;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_RST);

// --- Brújula ---
uint8_t MAG_ADDR = 0;
bool isQMC = false;

// Centro y radio para 128x64
const int cx = 96;
const int cy = 32;
const int radio = 31;

void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud);

// ========================
// I2C SCANNER para un bus específico
// ========================
void scanI2C(TwoWire &bus, const char* name) {
  Serial.printf("\nEscaneando %s...\n", name);
  for (uint8_t addr = 1; addr < 127; addr++) {
    bus.beginTransmission(addr);
    if (bus.endTransmission() == 0) {
      Serial.print("✅ Encontrado: 0x");
      Serial.println(addr, HEX);
    }
  }
}

// ========================
// QMC5883L init/read (usa "bus")
// ========================
bool initQMC(TwoWire &bus) {
  bus.beginTransmission(MAG_ADDR);
  bus.write(0x0B);
  bus.write(0x01);
  if (bus.endTransmission() != 0) return false;

  delay(10);

  uint8_t control = (0b11 << 6) | (0b01 << 4) | (0b10 << 2) | (0b01);
  bus.beginTransmission(MAG_ADDR);
  bus.write(0x09);
  bus.write(control);
  return (bus.endTransmission() == 0);
}

bool readQMC(TwoWire &bus, int16_t &mx, int16_t &my, int16_t &mz) {
  bus.beginTransmission(MAG_ADDR);
  bus.write(0x00);
  if (bus.endTransmission(false) != 0) return false;

  bus.requestFrom((int)MAG_ADDR, 6);
  if (bus.available() < 6) return false;

  int16_t x = bus.read() | (bus.read() << 8);
  int16_t y = bus.read() | (bus.read() << 8);
  int16_t z = bus.read() | (bus.read() << 8);

  mx = x; my = y; mz = z;
  return true;
}

// ========================
// HMC5883L init/read (usa "bus")
// ========================
bool initHMC(TwoWire &bus) {
  bus.beginTransmission(MAG_ADDR);
  bus.write(0x00); bus.write(0x70);
  if (bus.endTransmission() != 0) return false;

  bus.beginTransmission(MAG_ADDR);
  bus.write(0x01); bus.write(0xA0);
  if (bus.endTransmission() != 0) return false;

  bus.beginTransmission(MAG_ADDR);
  bus.write(0x02); bus.write(0x00);
  return (bus.endTransmission() == 0);
}

bool readHMC(TwoWire &bus, int16_t &mx, int16_t &my, int16_t &mz) {
  bus.beginTransmission(MAG_ADDR);
  bus.write(0x03);
  if (bus.endTransmission(false) != 0) return false;

  bus.requestFrom((int)MAG_ADDR, 6);
  if (bus.available() < 6) return false;

  int16_t x = (bus.read() << 8) | bus.read();
  int16_t z = (bus.read() << 8) | bus.read();
  int16_t y = (bus.read() << 8) | bus.read();

  mx = x; my = y; mz = z;
  return true;
}

float headingFromXY(int16_t mx, int16_t my) {
  float ang = atan2((float)my, (float)mx) * 180.0 / PI;
  if (ang < 0) ang += 360.0;
  return ang;
}

void setup() {
  Serial.begin(115200);
  delay(800);

  // Power OLED (VEXT)
  pinMode(VEXT_ENABLE, OUTPUT);
  digitalWrite(VEXT_ENABLE, LOW);
  delay(100);

  // I2C OLED (bus 0)
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(100000);

  // I2C Brújula (bus 1)
  Wire1.begin(MAG_SDA, MAG_SCL);
  Wire1.setClock(100000);

  // OLED
  u8g2.begin();
  u8g2.setContrast(255);

  Serial.println("\n=== OLED + BRUJULA (2 buses I2C) ===");
  scanI2C(Wire, "Wire (OLED 17/18)");
  scanI2C(Wire1, "Wire1 (MAG 5/6)");

  // Detectar brújula en Wire1
  for (uint8_t addr : {0x0D, 0x1E}) {
    Wire1.beginTransmission(addr);
    if (Wire1.endTransmission() == 0) {
      MAG_ADDR = addr;
      break;
    }
  }

  if (MAG_ADDR == 0x0D) {
    isQMC = true;
    Serial.println("🧭 Detectado: QMC5883L (0x0D) en Wire1");
    if (!initQMC(Wire1)) Serial.println("⚠️ No pude inicializar QMC");
  } else if (MAG_ADDR == 0x1E) {
    isQMC = false;
    Serial.println("🧭 Detectado: HMC5883L (0x1E) en Wire1");
    if (!initHMC(Wire1)) Serial.println("⚠️ No pude inicializar HMC");
  } else {
    Serial.println("❌ No se detectó brújula en Wire1 (0x0D ni 0x1E).");
  }
}

void loop() {
  int16_t mx=0, my=0, mz=0;
  bool ok = false;

  if (MAG_ADDR != 0) {
    ok = isQMC ? readQMC(Wire1, mx, my, mz) : readHMC(Wire1, mx, my, mz);
  }

 float anguloReal = ok ? headingFromXY(mx, my) : 0;

  // 🔢 ÁNGULO PARA EL NÚMERO (invertido como te gusta)
  float anguloNumero = 360.0 - anguloReal;
  anguloNumero = fmod(anguloNumero + 180.0, 360.0);

  // 🧭 ÁNGULO PARA LA FLECHA (sentido correcto)
  float anguloFlecha = anguloReal;

anguloFlecha = fmod(anguloFlecha + 180.0, 360.0);
  u8g2.clearBuffer();

  u8g2.drawCircle(cx, cy, radio);
  u8g2.drawCircle(cx, cy, radio - 1);

  dibujarFlechaGruesa(cx, cy, anguloFlecha, radio - 6);

  u8g2.setFont(u8g2_font_5x7_tr);
  char buffer[24];
  if (ok) snprintf(buffer, sizeof(buffer), "%3d deg", (int)anguloNumero);
  else    snprintf(buffer, sizeof(buffer), "MAG NO DATA");
  u8g2.drawStr(2, 8, buffer);

  u8g2.sendBuffer();
  delay(50);
}

// Tu función flecha (igual)
void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud) {
  float rad = angulo * PI / 180.0;

  int x_punta = cx + (int)(longitud * cos(rad));
  int y_punta = cy + (int)(longitud *  sin(rad));

  int x_cola = cx - (int)(longitud * cos(rad));
  int y_cola = cy - (int)(longitud * sin(rad));

  float perpRad = rad + PI / 2;
  int dx = (int)round(cos(perpRad));
  int dy = (int)round(sin(perpRad));

  u8g2.drawLine(x_cola, y_cola, x_punta, y_punta);
  u8g2.drawLine(x_cola + dx, y_cola + dy, x_punta + dx, y_punta + dy);
  u8g2.drawLine(x_cola - dx, y_cola - dy, x_punta - dx, y_punta - dy);

  int tamanoPunta = 7;
  float angIzq = (angulo + 150) * PI / 180.0;
  float angDer = (angulo - 150) * PI / 180.0;

  int x_izq = x_punta + (int)(tamanoPunta * cos(angIzq));
  int y_izq = y_punta + (int)(tamanoPunta * sin(angIzq));
  int x_der = x_punta + (int)(tamanoPunta * cos(angDer));
  int y_der = y_punta + (int)(tamanoPunta * sin(angDer));

  u8g2.drawTriangle(x_punta, y_punta, x_izq, y_izq, x_der, y_der);
  u8g2.drawDisc(cx, cy, 2);
}
