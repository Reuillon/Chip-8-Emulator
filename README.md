A CHIP-8 Emulator written in C++

To Compile this code you will need SFML:

On MingW:
g++ main.cpp -o emulator.exe -lsfml-graphics -lsfml-window -lsfml-system -lopengl32 -lwinmm -lgdi32

On Linux:
g++ main.cpp -o emulator.out -lsfml-graphics -lsfml-window -lsfml-system

to run: 
in your terminal enter ./emulator.exe (FOR WINDOWS) or ./emulator.out with the path of the rom you are attempting to load

e.g. "./emulator.exe roms/BRIX"

Video demonstration:
https://www.youtube.com/watch?v=oFBpil8n-cA

Originally written in August 2020
