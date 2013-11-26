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
//  PiezoSensor -luokan toteutus
//
//----------------------------------------------------------------------------

#include "piezosensor.h"
#include "laitepinta.h"
#include "qglobal.h"

PiezoSensor::PiezoSensor()
{
}

PiezoSensor::~PiezoSensor()
{
    // suljetaan anturi handlella
    suljeLaite(_id);
}

void PiezoSensor::init()
{
    // avataan alustuksessa anturin handle
    _id = avaaLaite(AD_MITTAUS);
}

void PiezoSensor::getValue(int& value)
{
    // luetaan anturin mittausarvo
    double measuredValue = 0;   
    int measure = lueMittaus(_id, &measuredValue);

    // tarkistetaan paluuarvo
    if(measure <= 0)
    {
        qDebug("Mittaaminen ei onnistunut.");
    }
    else
    {
        // otetaan viimeisin arvo ylös
        _value = measuredValue;
    }
    
    // palautetaan mittausarvo tai viimeksi luettu arvo
    value = _value;
}
