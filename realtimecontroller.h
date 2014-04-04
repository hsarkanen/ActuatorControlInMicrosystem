//----------------------------------------------------------------------------
//
//  RealtimeController -luokan määrittely
//
//  Toteuttaa säätimen, 500µs:n jaksonajalla, käyttäen Xenomai
//  reaaliaikalaajennosta, luo reaaliaika säikeen/taskin
//  ja kommunikoi pääsäikeen ja reaaliaikasäikeen välillä käyttäen Xenomai Pipea
//  Luo tarvittavat toimilaitteet
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
