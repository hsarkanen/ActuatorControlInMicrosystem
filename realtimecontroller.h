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
//  RealtimeController -luokan määrittely
//
//  Toteuttaa säätimen loopin uusiksi niin että se käyttää Linuxin RTAI
//  reaaliaikalaajennosta 2000Hz deterministisen säädinloopin toteutumiseksi.
//  Määrittää PID-säätimen uusiksi, koska kp,ki ja kd arvoja pitää hieman skaalata,
//  jotta niillä saavutetaan parempi käyttöalue käyttöliittymällä.
//
//----------------------------------------------------------------------------


#ifndef REALTIME_CONTROLLER_H
#define REALTIME_CONTROLLER_H

#include "controllerinterface.h"

class RealtimeController : public ControllerInterface
{
public:
    
    // oletusrakenta ja -purkaja
	RealtimeController();
    ~RealtimeController();

    // nämä jäsenfunktiot on kuvattu ControllerInterface-luokan määrittelyssä
    // ----
    void setPIDParameters( int kp, int ki, int kd );
    void setScaleFactors( int sensor, int output );

    void setSetValue( int value );
    void setOutputVoltage( int value );

    void readSensorVoltage( int& value );
    void setMode( Mode mode );

    void start();
    void stop();

    void readOutputVoltage( int &value );
    void getMode( Mode& mode );
    void getState( State& state );
};

#endif
