#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "Ultrasonic.h"

#define button1 2
#define button2 3
#define button3 4
#define button4 5
#define button5 6
#define button6 7
#define pino_trigger 8
#define pino_echo 9

Ultrasonic ultrasonic(pino_trigger, pino_echo);
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX

DFRobotDFPlayerMini myDFPlayer;

char buf;
int pausa = 0;
int equalizacao = 0;

void setup(){
    /*pinMode(button1, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(button3, INPUT_PULLUP);
    pinMode(button4, INPUT_PULLUP);
    pinMode(button5, INPUT_PULLUP);
    pinMode(button6, INPUT_PULLUP);
    */
    //Comunicacao serial com o modulo
    mySoftwareSerial.begin(9600);
    //Inicializa a serial do Arduino
    Serial.begin(115200);
    //Verifica se o modulo esta respondendo e se o
    //cartao SD foi encontrado
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini"));
    Serial.println(F("Inicializando modulo DFPlayer... (3~5 segundos)"));
    if (!myDFPlayer.begin(mySoftwareSerial)){
        Serial.println(F("Nao inicializado:"));
        Serial.println(F("1.Cheque as conexoes do DFPlayer Mini"));
        Serial.println(F("2.Insira um cartao SD"));
        while (true);
    }
    Serial.println();
    Serial.println(F("Modulo DFPlayer Mini inicializado!"));
    //Definicoes iniciais
    myDFPlayer.setTimeOut(500); //Timeout serial 500ms
    myDFPlayer.volume(5); //Volume 5
    myDFPlayer.EQ(0); //Equalizacao normal
    //Mostra o menu de comandos
    Serial.println();
    Serial.print("Numero de arquivos no cartao SD: ");
    Serial.println(myDFPlayer.readFileCounts(DFPLAYER_DEVICE_SD));
    menu_opcoes();
}

void loop(){
    //Aguarda a entrada de dados pela serial
    float cmMsec, inMsec;
    long microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    inMsec = ultrasonic.convert(microsec, Ultrasonic::IN);
    //Exibe informacoes no serial monitor
    Serial.print("Distancia em cm: ");
    Serial.print(cmMsec);
    Serial.print(" - Distancia em polegadas: ");
    Serial.println(inMsec);
    ////////////////////////
    while (Serial.available() > 0){
        buf = Serial.read();

        //Reproducao
        if ((buf >= '1') && (buf <= '9')){
            Serial.print("Reproduzindo musica: ");
            Serial.println(buf);
            buf = buf - 48;
            myDFPlayer.play(buf);
            menu_opcoes();
        }

        //Parada
        if (buf == 's'){
            myDFPlayer.stop();
            Serial.println("Musica parada!");
            menu_opcoes();
        }

        //Pausa/Continua a musica
        if (buf == 'p'){
            pausa = !pausa;
            if (pausa == 0){
                Serial.println("Continua musica...");
                myDFPlayer.start();
            }
        
            if (pausa == 1){
                Serial.println("Musica pausada...");
                myDFPlayer.pause();
            }
            menu_opcoes();
        }

        //Seleciona equalizacao
        if (buf == 'e'){
            equalizacao++;
            if (equalizacao == 6){
                equalizacao = 0;
            }
            myDFPlayer.EQ(equalizacao);
            Serial.print("Equalizacao: ");
            Serial.print(equalizacao);
            Serial.println(" (0 = Normal, 1 = Pop, 2 = Rock, 3 = Jazz, 4 = Classic, 5 = Bass)");
            menu_opcoes();
        }

        //Aumenta volume
        if (buf == '+'){
            myDFPlayer.volumeUp();
            Serial.print("Volume atual:");
            Serial.println(myDFPlayer.readVolume());
            menu_opcoes();
        }

        //Diminui volume
        if (buf == '-'){
            myDFPlayer.volumeDown();
            Serial.print("Volume atual:");
            Serial.println(myDFPlayer.readVolume());
            menu_opcoes();
        }
    }
}   

void menu_opcoes(){
    Serial.println();
    Serial.println(F("Comandos:"));
    Serial.println(F(" [1-9] Para selecionar o arquivo MP3"));
    Serial.println(F(" [s] parar reproducao"));
    Serial.println(F(" [p] pausa/continua a musica"));
    Serial.println(F(" [e] seleciona equalizacao"));
    Serial.println(F(" [+ or -] aumenta ou diminui o volume"));
    Serial.println();
}

//https://www.arduinoecia.com.br/modulo-mp3-dfplayer-mini-dfrobot-arduino/
//https://www.filipeflop.com/blog/sensor-ultrassonico-hc-sr04-ao-arduino/