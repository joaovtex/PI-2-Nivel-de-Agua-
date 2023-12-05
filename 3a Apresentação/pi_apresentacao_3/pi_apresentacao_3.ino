// inclui biblioteca do display
#include <Wire.h>
#include "SSD1306Wire.h"

// inclue as bibliotecas de wi-fi e conexão com o mySQL
#include <MySQL_Generic.h>
#include <MySQL_Generic.hpp>
#include <ESP8266WiFi.h>

// define pinagem
#define sensorTopo  D1
#define sensorBaixo  D2
#define LED_1  D3
#define LED_2  D4
#define LED_3  D5
#define BUZZER D6

// define variáveis para guardar as condições dos sensores
bool sensorTopoAtivo = false;
bool sensorBaixoAtivo = false;
bool ultimoSensorTopoAtivo = false;
bool ultimoSensorBaixoAtivo = false;
int tempo = 400;

SSD1306Wire  display(0x3c, D7, D8);

// define as credenciais para a conexção com o wi-fi
const char* redeWIFI = "JV";
const char* senhaWIFI = "@Victor161931";

// define as credenciais para a conexão com o banco de dados
IPAddress servidor (85, 10, 205, 173);
char usuario[] = "grupopi";
char senha[] = "gjjlm123";
char bd[] = "reservatorio";

// cria os objetos de conexão e envio de infos para o banco de dados
MySQL_Connection conn((Client *)&client);
MySQL_Query *query_mem;

void setup() {
  //Inicia o monitor serial
  Serial.begin(9600);

  // inicia conexão com o wifi
  WiFi.begin(redeWIFI, senhaWIFI);

  // tenta conectar ao wi-fi enquanto a conexão não for concluída
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WIFI...");
  }
  Serial.println("WIFI conectado!");

  // tenta conectar ao banco de dados enquanto a conexão não for concluída
  while (!conn.connect(servidor, 3306, usuario, senha, bd)) {
    Serial.println("Conectando ao Banco de Dados.");
  }
  Serial.println("Conexão com o Banco de Dados concluída.");

  // configura pinos de entrada e saída
  pinMode(sensorTopo, INPUT);
  pinMode(sensorBaixo, INPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  Serial.println("Estado das boias");
  Serial.println();

  //Inicia o display
  Serial.begin(115200);
  display.init();
  display.flipScreenVertically();
}

void loop() {
  // atribui valor as variáveis que guardam o estado dos sensores
  sensorTopoAtivo = digitalRead(sensorTopo) == HIGH;
  sensorBaixoAtivo = digitalRead(sensorBaixo) == HIGH;

  // configura leds de acordo com o estado dos sensores
  digitalWrite(LED_1 , !sensorTopoAtivo && !sensorBaixoAtivo);
  digitalWrite(LED_2 , sensorBaixoAtivo && !sensorTopoAtivo);
  digitalWrite(LED_3 , sensorTopoAtivo && sensorBaixoAtivo);
  digitalWrite(LED_3 , sensorTopoAtivo);

  delay(10);

  // verifica se houve alteração no estado dos sensores
  if (sensorTopoAtivo != ultimoSensorTopoAtivo || sensorBaixoAtivo != ultimoSensorBaixoAtivo) {

    if (sensorTopoAtivo == HIGH) {
      Serial.println("Sensor topo ativo");
      //Tom simples para o buzzer
      tone(BUZZER, 1500, tempo);
      // chama função de inserção de infos no banco de dados
      insereInfosNoMySQL("Nível: Cheio");
      // chama funação para exibir no display que os dados foram salvos
      dados();
      // limpa display
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      // seleciona a fonte
      display.setFont(ArialMT_Plain_16);
      // seta no display a situação do reservatório
      display.drawString(63, 10, "Nivel de Agua:");
      display.drawString(63, 26, "Cheio");
      display.display();
    }

    else if (sensorTopoAtivo == LOW && sensorBaixoAtivo == HIGH) {
      Serial.println("Sensor baixo ativo");
      tone(BUZZER, 1300, tempo);
      insereInfosNoMySQL("Nível: Estável");
      dados();
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_16);
      display.drawString(63, 10, "Nivel de Agua:");
      display.drawString(63, 26, "Estável");
      display.display();
    }

    else {
      Serial.println("Sensores inativos");
      tone(BUZZER, 1100, tempo);
      insereInfosNoMySQL("Nível: Crítico");
      dados();
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_16);
      display.drawString(63, 10, "Nivel de Agua:");
      display.drawString(63, 26, "Crítico");
      display.display();
    }

  }

  // variáveis para guardar última checagem dos sensores
  ultimoSensorTopoAtivo = sensorTopoAtivo;
  ultimoSensorBaixoAtivo = sensorBaixoAtivo;

  delay(1000);
}

// função para realizar a inserção de dados ao banco de dados
void insereInfosNoMySQL(const char* situacao) {
  // atribui inserção ao banco de dados conectado
  MySQL_Query query_mem = MySQL_Query(&conn);

  // variável para guardar o comando de inserção MySQL que será utilizado
  String INSERT_SQL = "INSERT INTO reservatorio.relatorio (data_hora, situacao) VALUES (DATE_SUB(NOW(), INTERVAL 3 HOUR), '" + String(situacao) + "');";

  Serial.println("Enviando informações para o banco de dados: ");
  Serial.println(INSERT_SQL);

  // executa comando de inserção no mySQL e exibe a mensagem se deu certo ou errado
  if (!query_mem.execute(INSERT_SQL.c_str())) {
    Serial.println("Erro de inserção.");
  }
  else {
    Serial.println("Inserção concluída.");
  }
}

// função para exibir no display que os dados foram salvos
void dados() {
  //Mensagem informativa na mudança de estado da boia
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  //Seleciona a fonte
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 26, "Informação Salva!");
  display.display();
  delay (2000);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(63, 15, "Enviado para");
  display.drawString(63, 30, "Base de Dados");
  display.display();
  delay (2000);
}
