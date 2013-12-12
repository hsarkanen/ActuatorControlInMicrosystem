#ifndef HYSTERESISGRAPH_H
#define HYSTERESISGRAPH_H

#include <QDialog>

namespace Ui {
class HysteresisGraph;
}

class QGraphicsLineItem;

class HysteresisGraph : public QDialog
{
    Q_OBJECT
    
public:
    explicit HysteresisGraph(QWidget *parent = 0);
    ~HysteresisGraph();
    
    void drawGraph(double *x, double *y, unsigned int amount);
    void setScale(unsigned int x, unsigned int y);
    void setTitles(const QString &x, const QString &y);
    void clearGraph();
    void showNumbers(bool show);
private:
    Ui::HysteresisGraph *ui;
    QVector<QGraphicsLineItem*>m_lines;
};

#endif // HYSTERESISGRAPH_H
