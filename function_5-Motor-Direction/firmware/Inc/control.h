/*
 * control.h
 *
 *  Created on: 12 nov. 2021
 *      Author: Carole Meyer
 */

#ifndef CONTROL_H_
#define CONTROL_H_

extern int modeSpeed;
extern int modeSteer;

#include <stdbool.h>

/**
*	Controle les MARG, MARD et MAV à partir de modeSpeed et modeSteer recus via le CAN
**/
void car_control(int requested_speed, int requested_steer);


/* brief	Calculate the movement of the car according to the distance between the car and the location we want to join
 * param	double distance		Distance between the two GPS location in meters
 * retval	None
 * */
double go_straight_without_GPS(double distance);

/* brief	Manage the movement between the car and the destination
 * param	double distance		Distance between the car and the destination
 * 			double angleToGo	Angle in degrees between the car and the destination according to the North
 * 			int first			Set if the routine begins in a new location
 * retval 	double angleCarDiff	Angle in degrees traveled from the last car location
 * */
double direction_speed_management_without_GPS(double distance, double angleToGo, int first);

/* brief	Manage the movement of the car without GPS connected
 * param	double carLatitudeStart, carLongitudeStart	Car coordinates at the start of the routine
 * 			double destLatitude, destLongitude			Destination coordinates
 * retval 	None
 **/
void movement_without_GPS(double carLatitudeStart, double carLongitudeStart, double destLatitude, double destLongitude);

/* brief	Determine the angle between the North and the car
 * param	double carLatitudePrec, double carLongitudePrec		Previous GPS coordinates of the car
 * 			double carLatitude, double carLongitude				Actual GPS coordinates of the car
 * retval	double angleCar		Angle between the North and the car
 * */
double get_angle_car(double carLatPre, double carLongPre, double carLat, double carLong);

/* brief	Determine the angle between the North and the destination
 * param	double carLatitude, double carLongitude		Actual GPS coordinates of the car
 * 			double destLatitude, double destLongitude	Actual GPS coordinates of the destination
 * retval	double angleDest	Angle between the North and the destination
 * */
double get_angle_dest(double carLat, double carLong, double destLat, double destLong);

/* brief	Determine the angle we need to turn to join the right location
 * param	double angleCar		Angle between the North and the car
 * 			double angleDest	Angle between the North and the destination
 * retval	double angleToGo	Angle between the car and the destination
 * */
double get_angle_to_go(double angleCar, double angleDest);

/* brief	Manage the direction of the car according to the angle between the car axis and the GPS location
 * param	double angleToGo	Angle between the axis of the car and the GPS location in degrees
 * retval	int angleCommand	Command to control the steering of the wheels
 * */
int calculate_direction_command_with_GPS(double angleToGo);

/* brief	Control the speed and the steering of the car in real time according to the distance and the angle between the car and the location we want to join
 * param	double distance		Distance between the two GPS location in meters
 * 			double beta			Angle between the axis of the car and the GPS location in degrees
 * 			double alpha		Angle between the axis of the car and the North
 * retval	None
 * */
void direction_speed_management_with_GPS(double distance, double angleToGo);

/* brief	Manage the movement between the car and the destination
 * param	double carLatitude, carLongitude			Actual GPS coordinates of the car
 * 			double carLatitudePrec, carLongitudePrec	Previous GPS coordinates of the car
 * 			double destLatitude, destLongitude			Actual GPS coordinates of the destination
 * retval 	None
 * */
void movement_with_GPS(double carLat, double carLong, double carLatPrec, double carLongPrec, double destLat, double destLong);


/* brief	Make a 360 degrees turn
 * param	None
 * retval	None
 * */
void turn360(void);


#endif /* CONTROL_H_ */
