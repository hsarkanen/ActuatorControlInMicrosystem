#include "hysteresisdialog.h"
#include "ui_hysteresisdialog.h"

#include "hysteresisgraph.h"
#include "hysteresissingleton.h"

#include <QMessageBox>
#include <QDebug>

HysteresisDialog::HysteresisDialog(QWidget *parent) :
    QDialog(parent), m_hysteresisGraph(NULL),
    ui(new Ui::HysteresisDialog)
{
    ui->setupUi(this);
    m_hysteresisGraph = new HysteresisGraph(this);
    m_hysteresisGraph->setScale(6, 6);
    m_hysteresisGraph->setTitles("Control [V]", "Measurement [V]");
    // TODO: Testaa numeronäyttöjen toiminta
    m_hysteresisGraph->showNumbers(false);
}

HysteresisDialog::~HysteresisDialog()
{
    delete ui;
    delete m_hysteresisGraph;
    m_hysteresisGraph = 0;
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
}

void HysteresisDialog::on_startpushButton_clicked()
{
    // haetaan varaan HysteresisSingletonin tietorakenne
    HysteresisData& data = getHysteresisData();

    // haetaan k�ytt�liittym�st� amplitudi
    const double A = ui->amplitudedoubleSpinBox->value();

    // haetaan k�ytt�liittym�st� taajuus
    const double F = ui->frequencydoubleSpinBox->value();

    // haetaan k�ytt�liittym�st� jaksojen m��r�
    const unsigned int N =  ui->amountspinBox->value();

    // aluksi tila yl�sp�in
    Ui::Wave w = Ui::WAVE_UP;

    // lasketaan j�nnitteen muutos samplea kohti

    double increment = 4.0 * A / data.samples * F;

    // aloitetaan kolmioaalto 0 (V)
    double v = 0;

    // lasketaan kolmioaallon samplejen m��r�
    unsigned int size = 0;

    // toistoja samplejen m��r� kertaa jaksojen m��r� jaettuna
    // taajuudella
    for(unsigned int i = 0; i < data.samples * N / F; i++)
    {
        switch(w)
        {
        // aalto nousee yl�sp�in inkrementin v�lein kunnes on
        // amplitudin suuruinen ja vaihtaa sitten suuntaa
        case Ui::WAVE_UP:
            v += increment;
            if( v >= A )
            {
                // vaihdetaan suunta alesp�in
                w = Ui::WAVE_DOWN;
            }
            break;

            // aalto laskee kunnes ollaan negatiivisen amplitudin kohdalla
            // ja vaihtaa taas suuntaa
        case Ui::WAVE_DOWN:
            v -= increment;
            if( v < -A )
            {
                // vaihdetaan suuntaa
                w = Ui::WAVE_ZERO;
            }
            break;

            // jatketaan kunnes palataan 0 (V) kohdalle ja jakso on valmis
        case Ui::WAVE_ZERO:
            v += increment;
            if( v > 0 )
            {
                w = Ui::WAVE_UP;
            }
            break;
        }
        // tallenetaan ohjauksen arvo HysteresisSinglentonin tietorakenteeseen
        // t�ss� mahdollinen ylivuoto joka varmaan havaitaan testausvaiheessa
        // ohjelma on demonstroitu ja lukittu siten muutoksilta, joten t�t�
        // ei nyt korjata
        data.output[i] = v;
        //qDebug( QString::number(i) +": " + QString::number(v) );

        // lasketaan samplejen m��r��
        size++;
    }

    // tarkistetaan ettei ylitet� varatun muistin määrää
    // TODO: tämä tarkistus pitäisi tehdä aiemmin
    if( size > data.size )
    {
        QMessageBox::warning(this, "", QString("Too much control values."));
        return;
    }

    // asetetaan ylös HysteresisSingletonin tietorakenteeseen kolmioaallon koko
    data.controlValuesAmount = size;

    // estetään dialogin lopettaminen tai analyysin aloitus analyysin aikana
    ui->startpushButton->setEnabled(false);
    ui->cancelpushButton->setEnabled(false);

    // liipastaan käytiin hystereesianalyysi
    mController->startHysteresisAnalysis();

    // UI on tässä jummissa kunnes tulokset tulee
    handleResults();

}

void HysteresisDialog::handleResults()
{
    qDebug() << "Handle results";
    HysteresisData& data = getHysteresisData();
    // näytetään graafi
    m_hysteresisGraph->show();
    // tyhjennetään graafi
    m_hysteresisGraph->clearGraph();
    // piirretään graafi HysteresisSingletonin sisätämien tietojen mukaan
    m_hysteresisGraph->drawGraph(data.output, data.measurement, data.controlValuesAmount);
    // sallitaan uudelleen hystereesianalyysin aloitus tai dialogin sulkeminen
    ui->startpushButton->setEnabled(true);
    ui->cancelpushButton->setEnabled(true);
}
