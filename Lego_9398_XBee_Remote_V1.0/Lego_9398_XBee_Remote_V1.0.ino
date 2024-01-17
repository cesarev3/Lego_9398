/*
*******************************************************
 Programa de recepcao de comandos Via XBee - Game Pad
 Acionador de motores com Ponte H
 Autor Magneto - Outubro 2016
 *******************************************************
 */
// Definicao dos pinos analogicos
const int pot1 = A0; // potenciometro de direcao
const int ldr = A4; // sensor crepuscular
const int sensorAlarme = A5; // sensor alarme (portas)

// Definicao dos pinos digitais

// Sentido dos motores de deslocamento - (Motor B)
const int mB1 = 4;
const int mB2 = 6;

// Sentido do motor de direcao - (Motor A)
const int mA1 = 8;
const int mA2 = 7;

// Velocidades dos motores A e B (PWM)
const int vMA = 9;
const int vMB = 5;

// Identifica posicao central direcao
const int sensorCentro = 2;

// LED Sensor Centro
//const int ledCentro = 13;

// Leds de iluminacao
const int setaEsquerda = 12;
const int setaDireita = 13;
const int farois = 3;
const int trasEsquerda = 10;
const int trasDireita = 11;

// Alarme
const int buzzer = A6;
const int ledAlarme = A7;

int cEnergyMA = 6; // coeficiente de energia dado pelo range do driver do motor A (0 a 255)
int cEnergyMB = 8; // coeficiente de energia dado pelo range do driver do motor B (0 a 255)
int cEnergyCentro = 50; // coeficiente de energia de retorno ao centro

// Escalas dos joysticks
int scaleJoy2 = 0; // deslocamento
int scaleJoy3 = 0; // direcao

// Trimagem dos Joysticks - ver programa do TX
int trimJoy2 = 98;
int trimJoy3 = 157;

// Variavel de entrada de dados pela Serial
int incomingByte;

// Variavel para armazenar a informacao do centro
boolean motorCentral = 0;
int valorDirecao = 0;

// Memoria da posicao central
int memoriaCentral = 0;
int scaleMemory1 = 0;

// Variaveis de luzes
byte entradaLuzes = 0;
byte luzes = 0;
byte farolAlto = 0;
byte piscaAlerta = 0;
byte freio = 0;
byte piscaDireito = 0;
byte piscaEsquerdo = 0;
byte flipFarol = 0;
byte valorBaixo = 50;

// controle de tempo
unsigned int segundo1;
unsigned int segundo2;
unsigned int segundo3;
unsigned int segundo4;
unsigned int segundo = 0;
unsigned long calculoTempo;

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
  // pinMode(ledCentro, OUTPUT);
  pinMode(setaEsquerda,OUTPUT);
  pinMode(setaDireita,OUTPUT);
  pinMode(farois,OUTPUT);
  pinMode(trasEsquerda,OUTPUT);
  pinMode(trasDireita,OUTPUT);
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
 // entradaLuzes = analogRead(ldr);
  
 // Rotinas de Pisca
 if(piscaAlerta == 1 && luzes == 0) {
  ligaPiscaAlerta();
  }
  else if(piscaAlerta == 1 && luzes == 1) { 
  ligaPiscaAlerta();
  }
  
// Controle Farol Alto
if(farolAlto == 1) {
  acendeFarolAlto();
}
else if(farolAlto == 0) {
  apagaFarolAlto();
}

// Pisca Alerta
if(piscaAlerta == 1) {
  ligaPiscaAlerta();
}
else if(piscaAlerta == 0) {
  desligaPiscaAlerta();
}

// Pisca Esquerdo
if(piscaEsquerdo == 1) {
  ligaPiscaEsquerdo();
}
else if(piscaEsquerdo == 0) {
  desligaPiscaEsquerdo();
}

// Pisca Direito
if(piscaDireito == 1) {
  ligaPiscaDireito();
}
else if(piscaDireito == 0) {
  desligaPiscaDireito();
}

// Freio
if(freio ==1) {
  acendeFreio();
}
else if(freio == 0) {
  apagaFreio();
}

// Controle de luzes
//if(luzes == 1) {
//  acendeLuzes();
//}
else if(luzes == 0) {
  apagaLuzes();
}

  // Verificando se tem entrada de dados na Serial
  if (Serial.available() > 0) {
    // Lendo o ultimo dado recebido
    incomingByte = Serial.read();

    // Identificar J2UpDown - Joystick da Esquerda
    if(incomingByte >= 68 && incomingByte <= 127) {
      scaleJoy2 = (incomingByte - trimJoy2);

      if(scaleJoy2 > 0) {
        digitalWrite(mB1,HIGH);
        digitalWrite(mB2,LOW);
        freio = 0;
      }
      else if(scaleJoy2 < 0) {
        scaleJoy2 = scaleJoy2 * -1;
        digitalWrite(mB1,LOW);
        digitalWrite(mB2,HIGH);
        freio = 0;
      }
      else if(scaleJoy2 == 0) {
        freio = 1;
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
      // retorno automatico para centro da direita
      if(scaleJoy3 == 0 && valorDirecao < 547 && scaleJoy2 != 0 && motorCentral == 0) {
        // corrigindo para a esquerda
        digitalWrite(mA1,HIGH);
        digitalWrite(mA2,LOW);
        analogWrite(vMA, cEnergyCentro);
      }
      // retorno automatico para centro da esquerda
      if(scaleJoy3 == 0 && valorDirecao > 577 && scaleJoy2 !=0 && motorCentral == 0) {
        // corrigindo para a direita
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
  //delay(500);

}

void acendeLuzes() {
      if(farolAlto == 1) {
       digitalWrite(farois, 255);
       digitalWrite(trasEsquerda, valorBaixo);
       digitalWrite(trasDireita, valorBaixo);
}
else if(farolAlto == 0) {
       digitalWrite(farois, valorBaixo);
       digitalWrite(trasEsquerda, valorBaixo);
       digitalWrite(trasDireita, valorBaixo);
}
}

void apagaLuzes() {
      if(farolAlto == 1) {
       digitalWrite(farois, 255);
       digitalWrite(trasEsquerda, 0);
       digitalWrite(trasDireita, 0);
}
else if(farolAlto == 0) {
       digitalWrite(farois, 0);
       digitalWrite(trasEsquerda, 0);
       digitalWrite(trasDireita, 0);
}
}

void acendeFarolAlto() {
      digitalWrite(farois, 255);
}      

void apagaFarolAlto() {
      if(luzes == 1) {
        digitalWrite(farois, valorBaixo);
}  
else if(luzes == 0) {
        digitalWrite(farois, 0);
}
}

void ligaPiscaAlerta() {
      digitalWrite(setaEsquerda, HIGH);
      digitalWrite(trasEsquerda, 255);
      digitalWrite(setaDireita, HIGH);
      digitalWrite(trasDireita, 255);
}

void desligaPiscaAlerta() {
      if(luzes == 1) {
        digitalWrite(setaEsquerda, LOW);
        digitalWrite(trasEsquerda, valorBaixo);
        digitalWrite(setaDireita, LOW);
        digitalWrite(trasDireita, valorBaixo);
}
else if(luzes == 0) {
        digitalWrite(setaEsquerda, LOW);
        digitalWrite(trasEsquerda, 0);
        digitalWrite(setaDireita, LOW);
        digitalWrite(trasDireita, 0);
}
}

void acendeFreio() {
      digitalWrite(trasEsquerda, 255);
      digitalWrite(trasDireita, 255);
}

void apagaFreio() {
  if(luzes == 1) {
        digitalWrite(trasEsquerda, valorBaixo);
        digitalWrite(trasDireita, valorBaixo);
}
else if(luzes == 0) {
        digitalWrite(trasEsquerda, 0);
        digitalWrite(trasDireita, 0);
}
}
   
void ligaPiscaEsquerdo() {
      digitalWrite(setaEsquerda, HIGH);
      digitalWrite(trasEsquerda, 255);
}  

void ligaPiscaDireito() {
      digitalWrite(setaDireita, HIGH);
      digitalWrite(trasDireita, 255);
}  

void desligaPiscaEsquerdo() {
      if(luzes == 1) {
      digitalWrite(setaEsquerda, LOW);
      digitalWrite(trasEsquerda, valorBaixo);
}  
else if(luzes == 0) {
      digitalWrite(setaEsquerda, LOW);
      digitalWrite(trasEsquerda, LOW);
}
}

void desligaPiscaDireito() {
      if(luzes == 1) {
      digitalWrite(setaDireita, LOW);
      digitalWrite(trasDireita, valorBaixo);
}  
else if(luzes == 0) {
      digitalWrite(setaDireita, LOW);
      digitalWrite(trasDireita, LOW);
}
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





















