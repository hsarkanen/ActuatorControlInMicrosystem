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
