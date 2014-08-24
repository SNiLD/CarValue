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
}


CarValueWindow::~CarValueWindow()
{
}


void CarValueWindow::setModel(QAbstractItemModel* model)
{
    m_ui->trafiTreeView->setModel(model);
}
