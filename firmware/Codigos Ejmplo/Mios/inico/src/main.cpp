#include <Arduino.h>
#include <U8g2lib.h>
#include <math.h>

#define OLED_SDA 5
#define OLED_SCL 6

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, OLED_SCL, OLED_SDA);

float angulo = 0;

const int cx = 36;
const int cy = 20;
const int radio = 17;

void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud);

void setup() {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.setContrast(255);
  
  Serial.println("\n=== BRUJULA CON FLECHA GRUESA ===");
}

void loop() {
  u8g2.clearBuffer();
  
  // CIRCULO EXTERIOR
  u8g2.drawCircle(cx, cy, radio);
  u8g2.drawCircle(cx, cy, radio - 1);
  
  // MARCAS N, S, E, O
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(cx - 3, cy - radio - 3, "N");
  u8g2.drawStr(cx - 3, cy + radio + 8, "S");
  u8g2.drawStr(cx + radio + 3, cy + 3, "E");
  u8g2.drawStr(cx - radio - 8, cy + 3, "O");
  
  // MARCAS CADA 30 grados
  for(int a = 0; a < 360; a += 30) {
    float rad = a * PI / 180.0;
    int x1 = cx + (int)((radio - 3) * cos(rad));
    int y1 = cy + (int)((radio - 3) * sin(rad));
    int x2 = cx + (int)((radio - 1) * cos(rad));
    int y2 = cy + (int)((radio - 1) * sin(rad));
    u8g2.drawLine(x1, y1, x2, y2);
  }
  
  // FLECHA GRUESA
  dibujarFlechaGruesa(cx, cy, angulo, radio - 4);
  
  // ANGULO
  u8g2.setFont(u8g2_font_4x6_tr);
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%d", (int)angulo);
  u8g2.drawStr(2, 6, buffer);
  
  u8g2.sendBuffer();
  
  Serial.printf("Angulo: %.0f\n", angulo);
  
  angulo += 2;
  if (angulo >= 360) angulo = 0;
  
  delay(30);
}

// ========================================
// FLECHA GRUESA ESTILO PIXEL/CURSOR
// ========================================
void dibujarFlechaGruesa(int cx, int cy, float angulo, int longitud) {
  float rad = angulo * PI / 180.0;
  
  // PUNTA (lado derecho)
  int x_punta = cx + (int)(longitud * cos(rad));
  int y_punta = cy + (int)(longitud * sin(rad));
  
  // COLA (lado izquierdo)
  int x_cola = cx - (int)(longitud * cos(rad));
  int y_cola = cy - (int)(longitud * sin(rad));
  
  // ========================================
  // LINEA CENTRAL GRUESA (3 pixeles de ancho)
  // ========================================
  // Calcular vector perpendicular para hacer la linea gruesa
  float perpRad = rad + PI/2;
  int dx = (int)(cos(perpRad));
  int dy = (int)(sin(perpRad));
  
  // Dibujar 3 lineas paralelas
  u8g2.drawLine(x_cola, y_cola, x_punta, y_punta);
  u8g2.drawLine(x_cola + dx, y_cola + dy, x_punta + dx, y_punta + dy);
  u8g2.drawLine(x_cola - dx, y_cola - dy, x_punta - dx, y_punta - dy);
  
  // ========================================
  // PUNTA DE LA FLECHA (triangulo grande)
  // ========================================
  int tamanoPunta = 6;
  
  float anguloIzq = (angulo + 150) * PI / 180.0;
  float anguloDer = (angulo - 150) * PI / 180.0;
  
  int x_izq = x_punta + (int)(tamanoPunta * cos(anguloIzq));
  int y_izq = y_punta + (int)(tamanoPunta * sin(anguloIzq));
  
  int x_der = x_punta + (int)(tamanoPunta * cos(anguloDer));
  int y_der = y_punta + (int)(tamanoPunta * sin(anguloDer));
  
  // Triangulo relleno (dibujando lineas horizontales)
  u8g2.drawTriangle(x_punta, y_punta, x_izq, y_izq, x_der, y_der);
  
  // Hacer la punta mas gruesa
  u8g2.drawLine(x_punta, y_punta, x_izq, y_izq);
  u8g2.drawLine(x_punta, y_punta, x_der, y_der);
  u8g2.drawLine(x_izq, y_izq, x_der, y_der);
  
  // ========================================
  // COLA DE LA FLECHA
  // ========================================
  int tamanoCola = 4;
  
  float colaIzq = (angulo + 180 + 45) * PI / 180.0;
  float colaDer = (angulo + 180 - 45) * PI / 180.0;
  
  int xc_izq = x_cola + (int)(tamanoCola * cos(colaIzq));
  int yc_izq = y_cola + (int)(tamanoCola * sin(colaIzq));
  int xc_der = x_cola + (int)(tamanoCola * cos(colaDer));
  int yc_der = y_cola + (int)(tamanoCola * sin(colaDer));
  
  // Lineas de la cola
  u8g2.drawLine(x_cola, y_cola, xc_izq, yc_izq);
  u8g2.drawLine(x_cola, y_cola, xc_der, yc_der);
  
  // Hacer cola mas gruesa
  u8g2.drawLine(x_cola + dx, y_cola + dy, xc_izq, yc_izq);
  u8g2.drawLine(x_cola - dx, y_cola - dy, xc_der, yc_der);
  
  // Punto central pequeno
  u8g2.drawPixel(cx, cy);
}