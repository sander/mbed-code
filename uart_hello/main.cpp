#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/UARTService.h"

#define NAME "Hello UART"
#define MAX_LENGTH 10
#define MIN(a,b) ((a) < (b) ? (a) : (b))

BLEDevice ble;
UARTService *uartServicePtr;
Ticker echoer;
uint8_t data[MAX_LENGTH] = "(nothing)";
uint16_t len = strlen("(nothing)");

void echo() {
  ble.updateCharacteristicValue(
      uartServicePtr->getRXCharacteristicHandle(),
      data, len);
}

void onConnection(const Gap::ConnectionCallbackParams_t *params) {
  echoer.attach(&echo, 1.0);
}
 
void onDisconnection(const Gap::DisconnectionCallbackParams_t *params) {
  echoer.detach();
  ble.startAdvertising();
}
 
void onDataWritten(const GattWriteCallbackParams *params) {
  if ((uartServicePtr != NULL)
      && (params->handle == uartServicePtr->getTXCharacteristicHandle())) {
    len = params->len;
    memcpy(data, params->data, MIN(MAX_LENGTH, len));
  }
}

int main() {
  ble.init();
  ble.onConnection(onConnection);
  ble.onDisconnection(onDisconnection);
  ble.onDataWritten(onDataWritten);
 
  ble.accumulateAdvertisingPayload(
      GapAdvertisingData::BREDR_NOT_SUPPORTED);
  ble.setAdvertisingType(
      GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.accumulateAdvertisingPayload(
      GapAdvertisingData::SHORTENED_LOCAL_NAME,
      (const uint8_t *)NAME,
      sizeof(NAME) - 1);
  ble.accumulateAdvertisingPayload(
      GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
      (const uint8_t *)UARTServiceUUID_reversed,
      sizeof(UARTServiceUUID_reversed));
  ble.accumulateScanResponse(
      GapAdvertisingData::SHORTENED_LOCAL_NAME,
      (const uint8_t *)NAME,
      sizeof(NAME) - 1);
  ble.accumulateScanResponse(
      GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
      (const uint8_t *)UARTServiceUUID_reversed,
      sizeof(UARTServiceUUID_reversed));

  ble.setDeviceName((const uint8_t *)NAME);
 
  ble.setAdvertisingInterval(Gap::MSEC_TO_GAP_DURATION_UNITS(1000));
  ble.startAdvertising();
 
  UARTService uartService(ble);
  uartServicePtr = &uartService;

  while (true) {
    ble.waitForEvent();
  }
}
