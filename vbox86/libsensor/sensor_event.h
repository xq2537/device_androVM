/***********************************************************************
 * Module:  sensor_event.h
 * Author:  Morgan
 * Modified: Thursday, August 09, 2012 3:04:57 PM
 * Purpose: Declaration of the class sensor_event
 ***********************************************************************/

 #if !defined(__BuildroidSensors_sensor_event_h)
#define __BuildroidSensors_sensor_event_h



#define PORT 9292
#define VALUE_SIZE 16

struct sensor_event
{
   /* Type of the sensor event */
   int sensorType;
   char value[VALUE_SIZE];
   int time;
};

#endif
