#include <fstream>
#include <iostream>
#include <cstdlib> 
#include <SFML/Graphics.hpp>

//SET WINDOW SIZE
int scale = 20;
sf::RenderWindow window(sf::VideoMode(64 * scale, 32 * scale), "Chip-8");
using  std::cout;


//CLASS FOR DRAWING PIXELS USING SFML
//init(x,y)
//delP(x,y)
class pixel
{
	sf::RectangleShape sq;
	sf::RectangleShape del;
	public:
	//CREATES PIXEL AT GIVEN COORDINATES
	void init(int x, int y)
	{
		sq.setSize(sf::Vector2f(1 * scale,1 * scale));
		sq.setPosition(x * scale,y * scale);
		window.draw(sq);
	}
	
	//DELETES PIXEL AT GIVEN COORDINATES
	void delP(int x, int y)
	{
		del.setFillColor(sf::Color::Black);
		del.setSize(sf::Vector2f(1 * scale, 1 * scale));
		del.setPosition(x * scale, y * scale);
		window.draw(del);
	}
};


//PROGRAM MEMORY (LAZY METHOD)
unsigned short *op;
int size;


class emulator
{
	private:
		unsigned char font[80] = 
		{
			0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
			0x20, 0x60, 0x20, 0x20, 0x70,		// 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
			0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
			0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
			0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
			0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
			0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
			0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
			0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
			0xF0, 0x80, 0xF0, 0x80, 0x80		// F
		};
	
		
		char16_t opcode;	
	
		unsigned char memory[4096] = {0};
		unsigned char v[16] = {0};

		char16_t i = 0x0;
		char16_t stack[16] = {0};
		bool display[64][32] = {0};
		char16_t pc = 0x200;
	
		bool keys[16] = {0};
		int sp = 0;
		int delay_timer = 0;
		int sound_timer = 0;
		int timer = 0;
		pixel d;
	
	public:
		
		//SETS ALL MEMORY CELLS AND REGISTERS TO 0
		void clearMem()
		{
			for (int i = 0; i < sizeof(memory); i++)
			{
				memory[i]=0;
				if (i < 16){keys[i] = 0; stack[i] = 0; v[i] = 0;}
			}
			for (int j = 0; j < 80; j++)
			{
				memory[j + 0x50] = font[j];
			}
		}
	
		//LOADS PROGRAM INTO MEMORY
		void loadProg()
		{
			clearMem();
			cout << "\n\n\n";
			unsigned short ins;
			for (int i = 0; i < size*2; i+=2)
			{
				ins = *(op + (i/2));
				//cout << ins << " \n";
				memory[0x200 + i] = ins >> 8;
				ins = ins << 8;
				memory[0x200 + i + 1] = ins >> 8;
			}
			
		}
	
		//KEYBOARD MAPPING
		void keyPress()
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)){keys[0] = 1;}
			else{keys[0] = 0;}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){keys[1] = 1;}
			else{keys[1] = 0;}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){keys[2] = 1;}
			else{keys[2] = 0;}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)){keys[3] = 1;}
			else{keys[3] = 0;}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){keys[4] = 1;}
			else{keys[4] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){keys[5] = 1;}
			else{keys[5] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){keys[6] = 1;}
			else{keys[6] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){keys[7] = 1;}
			else{keys[7] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){keys[8] = 1;}
			else{keys[8] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){keys[9] = 1;}
			else{keys[9] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)){keys[0xA] = 1;}
			else{keys[0xA] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)){keys[0xB] = 1;}
			else{keys[0xB] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)){keys[0xC] = 1;}
			else{keys[0xC] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)){keys[0xD] = 1;}
			else{keys[0xD] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)){keys[0xE] = 1;}
			else{keys[0xE] = 0;}			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)){keys[0xF] = 1;}
			else{keys[0xF] = 0;}
			
				
		}
		

		//MAIN EMULATOR RUNTIME
		void run()
		{
			opcode = (char16_t)(memory[pc] << 8) | memory[pc + 1];
			//cout << std::hex << "ADDRESS: " << (int)pc << " OPCODE: " <<(int)opcode << "\n";
			//DONT FORGET TO BIT SHIFT WHEN USING BITWISE OPERATORS
			keyPress();
			switch(opcode & 0xF000)
			{
				//CLEARS DISPLAY
				case (0x0000):
				{
					//0x00E0 CLEARS DISPLAY
					if (opcode == 0x00E0)
					{
						for (int x = 0; x< 64; x++)
						{
							for (int y = 0; y< 32; y++)
							{
								display[x][y] = 0;
							}
						}
						pc += 2;
					}
					else if (opcode == 0x00EE)
					{
						//RETURN FROM SUBROUTINE
						sp -=1;	
						pc = stack[sp];
						pc += 2;
					}
					else
					{
						cout << "Program Complete!\n";
						exit(EXIT_FAILURE);
					}
					break;
				}	
					
				//0x1NNN SETS PROGRAM COUNTER TO NNN
				case (0x1000):
				{
					pc = opcode & 0x0FFF; 
					break;
				}
					
				//0x2NNN CALL SUBROUTINE AT NNN
				case(0x2000):
				{
					stack[sp] = pc;
					sp += 1;
					pc = opcode & 0x0FFF;
					break;
				}
					
				//SKIPS INSTRUCTION IF VALUES ARE EQUAL
				case (0x3000):
				{
					if (v[((opcode & 0x0F00) >> 8)] == (opcode & 0x00FF))
					{
						pc += 2;
					}
					pc += 2;
					break;
				}
					
				//SKIPS INSTRUCTION IF VALUES ARE NOT EQUAL
				case (0x4000):
				{
					if (v[((opcode & 0x0F00) >> 8)] != (opcode & 0x00FF))
					{
						pc += 2;
					}
					pc += 2;
					break;
				}
					
				//SKIPS INSTRUCTION IF REGISTER IS EQUAL TO ANOTHER REGISTER
				case (0x5000):
				{
					if ((opcode & 0x000F) != 0)
					{
						cout << "unsupported opcode!\n";
						exit(EXIT_FAILURE);
					}
					if (v[((opcode & 0x0F00) >> 8)] == v[((opcode & 0x00F0) >> 4)])
					{
						pc += 2;
					}
					pc += 2;
					break;
				}
					
				//SETS REGISTER VALUE TO LAST BYTE OF INSTRUCTION
				case (0x6000):
				{
					v[((opcode & 0x0F00) >> 8)] = (opcode & 0x00FF);
					pc += 2;
					break;
				}
					
				//ADDS VALUE OF LAST BYTE TO REGISTER
				case (0x7000):
				{
					v[((opcode & 0x0F00) >> 8)] += (opcode & 0x00FF);
					pc += 2;
					break;
				}	
				
				//OPERATIONS
				case (0x8000):
				{
					switch(opcode & 0x000F)
					{
						//SET REGISTER EQUAL TO ANOTHER
						case (0x0000):
						{
							v[((opcode & 0x0F00)>>8)] = v[((opcode & 0x00F0)>>4)];
							pc += 2;
							break;
						}
						//BITWISE OR OF TWO REGISTERS
						case (0x0001):
						{
							v[((opcode & 0x0F00)>>8)] |=  v[((opcode & 0x00F0)>>4)];
							pc += 2;
							break;
						}
						//BITWISE AND FOR TWO REGISTERS
						case (0x0002):
						{
							v[((opcode & 0x0F00)>>8)] &= v[((opcode & 0x00F0)>>4)];
							pc += 2;
							break;
						}
						//XOR FOR TWO REGISTERS
						case (0x0003):
						{
							v[((opcode & 0x0F00)>>8)] ^= v[((opcode & 0x00F0)>>4)];
							pc += 2;
							break;
						}
						//ADDS TWO REGISTERS AND BITWISE ANDS THEM
						case (0x0004):
						{
							if (v[((opcode & 0x00F0)>>4)] > (0xFF - v[((opcode & 0x0F00) >> 8)]))
							{
								v[0x000F] = 0x0001;
							}
							else
							{
								v[0x000F] = 0x0000;
							}
							v[((opcode & 0x0F00)>>8)] += v[((opcode & 0x00F0)>>4)];
							pc += 2;
							break;
						}
						//COMPARES REGISTER (if X > Y)
						case (0x0005):
						{
							if ((v[((opcode & 0x0F00)>>8)] < v[((opcode & 0x00F0)>>4)]))
							{
								v[0x000F] = 0x0000;
							}
							else
							{
								v[0x000F] = 0x0001;
							}
							v[((opcode & 0x0F00)>>8)] = (v[((opcode & 0x0F00)>>8)] - v[((opcode & 0x00F0)>>4)]) & 0x00FF;
							pc += 2;
							break;
						}
						//IF LEAST SIGNIFICANT BIT 
						//IS ONE SET FLAG TO ONE OTHERWISE 0
						case (0x0006):
						{
							if (((v[(opcode & 0x0F00) >> 8]) % 0x0002) == 0x0001)
							{
								v[0x000F] = 0x0001;
							}
							else
							{
								v[0x000F] = 0x0000;
							}
							v[((opcode & 0x0F00)>>8)] /= 0x0002;
							pc += 2;
							break;
						}
						//COMPARES TWO REGISTERS AND 
						//SETS FLAG THEN SUBTRACTS 
						//ONE REGISTER FROM ANOTHER
						case (0x0007):
						{
							if ((v[((opcode & 0x00F0)>>4)] < v[((opcode & 0x0F00)>>8)]))
							{
								v[0x000F] = 0x0000;
							}
							else
							{
								v[0x000F] = 0x0001;
							}
							v[((opcode & 0x0F00)>>8)] = v[((opcode & 0x00F0)>>4)] - v[((opcode & 0x0F00)>>8)];
							pc += 2;
							break;
						}
						//IF MOST SIGNIFICANT BIT IS EQUAL TO ONE
						//SET FLAG AND MULTIPLY REGISTER BY 2
						case (0x000E):
						{
							if ((v[((opcode & 0x0F00) >> 8)] >> 15) == 1)
							{
								v[0x000F] = 1;
							}
							else
							{
								v[0x000F] = 0;
							}
							v[((opcode & 0x0F00) >> 8)] <<= 0x0001;
							pc += 2;
							break;
						}
						default:
						{
							cout << "unsupported opcode!\n";
							exit(EXIT_FAILURE);
						}
					}
					break;
				}
					
				//SKIP INSTRUCTION IF REGISTER NOT EQUAL TO ANOTHER
				case (0x9000):
				{
					if ((opcode & 0x000F) != 0)
					{
						cout << "unsupported opcode!\n";
						exit(EXIT_FAILURE);
					}
					if (v[((opcode & 0x0F00) >> 8)] != v[((opcode & 0x00F0) >> 4)])
					{
						pc += 2;
					}
					pc += 2;
					break;
				}
				//LOAD VALUE INTO REGISTER I
				case (0xA000):
				{
					i = (opcode & 0x0FFF);
					pc += 2;
					break;
				}
				//JUMP TO LOCATION 
				case (0xB000):
				{
					pc = (opcode & 0x0FFF) + v[0];
					break;
				}
				//GET RANDOM BYTE THEN BITWISE AND IT WITH ANOTHER BYTE
				//AND STORE IN REGISTER
				case (0xC000):
				{
					v[((opcode & 0x0F00) >> 8)] = ((rand() % 256) & (opcode & 0x00FF));
					pc += 2;
					break;
				}
				//DRAW SPRITE AND CHECKS COLLISIONS
				case (0xD000):
				{
					v[0xF] = 0;
					for (int j = 0;  j < (opcode & 0x000F); j++)
					{
						for (int l = 0; l < 8; l++)
						{

							if (((memory[i + j] >> l) & 0b00000001) == 1)
							{

								//IF STATEMENT CHECKS FOR OVERLAP
								if (display[(v[((opcode & 0x0F00) >> 8)] + (7 - l)) % 64][(v[((opcode & 0x00F0) >> 4)] + j)  % 32] == 1)
								{
									//delete pixel
									v[0xF] = 1;
									display[(v[((opcode & 0x0F00) >> 8)] + (7 - l)) % 64][(v[((opcode & 0x00F0) >> 4)] + j)  % 32] = 0;									
									
								}
								else
								{
									//LOADS SPRITE INTO BUFFER
									display[(v[((opcode & 0x0F00) >> 8)] + (7 - l)) % 64][(v[((opcode & 0x00F0) >> 4)] + j)  % 32] = 1;
								}
							}
						}
					}
					pc += 0x2;
					break;		
				}
				case (0xE000):
				{
					//CHECK IF KEY IS PRESSED
					if ((opcode & 0x00FF) == 0x009E)
					{
						if (keys[v[((opcode & 0x0F00) >> 8)]] == 1)
						{
							pc += 2;
						}
					}
					//CHECK IF KEY IS NOT PRESSED
					else if ((opcode & 0x00FF) == 0x00A1)
					{
						if (keys[v[((opcode & 0x0F00) >> 8)]] == 0)
						{
							pc += 2;
						}
					}
					else
					{
						cout << "unsupported opcode!\n";
						exit(EXIT_FAILURE);
					}
					pc += 2;
					break;
				}
				case (0xF000):
				{
					switch (opcode & 0x00FF)
					{
						//SET REGISTER TO DELAY TIMER
						case (0x0007):
						{
							v[((opcode & 0x0F00) >> 8)] = delay_timer;	
							pc += 2;
							break;
						}
						//HALTS CPU AND WAITS FOR INPUT
						case (0x000A):
						{
							for (int j = 0; j < 16; j++)
							{
								if (keys[j] == 1)
								{
									v[((opcode & 0x0F00) >> 8)] = j;
									pc += 2;
								}
							}
							break;
						}
						//SET DELAY TIMER TO 
						case (0x0015):
						{
							delay_timer = v[((opcode & 0x0F00) >> 8)];
							pc += 2;
							break;
						}
						//SET SOUND TIMER EQUAL TO REGISTER
						case (0x0018):
						{
							sound_timer = v[((opcode & 0x0F00) >> 8)];
							pc += 2;
							break;
						}
						//ADD REGISTER TO REGISTER I
						case (0x001E):
						{
							if (i + v[((opcode & 0x0F00) >> 8)] > 0xFFF)
							{
								v[0xf] =1;
							}
							else
							{
								v[0xf] =0;
							}
							i = i + v[((opcode & 0x0F00) >> 8)];
							pc += 2;
							break;
						}
						//LOAD FONT SET 
						case (0x0029):
						{
							i = 0x50 + v[(opcode & 0x0F00) >> 8] * 0x5; 
							pc += 2;
							break;
						}
						//STORE BCD IN MEMORY CELLS I, I+1, I+2 FROM REGISTER
						case (0x0033):
						{
							memory[i] = (int)(v[((opcode & 0x0F00) >> 8)] / 100);
							memory[i+1] = (int)((v[((opcode & 0x0F00) >> 8)] / 10) % 10);
							memory[i+2] = (int)(v[((opcode & 0x0F00) >> 8)] % 10);
							pc += 2;
							break;
						}
						//SET MEMORY CELLS IN RANGE OF V0 TO NIBBLE
						case (0x0055):
						{
							for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
							{
								memory[i + j] = v[j];
							}
							pc += 2;
							break;
						}
						//SET ALL REGISTERS IN RANGE EQUAL TO MEMORY + I
						case(0x0065):
						{
							for (int j = 0; j <= ((opcode & 0x0F00) >> 8); j++)
							{
								v[j] = memory[i + j];
							}
							pc += 2;
							break;
						}
						default:
						{
							cout << "unsupported opcode!\n";
							exit(EXIT_FAILURE);
						}
					}
					break;
				}
				
				default:
					cout << "unsupported opcode!\n";
					exit(EXIT_FAILURE);
			}
			
			//DRAWS BUFFER
			for (int y = 0; y < 32; y++)
			{
				for (int x = 0; x < 64; x++)
				{
					if (display[x][y] == 1)
					{
						d.init(x, y);
					}
					else
					{
						d.delP(x, y);
					}	
				}
			}
			//DECREMENTS TIMERS 
			if (delay_timer != 0)
			{
				delay_timer -= 1;
			}
			if (sound_timer != 0)
			{
				cout << '\a';
				cout << "\n";
				sound_timer -= 1;
			}
			
			
		}	
};

//LOADS OPCODES FROM FILE INTO ARRAY
class FileRead
{
	public:
	//INITIALIZE ROM TO BE LOADED 
	//ROM NAME PARAMETER
	
	//PUT ARRAY IN PARAMETER
	void init(std::string rom)
	{
		//LOAD INTO BUFFER
		std::ifstream readF(rom, std::ios::binary);
		std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(readF), {});
		readF.close();
		int s = buffer.size();
		
		cout << std::dec;
		s = s/2;
		
		//LOAD INSTRUCTIONS INTO AN ARRAY OF SHORT(UNSIGNED)
		unsigned short ops[s];
		for (int i = 0;  i < s*2; i+=2)
		{
			ops[i/2] = buffer[i];
			ops[i/2] = ops[i/2] << 8;
			ops[i/2] += buffer[i+1];
		}
		
		op = ops;
		size = s;
		
		//SOME REASON THIS IS NEEDED FOR 
		//POINTER TO BE ACCESSED GLOBALLY???
		for (int i = 0; i < size; i++)
		{
			cout<< std::hex << *(op+i) << "\n";
		}
	}	
};



int main(int argc, char** argv)
{
	//THROW ERROR IF NO ROM IS SPECIFIED
	if (argc < 2)
	{
		cout << "ERROR please specify rom!";
		exit(EXIT_FAILURE);
	}
	//COMMANDLINE ARGUMENT FOR ROM
	std::string rm = argv[1];
	
	//OBJECTS
	FileRead f;	
	emulator e;	
	pixel p;
 


	//EMULATOR
	f.init("roms/" + rm);
	e.loadProg();
	
	//MAIN RUNTIME LOOP
	while (window.isOpen())
    {
		e.run();
		sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            window.close();
        }
		window.display();
    }
	
    return 0;
}

