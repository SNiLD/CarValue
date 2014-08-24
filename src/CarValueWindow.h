/*
 * CarValueWindow.h
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#ifndef CARVALUEWINDOW_H_
#define CARVALUEWINDOW_H_

#include <QAbstractItemModel>
#include <QMainWindow>


namespace Ui { class CarValueWindowClass; }


class CarValueWindow : public QMainWindow
{
public:
    CarValueWindow(QWidget* parent = 0);
    virtual ~CarValueWindow();

    void setModel(QAbstractItemModel* model);

private:
    QScopedPointer<Ui::CarValueWindowClass> m_ui;
};

#endif /* CARVALUEWINDOW_H_ */
