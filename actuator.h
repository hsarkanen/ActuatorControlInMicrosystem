//----------------------------------------------------------------------------
//
//  Actuator -luokan määrittely
//
//  Abstrakti-luokka toimilaitteille. Toimilaitteen voi alustaa ja sille
//  voi asettaa ohjauksen ja siltä voi lukea ohjauksen.
//  Todelliset toimilaitteet pitää periyttää tästä luokasta.
//
//----------------------------------------------------------------------------


#ifndef ACTUATOR_H
#define ACTUATOR_H

class Actuator
{
public:
    // oletusrakenteja ja -purkaja
    Actuator() {}
	virtual ~Actuator() {}
	
    // toimilaitteen alustus. pitää kutsua ainakin kerran ennen setValue/getValue käyttöä
	virtual void init() = 0;

    // toimilaitteen ohjauksen asettaminen
    virtual void setValue(int value) = 0;

    // toimilaitteen ohjauksen lukeminen
    virtual void getValue(int& value) = 0;

    // toimilaitteen vahvistuksen asettaminen
    virtual void setScale(int scale) = 0;
};

#endif
