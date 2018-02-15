
#define GPIO_PIN_NUM  (0)


void setup(void) {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(GPIO_PIN_NUM, OUTPUT);
}

void loop(void) {
  digitalWrite(GPIO_PIN_NUM, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(GPIO_PIN_NUM, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
