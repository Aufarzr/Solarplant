#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int pinWaterLevel = A0; // Pin Arduino yang terhubung ke sensor level air
const int pinFlowmeter = 2;    // Pin Arduino yang terhubung ke flowmeter
const int pinPump = 7;         // Pin Arduino yang terhubung ke relay untuk mengontrol pompa

volatile int flowPulses = 0;
float flowRate = 0.0;
unsigned int flowMilliLitres = 0;
unsigned long totalMilliLitres = 0;
unsigned long oldTime = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the I2C address to 0x27 (you may need to adjust this)

void setup() {
  pinMode(pinWaterLevel, INPUT); // Mengatur pin sensor level air sebagai INPUT
  pinMode(pinFlowmeter, INPUT);  // Mengatur pin flowmeter sebagai INPUT
  pinMode(pinPump, OUTPUT);      // Mengatur pin relay untuk pompa sebagai OUTPUT

  attachInterrupt(digitalPinToInterrupt(pinFlowmeter), pulseCounter, FALLING);

  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();
}

void loop() {
  int waterLevel = analogRead(pinWaterLevel); // Membaca nilai level air dari sensor

  // Mengecek jika level air di bawah 300
  if (waterLevel < 70) {
    digitalWrite(pinPump, HIGH); // Mengaktifkan pompa air
  } else {
    digitalWrite(pinPump, LOW);  // Mematikan pompa air
  }

  updateFlow(); // Update flowmeter data
  displayData(); // Display data on LCD

  delay(500); // Menunggu 1 detik sebelum membaca ulang level air dan data flowmeter
}

void pulseCounter() {
  flowPulses++;
}

void updateFlow() {
  unsigned long currentTime = millis();

  if (currentTime - oldTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(pinFlowmeter));

    flowRate = ((1000.0 / (currentTime - oldTime)) * flowPulses) / 7.5; // Flow rate in L/min
    flowMilliLitres = (flowPulses * 5.5); // Flow in mL
    totalMilliLitres += flowMilliLitres;

    flowPulses = 0;
    oldTime = currentTime;

    attachInterrupt(digitalPinToInterrupt(pinFlowmeter), pulseCounter, FALLING);
  }
}

void displayData() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(analogRead(pinWaterLevel));

  lcd.setCursor(0, 1);
  lcd.print("Flow: ");
  lcd.print(flowRate);
  lcd.print(" L/min");
}
