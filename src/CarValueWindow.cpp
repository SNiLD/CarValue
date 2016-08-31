/*
 * CarValueWindow.cpp
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#include "CarValueWindow.h"

#include "ui_CarValueWindow.h"


CarValueWindow::CarValueWindow(QWidget* parent) :
        QMainWindow(parent),
    m_ui(new Ui::CarValueWindowClass)
{
    m_ui->setupUi(this);

    connect(m_ui->fetchPushButton, SIGNAL(clicked()), this, SIGNAL(fetchAll()));
    connect(m_ui->parsePushButton, SIGNAL(clicked()), this, SIGNAL(parseAll()));
}


CarValueWindow::~CarValueWindow()
{
}


void CarValueWindow::setTrafiModel(QAbstractItemModel* model)
{
    m_ui->trafiTreeView->setModel(model);
}


void CarValueWindow::setNettiautoModel(QAbstractItemModel* model)
{
    m_ui->nettiautoTreeView->setModel(model);
}
