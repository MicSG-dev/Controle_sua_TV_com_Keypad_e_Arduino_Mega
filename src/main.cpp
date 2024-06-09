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
struct TeclasTv
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

const byte ROWS = 4; // Quatro linhas do teclado keypad
const byte COLS = 4; // Quatro colunas do teclado keypad

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
// -----    currentAddressVar: endereço da variável atual;
// -----    nextAddressVar: o resultado do cálculo, sendo este o endereço da próxima variável à ser gravada na memória
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

// Protótipo das funções

String getProtocol(decode_type_t);
unsigned long getTeclaControleRemoto();
void enviarComando(unsigned long data, int nbits);


const int pinoLed = 2; // Define o pino do LED como pino 2

void setup()
{
  pinMode(pinoLed, OUTPUT); // define o pino do LED como saída

  // algoritmo para o STATUS DO LED (Apagado): sem fazer nenhuma ação, esperando o usuário executar algo;
  // o LED fica apagado
  digitalWrite(pinoLed, LOW); // inicia o led no modo APAGADO

  Serial.begin(115200); // Define a taxa de baudrate da interface Serial com 115200 bits por segundo
}

unsigned long timerLED;       // timer para controle de status (aceso/apagado) do LED indicador
const int tempoLimite = 5000; // tempo limite para operações no teclado (5 segundos)
unsigned long timerTeclado;   // timer para controle de tempo limite em operações no teclado

void loop()
{
  char teclaPressionada = keypad.getKey(); // obtém a tecla pressionada pelo usuário

  if (teclaPressionada != NO_KEY) // se alguma tecla foi pressionada, ...
  {

    // algoritmo para o STATUS DO LED (Pisca 1 vez e apaga): ao usuário pressionar algum botão (será aceso o LED) e após soltar o botão (será apagado o LED);
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

      timerTeclado = millis();      // armazena o valor atual do temporizador millis
      bool permanecerNoMenu = true; // variável que controla se o loop do 'menu' deve ser interrompido ou não
      Serial.println("Cadastrar teclas. Pressione alguma tecla de 0 a 9, * ou # para iniciar o cadastramento.");

      TeclasTv teclasTv; // instancia a estrutura que armazenará as tecals da TV

      while (permanecerNoMenu) // loop do 'menu' de cadastro de códigos IR nas teclas
      {

        // algoritmo para o STATUS DO LED (Pisca-pisca lento): Cadastrando novas teclas;
        // a cada 1 segundo (1000 milissegundos) o LED inverte seu estado: ora fica aceso, ora fica apagado
        if (millis() - timerLED >= 1000)
        {
          digitalWrite(pinoLed, !digitalRead(pinoLed));
          timerLED = millis();
        }

        // obtém a tecla pressionada pelo usuário
        teclaPressionada = keypad.getKey();

        if (teclaPressionada != NO_KEY) // se alguma tecla foi pressionada, ...
        {
          unsigned long tecla;      // variável para armazenar a tecla do controle da TV pressionada pelo usuário
          switch (teclaPressionada) // se a tecla pressionada do Keypad pelo usuário
          {
          case 'B': // caso a tecla pressionada do Keypad seja a B (FUNÇÃO: Salvar teclas modificadas), ...

            timerLED = millis(); // atualiza o valor do temporizador de controle do estado do LED
            Serial.println("Teclas modificadas foram salvas. Saindo do menu...");

            // algoritmo para o LED indicador de status: Aceso por um período de tempo, Teclas modificadas salvas;
            // durante 2 segundos (2000 millisegundos) o LED permanece aceso. Após este período, o led apaga
            digitalWrite(pinoLed, HIGH);
            while (millis() - timerLED <= 2000)
            {
              delay(1);
            }
            digitalWrite(pinoLed, LOW);

            // se o membro _0 (tecla 0) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._0 != 999)
            {
              tecla0 = teclasTv._0;
            }

            // se o membro _1 (tecla 1) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._1 != 999)
            {
              tecla1 = teclasTv._1;
            }

            // se o membro _2 (tecla 2) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._2 != 999)
            {
              tecla2 = teclasTv._2;
            }

            // se o membro _3 (tecla 3) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._3 != 999)
            {
              tecla3 = teclasTv._3;
            }

            // se o membro _4 (tecla 4) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._4 != 999)
            {
              tecla4 = teclasTv._4;
            }

            // se o membro _5 (tecla 5) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._5 != 999)
            {
              tecla5 = teclasTv._5;
            }

            // se o membro _6 (tecla 6) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._6 != 999)
            {
              tecla6 = teclasTv._6;
            }

            // se o membro _7 (tecla 7) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._7 != 999)
            {
              tecla7 = teclasTv._7;
            }

            // se o membro _8 (tecla 8) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._8 != 999)
            {
              tecla8 = teclasTv._8;
            }

            // se o membro _9 (tecla 9) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._9 != 999)
            {
              tecla9 = teclasTv._9;
            }

            // se o membro _asterisco (tecla diminuir volume) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._asterisco != 999)
            {
              teclaAsterisco = teclasTv._asterisco;
            }

            // se o membro _cerquilha (tecla aumentar volume) da estrutura TeclasTv possuir um valor diferente de 999 (alterado), salva na EEPROM o seu valor
            if (teclasTv._cerquilha != 999)
            {
              teclaCerquilha = teclasTv._cerquilha;
            }

            // reseta os valores dos membros da estrutura TeclasTv
            teclasTv._0 = 999;
            teclasTv._1 = 999;
            teclasTv._2 = 999;
            teclasTv._3 = 999;
            teclasTv._4 = 999;
            teclasTv._5 = 999;
            teclasTv._6 = 999;
            teclasTv._7 = 999;
            teclasTv._8 = 999;
            teclasTv._9 = 999;
            teclasTv._0 = 999;
            teclasTv._asterisco = 999;
            teclasTv._cerquilha = 999;

            permanecerNoMenu = false; // interrompe o loop do 'menu' (sair fora do menu)
            break;
          case '1':
            Serial.println("Pressione a tecla 1 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._1 = tecla;
            }
            break;
          case '2':
            Serial.println("Pressione a tecla 2 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._2 = tecla;
            }
            break;
          case '3':
            Serial.println("Pressione a tecla 3 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._3 = tecla;
            }
            break;
          case '4':
            Serial.println("Pressione a tecla 4 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._4 = tecla;
            }
            break;
          case '5':
            Serial.println("Pressione a tecla 5 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._5 = tecla;
            }
            break;
          case '6':
            Serial.println("Pressione a tecla 6 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._6 = tecla;
            }
            break;
          case '7':
            Serial.println("Pressione a tecla 7 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              Serial.print("0x");
              Serial.println(tecla, HEX);
              teclasTv._7 = tecla;
            }
            break;
          case '8':
            Serial.println("Pressione a tecla 8 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._8 = tecla;
            }
            break;
          case '9':
            Serial.println("Pressione a tecla 9 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._9 = tecla;
            }
            break;
          case '0':
            Serial.println("Pressione a tecla 0 do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._0 = tecla;
            }
            break;
          case '*':
            Serial.println("Pressione a tecla ABAIXAR VOLUME do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._asterisco = tecla;
            }
            break;
          case '#':
            Serial.println("Pressione a tecla AUMENTAR VOLUME do controle remoto");
            tecla = getTeclaControleRemoto();

            if (tecla != 999)
            {
              teclasTv._cerquilha = tecla;
            }
            break;
          }
        }
      }
      digitalWrite(pinoLed, LOW);
      break;
    }
    case 'C': // caso seja a tecla C, entra no modo de exclusão de todos os códigos IR das teclas cadastrados
    {

      timerLED = millis(); // Armazena o tempo atual em milissegundos

      // Exibe mensagem informando que todos os cadastros de teclas estão sendo excluídos
      Serial.println("Excluir cadastro de todas teclas");

      // algoritmo para o STATUS DO LED (Pisca-pisca rápido): Limpando toda memória contendo o cadastro de teclas.
      // a cada 100 milissegundos o LED inverte seu estado: ora fica aceso, ora fica apagado
      while (millis() - timerLED <= 2000)
      {
        for (int i = 0; i < 25; i++) // 2,5 segundos
        {
          delay(100);
          digitalWrite(pinoLed, !digitalRead(pinoLed));
        }
      }
      digitalWrite(pinoLed, LOW);

      // Reseta os códigos IR das teclas para o valor 999, indicando que estão "vazios"
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
    case 'D': // caso seja a tecla D, entra no modo de exibição de todos os códigos IR das teclas cadastrados
    {
      Serial.println("Dados das teclas cadastradas:\n"); // Exibe mensagem inicial indicando o início da exibição dos dados

      // Exibe o código cadastrado para a tecla 1
      Serial.print("Tecla 1: ");
      if (tecla1 != 999) // Verifica se a tecla 1 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 1
        Serial.print(tecla1);
        Serial.print(" (0x");
        Serial.print(tecla1, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 1 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 2
      Serial.print("Tecla 2: ");
      if (tecla2 != 999) // Verifica se a tecla 2 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 2
        Serial.print(tecla2);
        Serial.print(" (0x");
        Serial.print(tecla2, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 3
      Serial.print("Tecla 3: ");
      if (tecla3 != 999) // Verifica se a tecla 3 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 3
        Serial.print(tecla3);
        Serial.print(" (0x");
        Serial.print(tecla3, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 3 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 4
      Serial.print("Tecla 4: ");
      if (tecla4 != 999) // Verifica se a tecla 4 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 4
        Serial.print(tecla4);
        Serial.print(" (0x");
        Serial.print(tecla4, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 4 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 5
      Serial.print("Tecla 5: ");
      if (tecla5 != 999) // Verifica se a tecla 5 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 5
        Serial.print(tecla5);
        Serial.print(" (0x");
        Serial.print(tecla5, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 5 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 6
      Serial.print("Tecla 6: ");
      if (tecla6 != 999) // Verifica se a tecla 6 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 6
        Serial.print(tecla6);
        Serial.print(" (0x");
        Serial.print(tecla6, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 6 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 7
      Serial.print("Tecla 7: ");
      if (tecla7 != 999) // Verifica se a tecla 7 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 7
        Serial.print(tecla7);
        Serial.print(" (0x");
        Serial.print(tecla7, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 7 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 8
      Serial.print("Tecla 8: ");
      if (tecla8 != 999) // Verifica se a tecla 8 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 8
        Serial.print(tecla8);
        Serial.print(" (0x");
        Serial.print(tecla8, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 8 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 9
      Serial.print("Tecla 9: ");
      if (tecla9 != 999) // Verifica se a tecla 9 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 9
        Serial.print(tecla9);
        Serial.print(" (0x");
        Serial.print(tecla9, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 9 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla 0
      Serial.print("Tecla 0: ");
      if (tecla0 != 999) // Verifica se a tecla 0 está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla 0
        Serial.print(tecla0);
        Serial.print(" (0x");
        Serial.print(tecla0, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla 0 não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla de reduzir volume (tecla *)
      Serial.print("Tecla REDUZIR VOLUME: ");
      if (teclaAsterisco != 999) // Verifica se a tecla de reduzir volume está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla de reduzir volume
        Serial.print(teclaAsterisco);
        Serial.print(" (0x");
        Serial.print(teclaAsterisco, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla de reduzir volume não está cadastrada
        Serial.println("-");
      }

      // Exibe o código cadastrado para a tecla de aumentar volume (tecla #)
      Serial.print("Tecla AUMENTAR VOLUME: ");
      if (teclaCerquilha != 999) // Verifica se a tecla de aumentar volume está cadastrada
      {
        // Exibe o valor decimal e hexadecimal do código IR da tecla de aumentar volume
        Serial.print(teclaCerquilha);
        Serial.print(" (0x");
        Serial.print(teclaCerquilha, HEX);
        Serial.println(")");
      }
      else // caso a tecla não esteja cadastrada
      {
        // Indica que a tecla de aumentar volume não está cadastrada
        Serial.println("-");
      }

      // Linha em branco para separar a exibição
      Serial.println();
      break;
    }
    case '*': // caso seja a tecla *, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal para reduzir o volume da TV está sendo enviado
      Serial.println("Enviando sinal para Reduzir volume TV");

      if (teclaAsterisco != 999) // Verifica se a tecla de reduzir volume está cadastrada
      {
        enviarComando(teclaAsterisco, tamanhoCode); // Envia o comando IR usando a função enviarComando com o código da tecla e o tamanho do código
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '#': // caso seja a tecla #, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal para aumentar o volume da TV está sendo enviado
      Serial.println("Enviando sinal para Aumentar volume TV");

      if (teclaCerquilha != 999) // Verifica se a tecla de aumentar volume está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla e o tamanho do código
        enviarComando(teclaCerquilha, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '1': // caso seja a tecla 1, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 1 está sendo enviado
      Serial.println("Enviando sinal da tecla 1");

      if (tecla1 != 999) // Verifica se a tecla 1 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 1 e o tamanho do código
        enviarComando(tecla1, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '2': // caso seja a tecla 2, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 2 está sendo enviado
      Serial.println("Enviando sinal da tecla 2");

      if (tecla2 != 999) // Verifica se a tecla 2 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 2 e o tamanho do código
        enviarComando(tecla2, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '3': // caso seja a tecla 3, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 3 está sendo enviado
      Serial.println("Enviando sinal da tecla 3");

      if (tecla3 != 999) // Verifica se a tecla 3 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 3 e o tamanho do código
        enviarComando(tecla3, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '4': // caso seja a tecla 4, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 4 está sendo enviado
      Serial.println("Enviando sinal da tecla 4");

      if (tecla4 != 999) // Verifica se a tecla 4 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 4 e o tamanho do código
        enviarComando(tecla4, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '5': // caso seja a tecla 5, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 5 está sendo enviado
      Serial.println("Enviando sinal da tecla 5");

      if (tecla5 != 999) // Verifica se a tecla 5 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 5 e o tamanho do código
        enviarComando(tecla5, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '6': // caso seja a tecla 6, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 6 está sendo enviado
      Serial.println("Enviando sinal da tecla 6");

      if (tecla6 != 999) // Verifica se a tecla 6 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 6 e o tamanho do código
        enviarComando(tecla6, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '7': // caso seja a tecla 7, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 7 está sendo enviado
      Serial.println("Enviando sinal da tecla 7");

      if (tecla7 != 999) // Verifica se a tecla 7 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 7 e o tamanho do código
        enviarComando(tecla7, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando pois a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '8': // caso seja a tecla 8, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 8 está sendo enviado
      Serial.println("Enviando sinal da tecla 8");

      if (tecla8 != 999) // Verifica se a tecla 8 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 8 e o tamanho do código
        enviarComando(tecla8, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando porque a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '9': // caso seja a tecla 9, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 9 está sendo enviado
      Serial.println("Enviando sinal da tecla 9");

      if (tecla9 != 999) // Verifica se a tecla 9 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 9 e o tamanho do código
        enviarComando(tecla9, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando porque a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    case '0': // caso seja a tecla 0, tenta enviar o código IR desta tecla
    {
      // Exibe mensagem informando que o sinal da tecla 0 está sendo enviado
      Serial.println("Enviando sinal da tecla 0");

      if (tecla0 != 999) // Verifica se a tecla 0 está cadastrada
      {
        // Envia o comando IR usando a função enviarComando com o código da tecla 0 e o tamanho do código
        enviarComando(tecla0, tamanhoCode);
      }
      else // caso não esteja cadastrada
      {
        // Exibe mensagem informando que não foi possível enviar o comando porque a tecla não está cadastrada
        Serial.println("Nao foi possivel enviar esta tecla para a TV. Tente cadastrar a tecla antes.");
      }
      break;
    }
    }
  }
}

// Função para obter a tecla pressionada no controle remoto
unsigned long getTeclaControleRemoto()
{
  timerTeclado = millis();                // Armazena o tempo atual em milissegundos
  IRrecv irrecv(IR_RECEIVE_PIN);          // Inicializa o receptor IR no pino especificado
  irrecv.enableIRIn();                    // Habilita a recepção de sinais IR
  decode_results results;                 // Estrutura para armazenar os resultados da decodificação IR
  unsigned long teclaApertada_temp = 999; // Variável temporária para armazenar a tecla pressionada inicialmente
  unsigned long teclaApertada = 999;      // Variável final para armazenar a tecla confirmada

  bool permanecerNoSubMenu = true;                                      // Variável de controle para manter o loop enquanto estiver no submenu
  while (millis() - timerTeclado <= tempoLimite && permanecerNoSubMenu) // Loop que continua enquanto o tempo limite não for atingido e estiver no submenu
  {
    // algoritmo para o STATUS DO LED (Aceso por um período de tempo): Teclas modificadas salvas;
    // durante 1 segundo (1000 milissegundos) o LED fica aceso e após é apagado
    if (millis() - timerLED >= 1000) // Verifica se 1 segundo se passou para alternar o estado do LED
    {
      digitalWrite(pinoLed, !digitalRead(pinoLed)); // Alterna o estado do LED

      timerLED = millis(); // Atualiza o timer do LED
    }

    if (irrecv.decode(&results)) // Verifica se um comando IR foi recebido
    {
      if (results.value != 0xFFFFFFFF) // Verifica se o valor recebido não é um código repetido
      {
        if (teclaApertada_temp == 999) // Se ainda não há tecla temporária armazenada
        {
          teclaApertada_temp = results.value; // Armazena o valor recebido temporariamente

          // Solicita confirmação da tecla pressionada ao usuário
          Serial.print("Aperte novamente a mesma tecla para confirmar: 0x");
          Serial.println(results.value, HEX);

          timerTeclado = millis(); // Reinicia o timer do teclado
        }
        else // caso já haja uma tecla temporária armazenada
        {
          if (teclaApertada_temp == results.value) // Se o valor recebido é igual ao valor temporário (CONFIRMADO), ...
          {
            // Informa ao usuário sobre a confirmação da tecla pressionada
            Serial.print("Tecla confirmada: 0x");
            Serial.println(results.value, HEX);

            teclaApertada = teclaApertada_temp; // atribui à variável de retorno o valor da tecla confirmada
            permanecerNoSubMenu = false;        // Sai do submenu
          }
          else // caso a confirmação de tecla falhe, ...
          {
            // Informa que a tecla pressionada é diferente
            Serial.println("Chave de Tecla diferente.");

            permanecerNoSubMenu = false; // Sai do submenu
          }
        }
      }

      irrecv.resume(); // Prepara o receptor IR para receber o próximo valor
    }

    delay(100); // Atraso de 100 ms antes de verificar novamente se um comando IR foi recebido
  }

  // Verifica se nenhuma tecla foi confirmada dentro do tempo limite
  if (teclaApertada == 999)
  {
    if (teclaApertada_temp == 999) // Se nenhuma tecla foi pressionada
    {
      Serial.print("Nenhuma tecla pressionada dentro do tempo limite. Tente novamente selecionando ");
    }
    else // se não, caso a tecla pressionada não foi confirmada
    {
      Serial.print("Nao houve confirmacao de tecla. Tente novamente selecionando ");
    }
  }
  else // se não, se ocorreu tudo certo
  {
    Serial.print("Insercao de dado finalizada. Pressione "); // Informa que a inserção de dados foi finalizada
  }

  Serial.println("alguma tecla para cadastrar ou pressione B para salvar."); // Informa ao usuário para continuar cadastrando novas teclas ou para finalizar salvando as tecclas com a opção B

  // Verifica se o protocolo da tecla pressionada é diferente do esperado
  if (results.decode_type != protocolo && teclaApertada_temp == results.value)
  {

    // informa ao usuário que ocorreu um erro
    Serial.print('\n');
    Serial.println("!!! ERRO !!!");
    Serial.print("Nao sera possivel continuar. A codigo da tecla pressionada esta em um protocolo diferente para o qual este sketch foi construido. Por favor, va ate a linha 6 e altere para o protocolo adequado. O protocolo da tecla é ");
    Serial.print(getProtocol(results.decode_type));
    Serial.print(". Alem disso, altere o conteudo da funcao enviarComando() para o do protocolo compativel\n Se achar que isto é um erro, reinicie o Arduino MEGA.");

    // Liga o LED para indicar erro e entra em loop infinito
    digitalWrite(pinoLed, HIGH);
    while (1)
    {
    }
  }

  tamanhoCode = results.bits; // Armazena o número de bits do código IR

  return teclaApertada; // Retorna a tecla pressionada e confirmada
}

// Função para enviar um comando IR (infravermelho)
void enviarComando(unsigned long data, int nbits)
{
  // ALTERE ESTE MÉTODO CASO O PROTOCOLO DE SEU CONTROLE REMOTO SEJA DIFERENTE DE NEC
  // Chama o método sendNEC para enviar os dados IR usando o protocolo NEC
  // 'data' é o comando que será enviado e 'nbits' é o número de bits desse comando
  irsend.sendNEC(data, nbits);
}

// Função que retorna o nome do protocolo IR obtido à partir do parâmetro informado
String getProtocol(decode_type_t decode_type) 
{
  switch (decode_type)
  {
  case UNKNOWN:
    return "UNKNOWN";
    break;
  case UNUSED:
    return "UNUSED";
    break;
  case RC5:
    return "RC5";
    break;
  case RC6:
    return "RC6";
    break;
  case NEC:
    return "NEC";
    break;
  case SONY:
    return "SONY";
    break;
  case PANASONIC:
    return "PANASONIC";
    break;
  case JVC:
    return "JVC";
    break;
  case SAMSUNG:
    return "SAMSUNG";
    break;
  case WHYNTER:
    return "WHYNTER";
    break;
  case AIWA_RC_T501:
    return "AIWA_RC_T501";
    break;
  case LG:
    return "LG";
    break;
  case SANYO:
    return "SANYO";
    break;
  case MITSUBISHI:
    return "MITSUBISHI";
    break;
  case DISH:
    return "DISH";
    break;
  case SHARP:
    return "SHARP";
    break;
  case SHARP_ALT:
    return "SHARP_ALT";
    break;
  case DENON:
    return "DENON";
    break;
  case LEGO_PF:
    return "LEGO_PF";
    break;
  case BOSEWAVE:
    return "BOSEWAVE";
    break;
  case MAGIQUEST:
    return "MAGIQUEST";
    break;
  }
}