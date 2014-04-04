//----------------------------------------------------------------------------
//
//  Käyttöliittymän toteutus
//
//----------------------------------------------------------------------------

#include "ui_mainwindow.h"
#include "qglobal.h"
#include "realtimecontroller.h"
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), outputvoltage(0.0), setvalue(0.0), kp(0.0), ki(0.0), kd(0.0)
{
    // varataan ruudunpäivitysajastimen muisti
    statusTimer = new QTimer();

    // varataan realtimecontrolleri
    mController = new RealtimeController();

    // asetetaan ruudunpäivitysajastimelle jaksonaika 200ms eli 5Hz
    statusTimer->start(200);

    ui->setupUi(this);

    // ylävalikko ja pikavalinta ohjelmasta poistumiselle nopeuttamaan testausta
    QMenu* fileMenu = menuBar()->addMenu("File");

    QAction* quitAct = new QAction("Quit", this);
    quitAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    fileMenu->addAction(quitAct);
    connect(quitAct, SIGNAL(triggered()),this,SLOT(close()));

    // kytketään ruudunpäivitysajastimen käsittelijä
    connect(statusTimer, SIGNAL(timeout()), this, SLOT(statusTimerTimeout()));
}

MainWindow::~MainWindow()
{
    // vapautetaan controlleri
    delete mController;
    // vapautetaan käyttöliittymä
    delete ui;
}

void MainWindow::on_startPushButton_clicked()
{
    qDebug("Startti painettu.");

    // estetään käynnistäminen jos säätö on jo käynnissä
    RealtimeController::State state;
    mController->getState(state);
    if( state == RealtimeController::STARTED )
    {
        return;
    }

    // estetään PID-säätimen käynnistäminen jos PID-säätimen parametreja ei ole asetettu, ei pitäisi olla mahdollista
    RealtimeController::Mode mode;
    mController->getMode(mode);
    if(mode == RealtimeController::MODE_PID &&
            kp == 0.0 && ki == 0.0 && kd == 0.0)
    {
        QMessageBox::warning(this, "", QString("Set PID-controller parameters!"));
    }
    else
    {
        // jos kaikki onnistuu niin käynnistettään säätö
        mController->start();
    }
}

void MainWindow::on_stopPushButton_clicked()
{
    qDebug("Stoppi painettu.");

    // estetään pysäyttäminen jos säätö on jo pysäytetty
    RealtimeController::State state;
    mController->getState(state);
    if( state == RealtimeController::STOPPED)
    {
        return;
    }

    // pysäytetään säätö
    mController->stop();
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
    RealtimeController::State state;
    mController->getState(state);
    if(state == RealtimeController::STARTED)
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
    RealtimeController::Mode mode;
    mController->getMode(mode);
    if(mode != RealtimeController::MODE_MANUAL)
    {
        qDebug("Changing to manual mode");
        mController->setMode( RealtimeController::MODE_MANUAL);
    }
}

void MainWindow::on_closedmodeRadioButton_clicked()
{
    RealtimeController::Mode mode;
    mController->getMode(mode);
    if(mode != RealtimeController::MODE_PID)
    {
        qDebug("Changing to PID-mode");
        mController->setMode( RealtimeController::MODE_PID);
    }
}
