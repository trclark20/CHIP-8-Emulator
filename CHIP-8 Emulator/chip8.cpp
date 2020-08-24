#include "chip8.h"
#include <ios>

unsigned char num;

void chip8::initialize()
{
	//Initialize registers and memory
	pc = 0x200;		//program counter starts at 0x200
	opcode = 0;		//reset opcode
	I = 0;			//reset index register
	sp = 0;			//reset stack pointer;

	//clear display
	for (int i = 0; i < 2048; i++)
		gfx[i] = 0;

	//clear stack, registers V0-VF
	for (int i = 0; i < 16; i++)
	{
		stack[i] = 0;
		V[i] = 0;
	}
	
	//clear memory
	for (int i = 0; i < sizeof(memory); i++)
		memory[i] = 0;

	//load fontset
	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	//reset timers
	delay_timer = 0;
	sound_timer = 0;
}

void chip8::loadGame(const char *gameName)
{
	#pragma warning(disable:4996)
	FILE* romFile;
	errno_t err;
	romFile = fopen(gameName, "rb");

	if (romFile == NULL)
		printf("Error : errno='%s'.\n", strerror(errno));
	else
	{
		//load program into memory (starting at 0x200)
		fseek(romFile, 0, SEEK_END);
		long rom_size = ftell(romFile);
		rewind(romFile);

		char* buffer = (char*)malloc(sizeof(char) * rom_size);
		auto bufferSize = sizeof(char) * rom_size;

		if (buffer != NULL)
		{
			size_t result = fread(buffer, sizeof(char), rom_size, romFile);
			if (result != rom_size) printf("Reading error");
			for (int i = 0; i < bufferSize; i++)
			{
				memory[i + 512] = buffer[i];
			}
		}
	}
}

void chip8::emulateCycle(int numberOfCycles)
{
	//Fetch opcode (next 2 bytes)
	for (int i = 0; i < numberOfCycles; i++)
	{
		opcode = memory[pc] << 8 | memory[pc + 1];

		printf("Executing opcode: 0x%X\n", opcode);
		//Decode opcode
		switch (opcode & 0xF000)
		{
			//Handle opcodes starting with 0
		case 0x0000:
			switch (opcode & 0x000F)
			{
			case 0x0000: //0x00E0: Clears the screen
				for (int i = 0; i < 2048; i++)
					gfx[i] = 0;
				pc += 2;
				break;
			case 0x000E: //0x00EE: Returns from subroutine
				sp--;
				pc = stack[sp];
				break;
			default:
				printf("Unknown opcode: 0x%X\n", opcode);
			}
			break;

		case 0x1000: //0x1NNN: jump to address NNN
			pc = opcode & 0x0FFF;
			break;
		case 0x2000: //0x2NNN: calls subroutine at address NNN
			pc += 2;
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000: //0x3XNN: if Vx == NN skip next instruction
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		case 0x4000: //0x4XNN: if Vx != NN skip next instruction
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		case 0x5000: //0x5XY0: if Vx == Vy skip next instruction
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
			break;
		case 0x6000: //0x6XNN: set Vx to NN
			V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
			pc += 2;
			break;
		case 0x7000: //0x7XNN: add NN to Vx (carry flag unchanged)
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
			break;

		//Handle opcodes starting with 8
		case 0x8000:
			switch (opcode & 0x000F)
			{
			case 0x0000: //0x8XY0: Vx = Vy
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0001: //0x8XY1: Vx = Vx|Vy
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0002: //0x8XY2: Vx = Vx&Vy
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0003: //0x8XY3: Vx=Vx^Vy
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0004: //0x8XY4: Vx += Vy, Vf = 1 if carry, 0 if not
				if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
					V[0xF] = 1;
				else
					V[0xF] = 0;
				V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0005: //0x8XY5: Vx -=Vy, Vf = 0 if borrow, 1 if not
				if (V[(opcode & 0x0F00) >> 8] < (0x0 + V[(opcode & 0x00F0) >> 4]))
					V[0xF] = 0;
				else
					V[0xF] = 1;
				V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
				pc += 2;
				break;
			case 0x0006: //0x8XY6: Vx>>=1 (store lsb of Vx in Vf, shift Vx right 1)
				V[0xF] = (V[(opcode & 0x0F00) >> 8] & 0x01);
				V[(opcode & 0x0F00) >> 8] >>= 1;
				pc += 2;
				break;
			case 0x0007: //0x8XY7: Vx=Vy-Vx (set Vx to Vy minus Vx, Vf = 0 when borrow, 1 if not)
				if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
					V[0xF] = 0;
				else
					V[0xF] = 1;
				V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x000E: //0x8XYE: Vx <<= 1 (store msb of Vx in Vf, shift Vx left 1)
				V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
				V[(opcode & 0x0F00) >> 8] <<= 1;
				pc += 2;
				break;
			default:
				printf("Unknown opcode: 0x%X\n", opcode);
			}
			break;

		case 0x9000: //0x9XY0: if(Vx!=Vy) (skips next instruction if Vx!=Vy)
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
			break;
		case 0xA000: //0xANNN: Sets I to address NNN
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		case 0xB000: //0xBNNN: PC=V0+NNN (Jumps to address NNN + V0)
			pc = (V[0x0] + (opcode & 0x0FFF));
			break;
		case 0xC000: //0xCXNN: Vx=rand()&NN (set Vx to result of random number and NN)
			num = rand() % 256;
			V[(opcode & 0x0F00) >> 8] = num & (opcode & 0x00FF);
			pc += 2;
			break;
		case 0xD000: //0xDXYN: draw(Vx,Vy,N)
		{
			x = V[(opcode & 0x0F00) >> 8];
			y = V[(opcode & 0x00F0) >> 4];
			height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if (gfx[(x + xline + ((y + yline) * 64))] == 1)
							V[0xF] = 1;
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
		}
		break;

		//handle opcodes starting with E
		case 0xE000:
			switch (opcode & 0x000F)
			{
			case 0x000E: //0xEX9E: if (key()==Vx) (skip next instruction if key stored in Vx is pressed)
				if (key[V[(opcode & 0x0F00) >> 8]] != 0)
					pc += 4;
				else
					pc += 2;
				break;
			case 0x0001: //0xEXA1: if (key() !=Vx) (skip next instruction if key stored in Vx isn't pressed)
				if (key[V[(opcode & 0x0F00) >> 8]] == 0)
					pc += 4;
				else
					pc += 2;
				break;
			default:
				printf("Unknown opcode: 0x%X\n", opcode);
			}
			break;

		//handle opcodes starting with F
		case 0xF000:
			switch (opcode & 0x000F)
			{
			case 0x0007: //0xFX07: Set Vx to value of delay timer
				V[(opcode & 0x0F00) >> 8] = delay_timer;
				pc += 2;
				break;
			case 0x000A: //0xFX0A: Key press is awaited, then stored in Vx (blocking)
				for (int i = 0; i < sizeof(key); i++)
				{
					if (key[i] != 0)
					{
						V[(opcode & 0x0F00) >> 8] = i;
						pc += 2;
					}
					else {}
				}
				break;
			case 0x0008: //0xFX18: Set sound timer to Vx
				sound_timer = V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x000E: //0xFX1E: Add Vx to I, VF not affected
				I += V[(opcode & 0x0F00) >> 8];
				pc += 2;
				break;
			case 0x0009: //0xFX29: Set I to location of sprite for character in Vx. Characters 0-F (hex) are represented by 4x5 font
				I = V[(opcode & 0x0F00) >> 8] * 0x5;
				pc += 2;
				break;
			case 0x0003: //0xFX33: take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2
				memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
				memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
				memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
				pc += 2;
				break;

			//Handle opcodes ending in 5
			case 0x0005:
				switch (opcode & 0x00F0)
				{
				case 0x0010: //0xFX15: Set delay timer to Vx
					delay_timer = V[(opcode & 0x0F00) >> 8];

					pc += 2;
					break;
				case 0x0050: //0xFX55: Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
					for (int i = 0; i < V[(opcode & 0x0F00) >> 8] + 1; i++)
						memory[I + i] = V[i];
					pc += 2;
					break;
				case 0x0060: //0xFX65: Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
					for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
						V[i] = memory[I + i];
					pc += 2;
					break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
				}
				break;
			}
			break;

		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
		}

		//Update timers
		if (delay_timer > 0)
			--delay_timer;

		if (sound_timer > 0)
		{
			if (sound_timer == 1)
				printf("BEEP!\n");
			--sound_timer;
		}
	}
}