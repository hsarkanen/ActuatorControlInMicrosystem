#ifndef LAITEPINTA_H
#define LAITEPINTA_H

//Kommentoi pois jos itse mittakortti on asennettu
//#define EI_LAITTEITA

#ifdef __cplusplus
extern "C" {
#endif

typedef int laite_id;

typedef enum {
  AD_MITTAUS,
  DA_OHJAUS
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
