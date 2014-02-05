#include "ui_mainwindow.h"
#include "hysteresisdialog.h"
#include "qglobal.h"
#include "realtimecontroller.h"
#include <QMessageBox>
#include <QTimer>
#include "simplecontroller.h"
#include "hysteresissingleton.h"
#include "scopedmutex.h"
#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), outputvoltage(0.0), setvalue(0.0), kp(0.0), ki(0.0), kd(0.0)
{
    // varataan HysteresisSingletonin resurssit
    HysteresisData& data = getHysteresisData();

    // Critical section
    {
        ScopedMutex hystMutex(data.mutex);

        // varataan HysteresisSingleton tietorakenteen
        // taulujen maksimikooksi 8Mt
        data.size = 8000000;
        data.output = new double[data.size];
        data.measurement = new double[data.size];

        // asetetaan oletuskoko samplejen m��r�lle jaksossa
        data.samples = 2000.0;
    }

    // varataan hystereesi dialogin muisti
    hd = new HysteresisDialog(this);

    // varataan ruudunpäivitysajastimen muisti
    statusTimer = new QTimer();

    // varataan realtimecontrolleri
//    mController = new SimpleController();
    mController = new RealtimeController();

    // asetetaan dialogille osoitin controlleriin
    hd->setController(mController);

    // asetetaan ruudunp�ivitysajastimelle jaksonaika 200ms eli 5Hz
    statusTimer->start(200);

    ui->setupUi(this);

    // ylävalikko ja pikavalinta ohjelmasta poistumiselle nopeuttamaan testausta
    QMenu* fileMenu = menuBar()->addMenu("File");

    QAction* quitAct = new QAction("Quit", this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));

    // kytket��n ruudunp�ivitysajastimen k�sittelij�
    connect(statusTimer, SIGNAL(timeout()), this, SLOT(statusTimerTimeout()));

}

MainWindow::~MainWindow()
{
    // vapautetaan hystereesi dialogi
    delete hd;
    // vapautetaan controlleri
    delete ui;
}

void MainWindow::on_startPushButton_clicked()
{
    qDebug("Startti painettu.");
    // estet��n k�ynnist�minen jos s��t� on jo k�ynniss�
    ControllerInterface::State state;
    mController->getState(state);
    if( state == ControllerInterface::STARTED )
    {
        return;
    }


    // estet��n PID-s��timen k�ynnist�minen jos PID-s��timen parametreja ei ole asetettu, ei pit�isi olla mahdollista
    ControllerInterface::Mode mode;
    mController->getMode(mode);
    if(mode == ControllerInterface::MODE_PID &&
            kp == 0.0 && ki == 0.0 && kd == 0.0)
    {
        QMessageBox::warning(this, "", QString("Set PID-controller parameters!"));
    }
    else
    {
        // jos kaikki onnistuu niin k�ynnistett��n s��t�
        mController->start();
    }
}

void MainWindow::on_stopPushButton_clicked()
{
    qDebug("Stoppi painettu.");

    // estet��n pys�ytt�minen jos s��t� on jo pys�ytetty
    ControllerInterface::State state;
    mController->getState(state);
    if( state == ControllerInterface::STOPPED)
    {
        return;
    }

    // pys�ytet��n s��t�
    mController->stop();
}

void MainWindow::on_hysteresisanalysispushButton_clicked()
{
    // estet��n hystereesi dialogin k�ynnist�minen
    // jos s��t� on pys�ytetty tai PID-s��t� on valittuna

    ControllerInterface::Mode mode;
    mController->getMode(mode);
    if(mode == ControllerInterface::MODE_PID )
    {
        QMessageBox::warning(this, "",
                             QString("Cannot do hysteresisanalysis in Closed control mode!\nPlease set manual mode and start control."));
        return;
    }

    ControllerInterface::State state;
    mController->getState(state);
    if(state == ControllerInterface::STOPPED )
    {
        QMessageBox::warning(this, "",
                             QString("Cannot do hysteresisanalysis when actuator is stopped!\nPlease set manual mode and start control."));
        return;
    }

    // k�ynnistet��n hystereesi dialogi modaalisena
    hd->exec();
}

void MainWindow::on_setpushButton_clicked()
{
    if(ui->pdoubleSpinBox->value() == 0 && ui->idoubleSpinBox->value() == 0 && ui->ddoubleSpinBox->value() == 0)
    {
        QMessageBox::warning(this, "", QString("Set PID-controller parameters!"));
        return;
    }

    // Skaalaa millivaluet inteiksi
    kp = 1000 * ui->pdoubleSpinBox->value();
    ki = 1000 * ui->idoubleSpinBox->value();
    kd = 1000 * ui->ddoubleSpinBox->value();
    setvalue = 1000 * ui->setvaluedoubleSpinBox->value();
    outputvoltage = 1000 * ui->voltagedoubleSpinBox->value();
    sensorscale = 1000 * ui->sensordoubleSpinBox->value();
    actuatorscale = 1000 * ui->actuatordoubleSpinBox->value();

    // asetataan arvot controllerille
    qDebug()<< "Set PID: " << kp << " "<< ki<<" " << kd;
    mController->setPIDParameters(kp, ki, kd);
    qDebug()<< "Set value: " << setvalue;
    mController->setSetValue(setvalue);
    qDebug()<< "Set output: " << outputvoltage;
    mController->setOutputVoltage(outputvoltage);
    mController->setScaleFactors(sensorscale, actuatorscale);
}

// ajastimen käsittelijä
void MainWindow::statusTimerTimeout()
{
    // haetaan tila ja vaihdetaan labeli sen mukaiseksi
    ControllerInterface::State state;
    mController->getState(state);
    if(state == ControllerInterface::STARTED)
    {
        ui->statuslabel->setText("STARTED");
    }
    else
    {
        ui->statuslabel->setText("STOPPED");
    }

    // haetaan toimilaitteen viimeisin ohjausarvo ja anturin viimeisin mittausarvo
    // ja näytetään ne käyttäliittymässä
    int voltage = 0;
    mController->readOutputVoltage(voltage);
    ui->outputvoltagelabel->setText( QString("%1 V").arg(voltage / 1000.0, 10, 'f',3 ));
    mController->readSensorVoltage(voltage);
    ui->sensorvoltagelabel->setText(QString("%1 V").arg(voltage / 1000.0, 10,  'f',3 ));
}

void MainWindow::on_manualmodeRadioButton_clicked()
{
    ControllerInterface::Mode mode;
    mController->getMode(mode);
    if(mode != ControllerInterface::MODE_MANUAL)
    {
        qDebug("Changing to manual mode");
        mController->setMode( ControllerInterface::MODE_MANUAL);
    }
}

void MainWindow::on_closedmodeRadioButton_clicked()
{
    ControllerInterface::Mode mode;
    mController->getMode(mode);
    if(mode != ControllerInterface::MODE_PID)
    {
        qDebug("Changing to PID-mode");
        mController->setMode( ControllerInterface::MODE_PID);
    }
}
