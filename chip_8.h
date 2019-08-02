#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

/**
 * @brief Chip_8
 */
typedef struct _chip_8* Chip_8;

/**
 * @brief Allocate new structure Chip8
 * @return pointer to the structure
 */
Chip_8 initChip(void);

/**
 * @brief Frees memory allocated for the Chip
 * @param chip
 */
void destroyChip(Chip_8 chip);

/**
 * @brief Reads a file with a program
 * @param chip
 * @param filename
 */
void loadProgram(Chip_8 chip, const char *filename);

/**
 * @brief Check if the binaryArray is drawn
 * @param chip
 * @return 1 if it's drawn, 0 if not
 */
uint8_t isDrawn(Chip_8 chip);

/**
 * @brief Reach chip keys for keyboard event
 * @param chip
 * @return pointer to the array of keys of the Chip
 */
uint8_t* getKeys(Chip_8 chip);

/**
 * @brief Reach chip graphics array
 * @param chip
 * @return pointer to the binary array of the chip
 */
uint8_t* getGraphic(Chip_8 chip);

/**
 * @brief Read and execute one instruction
 * @param chip
 */
void execute(Chip_8 chip);


#endif // CHIP_8_H
