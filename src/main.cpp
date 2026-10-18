#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int MQ135_PIN = 34;
const int RED_LED = 25;
const int YELLOW_LED = 26;
const int GREEN_LED = 27;
const int BLUE_LED = 14;

const int THRESHOLD_GOOD = 1500;
const int THRESHOLD_BAD = 2500;
const int THRESHOLD_CRITICAL = 3500;

unsigned long previousMillis = 0;
const long interval = 2000;
bool blueLedState = false;

int readSensor() {
  long sum = 0;
  for(int i = 0; i < 10; i++) {
    sum += analogRead(MQ135_PIN);
    delay(10);
  }
  return sum / 10;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(MQ135_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.setTextColor(WHITE);
  display.println("    Hi!");
  display.display();
  delay(3000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.setTextColor(WHITE);
  display.println("** Checking LED's **");
  display.display();
  delay(2000);
  
  digitalWrite(RED_LED, HIGH); delay(1000); digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH); delay(1000); digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH); delay(1000); digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH); delay(1000); digitalWrite(BLUE_LED, LOW);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  for (int i = 30; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Warming up my sensor");
    display.setCursor(0, 20);
    display.print("Please wait: ");
    display.print(i);
    display.print(" sec");
    display.display();
    delay(1000);
  }
  
  display.clearDisplay();
  display.setCursor(0, 25);
  display.setTextSize(1);
  display.println("    System Ready!");
  display.display();
  delay(1000);
}

void loop() {

  int sensorValue = readSensor(); 

  if (sensorValue < THRESHOLD_GOOD) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);
  } 
  else if (sensorValue >= THRESHOLD_GOOD && sensorValue < THRESHOLD_BAD) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  } 
  else {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    
    digitalWrite(RED_LED, HIGH);
    delay(200);
    digitalWrite(RED_LED, LOW);
    delay(200);
    digitalWrite(RED_LED, HIGH);
  }

  unsigned long currentMillis = millis();

  if (sensorValue >= THRESHOLD_CRITICAL) {

    if (currentMillis - previousMillis >= 100) {
      previousMillis = currentMillis;
      blueLedState = !blueLedState; 
      digitalWrite(BLUE_LED, blueLedState);
    }
  } 
  else {

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      blueLedState = !blueLedState; 
      digitalWrite(BLUE_LED, blueLedState);
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(" *** Air Quality ***");
  display.setCursor(0, 20);
  display.print("Sensor MQ-135: ");
  display.println(sensorValue);
  
  display.setCursor(0, 40);
  if (sensorValue < THRESHOLD_GOOD) {
    display.println("Air status: GOOD");
  } else if (sensorValue >= THRESHOLD_GOOD && sensorValue < THRESHOLD_BAD) {
    display.println("Air status: MODERATE");
  } else {
    display.println("Air status: BAD");
  }

  display.drawRect(14, 52, 100, 10, WHITE);
  int barWidth = constrain(map(sensorValue, 800, 3000, 0, 100), 0, 100);
  display.fillRect(14, 52, barWidth, 10, WHITE);
  
  display.display();

  delay(1000); 
}