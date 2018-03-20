//
// Created by bychin on 19.03.18.
//

#include "chip8.h"

#include <fstream>
#include <iostream>


chip8::chip8() {
    std::cout << "Booting...\n";
    mt.seed(std::random_device()());
    // All pointers and data have been already cleared

    pc = 0x200;

    // Load fonts (starting point - 0x050 == 80)
    for (int i = 0; i < 80; ++i) {
        memory[i + 0x050] = fontset[i];
    }

    // Reset timers?
};

void chip8::LoadGame(std::string& game_name) {
    std::cout << "Loading game " << game_name << "\n";
    std::ifstream game_file;
    game_file.open("./ROMs/" + game_name, std::ios::in | std::ios::binary | std::ios::ate);

    std::streampos size = game_file.tellg();
    game_file.seekg(0, std::ios::beg);
    char temp_storage[static_cast<int>(size)];
    game_file.read(temp_storage, size);
    if (!game_file) {
        std::cerr << "Cannot read game's file!\n";
    }

    for (int i = 0; i < size; ++i) {
        memory[i + 0x200] = static_cast<uint8_t>(temp_storage[i]);
    }

    game_file.close();
}

void chip8::EmulateCycle() {
    // Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

    // Decode opcode
    // wxyz wxnn wnnn
    uint8_t  w   = (opcode >> 12) & 0xF;
    uint8_t  x   = (opcode >> 8) & 0xF;
    uint8_t  y   = (opcode >> 4) & 0xF;
    uint8_t  z   = opcode & 0xF;
    uint8_t  nn  = opcode & 0xFF;
    uint16_t nnn = opcode & 0xFFF;

    // TODO: make function pointers instead
    if (w == 0x0) { // 0---
        if (opcode == 0x00E0) { // Clears the screen
            for (auto& pixel : graphics)
                pixel = 0;
            draw_flag = true;

        } else if (opcode == 0x00EE) { // Returns from subroutine
            --sp;
            pc = stack[sp];
        } else { // 0NNN - Calls RCA 1802 program at NNN
            // Not necessary for most ROMs
        };
    } else if (w == 0x1) { // 1NNN - Jumps to address NNN
        pc = nnn;
    } else if (w == 0x2) { // 2NNN - Calls subroutine at NN
        stack[sp] = pc;
        ++sp;
        pc = nnn;
    } else if (w == 0x3) { // 3XNN - Skips the next instruction if (VX == NN)
        if (V[x] == nn)
            pc += 2;
    } else if (w == 0x4) { // 4XNN - Skips the next instruction if (VX != NN)
        if (V[x] != nn)
            pc += 2;
    } else if (w == 0x5) { // 5XY0 - Skips the next instruction if (VX == VY)
        if (V[x] == V[y])
            pc += 2;
    } else if (w == 0x6) { // 6XNN - Sets VX to NN
        V[x] = nn;
    } else if (w == 0x7) { // 7XNN - Adds NN to VX
        V[x] += nn;
    } else if (w == 0x8) { // 8---
        if (z == 0x0) { // 8XY0 - Sets VX to VY
            V[x] = V[y];
        } else if (z == 0x1) { // 8XY1 - Sets VX to VX | VY
            V[x] |= V[y];
        } else if (z == 0x2) { // 8XY2 - Sets VX to VX & VY
            V[x] &= V[y];
        } else if (z == 0x3) { // 8XY3 - Sets VX to VX xor VY
            V[x] ^=V[y];
        } else if (z == 0x4) { // 8XY4 - Adds VY to VX
            uint16_t sum = V[x] + V[y];
            V[0xF] = sum >> 8;
            V[x] = sum & 0xFF;
        } else if (z == 0x5) { // 8XY5 - Subs VY from VX
            uint16_t sub = V[x] - V[y];
            V[0xF] = !(sub >> 8);
            V[x] = sub & 0xFF;
        } else if (z == 0x6) { // 8XY6 - Shifts VY right by 1 and stores in VX
            V[0xF] = V[y] & 0x1;
            V[x] = (V[y] >> 1);
        } else if (z == 0x7) { // 8XY7 - Sets VX to VY - VX
            uint16_t sub = V[y] - V[x];
            V[0xF] = !(sub >> 8);
            V[x] = sub & 0xFF;
        } else if (z == 0xE) { // 8XYE - Shifts VY left by 1 and stores in VX
            V[0xF] = V[y] >> 7;
            V[x] = (V[y] << 1);
        };
    } else if (w == 0x9) { // 9---
        if (z == 0x0) { // 9XY0 - Skips the next instruction if (VX != VY)
            if (V[x] != V[y])
                pc += 2;
        };
    } else if (w == 0xA) { // ANNN - Sets NNN to I
        I = nnn;
    } else if (w == 0xB) { // BNNN - Jumps to address NNN + V0
        pc = nnn + V[0x0];
    } else if (w == 0xC) { // CXNN - Sets VX to rand() & NN
        V[x] = std::uniform_int_distribution<uint8_t>(0, 255)(mt) & nn;
    } else if (w == 0xD) { // DXYN - Draws a sprite at [VX, VY] that has 8px width and Npx height
        V[0xF] = 0x0;
        uint8_t pixel_x = V[x];
        uint8_t pixel_y = V[y];

        for (int line = 0; line < z; ++line) {
            uint8_t sprite_line = memory[I + line];
            uint16_t position = (pixel_y + line) * 64 + pixel_x;
            for (int i = 0; i < 8; ++i) {
                if ((sprite_line & (0x80 >> i)) != 0) {
                    if(graphics[position + i] == 1)
                        V[0xF] = 1;
                    graphics[position + i] ^= 1;
                }
            }
        }
        draw_flag = true;
    } else if (w == 0xE) { // E---
        if (nn == 0x9E) { // EX9E - Skips the next instruction if (VX == key())

        } else if (nn == 0xA1) { // EXA1 - Skips the next instruction if (VX != key())

        };
    } else if (w == 0xF) { // F---

    }


    // Execute opcode

    // Update timers
}

void chip8::SetKeys() {}
