/*
 * CarValueApplication.h
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#ifndef CARVALUEAPPLICATION_H_
#define CARVALUEAPPLICATION_H_

#include "CarValueWindow.h"

#include <QApplication>
#include <QStandardItemModel>


class CarValueApplication : public QApplication
{
public:
    CarValueApplication(int& argc, char** argv);
    virtual ~CarValueApplication();

private:
    CarValueWindow m_window;
    QStandardItemModel m_trafiModel;
    QStandardItemModel m_nettiautoModel;
};

#endif /* CARVALUEAPPLICATION_H_ */
