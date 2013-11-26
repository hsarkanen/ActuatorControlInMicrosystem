#include "hysteresisgraph.h"
#include "ui_hysteresisgraph.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsLineItem>

HysteresisGraph::HysteresisGraph(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HysteresisGraph)
{
    ui->setupUi(this);
    ui->graphicsView_HysteresisGraph->setScene(new QGraphicsScene(this));
    ui->graphicsView_HysteresisGraph->centerOn(0,0);

    QGraphicsLineItem *line = new QGraphicsLineItem(0,0,100,100);
    ui->graphicsView_HysteresisGraph->scene()->addItem(line);
}

HysteresisGraph::~HysteresisGraph()
{
    delete ui;
}
