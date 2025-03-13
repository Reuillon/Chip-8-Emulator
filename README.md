A CHIP-8 Emulator written in C++

To Compile this code you will need SFML:

On MingW:
g++ main.cpp -o emulator.exe -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32

On Linux:
g++ main.cpp -o emulator.out -lsfml-graphics -lsfml-window -lsfml-system

to run: 
in your terminal enter ./emulator.exe (FOR WINDOWS) or ./emulator.out with the rom you are trying to load

e.g. "./emulator.exe roms/BRIX.ch8"

Originally written in August 2020
