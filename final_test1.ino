// Button + Tilt Interaction Test
// Arduino Uno / Serial Monitor 9600 baud

const int BTN1 = 2;
const int BTN2 = 3;
const int BTN3 = 4;
const int TILT = 5;

int testMode = 0;

bool lastBtn1 = HIGH;
bool lastBtn2 = HIGH;
bool lastBtn3 = HIGH;
bool lastTilt = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(TILT, INPUT_PULLUP);

  randomSeed(analogRead(A0));

  Serial.println("=== 감정 조절 인터랙션 테스트 ===");
  Serial.println("1: 랜덤 순서대로 버튼 누르기");
  Serial.println("2: 버튼 연타해서 게이지 채우기");
  Serial.println("3: 버튼 꾹 눌러 게이지 채우기");
  Serial.println("4: 카운트 후 타이밍 맞춰 누르기");
  Serial.println("5: 틸트센서 흔들어 게이지 채우기");
  Serial.println();
}

void loop() {
  if (testMode == 0) testSequence();
  else if (testMode == 1) testMash();
  else if (testMode == 2) testHold();
  else if (testMode == 3) testTiming();
  else if (testMode == 4) testShake();
  else {
    Serial.println("모든 테스트 완료!");
    while (true);
  }
}

bool pressed(int pin, bool &lastState) {
  bool current = digitalRead(pin);

  if (lastState == HIGH && current == LOW) {
    delay(30);
    if (digitalRead(pin) == LOW) {
      lastState = current;
      return true;
    }
  }

  lastState = current;
  return false;
}

// 1. 랜덤 순서대로 버튼 누르기
void testSequence() {
  static int step = 0;
  static int sequence[5];
  static bool madeSequence = false;

  if (!madeSequence) {
    Serial.println("[TEST 1] 랜덤 순서대로 버튼 누르기");

    Serial.print("순서: ");
    for (int i = 0; i < 5; i++) {
      sequence[i] = random(1, 4); // 1~3
      Serial.print(sequence[i]);
      if (i < 4) Serial.print(" → ");
    }
    Serial.println();

    madeSequence = true;
    step = 0;
  }

  int input = 0;
  if (pressed(BTN1, lastBtn1)) input = 1;
  if (pressed(BTN2, lastBtn2)) input = 2;
  if (pressed(BTN3, lastBtn3)) input = 3;

  if (input != 0) {
    if (input == sequence[step]) {
      Serial.print("성공: ");
      Serial.println(input);
      step++;

      if (step >= 5) {
        Serial.println("순서 입력 성공!");
        Serial.println();
        madeSequence = false;
        testMode++;
        delay(1000);
      }
    } else {
      Serial.println("틀림! 새 순서로 다시 시작.");
      madeSequence = false;
      delay(500);
    }
  }
}

// 2. 버튼 연타해서 게이지 채우기
void testMash() {
  static int gauge = 0;
  static bool started = false;

  if (!started) {
    Serial.println("[TEST 2] 버튼 1을 연타해서 게이지 채우기");
    Serial.println("게이지가 천천히, 들쭉날쭉 찹니다.");
    started = true;
  }

  if (pressed(BTN1, lastBtn1)) {
    int addValue = random(2, 11); // 2~10
    gauge += addValue;
    gauge = constrain(gauge, 0, 100);

    Serial.print("+");
    Serial.print(addValue);
    Serial.print(" / 게이지: ");
    Serial.println(gauge);
  }

  if (gauge >= 100) {
    Serial.println("연타 성공!");
    Serial.println();
    gauge = 0;
    started = false;
    testMode++;
    delay(1000);
  }
}

// 3. 버튼 꾹 눌러 게이지 채우기
void testHold() {
  static int gauge = 0;
  static bool started = false;
  static unsigned long lastUpdate = 0;

  if (!started) {
    Serial.println("[TEST 3] 버튼 1을 꾹 눌러 게이지 채우기");
    started = true;
  }

  if (millis() - lastUpdate > 200) {
    lastUpdate = millis();

    if (digitalRead(BTN1) == LOW) {
      gauge += 8;
    } else {
      gauge -= 5;
    }

    gauge = constrain(gauge, 0, 100);

    Serial.print("게이지: ");
    Serial.println(gauge);
  }

  if (gauge >= 100) {
    Serial.println("꾹 누르기 성공!");
    Serial.println();
    gauge = 0;
    started = false;
    testMode++;
    delay(1000);
  }
}

// 4. 카운트 후 알맞은 타이밍에 버튼 누르기
void testTiming() {
  Serial.println("[TEST 4] 3, 2, 1 후에 버튼 1 누르기");
  delay(1000);
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  Serial.println("지금!");

  unsigned long startTime = millis();
  bool success = false;

  while (millis() - startTime < 1000) {
    if (pressed(BTN1, lastBtn1)) {
      success = true;
      break;
    }
  }

  if (success) {
    Serial.println("타이밍 성공!");
  } else {
    Serial.println("실패! 너무 늦음.");
  }

  Serial.println();
  testMode++;
  delay(1000);
}

// 5. 틸트센서 흔들어 게이지 채우기
void testShake() {
  static int gauge = 0;
  static bool started = false;
  static unsigned long lastShakeTime = 0;
  static unsigned long lastDrainTime = 0;

  if (!started) {
    Serial.println("[TEST 5] 틸트센서를 흔들어 게이지 채우기");
    Serial.println("계속 흔들어야 천천히 게이지가 찹니다.");
    started = true;
  }

  bool currentTilt = digitalRead(TILT);

  // 흔들림 감지
  if (currentTilt != lastTilt &&
      millis() - lastShakeTime > 300) {

    lastShakeTime = millis();
    lastTilt = currentTilt;

    int addValue = random(1,11); // +1~10 증가
    gauge += addValue;

    gauge = constrain(gauge,0,100);

    Serial.print("흔들림! +");
    Serial.print(addValue);
    Serial.print(" / 게이지: ");
    Serial.println(gauge);
  }

  // 가만히 있으면 게이지 조금 감소
  if (millis() - lastDrainTime > 500) {

    lastDrainTime = millis();

    gauge -= 1;
    gauge = constrain(gauge,0,100);

    Serial.print("현재 게이지: ");
    Serial.println(gauge);
  }

  if (gauge >= 100) {
    Serial.println("흔들기 성공!");
    Serial.println();

    gauge = 0;
    started = false;
    testMode++;

    delay(1000);
  }
}