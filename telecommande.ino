#include <RH_ASK.h>
#include <SPI.h>  // Nécessaire pour RadioHead

// Broches joystick
const int pinJoystickX = 0;       // GPIO0 (ADC1)
const int pinJoystickY = 1;       // GPIO1 (ADC2)
const int pinJoystickButton = 2;  // GPIO2 (bouton)

const int centre = 2048;          // Centre pour ADC 12 bits
const int seuil = 100;

int angleX = 90;
int angleY = 90;

// Init driver RF (TX sur GPIO5 par défaut ici)
RH_ASK rf_driver(2000, -1, 5); // Baudrate, RX, TX (TX sur GPIO5)

void setup() {
  Serial.begin(115200);
  pinMode(pinJoystickButton, INPUT_PULLUP);

  if (!rf_driver.init()) {
    Serial.println("Erreur : RF non initialisé !");
    while (1);
  }

  Serial.println("Émetteur RF prêt");
}

void loop() {
  int valX = analogRead(pinJoystickX);
  int valY = analogRead(pinJoystickY);
  int bouton = digitalRead(pinJoystickButton);

  // Déplacements convertis en angle
  if (valX > centre + seuil && angleX < 180) angleX++;
  else if (valX < centre - seuil && angleX > 0) angleX--;

  if (valY > centre + seuil && angleY > 0) angleY--;
  else if (valY < centre - seuil && angleY < 180) angleY++;

  // Format d'envoi
  char message[32];
  snprintf(message, sizeof(message), "X:%d,Y:%d,B:%d", angleX, angleY, bouton);

  // Envoi RF
  rf_driver.send((uint8_t *)message, strlen(message));
  rf_driver.waitPacketSent();

  Serial.println(message);
  delay(50);
}
