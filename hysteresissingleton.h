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
//  HysteresisSingleton -luokan määrittely
//
//  Hystereesin parametrit ja tulosket sisältävä tietorakenne. Johon
//  pääsee käsiksi Singletonina. Periaate hieman kuin globaalilla muuttujalla
//  Mutta takaa sen että tietorakenne on alustettu ensimmäisellä käyttökerralla.
//
//----------------------------------------------------------------------------


#ifndef HYSTERESIS_SINGLETON_H
#define HYSTERESIS_SINGLETON_H

#include <QMutex>

// Tietorakenne Hystereesiä varten
struct HysteresisData{

    // maksimikoko output ja measurement taulukoille
    unsigned int size;

    // ohjausarvojen taulukko. arvot voltteina
    double* output;

    // mittausarvojen taulukko. arvot voltteina
    double* measurement;

    // kertoo montako näytettä yksi kolmioaallon jakso sisältää
    double samples;

    // tietyn hystereesianalyysin sisältämän ohjaus ja mittausarvojen määrä
    unsigned int controlValuesAmount;
    QMutex mutex;
};

// Tietorakenteen viitteen antava globaali funktio
// Luo tietorakenteen ensimmäisellä käyttökerralla varaamalla
// muistin static-määreellä
HysteresisData& getHysteresisData();

#endif
