//----------------------------------------------------------------------------
//
//  PiezoSensor -luokan määrittely
//
//  Pietsoanturin määrittelevä luokka. Käyttää laitepinta rajapintaa
//  anturin lukemiseksi. Rajapinta selitetty Sensor-luokan määritettelyssä.
//
//----------------------------------------------------------------------------


#ifndef PIEZOSENSOR_H
#define PIEZOSENSOR_H

#include "sensor.h"

class PiezoSensor : public Sensor
{
public:
	PiezoSensor();
	~PiezoSensor();
	
	void init();	
    void getValue(int &value);
    void setScale(int scale);

private:

    // anturin id
    int _id;

    // anturin mittausarvo millivoltteina
    int _value;
};

#endif
