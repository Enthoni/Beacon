#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"


// Variável global para armazenar o último dispositivo detectado
String lastDeviceName = "";


int scanTime = 1; // In seconds
BLEScan* pBLEScan;
HardwareSerial mySoftwareSerial(1); /* UART1 (0), UART2 (1), UART3 (2). */
DFRobotDFPlayerMini myDFPlayer; /* Cria a variável "myDFPlayer" */


// Estrutura para armazenar o mapeamento entre nomes de dispositivos e números de faixas
struct DeviceMusicMapping {
    String deviceName;
    int musicTrack;
};


// Lista de mapeamentos entre nomes de dispositivos e números de faixas
DeviceMusicMapping musicMappings[] = {
    {"ESP32 - Beacon BLE", 2}, // Nome do dispositivo e número da faixa correspondente
    {"ESP32 - Biblioteca", 3},
    // Adicione mais mapeamentos conforme necessário
};


// Função para encontrar o número da faixa de música com base no nome do dispositivo
static int findMusicTrack(String deviceName) {
    for (size_t i = 0; i < sizeof(musicMappings) / sizeof(DeviceMusicMapping); i++) {
        if (musicMappings[i].deviceName == deviceName) {
            return musicMappings[i].musicTrack;
        }
    }
    return -1; // Retorna -1 se o dispositivo não estiver mapeado para nenhuma faixa de música
}


// Classe MyAdvertisedDeviceCallbacks
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        if (advertisedDevice.haveName()) {
            String deviceName = String(advertisedDevice.getName().c_str()); // Convertendo para String do Arduino
            int rssi = advertisedDevice.getRSSI(); // Obtém o RSSI (potência do sinal)


            Serial.printf("Dispositivo encontrado: %s\n", deviceName.c_str());
            Serial.printf("Potência do sinal (RSSI): %d dBm\n", rssi); // Imprime a potência do sinal


            // Verifica se o RSSI é menor que -85 e se o dispositivo é diferente do último detectado
            if (rssi > -94 && deviceName != lastDeviceName) {
                int musicTrack = findMusicTrack(deviceName); // Acesso direto à função findMusicTrack
                if (musicTrack != -1) {
                    Serial.printf("Iniciando reprodução da faixa de música: %d\n", musicTrack);
                    myDFPlayer.play(musicTrack); // Inicia a reprodução da música correspondente
                    lastDeviceName = deviceName; // Atualiza o último dispositivo detectado
                } else {
                    Serial.println("Dispositivo encontrado, mas sem faixa de música correspondente.");
                }
            }
        }
    }
};






void setup() {
    mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);  /* velocidade, tipo de comunicação, pinos RX, TX */
    Serial.begin(115200);                                                    
    Serial.println();
    Serial.println(F("Iniciando módulo DFPlayer ..."));
    Serial.println("Scanning...");
    if (!myDFPlayer.begin(mySoftwareSerial))
    {  /* Verifica o funcionamento do módulo.  Se não for capaz de identificar o módulo */
        Serial.println(myDFPlayer.readType(), HEX);
        Serial.println(F("Erro ao iniciar, verifique:"));
        Serial.println(F("1. A conexao do modulo."));
        Serial.println(F("2. Se o SD card foi inserido corretamente."));
        while (true);
    }
    Serial.println(F("DFPlayer Mini online.")); /* Ao identificar o funcionamento do módulo */
    myDFPlayer.setTimeOut(500); /* Ajusta o tempo de comunicação para 500ms */
    /* ----Ajuste do Volume---- */
    myDFPlayer.volume(22);  /* Volume de 0 a 30. */
    myDFPlayer.volumeUp();  /* Volume Up */
   
    /*----Ajusta o Equalizador---- */
    myDFPlayer.EQ(0); /*  0 = Normal, 1 = Pop, 2 = Rock, 3 = Jazz, 4 = Classic, 5 = Bass */
   
    /*----Define o dispositivo---- */
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    /*----Quantidade de Arquivos---- */
    Serial.print(F("Numero de arquivos no cartao SD: "));
    Serial.println(myDFPlayer.readFileCounts(DFPLAYER_DEVICE_SD));


    // Inicializa a música
    // myDFPlayer.play(2);  /* (1) Le o primeiro arquivo no cartão , se inserir (2) lê o segundo*/


    /*----Inicialização da parte Bluetooth---- */
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); // Create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // Active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // Less or equal setInterval value
}


void loop() {
    BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Dispositivos encontrados: ");
    Serial.println(foundDevices->getCount());
    Serial.println("Varredura concluída!");
    pBLEScan->clearResults();   // Deleta resultados do buffer da varredura BLE para liberar memória
    delay(1000);
}










