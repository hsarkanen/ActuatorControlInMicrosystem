#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <controllerinterface.h>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private Q_SLOTS:
    void statusTimerTimeout();
    void on_setpushButton_clicked();
    void on_startPushButton_clicked();
    void on_stopPushButton_clicked();
    void on_manualmodeRadioButton_clicked();
    void on_closedmodeRadioButton_clicked();


private:
    Ui::MainWindow *ui;
    ControllerInterface* mController;
    int outputvoltage;
    int setvalue;
    int kp;
    int ki;
    int kd;
    int actuatorscale;
    int sensorscale;
    QTimer* statusTimer;
};

#endif // MAINWINDOW_H
