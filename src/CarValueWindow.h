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
    Q_OBJECT

public:
    CarValueWindow(QWidget* parent = 0);
    virtual ~CarValueWindow();

    void setTrafiModel(QAbstractItemModel* model);
    void setNettiautoModel(QAbstractItemModel* model);

signals:
    void fetchAll();
    void parseAll();

private:
    QScopedPointer<Ui::CarValueWindowClass> m_ui;
};

#endif /* CARVALUEWINDOW_H_ */
