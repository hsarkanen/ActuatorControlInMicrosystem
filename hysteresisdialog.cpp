#include "hysteresisdialog.h"
#include "ui_hysteresisdialog.h"

#include "hysteresisgraph.h"

HysteresisDialog::HysteresisDialog(QWidget *parent) :
    QDialog(parent), m_hysteresisGraph(NULL),
    ui(new Ui::HysteresisDialog)
{
    ui->setupUi(this);
    m_hysteresisGraph = new HysteresisGraph(this);
}

HysteresisDialog::~HysteresisDialog()
{
    delete ui;
}

void HysteresisDialog::on_cancelpushButton_clicked()
{
    this->close();
}

// kontrollerin asetus, pit�isi kutsua vain kerran dialogin el�m�nkaaren aikana
// ettei signaali kytkeydy useasti
void HysteresisDialog::setController( ControllerInterface * controller )
{

    mController = controller;

    //connect(mController, SIGNAL( hysteresisResultsReady()),
      // this, SLOT(handleResults()));
}

void HysteresisDialog::on_startpushButton_clicked()
{
    m_hysteresisGraph->show();
}
