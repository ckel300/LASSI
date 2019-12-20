#ifndef _INITIALIZE_H_   // include guard
#define _INITIALIZE_H_

void Memory_Initialization_AtStart(void);

void Global_Array_Initialization_AtStart(void);

void Reset_Global_Arrays(void);

void Initial_Conditions_Simple(void);

int Float_To_Int(float val);

int modulo(int one, int two);

void Initial_Conditions_FromFile(void);

float Coordinate_Distance(int coordone, int coordtwo, int coord);

void Initial_Conditions_BreakBonds(void);

float Temperature_Function(int mode, long nGen);

void Calculate_Rot_Bias(float CurrentTemp);

#endif // _INITIALIZE_H_
