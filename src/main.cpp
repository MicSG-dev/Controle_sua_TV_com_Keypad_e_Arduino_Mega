/******************************************************************************
                      Controle sua TV com Keypad e Arduino Mega

                                  Sketch Principal

                         Criado em 11 de Março de 2024
                     por Michel Galvão (https://micsg.com.br)

              Eletrogate | Arduino, Robótica, IoT, Apostilas e Kits
                          https://www.eletrogate.com/
******************************************************************************/

// Inclusão das Bibliotecas
#include <Arduino.h>        // Inclui a biblioteca padrão do Arduino
#include "IRremote.h"       // Inclui a biblioteca IRremote para controle remoto
#include "EEPROM-Storage.h" // Inclui a biblioteca EEPROM-Storage para armazenamento em EEPROM
#include "Keypad.h"         // Inclui a biblioteca Keypad para uso de teclado matricial

#define protocolo NEC // Define o protocolo como NEC

// Estrutura para armazenar códigos de teclas da TV
struct ChavesTv
{
  unsigned long _1 = 999;         // Código da tecla 1
  unsigned long _2 = 999;         // Código da tecla 2
  unsigned long _3 = 999;         // Código da tecla 3
  unsigned long _4 = 999;         // Código da tecla 4
  unsigned long _5 = 999;         // Código da tecla 5
  unsigned long _6 = 999;         // Código da tecla 6
  unsigned long _7 = 999;         // Código da tecla 7
  unsigned long _8 = 999;         // Código da tecla 8
  unsigned long _9 = 999;         // Código da tecla 9
  unsigned long _0 = 999;         // Código da tecla 0
  unsigned long _asterisco = 999; // Código da tecla asterisco
  unsigned long _cerquilha = 999; // Código da tecla cerquilha
};

const byte ROWS = 4; // Quatro linhas do teclado
const byte COLS = 4; // Quatro colunas do teclado

char keys[ROWS][COLS] = {  // Definição das teclas no teclado matricial
    {'1', '2', '3', 'A'},  // Linha 1
    {'4', '5', '6', 'B'},  // Linha 2
    {'7', '8', '9', 'C'},  // Linha 3
    {'*', '0', '#', 'D'}}; // Linha 4

byte rowPins[ROWS] = {10, 12, 8, 7}; // Pinos das linhas do teclado
byte colPins[COLS] = {6, 5, 4, 3};   // Pinos das colunas do teclado

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // Inicializa o objeto keypad com as configurações fornecidas

#define IR_RECEIVE_PIN 11 // Define o pino de recebimento do IR como pino 11

IRsend irsend; // Criação do objeto para envio de comandos IR

// Definições de armazenamento em EEPROM para códigos de teclas da TV e de tamanho do código de tecla
// -----
// ----- Cálculo do endereço de variável na EEPROM:
// -----    sizeVar + checksum + previousAddressVar = nextAddressVar
// -----
// ----- Variáveis da Fórmula:
// -----    sizeVar: tamanho da variável;
// -----    checksum: byte adicional para uma soma de verificação
// -----    previousAddressVar: endereço da variável anterior (que está na posição anterior em relação a atual);
// -----    nextAddressVar: o resultado do cálculo, sendo este o endereço da variável atual à ser gravada na memória
// -----
EEPROMStorage<int> tamanhoCode(0, 999);               // variável armazenada na EEPROM no endereço 0 e que tem 2 bytes.  Cálculo endereço próxima variável: 2 + 1 + 0  = 3 bytes
EEPROMStorage<unsigned long> tecla1(3, 999);          // variável armazenada na EEPROM no endereço 3 e que tem 4 bytes.  Cálculo endereço próxima variável: 4 + 1 + 3  = 8 bytes
EEPROMStorage<unsigned long> tecla2(8, 999);          // variável armazenada na EEPROM no endereço 8 e que tem 4 bytes.  Cálculo endereço próxima variável: 4 + 1 + 8  = 13 bytes
EEPROMStorage<unsigned long> tecla3(13, 999);         // variável armazenada na EEPROM no endereço 13 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 13 = 18 bytes
EEPROMStorage<unsigned long> tecla4(18, 999);         // variável armazenada na EEPROM no endereço 18 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 18 = 23 bytes
EEPROMStorage<unsigned long> tecla5(23, 999);         // variável armazenada na EEPROM no endereço 23 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 23 = 28 bytes
EEPROMStorage<unsigned long> tecla6(28, 999);         // variável armazenada na EEPROM no endereço 28 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 28 = 33 bytes
EEPROMStorage<unsigned long> tecla7(33, 999);         // variável armazenada na EEPROM no endereço 33 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 33 = 38 bytes
EEPROMStorage<unsigned long> tecla8(38, 999);         // variável armazenada na EEPROM no endereço 38 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 38 = 43 bytes
EEPROMStorage<unsigned long> tecla9(43, 999);         // variável armazenada na EEPROM no endereço 43 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 43 = 48 bytes
EEPROMStorage<unsigned long> tecla0(48, 999);         // variável armazenada na EEPROM no endereço 48 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 48 = 53 bytes
EEPROMStorage<unsigned long> teclaAsterisco(53, 999); // variável armazenada na EEPROM no endereço 53 e que tem 4 bytes. Cálculo endereço próxima variável: 4 + 1 + 53 = 58 bytes
EEPROMStorage<unsigned long> teclaCerquilha(58, 999); // variável armazenada na EEPROM no endereço 58 e que tem 4 bytes.

unsigned long getTeclaControleRemoto();            // Declaração da função para obter código de tecla do controle remoto
void enviarComando(unsigned long data, int nbits); // Declaração da função para enviar comando IR

const int pinoLed = 2; // Define o pino do LED como pino 2

void setup()
{
  pinMode(pinoLed, OUTPUT); // define o pino do LED como saída

  digitalWrite(pinoLed, LOW); // inicia o led no modo APAGADO

  Serial.begin(115200); // Define a taxa de baudrate da interface Serial com 115200 bits por segundo
}

unsigned long timerLED; // timer para controle de status (aceso/apagado) do LED indicador
const int tempoLimite = 5000; // tempo limite para operações no teclado (5 segundos)
unsigned long timerTeclado; // timer para controle de tempo limite em operações no teclado

void loop()
{
  char teclaPressionada = keypad.getKey(); // obtém a tecla pressionada pelo usuário

  
  if (teclaPressionada != NO_KEY) // se alguma tecla foi pressionada, ...
  {

    // Pisca 1 vez e apaga: usuário pressionou algum botão (LED acende) e após, soltou o botão (LED apaga);
    digitalWrite(pinoLed, HIGH);
    timerLED = millis();
    while (millis() - timerLED <= 250) // durante 250 milissegundos o led ficará acesso
    {
      delay(1);
    }
    digitalWrite(pinoLed, LOW);

    // verifica qual tecla foi pressioanda e executa algum algoritmo dependendo da tecla pressionada
    switch (teclaPressionada)
    {
    case 'A': // caso seja a tecla A, entra no modo de cadastro de códigos IR nas teclas
    {

      timerTeclado = millis();
      bool permanecerNoMenu = true;
      Serial.println("Cadastrar teclas. Pressione alguma tecla de 0 a 9, * ou # para iniciar o cadastramento.");
      ChavesTv chavesTv;

      while (permanecerNoMenu)
      {
        if (millis() - timerLED >= 1000)
        {
          digitalWrite(pinoLed, !digitalRead(pinoLed));
          timerLED = millis();
        }
        teclaPressionada = keypad.getKey();

        if (teclaPressionada != NO_KEY)
        {
          unsigned long tecla;
          switch (teclaPressionada)
          {
          case 'B':

            timerLED = millis();
            Serial.println("Teclas modificadas foram salvas. Saindo do menu...");
            digitalWrite(pinoLed, HIGH);
            while (millis() - timerLED <= 2000)
            {
              delay(1);
            }
            digitalWrite(pinoLed, LOW);

            if (chavesTv._0 != 999)
            {
              tecla0 = chavesTv._0;
            }

            if (chavesTv._1 != 999)
            {
              tecla1 = chavesTv._1;
            }

            if (chavesTv._2 != 999)
            {
              tecla2 = chavesTv._2;
            }

            if (chavesTv._3 != 999)
            {
              tecla3 = chavesTv._3;
            }

            if (chavesTv._4 != 999)
            {
              tecla4 = chavesTv._4;
            }

            if (chavesTv._5 != 999)
            {
              tecla5 = chavesTv._5;
            }

            if (chavesTv._6 != 999)
            {
              tecla6 = chavesTv._6;
            }

            if (chavesTv._7 != 999)
            {
              tecla7 = chavesTv._7;
            }

            if (chavesTv._8 != 999)
            {
              tecla8 = chavesTv._8;
            }

            if (chavesTv._9 != 999)
            {
              tecla9 = chavesTv._9;
            }

            if (chavesTv._asterisco != 999)
            {
              teclaAsterisco = chavesTv._asterisco;
            }

            if (chavesTv._cerquilha != 999)
            {
              teclaCerquilha = chavesTv._cerquilha;
            }

            chavesTv._0 = 999;
            chavesTv._1 = 999;
            chavesTv._2 = 999;
            chavesTv._3 = 999;
            chavesTv._4 = 999;
            chavesTv._5 = 999;
            chavesTv._6 = 999;
            chavesTv._7 = 999;
            chavesTv._8 = 999;
            chavesTv._9 = 999;
            chavesTv._0 = 999;
            chavesTv._asterisco = 999;
            chavesTv._cerquilha = 999;

            permanecerNoMenu = false;
            break;
          case '1':
            Serial.println("Pressione a tecla 1 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._1 = tecla;
            }
            break;
          case '2':
            Serial.println("Pressione a tecla 2 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._2 = tecla;
            }
            break;
          case '3':
            Serial.println("Pressione a tecla 3 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._3 = tecla;
            }
            break;
          case '4':
            Serial.println("Pressione a tecla 4 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._4 = tecla;
            }
            break;
          case '5':
            Serial.println("Pressione a tecla 5 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._5 = tecla;
            }
            break;
          case '6':
            Serial.println("Pressione a tecla 6 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._6 = tecla;
            }
            break;
          case '7':
            Serial.println("Pressione a tecla 7 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              Serial.print("0x");
              Serial.println(tecla, HEX);
              chavesTv._7 = tecla;
            }
            break;
          case '8':
            Serial.println("Pressione a tecla 8 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._8 = tecla;
            }
            break;
          case '9':
            Serial.println("Pressione a tecla 9 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._9 = tecla;
            }
            break;
          case '0':
            Serial.println("Pressione a tecla 0 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._0 = tecla;
            }
            break;
          case '*':
            Serial.println("Pressione a tecla ABAIXAR VOLUME do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._asterisco = tecla;
            }
            break;
          case '#':
            Serial.println("Pressione a tecla AUMENTAR VOLUME do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              chavesTv._cerquilha = tecla;
            }
            break;
          }
        }
      }
      digitalWrite(pinoLed, LOW);
      break;
    }
    case 'C':
    {

      timerLED = millis();
      Serial.println("Excluir cadastro de todas teclas");

      while (millis() - timerLED <= 2000)
      {
        for (int i = 0; i < 25; i++) // 2,5 segundos
        {
          delay(100);
          digitalWrite(pinoLed, !digitalRead(pinoLed));
        }
      }
      digitalWrite(pinoLed, LOW);

      tecla1 = 999;
      tecla2 = 999;
      tecla3 = 999;
      tecla4 = 999;
      tecla5 = 999;
      tecla6 = 999;
      tecla7 = 999;
      tecla8 = 999;
      tecla9 = 999;
      tecla0 = 999;
      teclaAsterisco = 999;
      teclaCerquilha = 999;

      break;
    }
    case 'D':
    {
      Serial.println("Dados das teclas cadastradas:\n");

      Serial.print("Tecla 1: ");
      if (tecla1 != 999)
      {
        Serial.print(tecla1);
        Serial.print(" (0x");
        Serial.print(tecla1, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 2: ");
      if (tecla2 != 999)
      {
        Serial.print(tecla2);
        Serial.print(" (0x");
        Serial.print(tecla2, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 3: ");
      if (tecla3 != 999)
      {
        Serial.print(tecla3);
        Serial.print(" (0x");
        Serial.print(tecla3, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 4: ");
      if (tecla4 != 999)
      {
        Serial.print(tecla4);
        Serial.print(" (0x");
        Serial.print(tecla4, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 5: ");
      if (tecla5 != 999)
      {
        Serial.print(tecla5);
        Serial.print(" (0x");
        Serial.print(tecla5, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 6: ");
      if (tecla6 != 999)
      {
        Serial.print(tecla6);
        Serial.print(" (0x");
        Serial.print(tecla6, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 7: ");
      if (tecla7 != 999)
      {
        Serial.print(tecla7);
        Serial.print(" (0x");
        Serial.print(tecla7, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 8: ");
      if (tecla8 != 999)
      {
        Serial.print(tecla8);
        Serial.print(" (0x");
        Serial.print(tecla8, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 9: ");
      if (tecla9 != 999)
      {
        Serial.print(tecla9);
        Serial.print(" (0x");
        Serial.print(tecla9, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla 0: ");
      if (tecla0 != 999)
      {
        Serial.print(tecla0);
        Serial.print(" (0x");
        Serial.print(tecla0, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla REDUZIR VOLUME: ");
      if (teclaAsterisco != 999)
      {
        Serial.print(teclaAsterisco);
        Serial.print(" (0x");
        Serial.print(teclaAsterisco, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.print("Tecla AUMENTAR VOLUME: ");
      if (teclaCerquilha != 999)
      {
        Serial.print(teclaCerquilha);
        Serial.print(" (0x");
        Serial.print(teclaCerquilha, HEX);
        Serial.println(")");
      }
      else
      {
        Serial.println("-");
      }

      Serial.println();
      break;
    }
    case '*':
    {
      Serial.println("Enviando sinal para Reduzir volume TV");
      if (teclaAsterisco != 999)
      {
        enviarComando(teclaAsterisco, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '#':
    {
      Serial.println("Enviando sinal para Aumentar volume TV");
      if (teclaCerquilha != 999)
      {
        enviarComando(teclaCerquilha, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '1':
    {
      Serial.println("Enviando sinal da tecla 1");
      if (tecla1 != 999)
      {
        enviarComando(tecla1, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '2':
    {
      Serial.println("Enviando sinal da tecla 2");
      if (tecla2 != 999)
      {
        enviarComando(tecla2, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '3':
    {
      Serial.println("Enviando sinal da tecla 3");
      if (tecla3 != 999)
      {
        enviarComando(tecla3, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '4':
    {
      Serial.println("Enviando sinal da tecla 4");
      if (tecla4 != 999)
      {
        enviarComando(tecla4, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '5':
    {
      Serial.println("Enviando sinal da tecla 5");
      if (tecla5 != 999)
      {
        enviarComando(tecla5, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '6':
    {
      Serial.println("Enviando sinal da tecla 6");
      if (tecla6 != 999)
      {
        enviarComando(tecla6, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '7':
    {
      Serial.println("Enviando sinal da tecla 7");
      if (tecla7 != 999)
      {
        enviarComando(tecla7, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '8':
    {
      Serial.println("Enviando sinal da tecla 8");
      if (tecla8 != 999)
      {
        enviarComando(tecla8, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '9':
    {
      Serial.println("Enviando sinal da tecla 9");
      if (tecla9 != 999)
      {
        enviarComando(tecla9, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '0':
    {
      Serial.println("Enviando sinal da tecla 0");
      if (tecla0 != 999)
      {
        enviarComando(tecla0, tamanhoCode);
      }
      else
      {
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    }
  }
}

unsigned long getTeclaControleRemoto()
{
  timerTeclado = millis();
  IRrecv irrecv(IR_RECEIVE_PIN);
  irrecv.enableIRIn();
  decode_results results;
  unsigned long teclaApertada_temp = 999;
  unsigned long teclaApertada = 999;

  bool permanecerNoSubMenu = true;
  while (millis() - timerTeclado <= tempoLimite && permanecerNoSubMenu)
  {
    if (millis() - timerLED >= 1000)
    {
      digitalWrite(pinoLed, !digitalRead(pinoLed));
      timerLED = millis();
    }
    if (irrecv.decode(&results))
    {
      if (results.value != 0xFFFFFFFF)
      {
        if (teclaApertada_temp == 999)
        {
          teclaApertada_temp = results.value;
          Serial.print("Aperte novamente a mesma tecla para confirmar: 0x");
          Serial.println(results.value, HEX);
          timerTeclado = millis();
        }
        else
        {
          if (teclaApertada_temp == results.value)
          {
            Serial.print("Tecla confirmada: 0x");
            Serial.println(results.value, HEX);
            teclaApertada = teclaApertada_temp;
            permanecerNoSubMenu = false;
          }
          else
          {
            Serial.println("Chave de Tecla diferente.");
            permanecerNoSubMenu = false;
          }
        }
      }
      irrecv.resume(); // Receive the next value
    }
    delay(100);
  }
  if (teclaApertada == 999)
  {
    if (teclaApertada_temp == 999)
    {
      Serial.print("Nenhuma tecla pressionada dentro do tempo limite. Tente novamente selecionando ");
    }
    else
    {
      Serial.print("Nao houve confirmacao de tecla. Tente novamente selecionando ");
    }
  }
  else
  {
    Serial.print("Insercao de dado finalizada. Pressione ");
  }

  Serial.println("alguma tecla para cadastrar ou pressione B para salvar.");

  if (results.decode_type != protocolo && teclaApertada_temp == results.value)
  {
    Serial.print('\n');
    Serial.println("!!! ERRO !!!");
    Serial.print("Nao sera possivel continuar. A codigo da tecla pressionada esta em um protocolo diferente para o qual este sketch foi construido. Por favor, va ate a linha 6 e altere para o protocolo adequado. O protocolo da tecla possui o indice ");
    Serial.print(results.decode_type);
    Serial.print(". Alem disso, altere o conteudo da funcao enviarComando() para o do protocolo compativel\n Se achar que isto é um erro, reinicie o Arduino MEGA.");
    digitalWrite(pinoLed, HIGH);
    while (1)
    {
    }
  }
  tamanhoCode = results.bits;
  return teclaApertada;
}

void enviarComando(unsigned long data, int nbits)
{
  irsend.sendNEC(data, nbits);
}
