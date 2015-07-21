#include "eeprom.h"

#include <avr/eeprom.h>
#include <stdio.h>
#include <avr/pgmspace.h>

enum eeprom_map_t
{
  EEPROM_CAL_PROG,
  EEPROM_CAL_XMIN,
  EEPROM_CAL_XMAX,
  EEPROM_CAL_YMIN,
  EEPROM_CAL_YMAX
};

struct eeprom_map_item { uint8_t item, size; };

#define EEPROM_MAP_START struct eeprom_map_item eeprom_map[] = {
#define EEPROM_ITEM(item, size) {EEPROM_ ## item, size}
#define EEPROM_MAP_END };

EEPROM_MAP_START
  EEPROM_ITEM(CAL_PROG, sizeof(bool)),
  EEPROM_ITEM(CAL_XMIN, sizeof(int16_t)),
  EEPROM_ITEM(CAL_XMAX, sizeof(int16_t)),
  EEPROM_ITEM(CAL_YMIN, sizeof(int16_t)),
  EEPROM_ITEM(CAL_YMAX, sizeof(int16_t))
EEPROM_MAP_END

#define EEPROM_MAP_SIZE (sizeof(eeprom_map)/sizeof(*eeprom_map))

bool EEPROM::hasCalibration()
{
  int16_t read = read_byte(EEPROM_CAL_PROG);

  return read == 1;
}

void EEPROM::clearCalibration()
{
  write_byte(EEPROM_CAL_PROG, 0);
}

void EEPROM::saveCalibration(int16_t xMin, int16_t xMax, int16_t yMin, int16_t yMax)
{
  write_byte(EEPROM_CAL_PROG, 1);
  write_word(EEPROM_CAL_XMIN, xMin);
  write_word(EEPROM_CAL_XMAX, xMax);
  write_word(EEPROM_CAL_YMIN, yMin);
  write_word(EEPROM_CAL_YMAX, yMax);
}

void EEPROM::loadCalibration(int16_t * xMin, int16_t * xMax, int16_t * yMin, int16_t * yMax)
{
  *xMin = read_word(EEPROM_CAL_XMIN);
  *xMax = read_word(EEPROM_CAL_XMAX);
  *yMin = read_word(EEPROM_CAL_YMIN);
  *yMax = read_word(EEPROM_CAL_YMAX);
}

uint16_t EEPROM::getAddr(uint8_t item)
{
  uint16_t addr = 0;

  for(uint8_t i = 0; i < EEPROM_MAP_SIZE; i++)
  {
    if(eeprom_map[i].item == item)
      return addr;

    addr += eeprom_map[i].size;
  }

  printf_P(PSTR("EEPROM: could not find item %hu\n"), item);
  while(1);
}

uint16_t EEPROM::read_word(uint8_t item)
{
  uint16_t addr = getAddr(item);

  return eeprom_read_word((const uint16_t *)addr);
}

uint8_t EEPROM::read_byte(uint8_t item)
{
  uint16_t addr = getAddr(item);

  return eeprom_read_byte((const uint8_t *)addr);
}

void EEPROM::write_byte(uint8_t item, uint8_t data)
{
  uint16_t addr = getAddr(item);

  eeprom_write_byte((uint8_t *)addr, data);
}

void EEPROM::write_word(uint8_t item, uint16_t data)
{
  uint16_t addr = getAddr(item);

  eeprom_write_word((uint16_t *)addr, data);
}
