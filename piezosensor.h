//----------------------------------------------------------------------------
// ACI-32020 Automaation reaaliaikajärjestelmät, syksy 2011
//
//         Harjoitustyö: Mikrosysteemitekniikan toimilaiteohjaus
//
// Vastaava assistentti: David Hästbacka, david.hastbacka@tut.fi, huone sd114
//              Ryhmä 8: Petri Rokka (189637), Heikki Sarkanen (198905)
//
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
	
    private:

    // anturin id
    int _id;

    // anturin mittausarvo voltteina
    double _value;
};

#endif
