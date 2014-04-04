//----------------------------------------------------------------------------
//
//  Sensor -luokan määrittely
//
//  Abstrakti-luokka antureille. Anturin voi alustaa ja
//  siltä voi lukea mittausarvon.
//  Todelliset anturit pitää periyttää tästä luokasta.
//
//----------------------------------------------------------------------------

#ifndef SENSOR_H
#define SENSOR_H

class Sensor
{
public:

    // oletusrakentaja ja -purkaja
	Sensor() {}	
	virtual ~Sensor() {}
	
    // anturin alustus. pitää kutsua ainakin kerran ennen getValuen käyttöä
	virtual void init() = 0;	   

    // anturin mittaus arvon lukeminen
    virtual void getValue(int& value) = 0;

    // anturin vahvistuksen asettaminen
    virtual void setScale(int scale) = 0;
};

#endif
