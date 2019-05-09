/*
 * functions.h
 *
 *  Created on: Apr 22, 2019
 *      Author:
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/* Global Variables */
/*********************************/
extern int g_isample_count ;
extern int g_isound_count;
extern int g_iADC14Result;
extern int g_iverify;
extern int g_ilights;

extern float g_fstoreavg;
extern float g_fMeasureLux;
extern float g_fADCResult;
extern float g_fsample_avg;
extern float g_favg;
extern float g_vfsound[5];
extern float g_vfsamples[10];
extern float g_fkid_noise;

/*Functions*/
void LampsSelection();
void SetUp();
void InitialBlink();
void InitialState();
void LightsOn();
void LightsOff();
void ADCSamples();
void Sound();




#endif /* FUNCTIONS_H_ */
