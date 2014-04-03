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
//  SimpleController -luokan toteutus
//
//----------------------------------------------------------------------------

#include "simplecontroller.h"
#include "piezoactuator.h"
#include "piezosensor.h"
#include "scopedmutex.h"
#include "qglobal.h"
#include <QTimer>
#include <QMutex>
#include <QDebug>


SimpleController::SimpleController():
    _actuatorVoltage(0), _sensorVoltage(0), _state(ControllerInterface::STOPPED),
    _mutex(QMutex::Recursive)
{
    // varataan muisti anturille ja alustetaa se
    _sensor = new PiezoSensor();
    _sensor->init();

    // varataan muisti toimilaitteelle ja alustetaan se
    _actuator = new PiezoActuator();
    _actuator->init();

    // alustetaan skaalauskertoimet ykkösiksi
    _actuatorScaleFactor = 1;
    _sensorScaleFactor = 1;

    // alustetaan moodi suoraksi jänniteohjaukseksi eli manuaaliksi
    setMode(ControllerInterface::MODE_MANUAL);

    // ajastimen luominen
    _loopTimer = new QTimer();

    // oletuksena säätö pysäytettynä
    stop();        
}

SimpleController::~SimpleController()
{}

// valmiiksi annetun pid-säädin logiikan toteutus
double SimpleController::pid(double setValue, double measurement,  double kp, double ki, double kd)
{
    // varataan yksityisen tietorakenteen tiedot käyttöön
    // säie pysähtyy seuraavalle riville kunnes saa resurssit varattua
    ScopedMutex mutex(_mutex);

    // jaksoaika
    // pitäisi olla varmaankin 1/20 mutta tällä mentiin-ohjelman
    // demonstroinnissa
    const double T = 1/50.0;

    // jaetaan kp ja kd parametrit jotta saavutetaan parempi käyttöalue
    kp = kp / 100.0;
    kd = kd / 100.0;

    // e[0] nykyinen erosuure,  e[1] edellinen e[2] sitä edellinen
    _e[0] = setValue - measurement;

    // algoritmit k1=k[0],k2=k[1],k3=k[2] kertoimet
    _k[0] = kp + ki * T/2.0 + 2.0*kd/T;
    _k[1] = ki*T -  4.0*kd/T;
    _k[2] = -kp + ki*T/2.0 + 2.0*kd/T;

    // ohjauksen laskeminen
    // u[0] nykyinen ohjaus, u[1] edellinen u[2] sitä edellinen
    _u[0] = _k[0] * _e[0] +
              _k[1] * _e[1] +
              _k[2] * _e[2] + _u[2];

    // rajataan ulostulot -5 ja 5 (V) välille
    if( _u[0] > 5.0 )
    {
        _u[0] = 5.0;
    }
    else if( _u[0] < -5.0 )
    {
        _u[0] = -5.0;
    }

    // päivitetään edellisten kierrosten muuttujat
    _e[2] = _e[1];
    _e[1] = _e[0];

    _u[2] = _u[1];
    _u[1] = _u[0];

    qDebug("Debug:");
    qDebug()<<"S M E: " + QString::number(setValue) +" "+ QString::number(measurement)+" "+
      QString::number(setValue - measurement);
    qDebug()<<"P I D: " + QString::number(kp)+" "+ QString::number(ki)+" "+QString::number(kd);
    qDebug()<<"K     :" + QString::number(_k[0])+" "+ QString::number(_k[1])+" "+ QString::number(_k[2]);
    qDebug()<<"E     :" + QString::number(_e[0])+" "+ QString::number(_e[1])+" "+ QString::number(_e[2]);
    qDebug()<<"U     :" + QString::number(_u[0])+" "+ QString::number(_u[1])+" "+ QString::number(_u[2]);

    return _u[0];
}

void SimpleController::start()
{
    ScopedMutex mutex(_mutex);
    // resetoidaan säätimen tila
    // ohjausta käynnistettäessä
    _u[0] = 0;
    _u[1] = 0;
    _u[2] = 0;

    _k[0] = 0;
    _k[1] = 0;
    _k[2] = 0;

    _e[0] = 0;
    _e[1] = 0;
    _e[2] = 0;

    // tila käynnissä olevaksi
    _state = ControllerInterface::STARTED;
    qDebug("SimpleController: STARTED");

    // kytketään ajastimen signaali sen käsittelijään
    connect(_loopTimer, SIGNAL(timeout()), this, SLOT(loopTimerTimeout()));

    // käynnistetään ajastin 50ms jaksonajalla eli 20Hz taajuus
    _loopTimer->start(50);
}

void SimpleController::stop()
{
    ScopedMutex mutex(_mutex);

    // tila pysäytetyksi
    _state = ControllerInterface::STOPPED;
    qDebug("SimpleController: STOPPED");

    // pysäytetään ajastin
    _loopTimer->stop();
}

void SimpleController::setPIDParameters( int kp, int ki, int kd )
{
    ScopedMutex mutex(_mutex);

    // astetaan käyttöön PID säätimet parametrit
    _kp = kp;
    _ki = ki;
    _kd = kd;
}

void SimpleController::setScaleFactors( int sensor, int output )
{
    ScopedMutex mutex(_mutex);

    // asetetaan käyttöön skaalauskertoimet ulostulolle ja anturille
    _actuatorScaleFactor = output;
    _sensorScaleFactor = sensor;
}

void SimpleController::setSetValue( int value )
{
    ScopedMutex mutex(_mutex);

    // asetetaan käyttöön asetusarvo
    _setValue = value;
}

void SimpleController::setOutputVoltage( int value )
{
    ScopedMutex mutex(_mutex);

    // asetetaan käyttöön skaalattu ulostulojännite
    _actuatorVoltage = value * _actuatorScaleFactor;
}

void SimpleController::readSensorVoltage( int& value )
{
    ScopedMutex mutex(_mutex);

    // luetaan anturin mittausarvo
    // anturilukeman palautus viiteparametrina
    value = _sensorVoltage;
}

void SimpleController::setMode(   Mode mode )
{
    ScopedMutex mutex(_mutex);

    // asetetaan käyttöön moodi
    _mode = mode;
}

void SimpleController::readOutputVoltage( int &value )
{
    ScopedMutex mutex(_mutex);

    // luetaan toimilaitteen asetusarvo
    // asetusarvon palautus viiteparametrina
    _actuator->getValue(value);
}

void SimpleController::getMode( Mode& mode )
{
    ScopedMutex mutex(_mutex);

    // käytössä olevan moodin palautus viiteparametrina
    mode = _mode;
}

void SimpleController::getState( State& state )
{
    ScopedMutex mutex(_mutex);

    // käytössä olevan tilan palautus viiteparametrina
    state = _state;
}

void SimpleController::loop()
{
    ScopedMutex mutex(_mutex);
    // ei ohjata tai lueta antureita jos tila on pysäytetty
    if(_state == ControllerInterface::STOPPED)
    {
        return;
    }

    // haetaan anturin arvo ja käännetään se vastaluvuksi, jotta
    // todellinen anturi kulkisi samaan suuntaan toimilaitteen kanssa
    // skaalataan lopuksi anturin skaalauskertoimella
    int sensorValue = 0;
    int output;
    _sensor->getValue(sensorValue);
    _sensorVoltage = -sensorValue * _sensorScaleFactor;

    // tarkastetaan ajetaanko PID-säätöä vai suoraa ohjausta
    if( _mode == ControllerInterface::MODE_MANUAL)
    {
        // suorassa jänniteohjauksessa laitetaan suoraan käyttäjän syöttämä skaalattu arvo
        // ulostuloon
        output = _actuatorVoltage;
    }
    else
    {
        // PID-säädössä ajetaan algoritmi ja asetetaan ulostulo
        output = pid(_setValue, _sensorVoltage, _kp, _ki, _kd) *
                _actuatorScaleFactor;
    }

    // asetetaan ulostulo oikeasti toimilaitteelle
    _actuator->setValue(output);
//    qDebug()<< "asetettu " << output<< "toimilaitteelle";

}

// jokaisella ajastimen kierroksella eli jaksonajan välein
// kutsutaan kantaluokan loop()-jäsenfunktiota
// joka totetuttaa ohjauksen
void SimpleController::loopTimerTimeout()
{
    loop();
}
