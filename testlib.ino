#include "testlib.h"

#include <SPI.h>

void sendData(uint8_t chip_select, uint8_t address, uint8_t value) {
    digitalWrite(chip_select, LOW);
    SPI.transfer(address);
    SPI.transfer(value);
    digitalWrite(chip_select, HIGH);
}