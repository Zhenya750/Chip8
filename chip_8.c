#include "chip_8.h"

static const uint8_t font[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct _chip_8
{
    uint16_t opcode;            /**< current instruction */
    uint8_t memory[4096];       /**< memory */
    uint8_t V[16];              /**< registers */
    uint16_t I;                 /**< the address register */
    uint16_t pc;                /**< program counter */

    uint8_t graphic[64 * 32];   /**< graphics screen */

    uint8_t delay_timer;        /**< delay timer */
    uint8_t sound_timer;        /**< sound timer */

    uint16_t stack[16];
    uint16_t sp;                /**< stack pointer */

    uint8_t key[16];            /**< keyboard input */

    uint8_t drawn;              /**< draw flag */
    uint8_t key_awaited;        /**< key press awaited flag */
};

struct _chip_8* initChip()
{
    struct _chip_8 *chip = (struct _chip_8*) malloc(sizeof(struct _chip_8));

    if(chip == NULL){
        printf("Memory error\n");
        exit(EXIT_FAILURE);
    }

    chip->opcode = 0;
    chip->I      = 0;
    chip->pc     = 0x200;
    chip->sp     = 0;

    for(int i = 0; i < 80; i++){
        chip->memory[i] = font[i];
    }

    for(int i = 80; i < 4096; i++){
        chip->memory[i] = 0;
    }

    for(int i = 0; i < 64 * 32; i++){
        chip->graphic[i] = 0;
    }

    for(int i = 0; i < 16; i++){
        chip->V[i]      = 0;
        chip->stack[i]  = 0;
        chip->key[i]    = 0;
    }

    chip->delay_timer = 0;
    chip->sound_timer = 0;
    chip->drawn       = 0;
    chip->key_awaited = 0;

    return chip;
}

uint8_t isDrawn(struct _chip_8 *chip)
{
    return chip->drawn;
}

uint8_t* getKeys(Chip_8 chip)
{
    return chip->key;
}

uint8_t* getGraphic(Chip_8 chip)
{
    return chip->graphic;
}

void destroyChip(struct _chip_8 *chip)
{
    free(chip);
}

void loadProgram(struct _chip_8 *chip, const char *filename)
{
    if(chip == NULL){
        printf("Chip does not exist!\n");
        exit(EXIT_FAILURE);
    }

    if(filename == NULL){
        printf("Wrong filename!\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(filename, "rb");

    if(file == NULL){
        printf("file does not exist!\n");
        exit(EXIT_FAILURE);
    }

    int32_t c;

    for(int i = 512; i < 4096; i++){
        chip->memory[i] = 0;
    }

    while(1){

        if((c = fgetc(file)) == EOF){
            if(feof(file) == 0){
                printf("Error while reading the file\n");
            }
            break;
        }

        chip->memory[chip->pc++] = (uint8_t)c;
    }

    chip->pc = 0x200;

    if(fclose(file) == EOF){
        printf("Error while closing the file\n");
        exit(EXIT_FAILURE);
    }
}

void execute(struct _chip_8 *chip)
{
    if(chip == NULL){
        printf("Chip does not exist!\n");
        exit(EXIT_FAILURE);
    }

    chip->opcode = (uint16_t)(chip->memory[chip->pc] << 8) | chip->memory[chip->pc + 1];
    chip->drawn  = 0;

    //printf("%d: %04X\n", chip->pc, chip->opcode);

    switch(chip->opcode & 0xF000)
    {
        case 0x0:
        {
            switch(chip->opcode)
            {
                case 0x00E0: // 00E0 - Clears the screen
                {
                    for(int i = 0; i < 64 * 32; i++){
                        chip->graphic[i] = 0;
                    }
                    chip->drawn = 1;
                    chip->pc += 2;
                    break;
                }

                case 0x00EE: // 00EE - RET
                {
                    if(chip->sp == 0){
                        printf("Stack underflow!\n");
                        exit(EXIT_FAILURE);
                    }
                    chip->pc = chip->stack[--chip->sp];
                    chip->pc += 2;
                    break;
                }

                default:
                {
                    printf("Unknown operation: %04X\n", chip->opcode);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }

        case 0x1000: // 1NNN - JMP to address NNN
        {
            chip->pc = chip->opcode & 0x0FFF;
            break;
        }

        case 0x2000: // 2NNN - Call a subroutine at NNN
        {
            if(chip->sp == 16){
                printf("Stack overflow!\n");
                exit(EXIT_FAILURE);
            }
            chip->stack[chip->sp++] = chip->pc;
            chip->pc = chip->opcode & 0x0FFF;
            break;
        }

        case 0x3000: // 3XNN - Skip the next op if VX == NN
        {
            if(chip->V[(chip->opcode & 0x0F00) >> 8] == (chip->opcode & 0x00FF)){
                chip->pc += 2;
            }
            chip->pc += 2;
            break;
        }

        case 0x4000: // 4XNN - Skip the next op if VX != NN
        {
            if(chip->V[(chip->opcode & 0x0F00) >> 8] != (chip->opcode & 0x00FF)){
                chip->pc += 2;
            }
            chip->pc += 2;
            break;
        }

        case 0x5000: // 5XY0 - Skip the next op if VX == VY
        {
            if((chip->opcode & 0x000F) != 0){
                printf("Unknown operation: %04X\n", chip->opcode);
                exit(EXIT_FAILURE);
            }

            if(chip->V[(chip->opcode & 0x0F00) >> 8] == chip->V[(chip->opcode & 0x00F0) >> 4]){
                chip->pc += 2;
            }
            chip->pc += 2;
            break;
        }

        case 0x6000: // 6XNN - Set VX to NN
        {
            chip->V[(chip->opcode & 0x0F00) >> 8] = (chip->opcode & 0x00FF);
            chip->pc += 2;
            break;
        }

        case 0x7000: // 7XNN - Add NN to VX (Carry flag is not changed)
        {
            chip->V[(chip->opcode & 0x0F00) >> 8] += (chip->opcode & 0x00FF);
            chip->pc += 2;
            break;
        }

        case 0x8000:
        {
            switch(chip->opcode & 0x000F)
            {
                case 0x0000: // 8XY0 - Set VX = VY
                {
                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0001: // 8XY1 - Set VX = VX | VY (bitwise or)
                {
                    chip->V[(chip->opcode & 0x0F00) >> 8] |= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0002: // 8XY2 - Set VX = VX & VY (bitwise and)
                {
                    chip->V[(chip->opcode & 0x0F00) >> 8] &= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0003: // 8XY3 - Set VX = VX ^ VY (bitwise xor)
                {
                    chip->V[(chip->opcode & 0x0F00) >> 8] ^= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0004: // 8XY4 - Add VY to VX (VF = 1 if there's carry else VF = 0)
                {
                    if((0xFF - chip->V[(chip->opcode & 0x0F00) >> 8]) < chip->V[(chip->opcode & 0x00F0) >> 4]){
                        chip->V[0xF] = 1;   // carry flag
                    }
                    else{
                        chip->V[0xF] = 0;
                    }

                    chip->V[(chip->opcode & 0x0F00) >> 8] += chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0005: // 8XY5 - VY is subtracted from XY (VF = 0 if there's a borrow else VF = 1)
                {
                    if(chip->V[(chip->opcode & 0x0F00) >> 8] < chip->V[(chip->opcode & 0x00F0) >> 4]){
                        chip->V[0xF] = 0;   // carry flag
                    }
                    else{
                        chip->V[0xF] = 1;
                    }

                    chip->V[(chip->opcode & 0x0F00) >> 8] -= chip->V[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                    break;
                }

                case 0x0006: // 8XY6 - store the least significant bit of VX in VF and then VX >>= 1
                {
                    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] & 0x01;
                    chip->V[(chip->opcode & 0x0F00) >> 8] >>= 1;
                    chip->pc += 2;
                    break;
                }

                case 0x0007: // 8XY7 - VX = VY - VX (VF = 0 if there's a borrow else VF = 1)
                {
                    if(chip->V[(chip->opcode & 0x0F00) >> 8] > chip->V[(chip->opcode & 0x00F0) >> 4]){
                        chip->V[0xF] = 0;   // carry flag
                    }
                    else{
                        chip->V[0xF] = 1;
                    }

                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->V[(chip->opcode & 0x00F0) >> 4] -
                                                            chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;
                }

                case 0x000E: // 8XYE - store the most significant bit of VX in VF and then VX <<= 1
                {
                    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] >> 7;
                    chip->V[(chip->opcode & 0x0F00) >> 8] <<= 1;
                    chip->pc += 2;
                    break;
                }

                default:
                {
                    printf("Unknown operation: %04X\n", chip->opcode);
                    exit(EXIT_FAILURE);
                }
            }

            break;
        }

        case 0x9000: // 9XY0 - Skip the next op if VX != VY
        {
            if((chip->opcode & 0x000F) != 0){
                printf("Unknown operation: %04X\n", chip->opcode);
                exit(EXIT_FAILURE);
            }

            if(chip->V[(chip->opcode & 0x0F00) >> 8] != chip->V[(chip->opcode & 0x00F0) >> 4]){
                chip->pc += 2;
            }
            chip->pc += 2;
            break;
        }

        case 0xA000: // ANNN - Set I to the address NNN
        {
            chip->I = chip->opcode & 0x0FFF;
            chip->pc += 2;
            break;
        }

        case 0xB000: // BNNN - JMP to the address NNN + V0
        {
            chip->pc = (chip->opcode & 0x0FFF) + chip->V[0];
            break;
        }

        case 0xC000: // CXNN - Set VX = rand() & NN (rand in 0..255)
        {
            chip->V[(chip->opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (chip->opcode & 0x00FF);
            chip->pc += 2;
            break;
        }

        case 0xD000: // DXYN - Draw a sprite at (VX, VY) that has a width = 8 pixels and
                     // a height = N pixels. Each row is read from memory location I.
                     // I does not change after execution. VF is set to 1 if any screen
                     // pixels are flipped from set to unset when the sprite is drawn,
                     // and to 0 if that does not happen
        {

            uint8_t x = chip->V[(chip->opcode & 0x0F00) >> 8];
            uint8_t y = chip->V[(chip->opcode & 0x00F0) >> 4];
            uint8_t h = chip->opcode & 0x000F;
            uint8_t spriteRow;

            chip->V[0xF] = 0;
            for(int dy = 0; dy < h; dy++){

                spriteRow = chip->memory[chip->I + dy];
                for(int dx = 0; dx < 8; dx++){

                    if((spriteRow & (0x80 >> dx)) != 0){

                        if(chip->graphic[(x + dx + (y + dy) * 64) % (64 * 32)] == 1){
                            chip->V[0xF] = 1;
                        }
                        chip->graphic[(x + dx + (y + dy) * 64) % (64 * 32)] ^= 1;
                    }
                }
            }

            chip->drawn = 1;
            chip->pc += 2;
            break;
        }

        case 0xE000:
        {
            switch(chip->opcode & 0x00FF)
            {
                case 0x009E: // EX9E - Skip the next op if the key stored in VX is pressed
                {
                    if(chip->key[chip->V[(chip->opcode & 0x0F00) >> 8]] != 0){
                        chip->pc += 2;
                    }
                    chip->pc += 2;
                    break;
                }

                case 0x00A1: // EXA1 - Skip the next op if the key stored in VX isn't pressed
                {
                    if(chip->key[chip->V[(chip->opcode & 0x0F00) >> 8]] == 0){
                        chip->pc += 2;
                    }
                    chip->pc += 2;
                    break;
                }

                default:
                {
                    printf("Unknown operation: %04X\n", chip->opcode);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }

        case 0xF000:
        {
            switch(chip->opcode & 0x00FF)
            {
                case 0x0007: // FX07 - VX = delay_timer
                {
                    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->delay_timer;
                    chip->pc += 2;
                    break;
                }

                case 0x000A: // FX0A - A key press is awaited and then stored in VX
                             // Every op halted until next key event
                {
                    chip->key_awaited = 1;
                    for(uint8_t i = 0; i < 16 && chip->key_awaited == 1; i++){
                        if(chip->key[i] != 0){
                            chip->V[(chip->opcode & 0x0F00) >> 8] = i;
                            chip->key_awaited = 0;
                            chip->pc += 2;
                        }
                    }
                    break;
                }

                case 0x0015: // FX15 - Set delay_timer to VX
                {
                    chip->delay_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;
                }

                case 0x0018: // FX18 - Set sound_timer to VX
                {
                    chip->sound_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;
                }

                case 0x001E: // FX1E - I += VX
                {
                    chip->I += chip->V[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                    break;
                }

                case 0x0029: // FX29 - Set I to the location of the sprite for character in VX
                             // Characters 0..F(hex) are represented by a 4x5 font
                             // font set is stored in memory starting at the address 0
                {
                    chip->I = (chip->V[(chip->opcode & 0x0F00) >> 8] % 0x10) * 0x05;
                    chip->pc += 2;
                    break;
                }

                case 0x0033: // FX33 - Store binary-coded decimal representation of VX
                             // (I + 0) = BCD(3)
                             // (I + 1) = BCD(2)
                             // (I + 2) = BCD(1)
                {
                    chip->memory[chip->I + 0] =  chip->V[(chip->opcode & 0x0F00) >> 8] / 100;
                    chip->memory[chip->I + 1] = (chip->V[(chip->opcode & 0x0F00) >> 8] % 100) / 10;
                    chip->memory[chip->I + 2] =  chip->V[(chip->opcode & 0x0F00) >> 8] % 10;
                    chip->pc += 2;
                    break;
                }

                case 0x0055: // FX55 - Store V0...VX in memory at address I
                             // The offset from I is increased by 1 but I does not change
                {
                    uint8_t reg_count = (chip->opcode & 0x0F00) >> 8;

                    for(int i = 0; i <= reg_count; i++){
                        chip->memory[chip->I + i] = chip->V[i];
                    }
                    chip->pc += 2;
                    break;
                }

                case 0x0065: // FX65 - Fill V0..VX with values from memory at address I
                             // The offset from I is increased by 1 but I does not change
                {
                    uint8_t reg_count = (chip->opcode & 0x0F00) >> 8;

                    for(int i = 0; i <= reg_count; i++){
                        chip->V[i] = chip->memory[chip->I + i];
                    }
                    chip->pc += 2;
                    break;
                }

                default:
                {
                    printf("Unknown operation: %04X\n", chip->opcode);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }

        default:
        {
            printf("Unknown operation: %04X\n", chip->opcode);
            exit(EXIT_FAILURE);
        }
    }

    if(chip->delay_timer > 0){
        chip->delay_timer--;
    }

    if(chip->sound_timer > 0){
        if(chip->sound_timer == 1){
            printf("sound!\n");     //doSound()
        }
        chip->sound_timer--;
    }
}
