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
    void setScale(int scale);
    void setValue(int value);
    void getValue(int& value);
	
    private:

    // toimilaitteen id
    int _id;

    // toimilaitten ohjausarvo millivoltteina
    int _value;
};

#endif
