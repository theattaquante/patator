#include <Servo.h>
#include <RH_ASK.h>
#include <SPI.h>

// Objets Servo
Servo servoX;
Servo servoY1;
Servo servoY2;

// Pins
const int servoPinX = 9;
const int servoPinY1 = 10;
const int servoPinY2 = 11;
const int pinLed = 3;

// État
int angleX = 90;
int angleY = 90;
int etatButton = HIGH;

// Récepteur RF
RH_ASK rf_driver(2000, 4, -1);  // Baud, RX pin, TX désactivé

void setup() {
  servoX.attach(servoPinX);
  servoY1.attach(servoPinY1);
  servoY2.attach(servoPinY2);

  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);

  servoX.write(angleX);
  servoY1.write(angleY);
  servoY2.write(180 - angleY);

  Serial.begin(9600);

  if (!rf_driver.init()) {
    Serial.println("Erreur : RF non initialisé !");
    while (1);
  }

  Serial.println("Récepteur RF prêt");
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';

    Serial.print("Reçu : ");
    Serial.println((char *)buf);

    int btn = 1, x = 90, y = 90;

    if (sscanf((char *)buf, "X:%d,Y:%d,B:%d", &x, &y, &btn) == 3) {
      angleX = constrain(x, 0, 180);
      angleY = constrain(y, 0, 180);
      etatButton = btn;

      if (etatButton == LOW) {
        digitalWrite(pinLed, HIGH);
        servoX.write(90);
        servoY1.write(90);
        servoY2.write(90);
      } else {
        digitalWrite(pinLed, LOW);
        servoX.write(angleX);
        servoY1.write(angleY);
        servoY2.write(180 - angleY);
      }

      Serial.print("angleX: "); Serial.print(angleX);
      Serial.print(" | angleY: "); Serial.print(angleY);
      Serial.print(" | bouton: "); Serial.println(etatButton);
    } else {
      Serial.println("Trame invalide");
    }
  }
}
