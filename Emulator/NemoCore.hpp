#pragma once

#include <raylib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include "ErrorHandling.hpp"

#include "NemoDefinitions.hpp"
#include "ALU.hpp"

enum Instructions
{
	//No OPeration.
	NOP,
	/*
		Sets the program counter to the new value
		and pushes the current program counter
		value on to stack.
	*/
	JUMP,
	/*
		Pops the top value off of the stack
		and updates the program counter with
		the new value.
	*/
	RETURN,
	/*
		ADDs the value in register A with the
		value in register B.
	*/
	ADD,
	/*
		Sets the ram pointer to a certain value
		so that RAM can be modified with
		RAMSET.
	*/
	SET_RAM_POINTER,
	/*
		Sets the value in RAM at the location
		where the ram pointer is set to.
	*/
	RAM_SET,
	/*
		Updates the value in the chosen
		register with the data in RAM
		where the ram pointer points to
		(haha).
	*/
	LD,
	/*
		Goes to the next ROM bank in the
		list.
	*/
	NEXT_ROM_BANK,
	END,
};

enum Registers
{
	REGISTER_A,
	REGISTER_B,
	REGISTER_C,
	REGISTER_D,
};

#define ROM_BANK_COUNT 4

// RAM[ram_pointer]
class NemoCore
{
private:

	ALU arithmetic_logic_unit;

	void logMessage(std::string message)
	{
		static int messages_logged = 0;
		std::cout << std::dec << "message: " << messages_logged << ":\t" << message << "\n";
		messages_logged++;
	}
	/*
		The ROM will store the main
		program data.
		There will be 4 different ROM banks
	*/
	byte ROM[ROM_BANK_COUNT][0xff];
	byte* current_ROM_bank;
	int ROM_bank_index;

	/*
		The RAM will store memory
		for the programs use.

		range [0x00] -> 0xEF
		is used for user data.

		range [0xF0] -> [0xFF] is
		used for the stack.
	*/
	byte RAM[0xff];

	/*
		This will store data for the
		video.
	*/
	byte VRAM[0xff];
	
	/*
		This will make accessing the stack
		much easier to do.
	*/
	byte* pointer_to_where_stack_memory_is[0xf];
	int stack_position_tracker;

	byte program_counter, ram_pointer;
	bool draw_vram;

	byte next_instruction;
	byte next_parameter;
	byte next_data;

	byte register_a, register_b, register_c, register_d;

	bool is_paused;
	bool is_instruction_view;
	bool show_help_menu;

	void goToNextRomBank()
	{
		ROM_bank_index++;
		current_ROM_bank = ROM[ROM_bank_index];

		if(ROM_bank_index == ROM_BANK_COUNT)
		{
			ROM_bank_index = 0;
		}
	}

	/*
		These are where functions that are
		only needed for internal NemoCore
		operations will be put into.
	*/

	/*
		Sets up the super convenient pointer
		array that can be used to access
		the stack locations in RAM.
	*/
	void initialiseStackInformation()
	{
		stack_position_tracker = 0;
		for(int i = 0xf0; i != 0xff; i++)
		{
			pointer_to_where_stack_memory_is[i - 0xf0] = &RAM[i];
		}
	}

	byte* getRegister(byte chosen_register)
	{
		switch (chosen_register)
		{
		case REGISTER_A:
		{
			logMessage("\t\tREGISTER_A");
			return &register_a;
		}
		break;
		case REGISTER_B:
		{
			logMessage("\t\tREGISTER_B");
			return &register_b;
		}
		break;
		case REGISTER_C:
		{
			logMessage("\t\tREGISTER_C");
			return &register_c;
		}
		break;
		
		case REGISTER_D:
		{
			logMessage("\t\tREGISTER_D");
			return &register_d;
		}
		break;
		
		default:
		{
			return NULL;
		}
		break;
		}
	}
	/*
		This will execute a jump and add a
		new address to the stack so that when
		"RET" is called the program can jump
		to the last placed value in the stack.
	*/
	void executeJump(byte value_to_jump_to)
	{
		char message_string[64];
		sprintf(message_string, "Executing jump from [ 0x%02X ] to [ 0x%02X ]...", program_counter, value_to_jump_to);
		logMessage(message_string);

		static int times_stack_limit_has_been_hit_and_ignored = 0;

		if(stack_position_tracker == 0xe)
		{
			throwError("\"stack_position_tracker\" has hit maximum", INTERNAL_SOFTWARE_WARNING);
			return;
		}
		else
		{
			times_stack_limit_has_been_hit_and_ignored = 0;
		}

		// std::cout << "value_to_jump_to: " << std::hex << value_to_jump_to;
		// std::cout << "program_counter: " << std::hex << program_counter << "\n";
		// std::cout << "*pointer_to_where_stack_memory_is[0xf0 + stack_position_tracker]: " << std::hex << (int)*pointer_to_where_stack_memory_is[stack_position_tracker] << "\n";
		(*pointer_to_where_stack_memory_is[stack_position_tracker]) = program_counter;
		program_counter = value_to_jump_to;
		stack_position_tracker++;
		
		unwindProgramCounter();
	}

	void executeReturn()
	{
		if(stack_position_tracker == 0)
		{
			logMessage("Stack has nothing to return with");
			return;
		}
		
		char message_string[64];
		sprintf(message_string, "Executing return from [ 0x%02X ] to [ 0x%02X ]...", *pointer_to_where_stack_memory_is[stack_position_tracker - 1], program_counter);
		logMessage(message_string);
		
		stack_position_tracker--;
		program_counter = *pointer_to_where_stack_memory_is[stack_position_tracker];
		(*pointer_to_where_stack_memory_is[stack_position_tracker]) = 0;
	}

	void executeADD()
	{
		arithmetic_logic_unit.setMathRegisters(register_a, register_b);
		arithmetic_logic_unit.addRegisterContents();
		register_c = arithmetic_logic_unit.getResultOfMaths();
	}
	void executeSetRamPointer()
	{
		ram_pointer = next_data;
	}
	void executeRamSet()
	{
		RAM[ram_pointer] = next_data;
	}
	void executeLD()
	{
		*getRegister(next_parameter) = RAM[ram_pointer];
	}
	void executeNextRomBank()
	{
		goToNextRomBank();
	}
	
	void tickProgramCounter()
	{
		program_counter += 4;
	}
	void unwindProgramCounter()
	{
		program_counter -= 4;
	}

public:

	NemoCore()
	{
		draw_vram = false;
		is_paused = false;
		is_instruction_view = true;
		system("clear");

		logMessage("Initialising registers...");
		clearRegisters();
		
		logMessage("Initialising Stack pointer array...");
		initialiseStackInformation();
		
		logMessage("Clearing memory banks...");
		resetAllMemoryBanks();

		logMessage("[ NemoCore is ready to go! ]");

		current_ROM_bank = ROM[0];
		ROM_bank_index = 0;
	}

	void logMessageExternally(std::string message)
	{
		logMessage("External Message: " + message);
	}

	void printCurrentStackContents()
	{
		logMessage("\nPrinting stack contents:");
		
		std::cout << std::hex;
		for(int i = 0; i != 0x0f; i++)
		{
			logMessage(std::to_string(i) + "\t" + (i == stack_position_tracker ? ">" : " ") + " [ " + std::to_string(*pointer_to_where_stack_memory_is[i]) + " ]");
		}
		std::cout << std::dec;
	}
	
	void resetMemoryBank(byte* memory_bank)
	{
		for(int i = 0; i != 0xff; i++)
		{
			memory_bank[i] = 0;
		}
	}
	void resetAllMemoryBanks()
	{
		for(int i = 0; i != 4; i++)
		{
			resetMemoryBank(ROM[i]);
		}
		
		resetMemoryBank(RAM);
		resetMemoryBank(VRAM);
	}
	void clearRegisters()
	{
		stack_position_tracker = 0;
		program_counter = 0x00;
		ram_pointer = 0x00;

		next_instruction = 0x00;
		next_parameter = 0x00;
		next_data = 0x00;

		register_a = 0;
		register_b = 0;
		register_c = 0;
		register_d = 0;
	}
	
	/*
		Set the RAM pointer register
		to a certain value.
	*/
	void setRAMPointer(byte new_value)
	{
		ram_pointer = new_value;
	}
	void manuallyPassInstructionData(byte new_instruction, byte new_parameter, byte new_data)
	{
		next_instruction = new_instruction;
		next_parameter = new_parameter;
		next_data = new_data;
	}
	void setInstructionAtAddress(byte address, byte new_instruction, byte new_parameter, byte new_data)
	{
		current_ROM_bank[address] = new_instruction;
		current_ROM_bank[address + 1] = new_parameter;
		current_ROM_bank[address + 2] = new_data;
	}

	void executeInstruction()
	{
		switch(next_instruction)
		{
		case JUMP:
		{
			executeJump(next_data);
		}
		break;
		
		case RETURN:
		{
			executeReturn();
		}
		break;

		case ADD:
		{
			executeADD();
		}
		break;
		case SET_RAM_POINTER:
		{
			executeSetRamPointer();
		}
		break;
		case RAM_SET:
		{
			executeRamSet();
		}
		break;
		case LD:
		{
			executeLD();
		}
		break;
		case NEXT_ROM_BANK:
		{
			executeNextRomBank();
		}
		break;
		
		default:
		break;
		}
		next_instruction = 0x00;
		next_parameter = 0x00;
		next_data = 0x00;
	}

	void loadInstructionDataIntoRegisters()
	{
		next_instruction = current_ROM_bank[program_counter];
		next_parameter = current_ROM_bank[program_counter + 1];
		next_data = current_ROM_bank[program_counter + 2];
	} 

	void executeCycle()
	{
		if(is_paused)
		{
			return;
		}
		loadInstructionDataIntoRegisters();
		executeInstruction();
		tickProgramCounter();
	}

	void drawMemoryBank(byte* bank, int x, int y, int fontsize, int highlight_index)
	{
		std::string instruction_labels[] =
		{
			"NOP",
			"JMP",
			"RTN",
			"ADD",
			"SRP",
			"RST",
			"LD",
			"NRB",
		};

		int row_draw_coord = 0;
		int column_draw_coord = 0;
		
		int row_tracker = 0;
		
		char print_buffer[10] = "";

		//Draw the address column at the top.
		for(int i = 0; i != 0x10; i++)
		{
			sprintf(print_buffer, "0x%02X", i);
			DrawText(print_buffer, 50 + (x + (i * (fontsize * 2))), y, fontsize * 0.75, BLUE);
		}
		y += 20;
		
		DrawText("0x00", x, y, 15, RED);
		
		//Draw the main block of memory contents.
		for(int i = 0; i != 0xff; i++, row_tracker++)
		{
			/*
				Draw the address row at the left hand side
				for the current row.
			*/
			if(row_tracker == 0x10)
			{
				sprintf(print_buffer, "0x%02X", i);

				row_tracker = 0;
				column_draw_coord = 0;
				row_draw_coord += 20;
				DrawText(print_buffer, x, y + row_draw_coord, fontsize, RED);
			}
			column_draw_coord += fontsize * 2;
			
			if(
				bank == current_ROM_bank
				&& is_instruction_view
				&& !(i % 4)
				&& bank[i] < END
				)
			{
				sprintf(print_buffer, "[%s]", instruction_labels[bank[i]].c_str());
				DrawText(print_buffer, (x + 20) + column_draw_coord, y + row_draw_coord, fontsize * 0.75, (i == highlight_index ? GREEN : WHITE));
			}
			else
			{
				sprintf(print_buffer, "[%02X]", bank[i]);
				DrawText(print_buffer, (x + 20) + column_draw_coord, y + row_draw_coord, fontsize, (i == highlight_index ? GREEN : WHITE));
			}
		}
	}

	void visualiseMemoryBank(byte* bank, int x, int y, int size)
	{
		int row_draw_coord = 0;
		int column_draw_coord = 0;
		
		int row_tracker = 0;
		
		//Draw the main block of memory contents.
		for(int i = 0; i != 0xff; i++, row_tracker += size)
		{
			/*
				Draw the address row at the left hand side
				for the current row.
			*/
			if(row_tracker == 0x10)
			{
				row_tracker = 0;
				column_draw_coord = 0;
				row_draw_coord += size;
			}
			// offset + (size * draw_position)
			DrawRectangle(485 + ((x) * 9), 370 + ((y) * 9), 8, 8, WHITE);

			int x_draw = (x + (size * column_draw_coord));
			int y_draw = (y + (size * row_draw_coord));
			DrawLine(x, y, x_draw, y_draw, GREEN);
			// DrawRectangle((x * size + column_draw_coord), (y * size + row_draw_coord), size - 1, size - 1, RED);
			// DrawText(print_buffer, (x + 20) + column_draw_coord, y + row_draw_coord, fontsize, (i == highlight_index ? GREEN : WHITE));
		}
	}

	void drawRegister(int x, int y, char* label, byte register_to_draw)
	{
		char text[30];
		sprintf(text, "%s: [ 0x%02X ]", label, register_to_draw);
		DrawText(text, x, y, 15, WHITE);
	}

	void drawCyle()
	{
		char print_buffer[30] = "";
		sprintf(print_buffer, "Program Counter: [ 0x%02X ]", program_counter);
		DrawText(print_buffer, 10, 10, 10, WHITE);

		sprintf(print_buffer, "RAM pointer: [ 0x%02X ]", ram_pointer);
		DrawText(print_buffer, 10, 20, 10, WHITE);

		drawMemoryBank(current_ROM_bank, 10, 30, 13, program_counter);
		
		if(draw_vram)
		{
			drawMemoryBank(VRAM, 480, 30, 13, ram_pointer);//THIRD! </3
		}
		else
		{
			drawMemoryBank(RAM, 480, 30, 13, ram_pointer);//FIRST! <3
		}
		drawRegister(10, 380, (char*)"ROM BANK", ROM_bank_index);

		drawRegister(10, 400, (char*)"Register A", register_a);
		drawRegister(10, 420, (char*)"Register B", register_b);
		drawRegister(10, 440, (char*)"Register C", register_c);
		drawRegister(10, 460, (char*)"Register D", register_d);

		DrawRectangle(480, 365, 110, 110, ORANGE);
		DrawRectangle(485, 370, 100, 100, WHITE);
		visualiseMemoryBank(RAM, 485, 370, 10);
		
		if(show_help_menu)
		{
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
			DrawText("P: pauses\n", 0, 0, 4, BLACK);
		}
	}

	void controlsCycle()
	{
		if(IsKeyPressed(KEY_H))
		{
			show_help_menu = !show_help_menu;
		}
		if(IsKeyPressed(KEY_P))
		{
			is_paused = !is_paused;
		}

		if(!is_paused)
		{
			SetTargetFPS(4);
			return;
		}
		SetTargetFPS(60);
		/*
			These controls can only occur
			when the core is paused.
		*/
		if(IsKeyPressed(KEY_UP))
		{
			program_counter -= 0x10;
		}
		if(IsKeyPressed(KEY_DOWN))
		{
			program_counter += 0x10;
		}
		if(IsKeyPressed(KEY_LEFT))
		{
			program_counter -= 4;
		}
		if(IsKeyPressed(KEY_RIGHT))
		{
			program_counter += 4;
		}

		//Steps the program forward by one "frame".
		if(IsKeyPressed(KEY_R))
		{
			loadInstructionDataIntoRegisters();
			executeInstruction();
			tickProgramCounter();
		}

		int direction_of_change = 0;
		if(IsKeyDown(KEY_MINUS))
		{
			direction_of_change = -1;
		}
		if(IsKeyDown(KEY_EQUAL))
		{
			direction_of_change = +1;
		}

		if(IsKeyPressed(KEY_J))
		{
			is_instruction_view = !is_instruction_view;
		}

		if(IsKeyPressed(KEY_ONE))
		{
			current_ROM_bank[program_counter] += direction_of_change;
		}
		if(IsKeyPressed(KEY_TWO))
		{
			current_ROM_bank[program_counter + 1] += direction_of_change;
		}
		if(IsKeyPressed(KEY_THREE))
		{
			current_ROM_bank[program_counter + 2] += direction_of_change;
		}
		if(IsKeyPressed(KEY_FOUR))
		{
			current_ROM_bank[program_counter + 3] += direction_of_change;
		}
		
		static byte clipboard[4] = { 0, 0, 0, 0};

		if(IsKeyPressed(KEY_X))
		{
			clipboard[0] = current_ROM_bank[program_counter + 0];
			clipboard[1] = current_ROM_bank[program_counter + 1];
			clipboard[2] = current_ROM_bank[program_counter + 2];
			clipboard[3] = current_ROM_bank[program_counter + 3];

			current_ROM_bank[program_counter + 0] = 0;
			current_ROM_bank[program_counter + 1] = 0;
			current_ROM_bank[program_counter + 2] = 0;
			current_ROM_bank[program_counter + 3] = 0;
		}

		if(IsKeyPressed(KEY_V))
		{
			current_ROM_bank[program_counter + 0] = clipboard[0];
			current_ROM_bank[program_counter + 1] = clipboard[1];
			current_ROM_bank[program_counter + 2] = clipboard[2];
			current_ROM_bank[program_counter + 3] = clipboard[3];
		}
	}
};