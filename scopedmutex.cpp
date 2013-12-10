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
//  ScopedMutex -luokan toteutus
//
//----------------------------------------------------------------------------

#include "scopedmutex.h"

// rakentaja lukitsee mutexin
ScopedMutex::ScopedMutex(QMutex& pMutex)
    : mutex(pMutex)	
{
    mutex.lock();
}

// purkaja vapauttaa mutexin
ScopedMutex::~ScopedMutex()
{
    mutex.unlock();
}
