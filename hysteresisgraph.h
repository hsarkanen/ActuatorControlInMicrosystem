#ifndef HYSTERESISGRAPH_H
#define HYSTERESISGRAPH_H

#include <QDialog>

namespace Ui {
class HysteresisGraph;
}

class HysteresisGraph : public QDialog
{
    Q_OBJECT
    
public:
    explicit HysteresisGraph(QWidget *parent = 0);
    ~HysteresisGraph();
    
private:
    Ui::HysteresisGraph *ui;
};

#endif // HYSTERESISGRAPH_H
