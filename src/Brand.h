/*
 * BrandModel.h
 *
 *  Created on: 18 Aug 2016
 *      Author: SNiLD
 */

#ifndef SRC_BRAND_H_
#define SRC_BRAND_H_

#include <QPersistentModelIndex>
#include <QSharedPointer>
#include <QString>

class Model;

class Brand
{
public:
    Brand(const QString& brand, const QPersistentModelIndex& index);
    virtual ~Brand();

    const QString& getBrand() const;
    const QPersistentModelIndex& getIndex() const;
    QSharedPointer<Model> getModel(const QString& model) const;
    QList< QSharedPointer<Model> > getModels(const QString& model) const;

    void addModel(const QSharedPointer<Model>& model);

private:
    QString m_brand;
    QPersistentModelIndex m_index;
    QHash< QString, QSharedPointer<Model> > m_models;
};

#endif /* SRC_BRAND_H_ */
