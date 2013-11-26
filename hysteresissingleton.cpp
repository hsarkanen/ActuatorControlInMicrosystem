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
//  HysteresisSingleton -luokan toteutus
//
//----------------------------------------------------------------------------

#include "hysteresissingleton.h"

HysteresisData& getHysteresisData()
{
    // ensimmäisellä kutsulla datalle varataan muisti
    static HysteresisData data;

    // kaikille kutsuilla palautetaan viite tietorakenteeseen
    return data;
}
