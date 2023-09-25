#include <Arduino.h>
#include <ModbusMaster.h>

#define RE GPIO_NUM_10 
#define DE GPIO_NUM_11       
#define MODBUS_RX_PIN GPIO_NUM_8 
#define MODBUS_TX_PIN GPIO_NUM_9 
#define MODBUS_SERIAL_BAUD 4800 // Baud rate for esp32 and max485 communication

ModbusMaster node;

#define sensorID                    0x01
#define startAddress 0x00
#define totalCoils 64

uint16_t oldResponse[totalCoils];

struct Medidas {
  float temperatura;  // Temperatura en grados Celsius
  float humedad;      // Humedad relativa en porcentaje
  float conductividad;           // Conductividad eléctrica en microsiemens por centímetro (uS/cm)
};

Medidas medidas;

// Pin 4 made high for Modbus transmision mode
void modbusPreTransmission()
{
  
  digitalWrite(RE, HIGH);
  digitalWrite(DE, HIGH);
  delay(50);
}
// Pin 4 made low for Modbus receive mode
void modbusPostTransmission()
{
  digitalWrite(RE, LOW);
  digitalWrite(DE, LOW);
  //delay(5);

}


void setup() {
  // Debug console
  
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  modbusPostTransmission();

  Serial.begin(115200);
  Serial1.begin(MODBUS_SERIAL_BAUD, SERIAL_8N1, MODBUS_RX_PIN, MODBUS_TX_PIN);
  node.begin(sensorID, Serial1);

  node.postTransmission(modbusPostTransmission);
  node.preTransmission(modbusPreTransmission);

  Serial.println();
  Serial.println("Agriculture Node Ready");
  //Serial.println("Soil Temperature, Humidity, EC, pH and NPK Sensors\n");

  delay(1000);
}

void loop() {
  uint8_t result;
  int i;
  float reading;
  //Modbus function 0x03 Read HR
  result = node.readHoldingRegisters(startAddress, totalCoils);

  if (result == node.ku8MBSuccess) {
    Serial.println("Success, Received data: ");
    for (int i=0; i<totalCoils; i++){
      if (node.getResponseBuffer(i) != oldResponse[i]){
        Serial.print("Respuesta "); Serial.print(i); Serial.print(" : "); Serial.println(node.getResponseBuffer(i));
      }
      oldResponse[i] = node.getResponseBuffer(i);
    }

    medidas.humedad = node.getResponseBuffer(0)*0.1;
    medidas.temperatura = node.getResponseBuffer(1)*0.1;
    medidas.conductividad = node.getResponseBuffer(2);

    Serial.println("*********************");
    Serial.print("Humedad: "); Serial.print(medidas.humedad); Serial.println(" %");
    Serial.print("Temperatura: "); Serial.print(medidas.temperatura); Serial.println(" oC");
    Serial.println("*********************");
    Serial.println("");

  }else {
    Serial.print("Failed, Response Code: ");
    Serial.print(result, HEX);
    Serial.println("");
  }
    

  delay(30000);


}

