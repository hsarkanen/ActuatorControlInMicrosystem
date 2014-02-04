#ifndef HYSTERESISDIALOG_H
#define HYSTERESISDIALOG_H

#include <QDialog>
#include "controllerinterface.h"

namespace Ui {
class HysteresisDialog;
// aaltoja luovan tilakoneen tilat
enum Wave {WAVE_UP, WAVE_DOWN, WAVE_ZERO};
}

class HysteresisGraph;

class HysteresisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HysteresisDialog(QWidget *parent = 0);
    ~HysteresisDialog();
    
    void setController(ControllerInterface *controller);
public Q_SLOTS:
    void handleResults();
private Q_SLOTS:
    void on_startpushButton_clicked();
    void on_cancelpushButton_clicked();

protected:
    ControllerInterface* mController;

private:
    HysteresisGraph* m_hysteresisGraph;
    Ui::HysteresisDialog *ui;
};

#endif // HYSTERESISDIALOG_H
