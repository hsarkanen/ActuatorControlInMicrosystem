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
//  Laitepinta - valmiiksi annettu toteutus.
//
//----------------------------------------------------------------------------

#include "laitepinta.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#ifndef EI_LAITTEITA
//#include <comedilib.h>
#include <analogy/analogy.h>
#endif

#ifdef EI_LAITTEITA
#define lsampl_t unsigned int
// Simulated measurement value
static double simulatedData;
#endif

//lsampl_t voltage_to_rawdata(double voltage, double limits[],
//                            lsampl_t maxvalue);
//double rawdata_to_voltage(lsampl_t rawdata, double limits[],
//                          lsampl_t maxvalue);

typedef struct laite_data {
  int          alustettu;
  double       vahvistus;
  double       arvo;
  laite_tyyppi tyyppi;
  long     daq_maxval;
#ifndef EI_LAITTEITA
//  comedi_t*    daq_laite;
//  comedi_insn  daq_insn;
  a4l_sbinfo_t *sbinfo;
  a4l_chinfo_t *chinfo;
  a4l_rnginfo_t *rnginfo;
#endif
} laite;

//static double dev_range[] = {-10,10};
static a4l_desc_t daq_laite;
static laite laitteet[2] = {
#ifndef EI_LAITTEITA
//  {0,0.0,0.0,AD_MITTAUS,0,0,{0,0,0,0,0,{0,0,0}}},
//  {0,0.0,0.0,AD_MITTAUS,0,0,{0,0,0,0,0,{0,0,0}}}
#else
  {0,0.0,0.0,AD_MITTAUS,0}, {0,0.0,0.0,AD_MITTAUS,0}
#endif
};

laite_id avaaLaite(laite_tyyppi tyyppi) {
  int slot_index = 0;
  int err = 0;
  
  if( laitteet[0].alustettu == 0 && laitteet[1].alustettu == 0 ) {
#ifndef EI_LAITTEITA
//    laitteet[0].daq_laite = comedi_open("/dev/comedi0");
      daq_laite.sbdata = NULL;
      err = a4l_open(&daq_laite, "analogy0");
      if (err != 0){ printf("Opening error\n"); return -1;}
      // Varaa muistia descriptorin tietojen lukemiseen
      daq_laite.sbdata = malloc(daq_laite.sbsize);
      // Lue descriptorin tiedot
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
      err = a4l_get_rnginfo(&daq_laite, daq_laite.idx_read_subd, 0, 0,
                            &laitteet[slot_index].rnginfo);
      if (err != 0){ printf("Rangeinfo error\n"); return -1;}
      err = a4l_get_chinfo(&daq_laite, daq_laite.idx_read_subd, 0,
                           &laitteet[slot_index].chinfo);
      if (err != 0){ printf("Channelinfo error\n"); return -1;}
//    laitteet[slot_index].daq_insn.insn = INSN_READ;
//    laitteet[slot_index].daq_insn.subdev = 0;
//    laitteet[slot_index].daq_insn.chanspec = CR_PACK(1,0,AREF_DIFF);
//    laitteet[slot_index].daq_maxval
//        = comedi_get_maxdata(laitteet[slot_index].daq_laite, 0, 0);
  } else {
      err = a4l_get_rnginfo(&daq_laite, daq_laite.idx_write_subd, 0, 0,
                            &laitteet[slot_index].rnginfo);
      if (err != 0){ printf("Rangeinfo error\n"); return -1;}
      err = a4l_get_chinfo(&daq_laite, daq_laite.idx_write_subd, 0,
                           &laitteet[slot_index].chinfo);
      if (err != 0){ printf("Channelinfo error\n"); return -1;}
//    laitteet[slot_index].daq_insn.insn = INSN_WRITE;
//    laitteet[slot_index].daq_insn.subdev = 1;
//    laitteet[slot_index].daq_insn.chanspec = CR_PACK(0,0,AREF_GROUND);
//    laitteet[slot_index].daq_maxval
//        = comedi_get_maxdata(laitteet[slot_index].daq_laite, 1, 0);
  }
  laitteet[slot_index].daq_maxval = laitteet[slot_index].rnginfo->max;
//  laitteet[slot_index].daq_insn.n = 1;
#else
  laitteet[slot_index].daq_maxval = 65535;
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
  laitteet[id - 1].arvo       = 0;
  laitteet[id - 1].tyyppi     = AD_MITTAUS;
  laitteet[id - 1].daq_maxval = 0;

#ifndef EI_LAITTEITA
  if( laitteet[0].alustettu == 0 && laitteet[1].alustettu == 0 &&
      daq_laite.fd != 0 ) {
//    comedi_close(laitteet[id - 1].daq_laite);
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
  lsampl_t data = 0;
  laite *lte = 0;
  int err = 0;

  if( id < 1 || id > 2 || mittaus == 0 ) return -1;

  lte = &laitteet[id - 1];
  if( lte->tyyppi != AD_MITTAUS ) return -1;

#ifndef EI_LAITTEITA
//  lte->daq_insn.data = &data;
//  err = comedi_do_insn(lte->daq_laite, &lte->daq_insn);
  err = a4l_sync_read(&daq_laite, 0, 0, 0, &data, sizeof(data));
  if (err < 0){ printf("Read error\n"); return -1;}
//  *mittaus = rawdata_to_voltage(data, dev_range, lte->daq_maxval);
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
  lsampl_t data = 0;
  laite *lte = 0;
  int err = 0;

  if( id < 1 || id > 2 ) return 0;
  
#ifndef EI_LAITTEITA
  
  lte = &laitteet[id - 1];
  if( lte->tyyppi != DA_OHJAUS ) return -1;

  ohjaus *= lte->vahvistus;

//  data = voltage_to_rawdata(ohjaus, dev_range, lte->daq_maxval);
//  printf("Writing: %d to output\n", ohjaus);
  err = a4l_dtoraw(lte->chinfo, lte->rnginfo, &data, &ohjaus, 1);
  if (err < 0){ printf("Double to raw conversion error\n"); return -1;}
//  lte->daq_insn.data = &data;
//  err = comedi_do_insn(lte->daq_laite, &lte->daq_insn);
  err = a4l_sync_write(&daq_laite, 1, 0, 0, &data, sizeof(data));
  if (err < 0){ printf("Write error\n"); return -1;}
#else
  
  lte = &laitteet[id - 1];
  ohjaus *= lte->vahvistus;

  double multiplier = 0.01;
  
  if( (simulatedData > 0 && simulatedData >= ohjaus) || 
      (simulatedData < 0 && simulatedData <= ohjaus) )
      simulatedData = ohjaus;
  else if( (simulatedData + multiplier*ohjaus) > 10 )
      simulatedData = 10;
  else if( (simulatedData + multiplier*ohjaus) < -10)
      simulatedData = -10;
  else
      simulatedData = (simulatedData + (multiplier*ohjaus));
  
#endif
  return err;
}

lsampl_t voltage_to_rawdata(double voltage, double limits[], lsampl_t maxvalue) {
  double tmp = (voltage - limits[0]) / (limits[1] - limits[0])
    * (double)maxvalue;
  lsampl_t conv_smpl = 0;
  if( tmp < 0 ) conv_smpl = 0;
  else if( tmp > (double)maxvalue ) conv_smpl = maxvalue;
  else conv_smpl = (lsampl_t)((int)(tmp + 0.5));

  return conv_smpl;
}

double rawdata_to_voltage(lsampl_t rawdata, double limits[], lsampl_t maxvalue) {
  double conv_phys = (double)rawdata;
  conv_phys /= (double)maxvalue;
  conv_phys *= (limits[1] - limits[0]);
  conv_phys += limits[0];

  return conv_phys;
}
