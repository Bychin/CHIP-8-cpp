//
// Created by bychin on 19.03.18.
//

#include "chip8.h"

#include <fstream>
#include <iostream>


chip8::chip8() {
    std::cout << "Booting...\n";
    // All pointers and data have been already cleared

    pc = 0x200;

    // Clear display

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
    char temp_storage[size];
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
    if (w == 0) { // 0---
        if (opcode == 0x00E0) { // Clears the screen
            for (int i = 0; i < 2048; ++i)
                graphics[i] = 0;
            draw_flag = true;

        } else if (opcode == 0x00EE) { // Returns from subroutine
            --sp;
            pc = stack[sp];
        } else { // 0NNN - Calls RCA 1802 program at NNN
            // Not necessary for most ROMs
        }
    } else if (w == 1) { // 1NNN - Jumps to address NNN
        pc = nnn;
    } else if (w == 2) { // 2NNN - Calls subroutine at NN
        ++sp;
        stack[sp] = pc;
        pc = nnn;
    } else if (w == 3) { // 3XNN - Skips the next instruction if (VX == NN)
        if (V[x] == nn)
            pc += 2;
    } else if (w == 4) { // 4XNN - Skips the next instruction if (VX != NN)
        if (V[x] != nn)
            pc += 2;
    } else if (w == 5) { // 5XY0 - Skips the next instruction if (VX == VY)
        if (V[x] == V[y])
            pc += 2;
    } else if (w == 6) { // 6XNN - Sets VX to NN
        V[x] = nn;
    } else if (w == 7) { // 7XNN - Adds NN to VX
        V[x] += nn;
    } else if (w == 8) { // 8---
        if (z == 0x0) { // 8XY0 - Sets VX to VY
            V[x] = V[y];
        } else if (z == 0x1) { // 8XY1 - Sets VX to VX | VY
            V[x] |= V[y];
        } else if (z == 0x2) { // 8XY2 - Sets VX to VX & VY
            V[x] &= V[y];
        } else if (z == 0x3) { // 8XY3 - Sets VX to VX xor VY
            V[x] ^=V[y];
        } else if (z == 0x4) { // 8XY4 - Adds VY to VX

        }
    } else if (w == 9) { // 9---

    } else if (w == 'A') { // A---

    } else if (w == 'B') { // B---

    } else if (w == 'C') { // C---

    } else if (w == 'D') { // D---

    } else if (w == 'E') { // E---

    } else if (w == 'F') { // F---

    }


    // Execute opcode

    // Update timers
}

void chip8::SetKeys() {}
