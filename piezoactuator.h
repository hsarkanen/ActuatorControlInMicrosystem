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
//  PiezoActuator -luokan määrittely
//
//  Pietsotoimilaitteen määrittelevä luokka. Määrittää luokan joka käyttää
//  laitepinta-rajapintaa ohjaamaan pietsotoimilaitetta. Rajapinta selitetty
//  Actuator-luokan määrittelyssä.
//
//----------------------------------------------------------------------------


#ifndef PIEZO_ACTUATOR_H
#define PIEZO_ACTUATOR_H

#include "actuator.h"

class PiezoActuator : public Actuator
{
public:
	PiezoActuator();
	~PiezoActuator();
	
	void init();
    void setValue(int value);
    void getValue(int& value);
	
    private:

    // toimilaitteen id
    int _id;

    // toimilaitten ohjausarvo millivoltteina
    int _value;
};

#endif
