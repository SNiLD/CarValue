/*
 * CarValueApplication.cpp
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#include "CarValueApplication.h"

CarValueApplication::CarValueApplication(int& argc, char** argv) :
        QApplication(argc, argv)
{
    m_window.setModel(&m_model);
    m_window.show();
}


CarValueApplication::~CarValueApplication()
{
}


QAbstractItemModel* CarValueApplication::getModel()
{
    return &m_model;
}
