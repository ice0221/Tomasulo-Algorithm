# Tomasulo-Algorithm
This project is about Tomasulo Algorithm which uses to improve OOE of processor.<br>
In this project, I set the issue and write back can operate in the same cycle , <br>
Then, ADD and SUB instruction need to spend two cycles on executing , 
MUL and DIV instruction need to spend eight cycles on executing

## Introduction important parameter:
1. RF[5] (int)		     : Record RF value
2. RAT_F[5] (string)   : Record RAT value,the value is showed by "RS+num"
3. op[5] (string)	     : Record operating symbol in RS table
4. RS_val1[5] (string) : Record operating RD1 in RS table
5. RS_val2[5] (string) : Record operating RD2 in RS table
6. RS_table_ADD (int)	 : The value is 1 or 0,indicating the position of RS_ADD table whether is full
7. RS_table_MUL (int)	 : The value is 1 or 0,indicating the position of RS_MUL table whether is full
8. RS_value(int)	     : The value of RS after writing back
9. RScycle[5] (int)	   : The cycle time of inst executing needs to wait
10.cycle (int)		     : The current cycle time
11.buffermsg		       : The message of write back buffer


## Input Rule
1. Use "," between two register
2. Use " " btween kind of instruction and register

## Input
```
  ADDI F1, F2, 1
  SUB F1, F3, F4
  DIV F1, F2, F3
  MUL F2, F3, F4
  ADD F2, F4, F2
  ADDI F4, F1, 2
  MUL F5, F5, F5
  ADD F1, F4, F4
```

## Output
```
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


```
