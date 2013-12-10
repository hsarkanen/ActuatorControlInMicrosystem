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
//  ScopedMutex -luokan määrittely
//
//  Toteuttaa mutexin joka lukittautuu skoopin eli { } -lohkon alussa ja
//  vapauttaa lukituksen sen lopussa. Toteutettu niin että rakentaja lukittaa
//  ja purkaja vapauttaa lukituksen. Käytettään luomalla ScopedMutex-luokasta
//  skoopin sisälle lokaali muuttuja.
//
//----------------------------------------------------------------------------


#ifndef SCOPED_MUTEX_H
#define SCOPED_MUTEX_H

#include <QMutex>

class ScopedMutex
{
public:
    ScopedMutex(QMutex& pMutex);
    ~ScopedMutex();
    
private:
    QMutex& mutex;
};

#endif
