#include "laitepinta.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#ifndef EI_LAITTEITA
#include <analogy/analogy.h>  // Xenomai NI PCI ajurin include
#endif

#ifdef EI_LAITTEITA
#define lsampl_t unsigned int
#define sampl_t unsigned short
// Simulated measurement value
static double simulatedData;
#endif

typedef struct laite_data {
  int          alustettu;
  double       vahvistus;
  laite_tyyppi tyyppi;
#ifndef EI_LAITTEITA
  a4l_sbinfo_t *sbinfo;
  a4l_chinfo_t *chinfo;
  a4l_rnginfo_t *rnginfo;
#endif
} laite;

static double dev_range[] = {-10.0,10.0};
#ifndef EI_LAITTEITA
static a4l_desc_t daq_laite;
#define READ_BUFFER_SIZE 6
#endif
static laite laitteet[2] = {
#ifndef EI_LAITTEITA
    {0,0.0,AD_MITTAUS,0,0,0},
    {0,0.0,AD_MITTAUS,0,0,0}
#else
    {0,0.0,AD_MITTAUS}, {0,0.0,AD_MITTAUS}
#endif
};

laite_id avaaLaite(laite_tyyppi tyyppi) {
  int slot_index = 0;
  int err = 0;
  
  if( laitteet[0].alustettu == 0 && laitteet[1].alustettu == 0 ) {
#ifndef EI_LAITTEITA
      daq_laite.sbdata = NULL;
      err = a4l_open(&daq_laite, "analogy0");
      if (err != 0){ printf("Opening error\n"); return -1;}
      daq_laite.sbdata = malloc(daq_laite.sbsize);
      err = a4l_fill_desc(&daq_laite);
      if (err != 0){ printf("Filling error\n"); return -1;}
#else
    simulatedData = 3;
#endif
  } else if( (laitteet[0].alustettu == 0 && laitteet[1].alustettu != 0) ) {
    slot_index = 0;
  } else if( (laitteet[1].alustettu == 0 && laitteet[0].alustettu != 0) ) {
    slot_index = 1;
  } else {
    return -1;
  }

#ifndef EI_LAITTEITA
  if( tyyppi == AD_MITTAUS ) {
      err = a4l_find_range(&daq_laite, 0, 0, A4L_RNG_VOLT_UNIT, dev_range[0], dev_range[1],
                           &laitteet[slot_index].rnginfo);
      if (err < 0){ printf("Rangeinfo error\n"); return -1;}
      err = a4l_get_chinfo(&daq_laite, 0, 0,
                           &laitteet[slot_index].chinfo);
      if (err != 0){ printf("Channelinfo error\n"); return -1;}
  } else {
      err = a4l_find_range(&daq_laite, 1, 0, A4L_RNG_VOLT_UNIT, dev_range[0], dev_range[1],
                           &laitteet[slot_index].rnginfo);
      if (err < 0){ printf("Rangeinfo error\n"); return -1;}
      err = a4l_get_chinfo(&daq_laite, 1, 0,
                           &laitteet[slot_index].chinfo);
      if (err != 0){ printf("Channelinfo error\n"); return -1;}
  }
#endif

  laitteet[slot_index].tyyppi = tyyppi;
  laitteet[slot_index].vahvistus = 1.0;
  laitteet[slot_index].alustettu = 1;
  return slot_index + 1;
}

void suljeLaite(laite_id id) {
  if( id < 1 || id > 2 ) return;
  int err = 0;
  laitteet[id - 1].alustettu  = 0;
  laitteet[id - 1].vahvistus  = 0;
  laitteet[id - 1].tyyppi     = AD_MITTAUS;

#ifndef EI_LAITTEITA
  if( laitteet[0].alustettu == 0 && laitteet[1].alustettu == 0 &&
      daq_laite.fd != 0 ) {
      if (daq_laite.sbdata != NULL) { free(daq_laite.sbdata); }
      err = a4l_close(&daq_laite);
      if (err != 0){ printf("Closing error\n"); return;}
      daq_laite.fd = 0;
  }
#endif
  return;
}

void asetaVahvistus(laite_id id, double vahvistus) {
  if( id < 1 || id > 2 ) return;
  laitteet[id - 1].vahvistus = vahvistus;
  return;
}

int lueMittaus(laite_id id, double* mittaus) {
  static sampl_t data = 0;
  laite *lte = 0;
  int err = 0;

  if( id < 1 || id > 2 || mittaus == 0 ) return -1;

  lte = &laitteet[id - 1];
  if( lte->tyyppi != AD_MITTAUS ) return -1;

#ifndef EI_LAITTEITA
  err = a4l_sync_read(&daq_laite, 0, CHAN(0), 0, &data, READ_BUFFER_SIZE);
  if (err < 0){ printf("Read error\n"); return -1;}
  err = a4l_rawtod(lte->chinfo, lte->rnginfo, mittaus, &data, 1);
  if (err < 0){ printf("Raw to int conversion error\n"); return -1;}
  *mittaus *= lte->vahvistus;
#else
  //data = 32767;
  // poistettu, jotta kÃ€sittely ja muutokset voitaisiin tehdÃ€ ohjauksen pÃ€ivityksen yhteydessÃ€
  // lisÃ€tty seuraavat:
  *mittaus = simulatedData;
  *mittaus *= lte->vahvistus;
  
#endif
  
  return err;
}

int ohjaaLaitetta(laite_id id, double ohjaus) {
  static sampl_t data = 0;
  laite *lte = 0;
  int err = 0;

  if( id < 1 || id > 2 ) return 0;
  
#ifndef EI_LAITTEITA
  
  lte = &laitteet[id - 1];
  if( lte->tyyppi != DA_OHJAUS ) return -1;

  ohjaus *= lte->vahvistus;

  if( ohjaus < dev_range[0] ) {ohjaus = dev_range[0];}
  else if( ohjaus > dev_range[1] ) {ohjaus = dev_range[1];}

  err = a4l_dtoraw(lte->chinfo, lte->rnginfo, &data, &ohjaus, 1);
  if (err < 0){ printf("Double to raw conversion error\n"); return -1;}
  err = a4l_sync_write(&daq_laite, 1, CHAN(0), 0, &data, sizeof(sampl_t));
  if (err < 0){ printf("Write error, code %d\n", err); return -1;}
#else
  
  lte = &laitteet[id - 1];
  ohjaus *= lte->vahvistus;

  double multiplier = 0.01;
  
  if( (simulatedData > 0 && simulatedData >= ohjaus) || 
      (simulatedData < 0 && simulatedData <= ohjaus) )
      simulatedData = ohjaus;
  else if( (simulatedData + multiplier*ohjaus) > dev_range[1] )
      simulatedData = dev_range[1];
  else if( (simulatedData + multiplier*ohjaus) < dev_range[0])
      simulatedData = dev_range[0];
  else
      simulatedData = (simulatedData + (multiplier*ohjaus));
  
#endif
  return err;
}
