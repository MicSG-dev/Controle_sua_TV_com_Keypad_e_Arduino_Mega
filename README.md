# Controle_sua_TV_com_Keypad_e_Arduino_Mega

## Bibliotecas

- Biblioteca Arduino-IRremote: https://github.com/Arduino-IRremote/Arduino-IRremote
- Biblioteca Keypad: https://www.arduino.cc/reference/en/libraries/keypad/
- Biblioteca EEPROM-Storage: https://github.com/porrey/EEPROM-Storage


Repositório do Post [Controle sua TV com Keypad e Arduino Mega](https://blog.eletrogate.com/?s=Controle+sua+TV+com+Keypad+e+Arduino+Mega) no blog Eletrogate ensinando a controlar uma TV com um Keypad e um Arduino Mega. Para enviar os comandos de troca de canal e de reduzir/aumentar o volume via LED infravermelho do arduino para a televisão através do Keypad, o usuário deverá antes fazer o cadastro de cada número de canal do controle remoto da TV (e também dos controles de volume) através do Keypad. As funções do Keypad serão: 
- A: Cadastrar teclas (0-9; *; #);
- B: Salvar teclas modificadas;
- C: Limpar toda memória contendo o cadastro de teclas.
- 0-9: insere o número do canal da tv;
- *: reduz o volume da tv
- #: aumenta o volume da tv.

Os códigos das teclas cadastradas serão salvos na EEPROM do Arduino Mega. Para auxiliar o usuário, será mostrado com um LED o que o sistema está processando atualmente: 
- Apagado: sem fazer nenhuma ação, esperando o usuário executar algo;
- Pisca 1 vez e apaga: ao usuário pressionar algum botão (será aceso o LED) e após soltar o botão (será apagado o LED);
- Pisca-pisca lento: Cadastrando novas teclas;
- Aceso por um período de tempo: Teclas modificadas salvas;
- Pisca-pisca rápido: Limpando toda memória contendo o cadastro de teclas.

Os comandos serão enviados do arduino para a tv via LED emissor infravermelho IR. Para o cadastro de teclas, será utilizado um Receptor Infravermelho que receberá o sinal do controle da tv e armazenará na memória do Arduino Mega. A biblioteca utilizada para controlar a recepção e o envio de dados via infravermelho é a IRremote. Será apresentada algumas das funções da biblioteca, contendo exemplo de uso para cada uma. Também haverá uso da biblioteca EEPROM-Storage.h, que permite ao desenvolvedor simplificar o uso da EEPROM, onde será mostrado como utilizar esta biblioteca.
