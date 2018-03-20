//
// Created by bychin on 19.03.18.
//

#ifndef CHIP_8_CPP_CHIP8_H
#define CHIP_8_CPP_CHIP8_H

#include <string>
#include <random>


class chip8 {

    /* CHIP-8 memory map:
     * 0x000-0x1FF - CHIP-8 interpreter
     * 0x050-0x0A0 - Font data (0 - F)
     * 0x200-0xFFF - Program ROM and work RAM
     */

    uint8_t memory[4096]{}; //CHIP-8 memory, 4096 memory locations of 8 bits

    uint16_t opcode = 0; // 2 bytes long CHIP-8 opcode
    uint8_t V[16]{}; // 8-bit data registers

    uint16_t I = 0; // address register
    uint16_t pc = 0; // program counter

    uint16_t stack[16]{};
    uint16_t sp = 0; // stack pointer

    // 60 Hz timers
    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t key[16]; // hex keyboard

    // CHIP-8 has black and white screen with 64x32 resolution (2048 pixels)
    uint8_t graphics[64 * 32]{};

    std::mt19937 mt;

    chip8();
    ~chip8() = default;

public:
    chip8(chip8 const&) = delete;
    chip8& operator= (chip8 const&) = delete;

    static chip8& Instance() {
        static chip8 chip;
        return chip;
    }

    bool draw_flag;

    void LoadGame(std::string&);
    void EmulateCycle();
    void SetKeys();
};

const uint8_t fontset[80] = {
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


#endif //CHIP_8_CPP_CHIP8_H
