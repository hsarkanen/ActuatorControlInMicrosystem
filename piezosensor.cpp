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

PiezoSensor::PiezoSensor(): _value(0)
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

void PiezoSensor::setScale(int scale)
{
    double dScale = scale / 1000.0;
    asetaVahvistus(_id, dScale);
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
        // skaalataan double voltit int millivolteiksi ja otetaan viimeisin arvo ylös
        _value = 1000 * measuredValue;
    }
    
    // palautetaan mittausarvo tai viimeksi luettu arvo
    value = _value;
}
