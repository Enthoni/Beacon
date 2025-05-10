#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
#include "BLEBeacon.h"
#include "esp_err.h"
#include "esp_bt.h"


#define GPIO_BREATHING_LIGHT 2
#define BEACON_UUID "2fc03570-8ae7-407f-a375-3d2d74d8fc0f"
#define ID_FABRICANTE_BEACON 0x4C00
#define MAJOR_BEACON 1
#define MINOR_BEACON 1
#define BEACON_DATA ""
#define BEACON_DATA_SIZE 26
#define BEACON_DATA_TYPE 0xFF


BLEAdvertising *pAdvertising;


void configura_beacon(void);


void configura_beacon(void)
{
    BLEBeacon ble_beacon = BLEBeacon();


    ble_beacon.setManufacturerId(ID_FABRICANTE_BEACON);
    ble_beacon.setProximityUUID(BLEUUID(BEACON_UUID));
    ble_beacon.setMajor(MAJOR_BEACON);
    ble_beacon.setMinor(MINOR_BEACON);


    BLEAdvertisementData advertisement_data = BLEAdvertisementData();
    advertisement_data.setFlags(0x04);


    std::string strServiceData = "";
    strServiceData += (char)BEACON_DATA_SIZE;
    strServiceData += (char)BEACON_DATA_TYPE;
    strServiceData += ble_beacon.getData();


    advertisement_data.addData(strServiceData);


    pAdvertising->setAdvertisementData(advertisement_data);


    // Ajuste da potência de transmissão (TX Power)
    // Níveis de potência disponíveis: ESP_PWR_LVL_N12, ESP_PWR_LVL_N9, ESP_PWR_LVL_N6, ESP_PWR_LVL_N3, ESP_PWR_LVL_N0, ESP_PWR_LVL_P3, ESP_PWR_LVL_P6, ESP_PWR_LVL_P9
    esp_err_t status = esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_N12); // Ajuste conforme necessário
    if (status != ESP_OK) {
        Serial.println("Falha ao ajustar a potência de transmissão");
    } else {
        Serial.println("Potência de transmissão ajustada com sucesso");
    }
}


void setup()
{
    Serial.begin(115200);
    Serial.println("Fazendo inicializacao do beacon...");


    pinMode(GPIO_BREATHING_LIGHT, OUTPUT);
    digitalWrite(GPIO_BREATHING_LIGHT, LOW);


    BLEDevice::init("ESP32 - Beacon BLE");


    BLEServer *pServer = BLEDevice::createServer();


    pAdvertising = BLEDevice::getAdvertising();
    BLEDevice::startAdvertising();


    configura_beacon();


    pAdvertising->start();


    Serial.println("O beacon foi inicializado e já está operando.");
}


void loop()
{
    digitalWrite(GPIO_BREATHING_LIGHT, LOW);
    delay(500);
    digitalWrite(GPIO_BREATHING_LIGHT, HIGH);
    delay(500);
}


