/*
 * Brands.h
 *
 *  Created on: 21 Aug 2016
 *      Author: SNiLD
 */

#ifndef SRC_BRANDS_H_
#define SRC_BRANDS_H_

#include <QHash>
#include <QModelIndex>
#include <QPersistentModelIndex>
#include <QSharedPointer>
#include <QString>

class QAbstractItemModel;
class Brand;
class Model;

class Brands
{
public:
    Brands(QAbstractItemModel* model);
    virtual ~Brands();

    QPersistentModelIndex createIndex(const QString& brand, const QString& model, int year);
    QList<QPersistentModelIndex> getIndexes(const QString& brand, const QString& model, int year) const;
    QList<QPersistentModelIndex> getIndexesBefore(const QString& brand, const QString& model, int year) const;

private:
    QAbstractItemModel* m_model;
    QHash< QString, QSharedPointer<Brand> > m_brands;

    QPersistentModelIndex addItem(const QString& value, const QModelIndex& parent);
    QSharedPointer<Brand> addBrand(const QString& brand);
    QSharedPointer<Model> addModel(const QString& model, const QSharedPointer<Brand>& brand);
    QPersistentModelIndex addYear(int year, const QSharedPointer<Model>& model);
};

#endif /* SRC_BRANDS_H_ */
