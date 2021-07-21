/*
     -
    / \    Neural Network CBS NTCore Badge firmware
---/   \  ---------------------------------------------
        \/

This code is a demonstration of the badge features.

'Press' the two neurons at the bottom of the badge to wake it up (if you have
dry hands you may need to wet your finger).

When the badge wakes up, it will begin simulation of a simple neural network
consisting of 3 neurons connected with 3 synapses. The network starts with
low synaptic weights and low overall bias producing a low spontaneous firing
rate. When neurons spikes the strengths of connected synapses are updated
based on a simple asymmetric spike time dependent plasticity. The asymmetry
and lack of inhibition encourages the network to settle into a state of
continuous high firing rate after a short time. To encourage transition to
this state, global network bias is increased in the absense of firing.

The below code contains comments intended to introduce some features of
microcontroller programming and should hopefully serve as teasers for
the utility and productivity that can be gained by learning how to use
the Arduino ecosystem.

This firmware is designed to run on the ATTiny85 installed on the badge.
It is written in C++ and uses the Arduino ecosystem for rapid code development.
To build the code, install the Arduino IDE and the ATTiny 'core' found here:

https://github.com/damellis/attiny

By following the installation instructions in the link above you can add
support for programming ATTiny microcontrollers using the Arduino IDE.

To program the badge you will need a AVR programmer like this one:

https://www.sparkfun.com/products/9825

Open this file in the Arduino IDE, select 'ATTiny25/45/85' in the Tools->Board
menu and 'USBTinyISP' in the Tools->Programmer menu. Next, connect the ICSP
header and select 'Upload Using Programmer' in the Sketch menu to compile and
upload the code.
*/


// Include code located within the installed 'core' used to put the attiny
// to 'sleep', a low power mode where only minimal features are active.

#include <avr/sleep.h>


// Define, or give readable names to pins that are connected to lights and
// the touch 'button'. These defines allow the below code to use 'LED1_PIN'
// instead of the less readable '1' when referring to the pin connected to the
// first light.

#define LED1_PIN 1
#define LED2_PIN 3
#define LED3_PIN 4
#define TOUCH_PIN 2


// Defines can also be used to set 'constants' such as the threshold for neuron
// activation and the number of timesteps for the refractory period.

#define THRESH_MV 70
#define REFRACTORY_TIMESTEPS 1


// Declare some global variables that will be available everywhere in the code
// below.

unsigned long simulation_timestep = 0;
float bias = 0;


// Lookup tables (luts) are used to store pre-computed values that are
// frequently used or take too much time to compute. The exponential function
// of the learning rule is one example precomputed below.

uint8_t delta_lut[] = {
  50, 47, 45, 43, 40, 38, 37, 35, 33, 31, 30, 28, 27, 26, 24, 23, 22,
  21, 20, 19, 18, 17, 16, 15, 15, 14, 13, 12, 12, 11, 11, 10, 10,  9,
  9,  8,  8,  7,  7,  7,  6,  6,  6,  5,  5,  5,  5,  4,  4,  4,  4,
  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,  2,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0};
uint8_t max_delta = sizeof(delta_lut) / sizeof(uint8_t);


// Lookup table of normally distributed 'random' numbers

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


// C++ is object oriented and supports classes.
// This 'Neuron' class contains variables and functions associated with
// simulating the neuron portion of the network. After defining a class,
// instances of that class can be created that have separate variables
// called attributes (such as 'mv' for membrane voltage) and functions
// called methods (such as 'update').

class Neuron {
  public:
    Neuron();
    float mv, syn_i;
    int ref;
    bool spike;
    void update();
};


// The constructor is called when a class instance is created and
// should be used to set attributes to starting values.

Neuron::Neuron() {
  mv = 0;
  syn_i = 0;
  ref = 0;
  spike = false;
}


// Class methods have access to attributes of the class (mv refers to the
// membrane voltage of the class instance on which 'update' was called).

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
    ref = REFRACTORY_TIMESTEPS;
    mv = 0;
  }
  if (mv < 0) {
    mv = 0;
  }
  syn_i = 0;
}


// Define a 'Synapse' class with pointers (references) to pre and post
// synaptic neuron class instances.

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
    pre_st = simulation_timestep;
  }
  if (post->spike) {
    post_st = simulation_timestep;
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


// Instantiate 3 neurons and synapses connecting them in a ring.

#define N_NEURONS 3
Neuron neurons[] = {Neuron(), Neuron(), Neuron()};
Synapse synapses[] = {
  Synapse(&(neurons[0]), &(neurons[1])),
  Synapse(&(neurons[1]), &(neurons[2])),
  Synapse(&(neurons[2]), &(neurons[0])),
};


// Declare an array of pins used to map neurons to lights.

int pins[] = {LED1_PIN, LED2_PIN, LED3_PIN};


// Define a function to reset the circuit.

void reset_circuit() {
  simulation_timestep = 0;
  for (int i=0; i<N_NEURONS; i++) {
    neurons[i].spike = false;
    neurons[i].mv = 0;
    synapses[i].current = 0;
    synapses[i].max_current = 1;
  }
}


// Define a function to run the circuit for a single timestep.

void run_circuit() {
  // first update all synapses so they can use spikes from the previous
  // timestep to update synaptic weights and inject current into post-synaptic
  // neurons
  for (int i=0; i<N_NEURONS; i++) {
    synapses[i].update();
  }

  // increase the global bias of the network (this is reset below on any spike)
  bias += 0.05;

  // update all the neurons
  for (int i=0; i<N_NEURONS; i++) {
    Neuron *n = &(neurons[i]);

    // add some random noise and bias currents
    n->syn_i += randn_numbers[random(0, n_randn_numbers)] + bias;

    n->update();

    // if a neuron spiked, turn on the corresponding light
    if (n->spike) {
      digitalWrite(pins[i], HIGH);
      bias = 0;
    } else {
      digitalWrite(pins[i], LOW);
    };
  }
  simulation_timestep += 1;
}


// Define a special function a 'interrupt service routine' or ISR that runs
// when a the touch 'button' is pressed. This interrupt will be triggered
// even when the microcontroller is sleeping.

ISR(INT0_vect) {
  bitClear(GIMSK, INT0);  // Disable INT0 interrupt
  sleep_disable();
  // U+1F480 base64:aHR0cHM6Ly9za3VsbGN0Zi5jb20v
}


// Before putting the microcontroller to sleep, turn off lights and disable
// some peripherals.

void sleep() {
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);

  // Disable ADC - saves about 324.5uA in sleep mode!
  ADCSRA = 0;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // Before fully going to sleep (sleeping the cpu) enable the INT0 interrupt
  // so the ISR will be triggered when the touch 'button' is pressed
  noInterrupts();
  bitSet(GIMSK, INT0);  // Enable INT0 interrupt
  interrupts();
  sleep_cpu();
}


// Arduino uses 2 special functions, 'setup' and 'loop'.
// setup is run once when the microcontroller is powered on.

void setup() {
  // most pins support multiple functions including digital input and output.
  // Set all pins connected to lights as digital outputs.
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);

  // Set the state of the light pins to 'LOW' which pulls the pin to ground
  // disabling the light.
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(LED3_PIN, LOW);
  
  // This is an example of lower level access to microcontroller functions
  // which are often abstracted away using the Arduino ecosystem. In this case
  // fully disable the analog to digital converter (to save power) by writing
  // directly to a special memory address (register) defined in the core as
  // ADCSRA (ADC status and control register A)
  ADCSRA = 0;
}


// loop is run after setup and repeats until power is removed.

void loop() {
  // put the microcontroller to sleep, this function will return when sleep
  // is ended because of a touch 'button' press
  sleep();

  // first reset the circuit to a default, low activity state
  reset_circuit();

  // run the circuit for as long as the touch 'button' is pressed
  while (!digitalRead(TOUCH_PIN)) {
    run_circuit();
  }
}
