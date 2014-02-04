#include "hysteresisgraph.h"
#include "ui_hysteresisgraph.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QDebug>

HysteresisGraph::HysteresisGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HysteresisGraph)
{
    ui->setupUi(this);
    ui->graphicsView_HysteresisGraph->setScene(new QGraphicsScene(this));
    ui->graphicsView_HysteresisGraph->centerOn(0,0);
    ui->graphicsView_HysteresisGraph->setFixedSize(600,600);
    // TODO: Tarvitaanko tätä?
    clearGraph();
}

HysteresisGraph::~HysteresisGraph()
{
    delete ui;
}

void HysteresisGraph::drawGraph(double *x, double *y, unsigned int amount)
{
    if ( !x || !y || amount < 1)
    {
        qDebug()  << "HysteresisGraph: Parameter error";
        return;
    }

    for (unsigned int i = 0; i < amount; i++)
    {
        QGraphicsLineItem *line = new QGraphicsLineItem((int)x[i], (int)y[i], (int)x[i+1], (int)y[i+1]);
        ui->graphicsView_HysteresisGraph->scene()->addItem(line);
        m_lines.push_back(line);
    }
}

void HysteresisGraph::setScale(unsigned int x, unsigned int y)
{
    ui->graphicsView_HysteresisGraph->scale(2*x, 2*y);
    ui->label_x_min->setText(QString::number(-1*(int)x));
    ui->label_x_max->setText(QString::number(x));
    ui->label_y_min->setText(QString::number(-1*(int)y));
    ui->label_y_max->setText(QString::number(y));
}

void HysteresisGraph::clearGraph()
{
    ui->lcdNumber_x->display(0.0);
    ui->lcdNumber_y->display(0.0);

    for (int i = 0; i < m_lines.size(); i++)
    {
        delete m_lines[i];
    }
    m_lines.clear();
}

void HysteresisGraph::setTitles(const QString &x, const QString &y)
{
    ui->label_x_axis->setText(x);
    ui->label_y_axis->setText(y);
}

void HysteresisGraph::showNumbers(bool show)
{
    ui->label_lcd_x->setShown(show);
    ui->label_lcd_y->setShown(show);
    ui->lcdNumber_x->setShown(show);
    ui->lcdNumber_y->setShown(show);
}
