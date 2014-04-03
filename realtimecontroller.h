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


class RealtimeController
{
public:
    
    // ohjaimen tilat, käynnissä ja pysätettynä
    enum State{ STARTED, STOPPED };

    // ohjaimen moodi: manuaalinen eli suora ohjaus ja PID-säätö
    enum Mode{ MODE_MANUAL, MODE_PID };

    // oletusrakenta ja -purkaja
	RealtimeController();
    ~RealtimeController();

    // PID-säätimen kp,ki ja kd vakioiden asettaminen
    void setPIDParameters( int kp, int ki, int kd );

    // Ohjauksen ja anturin skaalauskertoimien asettaminen
    void setScaleFactors( int sensor, int output );

    // PID-säätimen asetusarvon asettaminen
    //  - value: asetusarvo voltteina välillä 5 .. -5
    void setSetValue( int value );

    // Manuaalimoodin suoran ohjauksen asettaminen
    // - value: ohjausarvo voltteina välillä 5 .. -5
    void setOutputVoltage( int value );

    // Anturin arvon lukeminen
    // - value: anturin arvo viiteparametrina voltteina
    void readSensorVoltage( int& value );

    // Moodin asettaminen
    // - mode: Mode tyypin mukainen arvo manuaaliohjaukselle tai PID-säädölle
    void setMode( Mode mode );

    // Säädön käynnistäminen ja State vaihtuu STARTED tilaan
    void start();

    // Säädön pysäyttäminen ja State vaihtuu STOPPED tilaan
    void stop();

    // Nykyisen ohjauksen lukeminen
    // - value: nykyinen ohjaus viiteparametrina voltteina
    void readOutputVoltage( int &value );

    // Nykyisen moodin lukeminen
    // - mode: nykyinen moodi Mode-tyypin mukaisena
    void getMode( Mode& mode );

    // Nykyisen tilan lukeminen
    // -state: nykyinen tila State-tyypin mukaisena, eli STARTED tai STOPPED
    void getState( State& state );
};

#endif
