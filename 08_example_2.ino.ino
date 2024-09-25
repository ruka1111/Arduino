// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25      // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float distance;

  // wait until next sampling time. 
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // distance에 따라 LED 밝기 제어
  int led_brightness = calculateLEDBrightness(distance); // 밝기 계산 함수 호출

  // LED 밝기 제어
  analogWrite(PIN_LED, led_brightness);

  // output the distance and LED brightness to the serial port
  Serial.print("Min:");        Serial.print(_DIST_MIN);
  Serial.print(",distance:");  Serial.print(distance);
  Serial.print(",Max:");       Serial.print(_DIST_MAX);
  Serial.print(",LED brightness:");  Serial.println(led_brightness);

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}

// 장애물 거리에 따른 LED 밝기 계산
int calculateLEDBrightness(float distance) {
  // 범위 초과 처리
  if (distance < _DIST_MIN) distance = _DIST_MIN;
  if (distance > _DIST_MAX) distance = _DIST_MAX;

  float brightness;
  if (distance <= 150.0) {
    // 100mm ~ 150mm: 밝기 비례 조정 (밝기 감소)
    brightness = map(distance, 100, 150, 255, 127);
  } else if (distance <= 200.0) {
    // 150mm ~ 200mm: 밝기 50% 유지
    brightness = 127;
  } else if (distance <= 250.0) {
    // 200mm ~ 250mm: 밝기 비례 조정 (밝기 증가)
    brightness = map(distance, 200, 250, 127, 255);
  } else {
    // 250mm ~ 300mm: 밝기 비례 조정 (밝기 감소)
    brightness = map(distance, 250, 300, 127, 255);
  }

  return (int)brightness;
}
