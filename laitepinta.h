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
//  Laitepinta -luokan määrittely. Valmiiksi annettu koodi.
//
//  Toimilaitteen ja anturin käyttämisen mahdollistavan rajapinta.
//
//
//----------------------------------------------------------------------------


#ifndef LAITEPINTA_H
#define LAITEPINTA_H

//Kommentoi pois jos itse mittakortti on asennettu
#define EI_LAITTEITA

#ifdef __cplusplus
extern "C" {
#endif

typedef int laite_id;

typedef enum {
  AD_MITTAUS,
  DA_OHJAUS,
} laite_tyyppi;

laite_id avaaLaite(laite_tyyppi tyyppi);

void suljeLaite(laite_id id);

void asetaVahvistus(laite_id id, double vahvistus);

int lueMittaus(laite_id id, double* mittaus);

int ohjaaLaitetta(laite_id id, double ohjaus);

#ifdef __cplusplus
}
#endif

#endif
