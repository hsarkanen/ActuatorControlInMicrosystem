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
//  PiezoActuator -luokan toteutus
//
//----------------------------------------------------------------------------

#include "piezoactuator.h"
#include "laitepinta.h"
#include "qglobal.h"
	
PiezoActuator::PiezoActuator()
{
}

PiezoActuator::~PiezoActuator()
{
    // suljetaan purkajassa toimilaite handlella
    suljeLaite(_id);
}

void PiezoActuator::init()
{
    // avataan alustuksessa laitteen handle
    _id = avaaLaite(DA_OHJAUS);
}

void PiezoActuator::setValue(int value)
{
    // ohjataan toimilaitetta ja tarkistetaan paluuarvo
    int control = ohjaaLaitetta(_id, value);

    // tarkistetaan paluuarvo
    if(control <= 0)
    {
        qDebug("Ohjaaminen ei onnistunut.");
    }
    else
    {
        // otetaan ylös millä arvolla ohjattiin että voidaan kertoa se getValue:lla
        _value = value;
    }
}

void PiezoActuator::getValue(int &value)
{
    // palautetaan viiteparametrissa viimeksikäytetty ohjausarvo
    value = _value;
}