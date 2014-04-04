//----------------------------------------------------------------------------
//
//  PiezoActuator -luokan toteutus
//
//----------------------------------------------------------------------------

#include "piezoactuator.h"
#include "laitepinta.h"
#include <QDebug>
	
PiezoActuator::PiezoActuator(): _value(0)
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

void PiezoActuator::setScale(int scale)
{
    double dScale = scale / 1000.0;
    asetaVahvistus(_id, dScale);
}

void PiezoActuator::setValue(int value)
{
    // skaalataan int millivoltit double volteiksi
    double ohjaus = value / 1000.0;
    // ohjataan toimilaitetta ja tarkistetaan paluuarvo
    int control = ohjaaLaitetta(_id, ohjaus);

    // tarkistetaan paluuarvo
    if(control < 0)
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
