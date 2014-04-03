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
//  Simple Controller -luokan määrittely
//
//  Yksinkertaisen PID-säätimen toteustus ilman säiettä ja
//  kovaa reaaliaikaisuutta. Käyttää QTimeria 20Hz:lla.
//
//----------------------------------------------------------------------------

#ifndef SIMPLE_CONTROLLER_H
#define SIMPLE_CONTROLLER_H

#include "controllerinterface.h"
#include <QTimer>
#include <QMutex>

class SimpleController : public ControllerInterface
{
    Q_OBJECT
public:

    // oletusrakentaja ja purkaja
	SimpleController();
	~SimpleController();
	
    // nämä jäsenfunktiot on kuvattu ControllerInterface-luokan määrittelyssä
    // ----
    virtual void setPIDParameters( int kp, int ki, int kd );
    virtual void setScaleFactors( int sensor, int output );

    virtual void setSetValue( int value );
    virtual void setOutputVoltage( int value );

    virtual void readSensorVoltage( int& value );
    virtual void setMode( Mode mode );

    // käynnistys toteutettu uusiksi. käynnistää timerin ja kutsuu kantaluokan toteutusta
    void start();

    // pysäytys toteutettu uusiksi. pysättää timerin ja kutsuu kantaluokan toteutusta
    void stop();

    virtual void readOutputVoltage( int &value );
    virtual void getMode( Mode& mode );
    virtual void getState( State& state );

    // Varsinaisen säädön totetuttava funktio
    // Tätä kutsutaan niin usein kuin säätöloopin taajuus on
    // Toteuttaa moodin ja tilan mukaisen säätämisen
    // sen mukaan onko moodi tai tila käynnistetty
    //
    void loop();

    // pid-säädön toteuttava funktio
    double pid(double setValue, double measurement,  double kp, double ki, double kd);

private Q_SLOTS:

    // timerin toteutus. kutsuu kantaluokan loop-metodia aina kun timeri laukeaa
	void loopTimerTimeout();
	
    private:
    // osoitin ajastimeen
    QTimer* _loopTimer;

    // toimilaitteen nykyinen ohjaus voltteina
    int _actuatorVoltage;

    // anturin nykyinen arvo voltteina
    int _sensorVoltage;

    // toimilaitteen ja anturin skaalauskertoimet
    int _actuatorScaleFactor;
    int _sensorScaleFactor;

    // osoittimet toimilaitteeseen ja anturiin
    // joiden toteutus perustuu abstrakteihin
    // Sensor ja Actuator luokkiin
    Sensor* _sensor;
    Actuator* _actuator;

    // Nykyinen moodi
    ControllerInterface::Mode _mode;

    // Nykyinen tila
    ControllerInterface::State _state;

    // Nykyiset PID-säätimen säätöparametrit
    int _kp,_ki,_kd;

    // Nykyinen PID-säätimen asetusarvo
    int _setValue;

    // PID-säätimen tilaa kuvaavat muuttujat
    int _u[3];
    int _e[3];
    int _k[3];

    // Yksityisiä jäsenmuuttujia suojaava mutexi. Ettei
    // useamman säikeen käyttö sotkisi muuttujia. Vain
    // Yksi säie pääsee kerralla käyttämään muuttujia
    QMutex _mutex;
};
	
#endif
