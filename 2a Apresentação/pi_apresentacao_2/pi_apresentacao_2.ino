// define pinagem
#define sensorTopo  D1
#define sensorBaixo  D2
#define LED_1  D3
#define LED_2  D4
#define LED_3  D5

// define variáveis para guardar as condições dos sensores
bool sensorTopoAtivo = false;
bool sensorBaixoAtivo = false;
bool ultimoSensorTopoAtivo = false;
bool ultimoSensorBaixoAtivo = false;

void setup() {
  Serial.begin(9600);

  // configura pinos de entrada e saída
  pinMode(sensorTopo, INPUT);
  pinMode(sensorBaixo, INPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  Serial.println("Estado das boias");
  Serial.println();
}

void loop() {
  // atribui valor as variáveis que guardam o estado dos sensores
  sensorTopoAtivo = digitalRead(sensorTopo) == HIGH;
  sensorBaixoAtivo = digitalRead(sensorBaixo) == HIGH;

  // configura leds de acordo com o estado dos sensores
  digitalWrite(LED_1, !sensorTopoAtivo && !sensorBaixoAtivo);
  digitalWrite(LED_2, sensorBaixoAtivo && !sensorTopoAtivo);
  digitalWrite(LED_3, sensorTopoAtivo && sensorBaixoAtivo);
  digitalWrite(LED_3, sensorTopoAtivo);

  delay(10);

  // verifica se houve alteração no estado dos sensores, se sim, envia infos para o banco de dados, chamando a função "insereInfosNoMySQL"
  if (sensorTopoAtivo != ultimoSensorTopoAtivo || sensorBaixoAtivo != ultimoSensorBaixoAtivo) {
    if (sensorTopoAtivo == HIGH) {
      Serial.println("Sensor topo ativo");
    }
    else if (sensorTopoAtivo == LOW && sensorBaixoAtivo == HIGH) {
      Serial.println("Sensor baixo ativo");
    } else {
      Serial.println("Sensores inativos");
    }
  }

  // variáveis para guardar última checagem dos sensores
  ultimoSensorTopoAtivo = sensorTopoAtivo;
  ultimoSensorBaixoAtivo = sensorBaixoAtivo;

  delay(1000);
}
