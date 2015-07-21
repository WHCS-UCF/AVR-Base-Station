#ifndef WHCS_EEPROM_H
#define WHCS_EEPROM_H

#include <stdint.h>

class EEPROM
{
  public:
    static void begin();

    // LCD calibration
    static bool hasCalibration();
    static void clearCalibration();
    static void saveCalibration(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax);
    static void loadCalibration(int16_t * xMin, int16_t * xMax, int16_t * yMin, int16_t * yMax);
  private:
    static uint16_t getAddr(uint8_t item);
    static uint16_t read_word(uint8_t item);
    static uint8_t read_byte(uint8_t item);
    static void write_byte(uint8_t item, uint8_t data);
    static void write_word(uint8_t item, uint16_t data);
};

#endif // WHCS_EEPROM_H
