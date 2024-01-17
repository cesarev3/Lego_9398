/*
*******************************************************
 Programa de recepcao de comandos Via XBee - DualShock
 Acionador de motores com Ponte H
 Autor Magneto - Outubro 2016
 *******************************************************
 */

// *** Definicao dos pinos para movimento e direcao ***
const int mB1 = 4; // sentido do Motor B - deslocamento
const int mB2 = 6; // sentido do Motor B - deslocamento
const int mA1 = 8; // sentido do Motor A - de direcao
const int mA2 = 7; // sentido do Motor A - de direcao
const int vMA = 9; // velocidades do Motor A - PWM
const int vMB = 5; // velocidades dos Motor B - PWM
const int sensorCentro = 2; // sensor de centro do motor de direcao
const int pot1 = A0; // potenciometro de direcao

// *** Definicao das variaveis para movimento e direcao ***
int cEnergyMA = 6; // coeficiente de energia dado pelo range do driver do motor A (0 a 255)
int cEnergyMB = 8; // coeficiente de energia dado pelo range do driver do motor B (0 a 255)
int cEnergyCentro = 50; // coeficiente de energia de retorno ao centro
int scaleJoy2 = 0; // escala de deslocamento
int scaleJoy3 = 0; // escala de direcao
byte trimJoy2 = 98; // trimagem do joystick 2 conforme programa de teste do TX
byte trimJoy3 = 157; // trimagem do joystick 3 conforme programa de teste do TX
boolean motorCentral = 0; // posicao central do motor de direcao
int valorDirecao = 0; // valor do potenciometro da direcao 

// *** Definicao dos pinos para luzes ***
const int frenteEsquerda = 12; // seta da frente do lado esquerdo
const int frenteDireita = 13; // seta da frente do lado direito
const int farois = 3; // farois
const int atrasEsquerda = 10; // luz traseira do lado esquerdo
const int atrasDireita = 11; // luz traseira do lado direito
const int re = A3; // luz de re
const int ldr = A4; // sensor crepuscular

// *** Definicao das variaveis para luzes ***
boolean valorLDR = 0; // valor do LDR
boolean luzes = 0; // luzes de lanterna e farol baixo
boolean farolAlto = 0; // farol alto
boolean luzRe = 0; // luz de re
boolean piscaAlerta = 0; // pisca alerta
boolean freio = 0; // freio para as luzes traseiras
boolean piscaDireito = 0; // pisca do lado direito
boolean piscaEsquerdo = 0; // pisca do lado esquerdo
boolean flipFarol = 0; // alterna entre farol alto e baixo ou desligado
byte valorBaixo = 50; // PWM para farol baixo e lanterna
boolean ctrlSeta = 0; // controle da ativacao das setas
boolean ctrlFreio = 0; // controle da ativacao do freio
byte delayAceso = 50; // quantidade de tempo que permanecera acesso
byte delayApagado = 75; // quantidade de temppo que permanecera apagado

// *** Definicao dos pinos para alarme ***
const int sensorAlarme = A5; // sensor alarme (portas)
const int buzzer = A6; // sirene
const int ledAlarme = A7; // Led do painel

// *** Definicao das variaveis para alarme ***
boolean alarme = 0; // alarme ativado

// *** Definicao de outras variaveis ***
int incomingByte; // Variavel de entrada de dados pela Serial
unsigned int segundo = 0; // tempo em segundos
unsigned int segundoMemoria = 0; // memoria de tempo em segundos
unsigned long calculoTempo; // calculo do tempo

void setup() {
  // inicializa Serial
  Serial.begin(9600);

  // Inicializa pinos
  pinMode(mB1,OUTPUT);
  pinMode(mB2,OUTPUT);
  pinMode(mA1,OUTPUT);
  pinMode(mA2,OUTPUT);
  pinMode(vMA,OUTPUT);
  pinMode(vMB,OUTPUT);
  pinMode(sensorCentro,INPUT);
  pinMode(frenteEsquerda,OUTPUT);
  pinMode(frenteDireita,OUTPUT);
  pinMode(farois,OUTPUT);
  pinMode(atrasEsquerda,OUTPUT);
  pinMode(atrasDireita,OUTPUT);
  pinMode(re,OUTPUT);
  pinMode(buzzer,OUTPUT);
  pinMode(ledAlarme,OUTPUT);

}
void loop() {

  // contando os segundos
  if(millis() - calculoTempo < 0) {     
    calculoTempo=millis(); 
  }   
  else {     
    segundo=int((millis() - calculoTempo) / 1000);
  } 

  // Lendo os valores dos sensores de direcao e luzes
  motorCentral = digitalRead(sensorCentro);
  valorDirecao = analogRead(pot1);
  valorLDR = analogRead(ldr);

  // Rotinas de setas de direcao
  if(piscaAlerta == 1 && luzes == 0) {
    ligaPiscaAlerta();
    ctrlSeta = 1;
  }
  else if(piscaAlerta == 1 && luzes == 1) { 
    ligaPiscaAlerta1();
    ctrlSeta = 1;
  }
  else if(piscaAlerta == 0 && piscaDireito == 1 && luzes == 0) { 
    ligaPiscaDireito();
    ctrlSeta = 1;
  }
  else if(piscaAlerta == 0 && piscaDireito == 1 && luzes == 1) { 
    ligaPiscaDireito1();
    ctrlSeta = 1;
  }
  else if(piscaAlerta == 0 && piscaEsquerdo == 1 && luzes == 0) { 
    ligaPiscaEsquerdo();
    ctrlSeta = 1;
  }
  else if(piscaAlerta == 0 && piscaEsquerdo == 1 && luzes == 1) { 
    ligaPiscaEsquerdo1();
    ctrlSeta = 1;
  }

  // Rotina de Farol Alto
  if(farolAlto == 1) {
    acendeFarolAlto();
  }
  else if(farolAlto == 0 && luzes == 0) {
    apagaFarolAlto();
  }
  else if(farolAlto == 0 && luzes == 1) {
    apagaFarolAlto1();
  }

  // Rotina de Freio
  if(freio == 1 && luzes == 0 && ctrlSeta == 0) {
    acendeFreio();
    ctrlFreio = 1;
  }
  else if(freio == 0 && luzes == 0 && ctrlSeta == 0) {
    apagaFreio();
  }
  else if(freio == 0 && luzes == 1 && ctrlSeta == 0) {
    apagaFreio1();
  }

  // Rotina de Luzes
  if(luzes == 1 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 0) {
    acendeLuzes();
  }
  else if(luzes == 1 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 1) {
    acendeLuzes1();
  }
  else if(luzes == 0 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 0 && valorLDR < 60) {
    acendeLuzes();
  }
    else if(luzes == 0 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 1 && valorLDR < 60) {
    acendeLuzes1();
  }
  else if(luzes == 0 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 0 && valorLDR > 70) {
    apagaLuzes();
  }
  else if(luzes == 0 && ctrlSeta == 0 && ctrlFreio == 0 && farolAlto == 1 && valorLDR > 70) {
    apagaLuzes1();
  }
  
  // Rotina luz de re
  if(luzRe == 1) {
    acendeRe();
  }
  else if(luzRe == 0) {
    apagaRe();
  }
  
  // Restabelece os controles ao nivel inicial
  ctrlSeta = 0;
  ctrlFreio = 0;

  // Verificando se tem entrada de dados na Serial
  if (Serial.available() > 0) {
    incomingByte = Serial.read(); // Lendo o ultimo dado recebido

    // Identificar J2UpDown - Joystick da Esquerda
    if(incomingByte >= 68 && incomingByte <= 127) {
      scaleJoy2 = (incomingByte - trimJoy2);

      if(scaleJoy2 > 0) {
        digitalWrite(mB1,HIGH);
        digitalWrite(mB2,LOW);
        freio = 0;
        luzRe = 1;
      }
      else if(scaleJoy2 < 0) {
        scaleJoy2 = scaleJoy2 * -1;
        digitalWrite(mB1,LOW);
        digitalWrite(mB2,HIGH);
        freio = 0;
        luzRe = 0;
      }
      else if(scaleJoy2 == 0) {
        freio = 1;
        luzRe = 0;
      }
      analogWrite(vMB, scaleJoy2 * cEnergyMB);
    }

    // Identificar J3LeftRight - joystick da Direita
    if(incomingByte >= 128 && incomingByte <= 187) {
      scaleJoy3 = (incomingByte - trimJoy3);
      if(scaleJoy3 > 0) {
        digitalWrite(mA1,LOW);
        digitalWrite(mA2,HIGH);
        piscaDireito = 1;
      }
      else if(scaleJoy3 < 0) {
        scaleJoy3 = scaleJoy3 * -1;
        digitalWrite(mA1,HIGH);
        digitalWrite(mA2,LOW);
        piscaEsquerdo = 1;
      }
      else if(scaleJoy3 == 0) {
        piscaDireito = 0;
        piscaEsquerdo = 0;
      }
      analogWrite(vMA, scaleJoy3 * cEnergyMA);
      
      // retorno automatico para centro da direita - corrigindo para a esquerda
      if(scaleJoy3 == 0 && valorDirecao < 547 && scaleJoy2 != 0 && motorCentral == 0) {
        digitalWrite(mA1,HIGH);
        digitalWrite(mA2,LOW);
        analogWrite(vMA, cEnergyCentro);
      }
      // retorno automatico para centro da esquerda - corrigindo para a direita
      if(scaleJoy3 == 0 && valorDirecao > 577 && scaleJoy2 !=0 && motorCentral == 0) {
        digitalWrite(mA1,LOW);
        digitalWrite(mA2,HIGH);
        analogWrite(vMA, cEnergyCentro);
      }
    }
    // ligar farol Alto
    if(incomingByte == 3) {
      farolAlto = 1;
    }
    // Desligar Farol Alto
    if(incomingByte == 4) {
      farolAlto = 0;
    }
    // Ligar luzes
    if(incomingByte == 248) {
      luzes = 1;
    }
    // Desligar luzes
    if(incomingByte == 251) {
      luzes = 0;
      piscaAlerta = 0;
    }
    // Pisca Alerta - falta habilitar a piscada
    if(incomingByte == 250) {
      piscaAlerta = 1;
    }
  }
  //Debug das informacoes do motor
  //Serial.println(incomingByte);
  //Serial.print("Sensor de centro da direcao: ");
  //Serial.println(motorCentral);
  //Serial.print("Valor do Potenciometro: ");
  //Serial.println(valorDirecao);
  //Serial.print("Valor LDR: ");
  //Serial.println(valorLDR);
  //delay(500);
}

// *** Rotinas para as luzes ***
void acendeLuzes() {
  analogWrite(farois, valorBaixo);
  analogWrite(atrasEsquerda, valorBaixo);
  analogWrite(atrasDireita, valorBaixo);
}

void acendeLuzes1() {
  analogWrite(farois, 255);
  analogWrite(atrasEsquerda, valorBaixo);
  analogWrite(atrasDireita, valorBaixo);
}

void apagaLuzes() {
  analogWrite(farois, 0);
  analogWrite(atrasEsquerda, 0);
  analogWrite(atrasDireita, 0);
}

void apagaLuzes1() {
  analogWrite(farois, 255);
  analogWrite(atrasEsquerda, 0);
  analogWrite(atrasDireita, 0);
}

void acendeFarolAlto() {
  analogWrite(farois, 255);
}     

void apagaFarolAlto() {
  analogWrite(farois, 0);
} 

void apagaFarolAlto1() {
  analogWrite(farois, valorBaixo);
}  

void acendeFreio() {
  analogWrite(atrasEsquerda, 255);
  analogWrite(atrasDireita, 255);
}

void apagaFreio() {
  analogWrite(atrasEsquerda, 0);
  analogWrite(atrasDireita, 0);
}

void apagaFreio1() {
  analogWrite(atrasEsquerda, valorBaixo);
  analogWrite(atrasDireita, valorBaixo);
}

void ligaPiscaEsquerdo() {
    digitalWrite(frenteEsquerda, HIGH);
    analogWrite(atrasEsquerda, 255);
    delay(delayAceso);
    digitalWrite(frenteEsquerda, LOW);
    analogWrite(atrasEsquerda, 0);
    delay(delayApagado);
}  

void ligaPiscaEsquerdo1() {
    digitalWrite(frenteEsquerda, HIGH);
    analogWrite(atrasEsquerda, 255);
    delay(delayAceso);
    digitalWrite(frenteEsquerda, LOW);
    analogWrite(atrasEsquerda, valorBaixo);
    delay(delayApagado);
}

void ligaPiscaDireito() {
    digitalWrite(frenteDireita, HIGH);
    analogWrite(atrasDireita, 255);
    delay(delayAceso);
    digitalWrite(frenteDireita, LOW);
    analogWrite(atrasDireita, 0);
    delay(delayApagado);
}

void ligaPiscaDireito1() {
    digitalWrite(frenteDireita, HIGH);
    analogWrite(atrasDireita, 255);
    delay(delayAceso);
    digitalWrite(frenteDireita, LOW);
    analogWrite(atrasDireita, valorBaixo);
    delay(delayApagado);
}

void ligaPiscaAlerta() {
    digitalWrite(frenteEsquerda, HIGH);
    analogWrite(atrasEsquerda, 255);
    digitalWrite(frenteDireita, HIGH);
    analogWrite(atrasDireita, 255);
    delay(delayAceso);
    digitalWrite(frenteEsquerda, LOW);
    analogWrite(atrasEsquerda, 0);
    digitalWrite(frenteDireita, LOW);
    analogWrite(atrasDireita, 0);
    delay(delayApagado);
}  

void ligaPiscaAlerta1() {
    digitalWrite(frenteEsquerda, HIGH);
    analogWrite(atrasEsquerda, 255);
    digitalWrite(frenteDireita, HIGH);
    analogWrite(atrasDireita, 255);
    delay(delayAceso);
    digitalWrite(frenteEsquerda, LOW);
    analogWrite(atrasEsquerda, valorBaixo);
    digitalWrite(frenteDireita, LOW);
    analogWrite(atrasDireita, valorBaixo);
    delay(delayApagado);
}

void acendeRe() {
  digitalWrite(re, HIGH);
}

void apagaRe() {
  digitalWrite(re, LOW);
}

/* Disposicao dos botoes e comandos
 
 j1LeftRight = 8 a 67
 j2UpDown = 68 a 127 
 j3LeftRight = 128 a 187
 j4UpDown = 188 a 247
 joyBLeft = 1 
 joyBRight = 2 
 frontTopLeft = 3
 frontBotLeft = 4
 frontTopRight = 5 
 frontBotRight = 6
 upperLeft = 248;
 upperRight = 249;
 upperTop = 250;
 upperDown = 251;
 
 Potenciometro de direcao
 Esquerda: 720/759
 Direita: 345/377
 Centro: 548/576
 
 *****************************
 **  Limites dos Joysticks  **
 **  J1 Left: 8             **
 **  J1 Center: 36          **
 **  J1 Right: 67           **
 **                         **
 **  J2 Up: 68              **
 **  J2 Center: 98          **
 **  J2 Down: 127           **
 **                         **
 **  J3 Left: 128           **
 **  J3 Center: 157         **
 **  J3 Right: 187          **
 **                         **
 **  J4 Up: 188             **
 **  J4 Center: 217         **
 **  J4 Dowm: 247           **
 *****************************
 
 */



























