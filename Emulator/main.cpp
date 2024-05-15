#include <raylib.h>
#include <stdint.h>
#include <iostream>
#include <string>

#include "NemoCore.hpp"

int main(int argc, char const *argv[])
{
	uint64_t frame_counter = 0;
	
	InitWindow(0xff << 2, 0xff << 1, "Skemu");
	SetTargetFPS(2);

	NemoCore nemo_core;

	// nemo_core.setInstructionAtAddress(0x10, JUMP, 0x00, 0x20);
	// nemo_core.setInstructionAtAddress(0x30, RETURN, 0x00, 0x00);

	// nemo_core.setInstructionAtAddress(0x20, JUMP, 0x00, 0x30);
	// nemo_core.setInstructionAtAddress(0x60, RETURN, 0x00, 0x00);
	
	// nemo_core.setInstructionAtAddress(0x00, RAM_SET, 0x00, 0x01);
	// nemo_core.setInstructionAtAddress(0x04, LD, REGISTER_A, 0x00);
	// nemo_core.setInstructionAtAddress(0x08, SET_RAM_POINTER, 0x00, 0x02);
	// nemo_core.setInstructionAtAddress(0x0c, RAM_SET, 0x00, 0x02);
	// nemo_core.setInstructionAtAddress(0x10, LD, REGISTER_B, 0x00);
	// nemo_core.setInstructionAtAddress(0x14, ADD, 0x00, 0x00);

	while(!WindowShouldClose())
	{
		
		frame_counter++;
		// nemo_core.logMessageExternally("frame count:" + std::to_string(frame_counter));
		
		nemo_core.executeCycle();
		if(frame_counter == 10)
		{
			nemo_core.manuallyPassInstructionData(
													JUMP,
													0x00,
													0x30
													);
		}
		if(frame_counter == 14)
		{
			nemo_core.manuallyPassInstructionData(
													RETURN,
													0x00,
													0x00
													);
		}
		breakThePoint();
		ClearBackground(BLACK);
		nemo_core.drawCyle();
		EndDrawing();
		// triggerBreakPoint();
		breakThePoint();

		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			std::cout << "Point(" << GetMouseX() << ", " << GetMouseY() << ")\n";
			break;
		}
		nemo_core.controlsCycle();
	}

	CloseWindow();
	
	return NO_ERROR;
}