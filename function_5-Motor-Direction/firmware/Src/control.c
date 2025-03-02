/*
 * control.c
 *
 *  Created on: 12 nov. 2021
 *      Author: Carole Meyer
 */


/* Includes ------------------------------------------------------------------*/

#include "control.h"

#include "steering.h"
#include "wheels.h"

#include "gpio.h"


/* Private define ------------------------------------------------------------*/

#define DISABLED 	0

//Definition des modes de vitesse :: SPEED MODES
#define STOP 		50
#define REVERSE 	40
#define WALK 		58
#define JOG 		65
#define RUN 		75

//Definition des modes de direction :: STEERING MODES
#define STRAIGHT 	50
#define HARD_L 		10
#define MODT_L 		25
#define SOFT_L 		40
#define HARD_R 		90
#define MODT_R 		75
#define SOFT_R 		60

//Definition des pourcentages de differentiel
#define DIFF_NONE 	0
#define DIFF_SMALL 	5
#define DIFF_MEDIUM 10
#define DIFF_LARGE 	15

//Definition des types d'azimut
#define AZIMUT_FORWARD 	0
#define AZIMUT_LEFT		1
#define AZIMUT_RIGHT	2

/* Private variables ---------------------------------------------------------*/

/* Programs ------------------------------------------------------------------*/

/**
*	Controle les MARG, MARD et MAV à partir de modeSpeed et modeSteer recus via le CAN
**/
void car_control(int requested_speed, int requested_steer){
	int diff = DIFF_NONE;
	int azimut = AZIMUT_FORWARD;

	// Limite la commande contenue dans le message CAN dans l'intervalle [0,100]
	if (requested_speed > 75){requested_speed = 75;}

	// Classification de la commande CAN recue
	if (requested_speed == DISABLED) {requested_speed = DISABLED;}
	else if (requested_speed <= REVERSE) {requested_speed = REVERSE;}
	else if (requested_speed < WALK) {requested_speed = STOP;}
	else if (requested_speed < JOG) {requested_speed = WALK;}
	else if (requested_speed < RUN) {requested_speed = JOG;}
	else {requested_speed = RUN;}

	// Limite la commande contenue dans le message CAN dans l'intervalle [0,100]
	if (requested_steer > 100){requested_steer = 100;}

	// Classification de la commande CAN recue
	if (requested_steer == DISABLED) {requested_steer = DISABLED;}
	else if (requested_steer <= HARD_L) {requested_steer = HARD_L;}
	else if (requested_steer <= MODT_L) {requested_steer = MODT_L;}
	else if (requested_steer <= SOFT_L) {requested_steer = SOFT_L;}
	else if (requested_steer < SOFT_R) {requested_steer = STRAIGHT;}
	else if (requested_steer < MODT_R) {requested_steer = SOFT_R;}
	else if (requested_steer < HARD_R) {requested_steer = MODT_R;}
	else {requested_steer = HARD_R;}

	//Actionnement de la commande du moteur de direction
	switch(requested_steer) {
		case STRAIGHT:
			diff = DIFF_NONE;
			azimut = AZIMUT_FORWARD;
			steering_set_position(GPIO_PIN_SET, STRAIGHT);
			break;
		case HARD_L:
			diff = DIFF_LARGE;
			azimut = AZIMUT_LEFT;
			steering_set_position(GPIO_PIN_SET, HARD_L);
			break;
		case MODT_L:
			diff = DIFF_MEDIUM;
			azimut = AZIMUT_LEFT;
			steering_set_position(GPIO_PIN_SET, MODT_L);
			break;
		case SOFT_L:
			diff = DIFF_SMALL;
			azimut = AZIMUT_LEFT;
			steering_set_position(GPIO_PIN_SET, SOFT_L);
			break;
		case HARD_R:
			diff = DIFF_LARGE;
			azimut = AZIMUT_RIGHT;
			steering_set_position(GPIO_PIN_SET, HARD_R);
			break;
		case MODT_R:
			diff = DIFF_MEDIUM;
			azimut = AZIMUT_RIGHT;
			steering_set_position(GPIO_PIN_SET, MODT_R);
			break;
		case SOFT_R:
			diff = DIFF_SMALL;
			azimut = AZIMUT_RIGHT;
			steering_set_position(GPIO_PIN_SET, SOFT_R);
			break;
		default:
			diff = DIFF_NONE;
			azimut = AZIMUT_FORWARD;
			steering_set_position(GPIO_PIN_RESET, STRAIGHT);
			break;
		}

	//Actionnement de la commande des moteurs de propulsion
	switch(requested_speed) {
		case STOP:
			wheels_set_speed(GPIO_PIN_RESET, GPIO_PIN_RESET, STOP, STOP);
			break;
		case REVERSE:
			if (azimut == AZIMUT_RIGHT){wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100+diff/2))/100, (modeSpeed*(100-diff/2))/100);}
			else if (azimut == AZIMUT_LEFT) {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100-diff/2))/100, (modeSpeed*(100+diff/2))/100);}
			else {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, modeSpeed, modeSpeed);}
			break;
		case WALK:
			if (azimut == AZIMUT_RIGHT){wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100-diff/2))/100, (modeSpeed*(100+diff/2))/100);}
			else if (azimut == AZIMUT_LEFT) {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100+diff/2))/100, (modeSpeed*(100-diff/2))/100);}
			else {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, modeSpeed, modeSpeed);}
			break;
		case JOG:
			if (azimut == AZIMUT_RIGHT){wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100-diff/3))/100, (modeSpeed*(100+(2*diff)/3))/100);}
			else if (azimut == AZIMUT_LEFT) {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100+(2*diff)/3))/100, (modeSpeed*(100-diff/3))/100);}
			else {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, modeSpeed, modeSpeed);}
			break;
		case RUN:
			if (azimut == AZIMUT_RIGHT){wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, (modeSpeed*(100-diff))/100, modeSpeed);}
			else if (azimut == AZIMUT_LEFT) {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, modeSpeed, (modeSpeed*(100-diff))/100);}
			else {wheels_set_speed(GPIO_PIN_SET, GPIO_PIN_SET, modeSpeed, modeSpeed);}
			break;
		default:
			wheels_set_speed(GPIO_PIN_RESET, GPIO_PIN_RESET, STOP, STOP);
			break;
	}


}

