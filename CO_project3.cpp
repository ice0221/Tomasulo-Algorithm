//---------------------------------------------
//---------------------------------------------
//README
//
//Tomasulo Algorithm
//The version_1 is finished in 2021/06/19
//---------------------------------------------
//
//Introduction:
//
//The Tomasulo Algorithm is used to improve OOE of processor,
//In this project, I set the issue and write back can operate in the same cycle,
//Then, ADD and SUB instruction need to spend two cycle on executing,
//MUL and DIV instruction need to spend eight cycle on executing
//---------------------------------------------
//
//Introduction important parameter:
//
//1. RF[5] (int)		 : Record RF value
//2. RAT_F[5] (string)	 : Record RAT value,the value is showed by "RS+num"
//3. op[5] (string)		 : Record operating symbol in RS table
//4. RS_val1[5] (string) : Record operating RD1 in RS table
//5. RS_val2[5] (string) : Record operating RD2 in RS table
//6. RS_table_ADD (int)	 : The value is 1 or 0,indicating the position of RS_ADD table whether is full
//7. RS_table_MUL (int)	 : The value is 1 or 0,indicating the position of RS_MUL table whether is full
//8. RS_value(int)		 : The value of RS after writing back
//9. RScycle[5] (int)	 : The cycle time of inst executing needs to wait
//10.cycle (int)		 : The current cycle time
//11.buffermsg			 : The message of write back buffer
//
//---------------------------------------------
//
//The rule of input:
//1. Use "," between two register
//2. Use " " btween kind of instruction and register
//
//---------------------------------------------
//
//Input,Output Example:
/*
Input
ADDI F1, F2, 1
SUB F1, F3, F4
DIV F1, F2, F3
MUL F2, F3, F4
ADD F2, F4, F2
ADDI F4, F1, 2
MUL F5, F5, F5
ADD F1, F4, F4
*/
//
//Output of last cycle
/*
cycle:19

	--RF--
	F1 | 4 |
	F2 | 30 |
	F3 | 4 |
	F4 | 4 |
	F5 | 64 |
	------

	--RAT--
	F1 | |
	F2 | |
	F3 | |
	F4 | |
	F5 | |
	------ -

	--RS-------------- -
	RS1 | | | |
	RS2 | | | |
	RS3 | | | |
	------------------ -
	BUFFER: empty

	--RS-------------- -
	RS4 | | | |
	RS5 | | | |
	------------------ -
	BUFFER : empty
*/
//
//---------------------------------------------
//---------------------------------------------
#include <math.h>
#include <iostream>
#include <string>
#include <bitset>
#include <cstdlib>
#include <sstream>
using namespace std;
//---------------------------
//Delcare
//---------------------------
int cycle = 0;
int RF[5] = {0,2,4,6,8};
int RS_table_ADD[4] = { 0 };
int RS_table_MUL[3] = { 0 };
int RScycle[5] = { 0 };
int RS_value[5];
int NotRelease[5] = { 0 };
int Waitcycle = 0;
int InstEnd = 0;
string buffermsg[2];

void ProcessInst(string inst,string RAT_F[], string op[],string RS_val1[],string RS_val2[]);
void Release(string op[], string RS_val1[], string RS_val2[], string RAT_F[],int RS_value[], string buffermsg[]);
void Printfmsg(int RF[], string RAT_F[], string op[], string RS_val1[], string RS_val2[], string buffermsg[]);

void main()
{
	string inst;
	string code;
	int i, j, k = 0;//counter
	int shift = 0;
	int next_inst = 0;
	int End;
	string RAT_F[5];
	string op[5];
	string RS_val1[5];
	string RS_val2[5];
	for (i=0;i<=4;i++)
	{
		RAT_F[i] = "   ";
		op[i] = " ";
		RS_val1[i] = "   ";
		RS_val2[i] = "   ";
		RS_value[i] = -1;
		RScycle[i] = RScycle[i] - 1;
	}
	//Input
	cout << "RISC-V Input:\n";
	while (1)
	{
		getline(cin, inst);
		code.append(inst);
		code.append("\n");
		if (inst == "\0")
		{
			inst.clear();
			break;
		}
		inst.clear();
	}
	//Asembler
	i = 0;
	while (1)
	{
		//GET inst
		i = next_inst;
		while (1)
		{
			inst.append(1, code[i]);
			if (code[i + 1] == '\n')
			{
				break;
			}
			i = i + 1;
		}
		//Deal with inst
		ProcessInst(inst, RAT_F, op, RS_val1, RS_val2);
		next_inst = next_inst + inst.size() + 1;
		if (next_inst >= code.size() - 1)
		{
			break;
		}
		inst.clear();
	}
	InstEnd = 1;
	while (1)
	{
		End = 1;
		for (i=0;i<=4;i++)
		{
			if (RS_val1[i] != "   "|| RS_val2[i] != "   ")
			{
				End = 0;
			}
		}
		if (End == 1)
		{
			break;
		}
		else
		{
			ProcessInst(inst, RAT_F, op, RS_val1, RS_val2);
		}
	}
	buffermsg[0] = "empty";
	buffermsg[1] = "empty";
	for (i = 0; i <= 4; i++)
	{
		if (RAT_F[i] != "   ")
		{
			RF[i] = RS_value[(int)RAT_F[i][2]-48-1];
			RAT_F[i] == "   ";
		}
	}
	Printfmsg(RF, RAT_F, op, RS_val1, RS_val2, buffermsg);
	
	system("pause");
}

void ProcessInst(string inst,string RAT_F[], string op[], string RS_val1[], string RS_val2[])
{
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int index = 0;
	int RS_FULL_ADD = 0, RS_FULL_MUL = 0;
	string kind;
	string RS;
	string RD1,RD2;
	string symbol;
	buffermsg[0] = "empty";
	buffermsg[1] = "empty";
	string num2str;
	string reg_str;
	char reg_char;
	int reg_dec;
	int reg_dec2;
	int reg_dec3;
	//Disassemble insruction
	while (1)
	{
		kind.append(1, inst[i]);
		if (inst[i + 1] == ' ')
		{
			break;
		}
		i = i + 1;
	}
	i = i + 2;
	while (1)
	{
		RS.append(1, inst[i]);
		if (inst[i + 1] == ',')
		{
			break;
		}
		i = i + 1;
	}
	i = i + 3;
	while (1)
	{
		RD1.append(1, inst[i]);
		if (inst[i + 1] == ',')
		{
			break;
		}
		i = i + 1;
	}
	i = i + 3;
	for(i;i<=inst.size();i++)
	{
		RD2.append(1, inst[i]);
	}

	//Cycle = cycle + 1
	for (i = 0; i <= 4; i++)
	{
		if (RScycle[i] >= 0)
		{
			RScycle[i] = RScycle[i] - 1;
		}
		if (NotRelease[i]==1)
		{
			RScycle[i] = 0;
		}
	}
	cycle = cycle + 1;

	//Check which RScycle is empty
	//Release
	Release(op, RS_val1, RS_val2, RAT_F, RS_value, buffermsg);

	//Use i to record the empty position of RS_val1
	//Use j to record the empty position of RS_val2
	i = 0;
	j = 0;
	while (1)
	{
		if (RS_table_ADD[i] == 0)
		{
			break;
		}
		i++;
	}
	while (1)
	{
		if (RS_table_MUL[j] == 0)
		{
			break;
		}
		j++;
	}

	//Check which RS is full
	if (kind == "ADDI" || kind == "ADD"|| kind == "SUB")
	{
		if (i == 3)
		{
			RS_FULL_ADD = 1;
		}
	}
	else if (kind == "MUL"||kind == "DIV")
	{
		if (j == 2)
		{
			RS_FULL_MUL = 1;
		}
	}


	if (InstEnd == 0)
	{
		//Wait until releasing
		//Case1: RS full
		if (RS_FULL_ADD == 1 || RS_FULL_MUL == 1)
		{
			if (RS_FULL_ADD == 1)
			{
				Waitcycle = RScycle[0];
				if (Waitcycle > RScycle[1])
				{
					Waitcycle = RScycle[1];
				}
				if (Waitcycle > RScycle[2])
				{
					Waitcycle = RScycle[1];
				}
			}
			else if (RS_FULL_MUL == 1)
			{
				Waitcycle = RScycle[3];
				if (Waitcycle > RScycle[4])
				{
					Waitcycle = RScycle[4];
				}
			}
			while (Waitcycle > 0)
			{
				cycle = cycle + 1;
				Waitcycle = Waitcycle - 1;
				for (k = 0; k <= 4; k++)
				{
					RScycle[k] = RScycle[k] - 1;
					if (RScycle[k] == 0)
					{
						Release(op, RS_val1, RS_val2, RAT_F, RS_value, buffermsg);
					}
				}
			}
				//Use i to record the empty position of RS_val1
				//Use j to record the empty position of RS_val2
				i = 0;
				j = 0;
				while (1)
				{
					if (RS_table_ADD[i] == 0)
					{
						break;
					}
					i++;
				}
				while (1)
				{
					if (RS_table_MUL[j] == 0)
					{
						break;
					}
					j++;
				}
			}

			if (kind == "ADDI" || kind == "ADD")
			{
				index = i;
				symbol = "+";
				RScycle[index] = 2;
			}
			else if (kind == "SUB")
			{
				index = i;
				symbol = "-";
				RScycle[index] = 2;
			}
			else if (kind == "MUL")
			{
				index = j + 3;
				symbol = "*";
				RScycle[index] = 8;
			}
			else if (kind == "DIV")
			{
				index = j + 3;
				symbol = "/";
				RScycle[index] = 8;
			}

			//RS
			stringstream convert, convert2;
			op[index] = symbol;
			if (RD1[0] == 'F')
			{
				reg_str = RD1[1];
				reg_dec = atoi(reg_str.c_str());

				if (RAT_F[reg_dec - 1] != "   ")
				{
					reg_char = RAT_F[reg_dec - 1][2];
					reg_dec2 = (int)reg_char - 48;
					reg_dec3 = reg_dec2;
					if (reg_dec2 <= 2)
					{
						if (RS_table_ADD[reg_dec2 - 1] == 0)
						{
							RS_val1[index] = to_string(RS_value[reg_dec2 - 1]);
						}
						else
						{
							RS_val1[index] = RAT_F[reg_dec - 1];
						}
					}
					else
					{
						reg_dec3 = reg_dec3 - 3;
						if (RS_table_MUL[reg_dec3 - 1] == 0)
						{
							RS_val1[index] = to_string(RS_value[reg_dec2 - 1]);
						}
						else
						{
							RS_val1[index] = RAT_F[reg_dec - 1];
						}
					}
				}
				else
				{
					convert << RF[reg_dec - 1];
					RS_val1[index] = convert.str();
				}
			}
			else
			{
				RS_val1[index] = RD1[0];
			}

			if (RD2[0] == 'F')
			{
				reg_str = RD2[1];
				reg_dec = atoi(reg_str.c_str());
				if (RAT_F[reg_dec - 1] != "   ")
				{
					reg_char = RAT_F[reg_dec - 1][2];
					reg_dec2 = (int)reg_char - 48;
					reg_dec3 = reg_dec2;
					if (reg_dec2 <= 2)
					{
						if (RS_table_ADD[reg_dec2 - 1] == 0)
						{
							RS_val2[index] = to_string(RS_value[reg_dec2 - 1]);
						}
						else
						{
							RS_val2[index] = RAT_F[reg_dec - 1];
						}
					}
					else
					{
						reg_dec3 = reg_dec3 - 3;
						if (RS_table_MUL[reg_dec3 - 1] == 0)
						{
							RS_val2[index] = to_string(RS_value[reg_dec2 - 1]);
						}
						else
						{
							RS_val2[index] = RAT_F[reg_dec - 1];
						}
					}
				}
				else
				{
					convert2 << RF[reg_dec - 1];
					RS_val2[index] = convert2.str();
				}
			}
			else
			{
				RS_val2[index] = RD2[0];
			}

			//RAT & op
			if (symbol == "+" || symbol == "-")
			{
				RS_table_ADD[i] = 1;
				reg_str = RS[1];
				num2str = to_string(index + 1);
				RAT_F[atoi(reg_str.c_str()) - 1] = "RS" + num2str;
			}
			else
			{
				RS_table_MUL[j] = 1;
				reg_str = RS[1];
				num2str = to_string(index + 1);
				num2str = "RS" + num2str;
				RAT_F[atoi(reg_str.c_str()) - 1] = num2str;
			}

			Printfmsg(RF, RAT_F, op, RS_val1, RS_val2, buffermsg);
	}
	
}
void Release(string op[], string RS_val1[], string RS_val2[], string RAT_F[],int RS_value[],string buffermsg[])
{
	int k = 0;
	int l = 0;
	//Release
	for (k = 0; k <= 4; k++)
	{
		if (RScycle[k] == 0)
		{
			if (RS_val1[k][0] == 'R' || RS_val2[k][0] == 'R')
			{
				NotRelease[k] = 1;
				continue;
			}
			else
			{
				NotRelease[k] = 0;
			}
			//Generate Buffer Message
			if (op[k] == "+")
			{
				RS_value[k] = atoi(RS_val1[k].c_str()) + atoi(RS_val2[k].c_str());
				buffermsg[0] = "(RS" + to_string(k + 1);
				buffermsg[0] = buffermsg[0] + ") " + RS_val1[k] + op[k] + RS_val2[k];
			}
			else if (op[k] == "-")
			{
				RS_value[k] = atoi(RS_val1[k].c_str()) - atoi(RS_val2[k].c_str());
				buffermsg[0] = "(RS" + to_string(k + 1);
				buffermsg[0] = buffermsg[0] + ") " + RS_val1[k] + op[k] + RS_val2[k];
			}
			else if (op[k] == "*")
			{
				RS_value[k] = atoi(RS_val1[k].c_str()) * atoi(RS_val2[k].c_str());
				buffermsg[1] = "(RS" + to_string(k + 1);
				buffermsg[1] = buffermsg[1] + ") " + RS_val1[k] + op[k] + RS_val2[k];
			}
			else if ((op[k] == "/"))
			{
				RS_value[k] = atoi(RS_val1[k].c_str()) / atoi(RS_val2[k].c_str());
				buffermsg[1] = "(RS" + to_string(k + 1);
				buffermsg[1] = buffermsg[1] + ") " + RS_val1[k] + op[k] + RS_val2[k];
			}
			op[k] = " ";
			RS_val1[k] = "   ";
			RS_val2[k] = "   ";
			if (k <= 2)
			{
				RS_table_ADD[k] = 0;
			}
			else
			{
				RS_table_MUL[k - 3] = 0;
			}

			//Update RS table
			for (l = 0; l <= 4; l++)
			{
				if (RS_val1[l] == ("RS" + to_string(k+1)))
				{
					RS_val1[l] = to_string(RS_value[k]);
				}
				else if (RS_val2[l] == ("RS" + to_string(k+1)))
				{
					RS_val2[l] = to_string(RS_value[k]);
				}
			}
			//Do not print messge in two kinds condition
			if (Waitcycle != 0 || InstEnd==1)
			{
				Printfmsg(RF, RAT_F, op, RS_val1, RS_val2, buffermsg);
			}
		}
	}
}
void Printfmsg(int RF[], string RAT_F[],string op[], string RS_val1[], string RS_val2[],string buffermsg[])
{
	int i = 0;
	int shift = 0;
	cout << "----------------------------" << endl;
	cout << "cycle:" << cycle << endl << endl;
	//Build RF
	cout << "   --RF--" << endl;
	cout << "F1 |  " << RF[0] << " |" << endl;
	cout << "F2 |  " << RF[1] << " |" << endl;
	cout << "F3 |  " << RF[2] << " |" << endl;
	cout << "F4 |  " << RF[3] << " |" << endl;
	cout << "F5 |  " << RF[4] << " |" << endl;
	cout << "   ------";
	cout << endl << endl;
	//Build RAT
	cout << "   --RAT--" << endl;
	cout << "F1 | " << RAT_F[0] << " |" << endl;
	cout << "F2 | " << RAT_F[1] << " |" << endl;
	cout << "F3 | " << RAT_F[2] << " |" << endl;
	cout << "F4 | " << RAT_F[3] << " |" << endl;
	cout << "F5 | " << RAT_F[4] << " |" << endl;
	cout << "   -------";
	cout << endl << endl;
	//Build RS ADD/SUB
	for (i = 0; i <= 4; i++)
	{
		shift = 0;
		shift = 3 - RS_val1[i].size();
		if (shift == 2)
		{
			RS_val1[i].insert(1, " ");
			RS_val1[i].insert(0, " ");
		}
		else if (shift == 1)
		{
			RS_val1[i].insert(0, " ");
		}
		shift = 0;
		shift = 3 - RS_val2[i].size();
		if (shift == 2)
		{
			RS_val2[i].insert(1, " ");
			RS_val2[i].insert(0, " ");
		}
		else if (shift == 1)
		{
			RS_val2[i].insert(0, " ");
		}
	}
	cout << "    --RS---------------" << endl;
	cout << "RS1 | " << op[0] << " |  " << RS_val1[0] << " |  " << RS_val2[0] << " |" << endl;
	cout << "RS2 | " << op[1] << " |  " << RS_val1[1] << " |  " << RS_val2[1] << " |" << endl;
	cout << "RS3 | " << op[2] << " |  " << RS_val1[2] << " |  " << RS_val2[2] << " |" << endl;
	cout << "    -------------------";
	cout << endl;
	cout << "BUFFER: " << buffermsg[0];
	cout << endl << endl;
	//Build RS MUL/DIV
	cout << "    --RS---------------" << endl;
	cout << "RS4 | " << op[3] << " |  " << RS_val1[3] << " |  " << RS_val2[3] << " |" << endl;
	cout << "RS5 | " << op[4] << " |  " << RS_val1[4] << " |  " << RS_val2[4] << " |" << endl;
	cout << "    -------------------";
	cout << endl;
	cout << "BUFFER: " << buffermsg[1];
	cout << endl << endl;
}
