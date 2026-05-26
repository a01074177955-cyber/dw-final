// 오늘도참는다 VN — 게임 컨트롤러
// 핀: BTN1=2(피에조 대체), TILT=5(기울기센서), PIEZO=A1(압전 아날로그)
//
// 시리얼 프로토콜 (9600 baud):
//   READY   — 시작 완료
//   PIEZO   — 피에조/버튼 눌림 (분출)
//   SHAKE   — 기울기센서 감지 (참기)

const int BTN1  = 2;
const int TILT  = 5;
const int PIEZO_PIN = A1;   // 압전 아날로그 (없으면 BTN1로 대체)
const int PIEZO_THRESH = 200;

bool lastBtn1 = HIGH;
bool lastTilt = HIGH;
unsigned long lastTiltTime  = 0;
unsigned long lastPiezoTime = 0;
const unsigned long TILT_DEBOUNCE  = 200;
const unsigned long PIEZO_DEBOUNCE = 400;

void setup() {
  Serial.begin(9600);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(TILT, INPUT_PULLUP);
  Serial.println("READY");
}

void loop() {
  unsigned long now = millis();

  // ── 기울기센서 — SHAKE ──
  bool tilt = digitalRead(TILT);
  if (tilt != lastTilt && now - lastTiltTime > TILT_DEBOUNCE) {
    lastTiltTime = now;
    lastTilt = tilt;
    Serial.println("SHAKE");
  }

  // ── 압전 아날로그 — PIEZO ──
  int piezoVal = analogRead(PIEZO_PIN);
  if (piezoVal > PIEZO_THRESH && now - lastPiezoTime > PIEZO_DEBOUNCE) {
    lastPiezoTime = now;
    Serial.println("PIEZO");
  }

  // ── BTN1 (피에조 없을 때 대체) — PIEZO ──
  bool b1 = (digitalRead(BTN1) == LOW);
  if (lastBtn1 == HIGH && b1 && now - lastPiezoTime > PIEZO_DEBOUNCE) {
    lastPiezoTime = now;
    Serial.println("PIEZO");
  }
  lastBtn1 = b1 ? LOW : HIGH;

  delay(10);
}
