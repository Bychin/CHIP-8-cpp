#include <iostream>
#include <string>

#include "chip8.h"


int main() {
    //SetupGraphics();
    //SetupInput();

    std::string game = "SpaceInvaders";

    chip8& instance = chip8::Instance();
    instance.LoadGame(game);

    while (true) {
        instance.EmulateCycle();

        if (instance.draw_flag) {
            //DrawGraphics();
        }

        //instance.SetKeys();
    }

    return 0;
}