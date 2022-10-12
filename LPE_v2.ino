//Ajuste corrente: Motor rosca: 0.75V
//Motor Haste: 0.75V

// --- Mapeamento de Hardware ---
#define butUp 10  // Botao de troca de menu
#define butP 9  // Botao de selecao

#define sensor0 11  // Sensor do topo da rosca sem fim
#define sensor1 1  // Sensor da base da rosca sem fim
#define sensor2 4  // Sensor intermediario

#define slp2 5  // Sleep motor da haste
#define stp1 3  // Stepper motor da haste
#define dir1 2  // Dir motor da haste

#define slp1 8  // Sleep motor da rosca
#define stp 6  // Stteper do motor da rosca
#define dir 7  // Dir do motor da rosca

#define MOTOR_STEPS 200  // Numero de passos por revolucao
                         // do motor da rosca 16x200

// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal.h>  // Biblioteca LCD (versao 1.0.7)
#include "DRV8825.h"  // Biblioteca que controla o motor da
                      // rosca  (versao 1.1.4)

// --- Funcoes de controle do menu ---
void changeMenu();
void dispMenu();
void x_0();  // Configuracao da posicao inicial

// --- Funcoes de controle dos parametros ---
void Growth_rate();  // Velocidade de crescimento
void Growth_time();  // Tempo de crescimento
void Spin_time_fast();  // Tempo de centrifugacao
void Warming_time();  // Tempo de uniformizacao da temperatura
void Centrifuge_rate();  // Velocidade de centrifugacao

// --- Funcoes de execucao de movimentos---
void Run();  // Inicia o menu principal
void Repair();  // Desloca o motor para a posicao inicial
void repairing ();  // Retorna para configuração inicial
void moving ();  // Movimento entre um sensor e outro
void Warming();  // Tempo de uniformizacao da temperatura
void growth ();  // Crescimento
void spin_fast ();  // Tempo de centrifugacao
void Calibration();  // Ajuste da posicao dos sensores
void calibrating();
void sensor1_height();
void sensor2_height();
void sensor3_height();

void readSelect(char option);
// --- Variaveis Globais ---
long int  v1 = 0x00;  // Parametro de ajuste da velocidade de
                      // crescimento
long int v2 = 0xC8;  // Parametro de ajuste da velocidade de
                     // centrifugacao (mín. 200 rpm)
unsigned long  t1 = 0x00;  // Parametro de ajuste do tempo de
                           // crescimento
unsigned  int  t2 = 0x00;  // Parametro de ajuste do tempo de
                           // centrifugacao
unsigned  long  t3 = 0x00;  // Parametro de ajuste do tempo de
                           // uniformizacao da temperatura
int d = 1;  // Controle do sentido de rotacao do motor da rosca
long int i;  // Variavel de controle da velocidade de 
             // centrifugacao (funcao tone)
char menu = 0x01;  // Controle do menu principal
char submenu = 0x01;
char set1 = 0x00;  // Variavel auxiliar de controle das funcoes
                   // de execucao de movimento
char sets2 = 0x00;  // Variavel auxiliar de controle do sensor2
char sets3 = 0x00;  // Variavel auxiliar de controle do sensor1
char c1 = 0x00;
boolean t_butUp,  // Flag do botao de troca de menu
        t_butP,  // Flag do botao de selecao
        bs2,  // Flag do sensor 2
        bs3,  // Flag do sensor 1
        c2;

byte seta[8] = {
  B11000,
  B11100,
  B11110,
  B11111,
  B11110,
  B11100,
  B11000,
  B00000
};

// --- Hardwares---

LiquidCrystal disp(19,  // RS
                   18,  // E
                   17,  // D4
                   16,  // D5
                   15,  // D6
                   14);  // D7

DRV8825 stepper(MOTOR_STEPS, dir, stp);

// --- Configuracoes Iniciais ---
void setup()
{
  disp.begin(16, 2);  // Inicia o display

  pinMode(butUp, INPUT_PULLUP);  // Configura o botao de troca
                                 // de menu como Pull Up
  pinMode(butP,  INPUT_PULLUP);  // Configura o botao de selecao
                                 // como Pull Up

  pinMode(sensor0, INPUT);
  pinMode(sensor1, INPUT);  // Configura os sensores como
                            // entrada
  pinMode(sensor2, INPUT);
  disp.createChar(8, seta);
  bs2 = 0x00;  // Zera flag do sensor 2
  bs3 = 0x00;  // Zera flag do sensor 1
  c2 = 0x00;
  t_butUp   = 0x00;  // Zera flag do botão de troca de menu
  t_butP    = 0x00;  // Zera flag do botão de selecao

  pinMode(slp1, OUTPUT);
  digitalWrite(slp1, LOW);
  pinMode(slp2, OUTPUT);  // Desliga os sleeps dos motores e
                          // configura-os como saida
  digitalWrite(slp2, LOW);

  pinMode(7, OUTPUT);  // Configura o dir do motor da rosca
                       // como saida

  stepper.begin(4500);  // Configura a velocidade do motor 
                        // da rosca para x rpm

} // end setup

// --- Loop Infinito ---
void loop()
{
  x_0();
  changeMenu();  // Inicia o menu e desloca o motor para uma
                 // nova posicao inicial
  dispMenu();
}

// --- Desenvolvimento das Funcoes Auxiliares ---

// --- Configuracoes do menu---
void x_0()  // Desloca o motor para um nova posicao inicial
{
  if (digitalRead(sensor0) == HIGH) {
    digitalWrite(slp1, HIGH);
    stepper.move(16 * MOTOR_STEPS);
    digitalWrite(slp1, LOW);
  }
} // end funcao x_0

void changeMenu()  //Modifica o menu atual
{
  if (!digitalRead(butUp)) t_butUp = 0x01;  // Botao Down
                                 // pressionado? Seta flag
  if (digitalRead(butUp) && t_butUp)  // Botao Up solto e
                                      // flag setada?
  { //Sim...
    t_butUp = 0x00;  // Limpa flag

    disp.clear();  // Limpa display

    menu++;  // Incrementa menu

    if (menu > 0x08) menu = 0x01;  // Se menu maior que 8,
                                   // volta a ser 1

  }
} //end changeMenu

void dispMenu()  // Mostra o menu atual
{
  switch (menu)  // Controle da variavel menu
  {
    case 0x01: // Caso 1
      Warming_time(); // Chama a funcao Growth time
      break;
    case 0x02:  // Caso 2
      Growth_rate();  // Chama a funcao Growth rate
      break;
    case 0x03:  // Caso 3
      Centrifuge_rate();  // Chama a funcao Centrifuge rate
      break;
    case 0x04:  // Caso 4
      Growth_time();  // Chama funcao Spin time fast
      break;
    case 0x05:  // Caso 5
      Spin_time_fast();  // Chama funcao Warming time
      break;
    case 0x06:  // Caso 6
      Run();  // Chama funcao Run
      break;
    case 0x07:  // Caso 7
      Calibration();  // Chama funcao Calibration
      break;
    case 0x08:  // Caso 8
      Repair();  // Chama funcao Repair
      break;


  } //end disp menu
}

// --- Configuracoes das funcoes de controle dos parametros ---
void Growth_time()  // Tempo de crescimento
{
  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Growth time");  // Escreve o nome da funcao

  if (!digitalRead(butP)) t_butP = 0x01;  // Botao M 
                                    //pressionado? Seta flag
  if (t1 > 600) t1 = 0x00;  // Tempo maximo: 600s (10min)

  if (digitalRead(butP) && t_butP)  // Botao P solto e
                                    // flag setada?
  { //Sim...
    t_butP = 0x00;  // Limpa flag
    set1++;
    t1 += 0x1E;  // Incrementa t1 de 30s em 30s
  }
  if (set1 > 1) set1 = 0x01;  // Se maior que 1, volta a ser 1

  switch (set1)  // Controle do set1
  {
    case 0x01:  // Caso 1
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 1,
                             // linha 2
      disp.print(t1);  // Escreve o tempo de crescimento...
      disp.print(" s  ");  // em segundos
      break;
  }
} // end Growth time

void Growth_rate()  // Velocidade de crescimento
{
  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Growth rate");  // Escreve o nome da funcao

  if (!digitalRead(butP)) t_butP = 0x01;  // Botão M 
                                   // pressionado? Seta flag
  if (v1 > 200) v1 = 0x00;

  if (digitalRead(butP) && t_butP)  // Botão P solto e flag
                                    // setada?
  { //Sim...
    t_butP = 0x00;  // Limpa flag
    set1++;
    v1 += 0x0A;  // Incrementa v1 de 10 em 10
  }
  if (set1 > 1) set1 = 0x01;  // Se maior que 1, volta a ser 1

  switch (set1)  // Controle do set1
  {
    case 0x01:  // Caso 1
      disp.setCursor(0, 1);
      disp.print(v1);  // Escreve a frequencia de giro...
      disp.print(" rpm    ");  // em rpm
      break;

  }
} // end Growth rate

void Centrifuge_rate()  // Velocidade de crescimento
{
  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Centrifuge rate");  // Escreve o nome da funcao

  if (!digitalRead(butP)) t_butP = 0x01;  // Botão M
                                   // pressionado? Seta flag
  if (v2 > 1000) v2 = 0xC8;

  if (digitalRead(butP) && t_butP)  // Botao P solto e flag 
                                    // setada?
  { //Sim...
    t_butP = 0x00;  // Limpa flag
    set1++;
    v2 += 0x64;  // Incrementa v1 de 100 em 100
  }
  if (set1 > 1) set1 = 0x01;  // Se maior que 1,
                              // volta a ser 1

  switch (set1)  // Controle do set1
  {
    case 0x01:  // Caso 1
      disp.setCursor(0, 1);
      disp.print(v2);  // Escreve a frequencia de giro...
      disp.print(" rpm    ");  // em rpm
      break;

  }
} // end Growth rate

void Spin_time_fast()  // Tempo de centrifugacao
{

  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Spin off time");  // Escreve o nome da funcao

  if (!digitalRead(butP)) t_butP = 0x01;  // Botão M
                                   // pressionado? Seta flag
  if (t2 > 300) t2 = 0x00;  // Tempo maximo: 300s (5 min)

  if (digitalRead(butP) && t_butP)  // Botao P solto e flag
                                    // setada?
  { //Sim...
    t_butP = 0x00;  // Limpa flag
    set1++;
    t2 += 0x05;  // Incrementa t3 de 5s em 5s
  }
  if (set1 > 1) set1 = 0x01;  // Se maior que 1, volta a ser 1

  switch (set1)  // Controle do set1
  {
    case 0x01:  // Caso 1
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 1, 
                             // linha 2
      disp.print(t2);  // Escreve o tempo de centrifugacao...
      disp.print(" s  ");  // em segundos.
      break;
  }
}  // end spin time fast

void Warming_time()  // Tempo de uniformizacao da temperatura
{

  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Warming up time");  // Escreve o nome da funcao

  if (!digitalRead(butP)) t_butP = 0x01;  // Botao M
                                     // pressionado? Seta flag
  if (t3 > 600) t3 = 0x00;  // Tempo maximo: 600s (10 min)

  if (digitalRead(butP) && t_butP)  // Botao P solto e flag 
                                    // setada?
  { //Sim...
    t_butP = 0x00;  // Limpa flag
    set1++;
    t3 += 0x0A;  // Incrementa t3 de 10s em 10s
  }
  if (set1 > 1) set1 = 0x01;  // Se maior que 1, volta a ser 1

  switch (set1)  // Controle do set1
  {
    case 0x01:  // Caso 1
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 1,
                             // linha 2
      disp.print(t3);  // Escreve o tempo de uniformizacao de
                       // temperatura...
      disp.print(" s  ");  // em segundos
      break;
  }
} // end Warming time

// --- Funcoes das funcoes de execucao de movimentos ---

void moving ()  // Movimento entre um sensor e outro
{
  disp.setCursor(0, 0);
  disp.print("Moving...      ");  // Escreve o nome da funcao
                                  // em andamento
  digitalWrite(slp1, HIGH);  // Liga o motor da rosca
  stepper.move(d * 16 * MOTOR_STEPS);
  digitalWrite (slp1, LOW);
} // end moving

void Warming()  // Tempo de uniformizacao da temperatura
{
  disp.setCursor(0, 0);
  disp.print("Warming time...   ");  // Escreve o nome da
                                    // funcao em andamento

  delay(t3 * 1000);  // Para o motor da rosca por um tempo
                     // t3 segundos

  digitalWrite (slp2, HIGH);  // Liga o motor da haste
  for (i = 100; i < v1 * 55 ; i = i + 100) { // Acelera o motor
                  // da haste a uma velocidade v1 (1 rpm = 55hz)
    tone(stp1, i);  // gera um pulso de frequencia i (em hz)
    delay(100);
  }
} // end Warming

void growth ()  // Crescimento
{
  d = -1;  // Muda o sentido de giro do motor
  disp.setCursor(0, 0);
  disp.print("Growth...      ");  // Escreve o nome da funcao
                                  // em andamento
  delay(t1 * 1000);  // Mantem a haste girando no crescimento
              // (dentro do cadinho) por um tempo t1 segundos
} // end growth

void spin_fast ()  // Tempo de centrifugacao
{
  disp.setCursor(0, 0);
  disp.print("Spin off...      ");  // Escreve o nome da funcao
                                    // em andamento
  for (i = v1 * 55; i < v2 * 55; i = i + 100) {
       // Acelera o motor de uma velocidade v1 a Imax (1000 rpm)
    tone(stp1, i);
    delay(100);  // Em media 49s para acelerar e o mesmo tempo
                 // para desacelerar
  }

  delay(t2 * 1000);  // Mantem o a haste girando a 1000 rpm por
                     // um tempo t2 segundos

  long int j = i;
  for (j = v2 * 55; j > 0; j = j - 100) {
    tone(stp1, j);  // gera um pulso com frequencia imax (em hz)
    delay(100);
  }
  digitalWrite (slp2, LOW);  // Desliga o motor da haste
} // end spin fast

void repairing ()  // Movimento entre um sensor e outro
{
  disp.setCursor(0, 0);
  disp.print("Repairing...      "); 
                      // Escreve o nome da funcao em andamento
  digitalWrite(slp1, HIGH);  // Liga o motor da rosca
  stepper.move(-16 * MOTOR_STEPS);
  digitalWrite (slp1, LOW);  // Gira com a velocidade setada
} // end moving

// --- Run ---

void Run()  // Inicia a operacao
{
  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Run");  // Escreve o nome da funcao
  if (!digitalRead(butP))    t_butP    = 0x01;  // Botao M
                                 // pressionado? Seta flag
  if (digitalRead(butP) && t_butP)  //Botao P solto e flag
                                    // setada?
  { //Sim...

    while (1) {
      if (digitalRead(sensor0) == LOW) {  // Sensor 0
                                          // desligado?
        moving ();  // Chama a funçao moving
        if (!digitalRead(sensor2)) bs2 = 0x01;  // Sensor 2
                                           // acionado? Seta flag
        if (digitalRead(sensor2) && bs2)
        {
          bs2 = 0x00;  // Limpa flag
          sets2++;
          switch (sets2)
          {
            case 0x01:  // Sensor 2 acionado pela 1a vez (ida)
              Warming();  // Chama a funcao Warming
              break;
            case 0x02:  // Sensor 2 acionado pela 2a vez (volta)
              spin_fast();  // Chama a funcao spin_fast
              break;
          }
        }
        if (!digitalRead(sensor1)) bs3 = 0x01;  // Sensor 1
                                         // acionado? Seta flag
        if (digitalRead(sensor1) && bs3)
        {
          bs3 = 0x00;  // Limpa flag
          sets3++;
          switch (sets3)
          {
            case 0x01:
              growth ();  // Chama a funcao growth
              break;
          }
        }
      }
      else {  // Sensor 0 ligado?
        digitalWrite (slp1, LOW);  // Desliga o motor da rosca
        digitalWrite (slp2, LOW);  // Desliga o motor da haste
        disp.setCursor(0, 0);
        disp.print("Completed   ");  // Escreve a mensagem de
                                     // conclusao
      }
    }
  }
} // end Run

//---------------------------------------- - Repair ---------------------------------------- -

void Repair()
{
  disp.setCursor(0, 0);  //Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Repair");  // Escreve o nome da funcao
  if (!digitalRead(butP))    t_butP    = 0x01;  //Botao M
                                  // pressionado? Seta flag
  if (digitalRead(butP) && t_butP)  // Botao P solto e flag
                                    // setada?
  { //Sim...

    while (1) {
      if (digitalRead(sensor0) == LOW) {  // Sensor 0 desligado?
        repairing ();  // Chama a funcao repairing
      }
      else {  // Sensor 0 ligado?
        digitalWrite (slp1, LOW);  // Desliga o motor da rosca
        digitalWrite (slp2, LOW);  // Desliga o motor da haste
        disp.setCursor(0, 0);
        disp.print("Press reset   ");  // Escreve a mensagem de
                                       // conclusão
      }
    }
  }
}

//---------------------------------------- - Calibration ---------------------------------------- -

void Calibration()
{
  disp.setCursor(0, 0);  // Posiciona cursor na coluna 1,
                         // linha 1
  disp.print("Sensor height");  // Escreve o nome da funcao

  calibrating();


}

void calibrating()
{
  if (!digitalRead(butP))   t_butP   = 0x01;  // Botao Down 
                                      // pressionado? Seta flag
  if (digitalRead(butP) && t_butP)  // Botao Up solto e flag
                                    // setada?
  { //Sim...
    t_butP = 0x00;  //Limpa flag

    submenu++;  //Incrementa menu

    if (submenu > 0x03) submenu = 0x01;  // Se menu maior que 2,
                                         // volta a ser 1
  }
  switch (submenu)
  {
    case 0x01:  //Caso 1
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 2,
                             // linha 2
      disp.write(8);
      disp.print("Sensor bottom    ");
      readSelect(1);  // Le botão select com parâmetro 1

      break;  //break
    case 0x02:  //Caso 2
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 2,
                             // linha 2
      disp.write(8);
      disp.print("Sensor middle  ");
      readSelect(2);  // Le botao select com parametro 2
      break;

    case 0x03:  //Caso 2
      disp.setCursor(0, 1);  // Posiciona cursor na coluna 2,
                             // linha 2
      disp.write(8);
      disp.print("Sensor top     ");
      readSelect(3);  // Le botao select com parâmetro 2
      break;
  }
}
void readSelect(char option)  // Leitura do botao select para
                              // selecao de subMenus
{
  if (!digitalRead(butUp) && !digitalRead(butP))
  { //Sim...
    //Limpa flag

    switch (option)
    {
      case 0x01: sensor1_height(); break;

      case 0x02: sensor2_height(); break;

      case 0x03: sensor3_height(); break;

    } //end switch option

  } //end if
}
void sensor1_height()
{
  while (1)
  {
    disp.clear();
    disp.setCursor(0, 0);
    disp.print("Bottom position     ");
    stepper.begin(500);
    if (digitalRead(sensor1) == LOW) {
      digitalWrite(slp1, HIGH);
      stepper.move(150);
      digitalWrite(slp1, LOW);
    }
  }
}
void sensor2_height()
{
  while (1)
  {
    disp.clear();
    disp.setCursor(0, 0);
    disp.print("Middle position    ");
    stepper.begin(500);
    if (digitalRead(sensor2) == LOW) {
      digitalWrite(slp1, HIGH);
      stepper.move(150);
      digitalWrite(slp1, LOW);
    }
  }
}
void sensor3_height()
{
  while (1)
  {
    disp.clear();
    disp.setCursor(0, 0);
    disp.print("Top position     ");
    stepper.begin(500);
    if (digitalRead(sensor0) == LOW) {
      digitalWrite(slp1, HIGH);
      stepper.move(150);
      digitalWrite(slp1, LOW);
    }
  }
}
