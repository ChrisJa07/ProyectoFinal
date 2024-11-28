#include <WiFi.h>
#include <PubSubClient.h>

// Configuración WiFi
const char* ssid = "BUAP_Estudiantes"; 
const char* password = "f85ac21de4";    

// Configuración del broker MQTT (EMQX)
const char* mqtt_server = "broker.emqx.io"; // Dirección EMQX
const int mqtt_port = 1883;                // Puerto 

WiFiClient espClient; 
PubSubClient client(espClient);

// Tópicos MQTT
const char* topicPwm = "esp32/pwm";
const char* topicAdc = "esp32/adc";
const char* topicLed1 = "esp32/led1";
const char* topicLed2 = "esp32/led2"; 

// Pines
const int pwmPin = 19;       
const int adcPin = 34;       
const int ledPin1 = 18;      // Pin para el primer LED controlado por ON/OFF
const int ledPin2 = 5;       

// Variables
int pwmValue = 0;            // Valor del PWM
unsigned long lastAdcPublish = 0; // Tiempo de la última publicación ADC
const int adcPublishInterval = 2000; // Intervalo de publicación ADC (ms)

// Funciones
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();

void setup() {
  Serial.begin(115200);

  pinMode(pwmPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Aplicar el valor del PWM al LED
  analogWrite(pwmPin, pwmValue);

  // Publicar el valor del ADC periódicamente
  if (millis() - lastAdcPublish > adcPublishInterval) {
    int adcValue = analogRead(adcPin);
    String adcStr = String(adcValue);
    client.publish(topicAdc, adcStr.c_str());
    Serial.print("ADC publicado: ");
    Serial.println(adcStr);
    lastAdcPublish = millis();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Mensaje recibido en ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  // Control del PWM
  if (String(topic) == topicPwm) {
    pwmValue = constrain(message.toInt(), 0, 255); //  valor esté entre 0 y 255
    Serial.print("PWM actualizado: ");
    Serial.println(pwmValue);
  }

  // Control del primer LED (ON/OFF)
  if (String(topic) == topicLed1) {
    if (message.equalsIgnoreCase("ON")) {
      digitalWrite(ledPin1, HIGH);
      Serial.println("LED 1 encendido");
    } else if (message.equalsIgnoreCase("OFF")) {
      digitalWrite(ledPin1, LOW);
      Serial.println("LED 1 apagado");
    }
  }

  // Control del segundo LED (ON/OFF)
  if (String(topic) == topicLed2) {
    if (message.equalsIgnoreCase("ON")) {
      digitalWrite(ledPin2, HIGH);
      Serial.println("LED 2 encendido");
    } else if (message.equalsIgnoreCase("OFF")) {
      digitalWrite(ledPin2, LOW);
      Serial.println("LED 2 apagado");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");

    // Utilizar el Client ID proporcionado
    if (client.connect("mqttx_ff18dd4b")) { 
      Serial.println("Conectado");

      // Suscribirse a los tópicos
      client.subscribe(topicPwm);
      client.subscribe(topicLed1);
      client.subscribe(topicLed2);
    } else {
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

