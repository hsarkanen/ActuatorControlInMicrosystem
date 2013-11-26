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
//  ControllerInterface -luokan määrittely
//
//  ControllerInterface-luokka toimii rajapintana toteutettaville ohjauksille.
//  Tarkemman toiminnallisuuden voi lukea ohjelmiston määrittelydokumentista.
//
//----------------------------------------------------------------------------


#ifndef CONTROLLER_INTERFACE_H
#define CONTROLLER_INTERFACE_H

#include "actuator.h"
#include "sensor.h"
#include <QObject>
//#include "qvaluelist.h"


class ControllerInterface: public QObject
{
    Q_OBJECT
public:
   
    // ohjaimen tilat, käynnissä ja pysätettynä
	enum State{ STARTED, STOPPED };

    // ohjaimen moodi: manuaalinen eli suora ohjaus ja PID-säätö
	enum Mode{ MODE_MANUAL, MODE_PID };

    // oletusrakentaja ja -purkaja
	ControllerInterface() {}	
	virtual ~ControllerInterface() {}
	
    // PID-säätimen kp,ki ja kd vakioiden asettaminen
    virtual void setPIDParameters( int kp, int ki, int kd ) = 0;

    // Ohjauksen ja anturin skaalauskertoimien asettaminen
    virtual void setScaleFactors( int sensor, int output ) = 0;
	
    // PID-säätimen asetusarvon asettaminen
    //  - value: asetusarvo voltteina välillä 5 .. -5
    virtual void setSetValue( int value ) = 0;

    // Manuaalimoodin suoran ohjauksen asettaminen
    // - value: ohjausarvo voltteina välillä 5 .. -5
    virtual void setOutputVoltage( int value ) = 0;
	
    // Anturin arvon lukeminen
    // - value: anturin arvo viiteparametrina voltteina
    virtual void readSensorVoltage( int& value ) = 0;

    // Moodin asettaminen
    // - mode: Mode tyypin mukainen arvo manuaaliohjaukselle tai PID-säädölle
	virtual void setMode( Mode mode ) = 0;
	
    // Säädön käynnistäminen ja State vaihtuu STARTED tilaan
	virtual void start() = 0;

    // Säädön pysäyttäminen ja State vaihtuu STOPPED tilaan
	virtual void stop() = 0;
	
    // Nykyisen ohjauksen lukeminen
    // - value: nykyinen ohjaus viiteparametrina voltteina
    virtual void readOutputVoltage( int &value ) = 0;

    // Nykyisen moodin lukeminen
    // - mode: nykyinen moodi Mode-tyypin mukaisena
	virtual void getMode( Mode& mode ) = 0 ;

    // Nykyisen tilan lukeminen
    // -state: nykyinen tila State-tyypin mukaisena, eli STARTED tai STOPPED
	virtual void getState( State& state ) = 0;
	
    // Aloittaa hystereesianalyysin.
	virtual void startHysteresisAnalysis() = 0;
};
	
#endif 
