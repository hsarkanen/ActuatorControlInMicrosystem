#ifndef HYSTERESISDIALOG_H
#define HYSTERESISDIALOG_H

#include <QDialog>
#include "controllerinterface.h"

namespace Ui {
class HysteresisDialog;
}

class HysteresisGraph;

class HysteresisDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HysteresisDialog(QWidget *parent = 0);
    ~HysteresisDialog();
    
    void setController(ControllerInterface *controller);
private Q_SLOTS:
    void on_startpushButton_clicked();
    void on_cancelpushButton_clicked();

protected:
    ControllerInterface* mController;

private:
    Ui::HysteresisDialog *ui;
    HysteresisGraph* m_hysteresisGraph;
};

#endif // HYSTERESISDIALOG_H
