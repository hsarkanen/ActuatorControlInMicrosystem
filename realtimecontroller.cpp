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
//  RealtimeController -luokan toteutus
//
//----------------------------------------------------------------------------

#include "realtimecontroller.h"
#include "piezoactuator.h"
#include "piezosensor.h"
#include "qglobal.h"
#include <QTimer>
#include <QDebug>
#include <sys/mman.h>
#include <malloc.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
// xenomai includes
#include <native/task.h>
#include <native/pipe.h>

#define MAX_MESSAGE_LENGTH 10

RT_TASK task_desc;
RT_PIPE pipe_desc;
int pipefd = 0;

int pid(double setValue, double measurement,  double kp, double ki, double kd)
{
    // muuten sama toteutus PID-säätimen algoritmille kuin
    // kantaluokassa mutta jakson aika T on parempi 2000Hz reaaliaika säikeelle

    // PID-säätimen tilaa kuvaavat muuttujat
    static double u[3];
    static double e[3];
    static double k[3];

    const double T = 1/2000.0;

    e[0] = setValue - measurement;

    k[0] = kp + ki * T/2.0 + 2.0*kd/T;
    k[1] = ki*T -  4.0*kd/T;
    k[2] = -kp + ki*T/2.0 + 2.0*kd/T;

    u[0] = k[0] * e[0] +
               k[1] * e[1] +
               k[2] * e[2] + u[2];

    if( u[0] > 5.0 )
    {
        u[0] = 5.0;
    }
    else if( u[0] < -5.0 )
    {
        u[0] = -5.0;
    }

    e[2] = e[1];
    e[1] = e[0];

    u[2] = u[1];
    u[1] = u[0];

    qDebug()<<"PID-Debug:";
    qDebug()<<"S M E: " + QString::number(setValue) +" "+ QString::number(measurement)+" "+
      QString::number(setValue - measurement);
    qDebug()<<"P I D: " + QString::number(kp)+" "+ QString::number(ki)+" "+QString::number(kd);
    qDebug()<<"K     :" + QString::number(k[0])+" "+ QString::number(k[1])+" "+ QString::number(k[2]);
    qDebug()<<"E     :" + QString::number(e[0])+" "+ QString::number(e[1])+" "+ QString::number(e[2]);
    qDebug()<<"U     :" + QString::number(u[0])+" "+ QString::number(u[1])+" "+ QString::number(u[2]);

    return u[0] * 1000;
}

void realtimeLoop(void *arg)
{
    Q_UNUSED(arg);
    int xenoRet = 0;
    char bufChar[MAX_MESSAGE_LENGTH];
    int bufInt[MAX_MESSAGE_LENGTH];
    RealtimeController::Mode bufMode[MAX_MESSAGE_LENGTH];
    RealtimeController::State bufState[MAX_MESSAGE_LENGTH];
    int kp = 0, ki = 0, kd = 0;
    int setValue = 0;
    int actuatorVoltage = 0;
    int actuatorVoltageToSend = 0;
    int sensorVoltage = 0;
    int sensorVoltageToSend = 0;
    RealtimeController::Mode mode = RealtimeController::MODE_MANUAL;
    RealtimeController::Mode modeToSend;
    RealtimeController::State state = RealtimeController::STOPPED;
    RealtimeController::State stateToSend;
    int output = 0;
    PiezoSensor* sensor;
    PiezoActuator* actuator;

    // varataan muisti anturille ja alustetaa se
    sensor = new PiezoSensor();
    sensor->init();

    // varataan muisti toimilaitteelle ja alustetaan se
    actuator = new PiezoActuator();
    actuator->init();

    // Xenomai laajennoksen ajurin alustukset

    // määritetään jaksonaika 500us eli 500000ns
    RTIME interval = 500000;

    // määritetään taskia kutsuttavaksi jaksonajan välein
    xenoRet = rt_task_set_periodic(NULL, TM_NOW, interval);

    while(1)
    {
        // ajastaa silmukan kulkemaan määritetyissä jaksonajoissa
        if(rt_task_wait_period(NULL) < 0)
            qDebug("task error in realtime loop");

        if(state == RealtimeController::STARTED)
        {
            sensor->getValue(sensorVoltage);
            sensorVoltage *= -1; // kytkentä väärinpäin

            // tarkastetaan ajetaanko PID-säätöä vai suoraa ohjausta
            if( mode == RealtimeController::MODE_MANUAL)
            {
                // suorassa jänniteohjauksessa laitetaan suoraan käyttäjän syöttämä skaalattu arvo
                // ulostuloon
                output = actuatorVoltage;
            }
            else
            {
                // PID-säädössä ajetaan algoritmi ja asetetaan ulostulo
                output = pid(setValue/1000.0, sensorVoltage/1000.0, kp/1000.0, ki/1000.0, kd/1000.0);
                actuatorVoltage = output;
            }
        }

        // asetetaan ulostulo oikeasti toimilaitteelle
        actuator->setValue(output);
        // qDebug()<< "Set " << output<< "to actuator";

        // luetaan käyttöliittymästä mahdollisesti tullut viesti
        xenoRet = 0;
        xenoRet = rt_pipe_read(&pipe_desc, bufChar, sizeof(bufChar), TM_NONBLOCK);

        if(xenoRet > 0)
        {
            if(!strcmp(bufChar, "reaSen"))
            {
                sensorVoltageToSend = sensorVoltage;
                int *pBuf = &sensorVoltageToSend;
                if(rt_pipe_write(&pipe_desc, (void*)pBuf, sizeof(bufInt), P_NORMAL) < 1)
                    qDebug("rt write error");
                bufChar[0] = '0';
            }
            else if(!strcmp(bufChar, "reaOut"))
            {
                actuatorVoltageToSend = actuatorVoltage;
                int *pBuf = &actuatorVoltageToSend;
                if(rt_pipe_write(&pipe_desc, (void*)pBuf, sizeof(bufInt), P_NORMAL) < 1)
                    qDebug("rt write error");
                bufChar[0] = '0';
            }
            else if(!strcmp(bufChar, "reaMod"))
            {
                modeToSend = mode;
                RealtimeController::Mode *pBuf = &modeToSend;
                if(rt_pipe_write(&pipe_desc, (void*)pBuf, sizeof(bufInt), P_NORMAL) < 1)
                    qDebug("rt write error");
                bufChar[0] = '0';
            }
            else if(!strcmp(bufChar, "reaSta"))
            {
                stateToSend = state;
                RealtimeController::State *pBuf = &stateToSend;
                if(rt_pipe_write(&pipe_desc, (void*)pBuf, sizeof(bufInt), P_NORMAL) < 1)
                    qDebug("rt write error");
                bufChar[0] = '0';
            }
            else if (!strcmp(bufChar, "setSta"))
            {
                if(rt_pipe_read(&pipe_desc, bufState, sizeof(bufState), TM_INFINITE) < 1)
                    qDebug("rt read error");
                state = *(RealtimeController::State*)bufState;
                bufChar[0] = '0';
                qDebug("Set state: %d", state);
            }
            else if (!strcmp(bufChar, "setPID"))
            {
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                kp = *(int*)bufInt;
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                ki = *(int*)bufInt;
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                kd = *(int*)bufInt;
                bufChar[0] = '0';
                qDebug("Set PID-parameters p: %d, i: %d, d: %d", kp,ki,kd);
            }
            else if (!strcmp(bufChar, "setSet"))
            {
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                setValue = *(int*)bufInt;
                bufChar[0] = '0';
                qDebug("Set setValue: %d", setValue);
            }
            else if (!strcmp(bufChar, "setOut"))
            {
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                actuatorVoltage = *(int*)bufInt;
                bufChar[0] = '0';
                qDebug("Set actuatorVoltage: %d", actuatorVoltage);
            }
            else if (!strcmp(bufChar, "setSca"))
            {
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                sensor->setScale(*(int*)bufInt);
                if(rt_pipe_read(&pipe_desc, bufInt, sizeof(bufInt), TM_INFINITE) < 1)
                    qDebug("rt read error");
                actuator->setScale(*(int*)bufInt);
                bufChar[0] = '0';
                qDebug("Set sensor and, actuator scales");
            }
            else if (!strcmp(bufChar, "setMod"))
            {
                if(rt_pipe_read(&pipe_desc, bufMode, sizeof(bufMode), TM_INFINITE) < 1)
                    qDebug("rt read error");
                mode = *(RealtimeController::Mode*)bufMode;
                bufChar[0] = '0';
                qDebug("Set mode to: %d", mode);
            }
            else if(!strcmp(bufChar, "theEnd"))
            {
                qDebug("Bye!");
                break;
            }
            else
                qDebug("rt read error: unknown string");
        }
    }
}

RealtimeController::RealtimeController()
{
    // lukitaan ohjelman nykinen ja tuleva muisti niin että se pysyy RAM:ssa kokoajan
    mlockall(MCL_CURRENT | MCL_FUTURE);

    int xenoError = 0;

    // luodaan task-handle reaaliaikasäikeelle
    xenoError = rt_task_create(&task_desc, NULL, 0, 99, T_JOINABLE);

    xenoError = rt_pipe_create(&pipe_desc, NULL, 0, 0);

    if(xenoError != 0)
        qDebug("rt init error");

    // pysäytetään ohjaus oletuksena
    rt_task_start(&task_desc, &realtimeLoop, NULL);

    // luodaan pipe reaaliaikasäikeen kanssa kommunikointiin
    pipefd = open("/dev/rtp0", O_RDWR, 0);
    if (pipefd < 0)
        qDebug("Creating pipe failed");
    // pysäytetään säätö oletuksena
    stop();
}

RealtimeController::~RealtimeController()
{
    // signaloidaan säikeelle että sen pitää sammuttaa itsensä
    if(write(pipefd, "theEnd", sizeof("theEnd")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    // odotetaan kunnes säie on sammunut
    rt_task_join(&task_desc);
    // vapautetaan pipen resurssit
    rt_pipe_delete(&pipe_desc);
}

void RealtimeController::setPIDParameters(int kp, int ki, int kd)
{
    int *bufValue;
    if(write(pipefd, "setPID", sizeof("setPID")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &kp;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &ki;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &kd;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::setScaleFactors(int sensor, int output)
{
    int *bufValue;
    if(write(pipefd, "setSca", sizeof("setSca")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &sensor;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &output;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::setSetValue(int value)
{
    int *bufValue;
    if(write(pipefd, "setSet", sizeof("setSet")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &value;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::setOutputVoltage(int value)
{
    int *bufValue;
    if(write(pipefd, "setOut", sizeof("setOut")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &value;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::readSensorVoltage(int &value)
{
    int bufValue[MAX_MESSAGE_LENGTH];
    if(write(pipefd, "reaSen", sizeof("reaSen")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    if(read(pipefd, (void*)bufValue, sizeof(bufValue)) < 1)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    value = *(int*)bufValue;
}

void RealtimeController::setMode(RealtimeController::Mode mode)
{
    RealtimeController::Mode *bufValue;
    if(write(pipefd, "setMod", sizeof("setMod")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &mode;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::start()
{
    RealtimeController::State *bufValue;
    RealtimeController::State state = STARTED;
    if(write(pipefd, "setSta", sizeof("setSta")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    bufValue = &state;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::stop()
{
    RealtimeController::State *bufValue;
    RealtimeController::State state = STOPPED;
    if(write(pipefd, "setSta", sizeof("setSta")) < 0)
        qDebug("er2ror %d : %s\n", -errno, strerror(-errno));
    bufValue = &state;
    if(write(pipefd, (void*)bufValue, sizeof(bufValue)) < 0)
        qDebug("er2ror %d : %s\n", -errno, strerror(-errno));
}

void RealtimeController::readOutputVoltage(int &value)
{
    int bufValue[MAX_MESSAGE_LENGTH];
    if(write(pipefd, "reaOut", sizeof("reaOut")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    if(read(pipefd, (void*)bufValue, sizeof(bufValue)) < 1)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    value = *(int*)bufValue;
}

void RealtimeController::getMode(RealtimeController::Mode &mode)
{
    RealtimeController::Mode bufValue[MAX_MESSAGE_LENGTH];
    if(write(pipefd, "reaMod", sizeof("reaMod")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    if(read(pipefd, (void*)bufValue, sizeof(bufValue)) < 1)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    mode = *(RealtimeController::Mode*)bufValue;
}

void RealtimeController::getState(RealtimeController::State &state)
{
    RealtimeController::State bufValue[MAX_MESSAGE_LENGTH];
    if(write(pipefd, "reaSta", sizeof("reaSta")) < 0)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    if(read(pipefd, (void*)bufValue, sizeof(bufValue)) < 1)
        qDebug("error %d : %s\n", -errno, strerror(-errno));
    state = *(RealtimeController::State*)bufValue;
}
