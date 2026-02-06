#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Те же адреса, что и у кубсата
const byte ADDR_CMD [6] = "SAT01";  // писать сюда (команда для кубсата)
const byte ADDR_TELE[6] = "GS001";  // слушать отсюда (телеметрия)

RF24 radio(9, 10);  // CE=9, CSN=10

void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(90);
  radio.setAutoAck(true);

  // Приём телеметрии
  radio.openReadingPipe(1, ADDR_TELE);
  radio.startListening();

  Serial.println("Станция: команды через Serial:");
  Serial.println("  START     - запустить цикл сканирования");
  Serial.println("  STOP      - аварийный стоп (возврат в 0,0)");
  Serial.println("  LASER_ON  - включить лазер");
  Serial.println("  LASER_OFF - выключить лазер");
  Serial.println();
}

void sendCommand(const char *cmd) {
  radio.stopListening();               // переходим в режим передачи
  radio.openWritingPipe(ADDR_CMD);    // писаем на кубсат

  bool ok = radio.write(cmd, strlen(cmd) + 1);

  Serial.print("TX CMD: ");
  Serial.print(cmd);
  Serial.println(ok ? " [OK]" : " [FAIL]");

  radio.startListening();              // обратно слушаем телеметрию
}

void handleSerialInput() {
  if (!Serial.available()) return;

  String s = Serial.readStringUntil('\n');
  s.trim();
  s.toUpperCase();

  if (s == "START") {
    sendCommand("START");
  } else if (s == "STOP") {
    sendCommand("STOP");
  } else if (s == "LASER_ON") {
    sendCommand("LASER_ON");
  } else if (s == "LASER_OFF") {
    sendCommand("LASER_OFF");
  } else if (s.length() > 0) {
    Serial.println("Неизвестная команда");
  }
}

void handleTelemetry() {
  if (!radio.available()) return;

  char buf[32];
  radio.read(&buf, sizeof(buf));
  buf[31] = '\0';

  Serial.print("RX TELE: ");
  Serial.println(buf);
}

void loop() {
  handleSerialInput();
  handleTelemetry();
}
