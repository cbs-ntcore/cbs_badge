#include <avr/sleep.h>

#define LED1_PIN 1
#define LED2_PIN 3
#define LED3_PIN 4
#define TOUCH_PIN 2


#define THRESH_MV 70
#define REF_STEPS 1

unsigned long t = 0;

class Neuron {
  public:
    Neuron();
    float mv, syn_i;
    int ref;
    bool spike;
    void update();
};

Neuron::Neuron() {
  mv = 0;
  syn_i = 0;
  ref = 0;
  spike = false;
}

void Neuron::update() {
  spike = false;
  if (ref) {
    ref -= 1;
    mv = 0;
    return;
  }
  mv += syn_i;
  mv *= 0.99;
  if (mv > THRESH_MV) {
    spike = true;
    ref = REF_STEPS;
    mv = 0;
  }
  if (mv < 0) {
    mv = 0;
  }
  syn_i = 0;
}

uint8_t delta_lut[] = {
  50, 47, 45, 43, 40, 38, 37, 35, 33, 31, 30, 28, 27, 26, 24, 23, 22,
  21, 20, 19, 18, 17, 16, 15, 15, 14, 13, 12, 12, 11, 11, 10, 10,  9,
  9,  8,  8,  7,  7,  7,  6,  6,  6,  5,  5,  5,  5,  4,  4,  4,  4,
  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0};
uint8_t max_delta = sizeof(delta_lut) / sizeof(uint8_t);

class Synapse {
  public:
    Synapse(Neuron* presyn, Neuron* postsyn);
    float current;
    float max_current;
    unsigned long pre_st, post_st;
    void update();
  private:
    Neuron *pre;
    Neuron *post;
};

Synapse::Synapse(Neuron* presyn, Neuron* postsyn) {
  pre = presyn;
  post = postsyn;
  current = 0;
  max_current = 1;
  pre_st = 0;
  post_st = 0;
}

void Synapse::update() {
  if (pre->spike) {
    pre_st = t;
  }
  if (post->spike) {
    post_st = t;
  }
  if (pre_st && post_st) {
    int delta = post_st - pre_st;
    float mag;
    if (delta > 0) {
      if (delta < max_delta) {
        mag = delta_lut[delta];
        max_current += mag;
      }
    } else {
      if (-delta < max_delta) {
        mag = delta_lut[-delta];
        max_current -= mag * 0.1;
      }
    }
    pre_st = 0;
    post_st = 0;
  }
  if (pre->spike) {
    current = max_current;
  } else {
    current *= 0.9;
  }
  post->syn_i += current;
}

#define N_NEURONS 3
Neuron neurons[] = {Neuron(), Neuron(), Neuron()};
Synapse synapses[] = {
  Synapse(&(neurons[0]), &(neurons[1])),
  Synapse(&(neurons[1]), &(neurons[2])),
  Synapse(&(neurons[2]), &(neurons[0])),
};
int pins[] = {LED1_PIN, LED2_PIN, LED3_PIN};


int8_t randn_numbers[] = {
  -5,   3,  -9,  -5,   7,  -4,  -9,   3,  -5,   0,  -1,   4,   4,
   0,   2,  -5,   3,  10,  -9,  -1,  -7,  -3,  -1,  -3,   4,  -3,
   1,  -6,  -5,   0,  -1,  -1,  -3,   0,  -2,   6,  -1,  -1,   5,
   6,   0,   8,  -5,  12,  -1,   5,  -2,  -1,   0,  12,  -5,   2,
   3,  -3,   3,   6,   2,  -1,   1,  -2,   1,   0,  -6,   0,  -4,
  -10,  -2,  -3,  -1,   2,  -6,   0,   3,   5,   1,   5,   0,  -8,
   4,  -1,   0,   8,   3,  -3,   4,   0,   6,   1,   2,  -7,   0,
   0,  -5,   0,   3,   7,  -6,  -6,   4,  -8};
uint8_t n_randn_numbers = sizeof(randn_numbers) / sizeof(int8_t);


void reset_circuit() {
  t = 0;
  for (int i=0; i<N_NEURONS; i++) {
    neurons[i].spike = false;
    neurons[i].mv = 0;
    synapses[i].current = 0;
    synapses[i].max_current = 1;
  }
}

float bias = 0;
void run_circuit() {
  for (int i=0; i<N_NEURONS; i++) {
    synapses[i].update();
  }
  bias += 0.05;
  for (int i=0; i<N_NEURONS; i++) {
    Neuron *n = &(neurons[i]);
    // add some noise
    //n.syn_i = random(0, 50);
    //n.syn_i = random(0, 100);
    //n.syn_i += noise_distribution[random(0, n_noise_distribution)];
    n->syn_i += randn_numbers[random(0, n_randn_numbers)] + bias;
    //n->syn_i += randn_numbers[random(0, n_randn_numbers)];
    n->update();
    if (n->spike) {
      digitalWrite(pins[i], HIGH);
      bias = 0;
    } else {
      digitalWrite(pins[i], LOW);
    };
  }
  t += 1;
}
  

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
  reset_circuit();

  while (!digitalRead(TOUCH_PIN)) {
    run_circuit();
  }
}
