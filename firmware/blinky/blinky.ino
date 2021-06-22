#include <avr/sleep.h>

#define LED1_PIN 1
#define LED2_PIN 3
#define LED3_PIN 4
#define TOUCH_PIN 2

#define DELAY_MS 100


ISR(INT0_vect) {
  bitClear(GIMSK, INT0);  // Disable INT0 interrupt
  sleep_disable();
}

void sleep() {
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  // Disable ADC - saves about 324.5uA in sleep mode!
  ADCSRA = 0;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  noInterrupts();
  bitSet(GIMSK, INT0);  // Enable INT0 interrupt
  interrupts();
  sleep_cpu();

  // Restore ADC
  //ADCSRA = oldAdc;
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  
  // Disable ADC - saves about 324.5uA in sleep mode!
  ADCSRA = 0;
}

void loop() {
  sleep();

  while (!digitalRead(TOUCH_PIN)) {
    // blinky!
    digitalWrite(LED1_PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(LED3_PIN, LOW);
    delay(DELAY_MS);
    digitalWrite(LED2_PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(LED1_PIN, LOW);
    delay(DELAY_MS);
    digitalWrite(LED3_PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(LED2_PIN, LOW);
    delay(DELAY_MS);
  }
}
