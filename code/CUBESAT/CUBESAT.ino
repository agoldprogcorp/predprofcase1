#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// ==== nRF24 адреса ====
const byte ADDR_CMD [6] = "SAT01";  // команда: станция -> кубсат
const byte ADDR_TELE[6] = "GS001";  // телеметрия: кубсат -> станция

RF24 radio(9, 10);  // CE=9, CSN=10

// ==== Лазер ====
const int LASER_PIN = 2;

// ==== Шаговые моторы (28BYJ-48 + ULN2003) ====
const int yawPins[4]   = {3, 4, 5, 6};    // горизонтальная ось
const int pitchPins[4] = {7, 8, A0, A1};  // вертикальная ось

// 2048 шагов на 360° → ~5.7 шага на градус [web:56]
const float STEPS_PER_DEG = 2048.0 / 360.0;

// Текущее положение (в градусах)
int angleH = 0;   // горизонт (поворот)
int angleV = 0;   // вертикаль (наклон)

// Фаза последовательности (0..7) для каждого мотора
int phaseYaw   = 0;
int phasePitch = 0;

// Режимы по ТЗ [file:1]
enum ScanMode { MODE_IDLE = 0, MODE_H = 1, MODE_V = 2, MODE_D1 = 3, MODE_D2 = 4 };
ScanMode mode = MODE_IDLE;

bool scanning = false;         // сейчас выполняется цикл сканирования
int  stepIndex = 0;            // шаг внутри текущего режима (0..8)
unsigned long lastStepTime = 0;

// ==== Таблица шаговой последовательности (полный шаг) ====
const uint8_t STEP_SEQ[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

// ---------- НИЗКОУРОВНЕВОЕ УПРАВЛЕНИЕ МОТОРАМИ ----------

void stepYaw(int dir) { // dir = +1 или -1
  phaseYaw += dir;
  if (phaseYaw > 7) phaseYaw = 0;
  if (phaseYaw < 0) phaseYaw = 7;
  for (int i = 0; i < 4; i++) {
    digitalWrite(yawPins[i], STEP_SEQ[phaseYaw][i]);
  }
}

void stepPitch(int dir) {
  phasePitch += dir;
  if (phasePitch > 7) phasePitch = 0;
  if (phasePitch < 0) phasePitch = 7;
  for (int i = 0; i < 4; i++) {
    digitalWrite(pitchPins[i], STEP_SEQ[phasePitch][i]);
  }
}

void moveMotorToAngle(int targetDeg, int &currentDeg, bool yawMotor) {
  int deltaDeg = targetDeg - currentDeg;
  if (deltaDeg == 0) return;

  int steps = (int)round(abs(deltaDeg) * STEPS_PER_DEG);
  int dir = (deltaDeg > 0) ? +1 : -1;

  for (int i = 0; i < steps; i++) {
    if (yawMotor) stepYaw(dir);
    else          stepPitch(dir);
    delay(3);          // скорость вращения
  }

  // Отключаем катушки для экономии
  int const* pins = yawMotor ? yawPins : pitchPins;
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], LOW);
  }

  currentDeg = targetDeg;
}

// Переместить сразу оба угла
void setAngles(int targetH, int targetV) {
  moveMotorToAngle(targetH, angleH, true);
  moveMotorToAngle(targetV, angleV, false);
}

// ---------- СКАНИРОВАНИЕ ПО ТЗ ----------
// Возвращает true, если есть следующий шаг в этом режиме
bool getTargetForStep(ScanMode m, int idx, int &h, int &v) {
  // Все режимы используют индексы 0..8 (углы -40..40 с шагом 10)
  if (idx < 0 || idx > 8) return false;

  int base = -40 + idx * 10;

  switch (m) {
    case MODE_H:
      // Горизонтальный скан по ТЗ: фиксируем горизонталь 0°, меняем вертикаль -40..+40 [file:1]
      h = base;
      v = 0;
      return true;

    case MODE_V:
      // Вертикальный скан: фиксируем вертикаль 0°, меняем горизонталь -40..+40 [file:1]
      h = 0;
      v = base;
      return true;

    case MODE_D1:
      // Диагональ 1: (-40,-40) → (40,40)
      h = base;
      v = base;
      return true;

    case MODE_D2:
      // Диагональ 2: (-40,40) → (40,-40)
      h = base;
      v = 40 - idx * 10;
      return true;

    default:
      return false;
  }
}

// ---------- РАДИО И ТЕЛЕМЕТРИЯ ----------

void sendTelemetry() {
  // Формат: ID,горизонт,вертикаль,режим
  char buf[32];
  snprintf(buf, sizeof(buf), "SAT01,%d,%d,%d", angleH, angleV, (int)mode);

  radio.stopListening();
  radio.openWritingPipe(ADDR_TELE);
  radio.write(buf, strlen(buf) + 1);
  radio.startListening();

  Serial.print("TX: ");
  Serial.println(buf);
}

void handleCommand(const char *cmd) {
  Serial.print("RX CMD: ");
  Serial.println(cmd);

  if (strcmp(cmd, "START") == 0 && !scanning) {
    // Запуск цикла сканирования
    scanning = true;
    mode = MODE_H;
    stepIndex = 0;

    // Первый шаг — сразу (требование <=2 секунд до движения) [file:1]
    int h, v;
    if (getTargetForStep(mode, stepIndex, h, v)) {
      setAngles(h, v);
      sendTelemetry();
      stepIndex++;
      lastStepTime = millis();
    }
  } else if (strcmp(cmd, "STOP") == 0) {
    // Аварийный стоп
    scanning = false;
    mode = MODE_IDLE;
    setAngles(0, 0);
    sendTelemetry();
  } else if (strcmp(cmd, "LASER_ON") == 0) {
    digitalWrite(LASER_PIN, HIGH);
  } else if (strcmp(cmd, "LASER_OFF") == 0) {
    digitalWrite(LASER_PIN, LOW);
  }
}

void receiveRadio() {
  if (!radio.available()) return;

  char buf[32];
  radio.read(&buf, sizeof(buf));
  buf[31] = '\0';
  handleCommand(buf);
}

// Один шаг сканирования каждые 3 секунды
void updateScanning() {
  if (!scanning) return;

  if (millis() - lastStepTime < 3000) return;

  int h, v;
  if (getTargetForStep(mode, stepIndex, h, v)) {
    setAngles(h, v);
    sendTelemetry();
    stepIndex++;
    lastStepTime = millis();
  } else {
    // Режим завершён — переключаемся на следующий
    switch (mode) {
      case MODE_H:  mode = MODE_V;  break;
      case MODE_V:  mode = MODE_D1; break;
      case MODE_D1: mode = MODE_D2; break;
      case MODE_D2:
      default:
        // Все 4 режима пройдены — возврат в (0,0) и ожидание новой команды
        scanning = false;
        mode = MODE_IDLE;
        setAngles(0, 0);
        sendTelemetry();
        break;
    }
    stepIndex = 0;
  }
}

// ---------- setup / loop ----------

void setup() {
  Serial.begin(115200);

  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);

  for (int i = 0; i < 4; i++) {
    pinMode(yawPins[i], OUTPUT);
    pinMode(pitchPins[i], OUTPUT);
    digitalWrite(yawPins[i], LOW);
    digitalWrite(pitchPins[i], LOW);
  }

  // Радио
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(90);
  radio.setAutoAck(true);

  radio.openReadingPipe(1, ADDR_CMD);   // слушаем команды
  radio.startListening();

  Serial.println("CubeSat: готов, жду команду START по радио");
  setAngles(0, 0);   // начальное состояние (0°,0°) [file:1]
  sendTelemetry();
}

void loop() {
  receiveRadio();    // приём команд
  updateScanning();  // выполнение сканирования
}
