#pragma once

#include "ErrorHandling.hpp"
#include "NemoDefinitions.hpp"

class ALU
{
private:
	byte math_register_a, math_register_b;
	byte result_register;
public:

	ALU()
	{
		math_register_a = 0x00;
		math_register_b = 0x00;
		result_register = 0x00;
	}
	
	void setMathRegisters(byte new_math_register_a, byte new_math_register_b)
	{
		math_register_a = new_math_register_a;
		math_register_b = new_math_register_b;
	}
	void addRegisterContents()
	{
		result_register = math_register_a + math_register_b;
	}
	byte getResultOfMaths()
	{
		return result_register;
	}
};