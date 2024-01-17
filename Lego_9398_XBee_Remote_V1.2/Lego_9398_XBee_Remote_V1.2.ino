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
byte motorCentral = 0; // posicao central do motor de direcao
int valorDirecao = 0; // valor do potenciometro da direcao 

// *** Definicao dos pinos para luzes ***
const int frenteEsquerda = 12; // seta da frente do lado esquerdo
const int frenteDireita = A5; // seta da frente do lado direito
const int farois = 3; // farois
const int atrasEsquerda = 10; // luz traseira do lado esquerdo
const int atrasDireita = 11; // luz traseira do lado direito
const int re = A3; // luz de re
const int ldr = A4; // sensor crepuscular

// *** Definicao das variaveis para luzes ***
int valorLDR = 0; // valor do LDR
int gatilhoLDR = 250; // ponto onde o sensor crepuscular eh ativado
int gatilhoLDR2 = 90; // ponto onde o sensor crepuscular eh ativado
byte luzesAuto = 0; // sensor crepuscular ativado
byte lanterna = 0; // luzes de lanterna e farol baixo
byte farolAlto = 0; // farol alto
byte farolBaixo = 0; // farol baixo
byte luzRe = 0; // luz de re
byte piscaAlerta = 0; // pisca alerta
byte freio = 0; // freio para as luzes traseiras
byte piscaDireito = 0; // pisca do lado direito
byte piscaEsquerdo = 0; // pisca do lado esquerdo
byte flipFarol = 0; // alterna entre farol alto e baixo ou desligado
byte valorBaixo = 50; // PWM para farol baixo e lanterna
byte ctrlSeta = 0; // controle da ativacao das setas
byte ctrlFreio = 0; // controle da ativacao do freio
byte ctrlFarolAlto = 0; // controle da ativacao do farol Alto
byte ctrlFarolBaixo = 0; // controle da ativacao do farol Baixo
byte delayAceso = 50; // quantidade de tempo que permanecera acesso
byte delayApagado = 75; // quantidade de temppo que permanecera apagado

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

  if(valorLDR < gatilhoLDR) {
    luzesAuto = 1;
  }
  else luzesAuto = 0;
  
  // Rotinas de setas de direcao
  if(piscaAlerta == 1) {
    ligaPiscaAlerta();
    ctrlSeta = 1;
  }

  else if(piscaAlerta == 0 && piscaDireito == 1) { 
    ligaPiscaDireito();
    ctrlSeta = 1;
  }

  else if(piscaAlerta == 0 && piscaEsquerdo == 1) { 
    ligaPiscaEsquerdo();
    ctrlSeta = 1;
  }

  // Rotina de Freio
  if(freio == 1 && ctrlSeta == 0) {
    acendeFreio();
  }
  else if(freio == 0 && lanterna == 0 && ctrlSeta == 0) {
    apagaFreio();
  }
  else if(freio == 0 && lanterna == 1 && ctrlSeta == 0) {
    apagaFreio1();
  }

  // Rotina de Farol Alto
  if(farolAlto == 1) {
    acendeFarolAlto();
  }
  else if(farolAlto == 0 && farolBaixo == 0 && luzesAuto == 0) {
    apagaFarolAlto();
  }
  else if(farolAlto == 0 && farolBaixo == 1 && luzesAuto == 1) {
    apagaFarolAlto1();
  }
  else if(farolAlto == 0 && farolBaixo == 0 && luzesAuto == 1) {
    apagaFarolAlto1();
  }

  // Rotina de Farol Baixo
  if(farolBaixo == 1 && farolAlto == 0) {
    acendeFarolBaixo();
  }
  else if(farolBaixo == 0 && farolAlto == 0 && luzesAuto == 1) {
    acendeFarolBaixo();
  }
  else if(farolBaixo == 0 && farolAlto == 0 && luzesAuto == 0) {
    apagaFarolBaixo();
  }

  // Rotina de Lanterna
  if(lanterna == 1 && ctrlSeta == 0 && freio == 0) {
    acendelanterna();
  }
  else if(lanterna == 0 && ctrlSeta == 0 && freio == 0 && luzesAuto == 1) {
    acendelanterna();
  }
  else if(lanterna == 0 && ctrlSeta == 0 && freio == 0 && luzesAuto == 0) {
    apagalanterna();
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
  luzesAuto = 0;

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
    // Ligar lanterna e farol baixo
    if(incomingByte == 248) {
      lanterna = 1;
      farolBaixo = 1;
    }
    // Desligar lanterna, farol baixo, pisca alerta e alarme
    if(incomingByte == 251) {
      lanterna = 0;
      farolBaixo = 0;
      piscaAlerta = 0;
    }
    // Pisca Alerta
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
void ligaPiscaEsquerdo() {
  digitalWrite(frenteEsquerda, HIGH);
  analogWrite(atrasEsquerda, 255);
  delay(delayAceso);
  digitalWrite(frenteEsquerda, LOW);
  analogWrite(atrasEsquerda, 0);
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

void acendeFarolAlto() {
  analogWrite(farois, 255);
}     

void apagaFarolAlto() {
  analogWrite(farois, 0);
} 

void apagaFarolAlto1() {
  analogWrite(farois, valorBaixo);
}  

void acendeFarolBaixo() {
  analogWrite(farois, valorBaixo);
}     

void apagaFarolBaixo() {
  analogWrite(farois, 0);
} 

void acendelanterna() {
  analogWrite(atrasEsquerda, valorBaixo);
  analogWrite(atrasDireita, valorBaixo);
}

void apagalanterna() {
  analogWrite(atrasEsquerda, 0);
  analogWrite(atrasDireita, 0);
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





























