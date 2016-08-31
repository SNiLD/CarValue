/*
 * Brands.cpp
 *
 *  Created on: 21 Aug 2016
 *      Author: SNiLD
 */

#include "Brands.h"
#include "Brand.h"
#include "Model.h"

#include <QAbstractItemModel>
#include <QDebug>

Brands::Brands(QAbstractItemModel* model) :
    m_model(model)
{
}


Brands::~Brands()
{
}


QPersistentModelIndex Brands::createIndex(const QString& brand, const QString& model, int year)
{
    QSharedPointer<Brand> brandPtr;
    auto iBrand = m_brands.constFind(brand);

    if (iBrand == m_brands.constEnd())
    {
        brandPtr = addBrand(brand);
    }
    else
    {
        brandPtr = iBrand.value();
    }

    QSharedPointer<Model> modelPtr = brandPtr->getModel(model);

    if (!modelPtr)
    {
        modelPtr = addModel(model, brandPtr);
    }

    QPersistentModelIndex yearIndex = modelPtr->getYear(year);

    if (!yearIndex.isValid())
    {
        yearIndex = addYear(year, modelPtr);
    }

    return yearIndex;
}


QList<QPersistentModelIndex> Brands::getIndexes(const QString& brand, const QString& model, int year) const
{
    QList<QPersistentModelIndex> indexes;
    QSharedPointer<Brand> brandPtr;

    for (auto iBrand = m_brands.constBegin(); iBrand != m_brands.constEnd(); ++iBrand)
    {
        if (iBrand.key().toLower() != brand)
            continue;

        brandPtr = iBrand.value();
        break;
    }

    if (!brandPtr)
    {
        qDebug() << "Could not find brand" << brand;
        return indexes;
    }

    const QList< QSharedPointer<Model> >& models = brandPtr->getModels(model);

    if (models.isEmpty())
    {
        qDebug() << "Could not find model" << model << "for brand" << brand;
        return indexes;
    }

    for (auto iModel = models.constBegin(); iModel != models.constEnd(); ++iModel)
    {
        const QPersistentModelIndex& result = (*iModel)->getYear(year);

        if (!result.isValid())
        {
            qDebug() << "Could not find year" << year << "for model" << model << "of brand" << brand;
        }
        else
        {
            indexes << result;
        }
    }

    return indexes;
}


QList<QPersistentModelIndex> Brands::getIndexesBefore(const QString& brand, const QString& model, int year) const
{
    QList<QPersistentModelIndex> indexes;
    QSharedPointer<Brand> brandPtr;

    for (auto iBrand = m_brands.constBegin(); iBrand != m_brands.constEnd(); ++iBrand)
    {
        if (iBrand.key().toLower() != brand)
            continue;

        brandPtr = iBrand.value();
        break;
    }

    if (!brandPtr)
    {
        qDebug() << "Could not find brand" << brand;
        return indexes;
    }

    const QList< QSharedPointer<Model> >& models = brandPtr->getModels(model);

    if (models.isEmpty())
    {
        qDebug() << "Could not find model" << model << "for brand" << brand;
        return indexes;
    }

    for (auto iModel = models.constBegin(); iModel != models.constEnd(); ++iModel)
    {
        const QList<QPersistentModelIndex>& years = (*iModel)->getYearsBefore(year);

        if (years.isEmpty())
            qDebug() << "Could not find years before" << year << "for model" << model << "of brand" << brand;

        indexes << years;
    }

    return indexes;
}


QPersistentModelIndex Brands::addItem(const QString& value, const QModelIndex& parent)
{
    const int rowId(m_model->rowCount(parent));
    m_model->insertRow(rowId, parent);
    QPersistentModelIndex index(m_model->index(rowId, 0, parent));
    m_model->setData(m_model->index(rowId, 0, parent), value);

    if (!m_model->insertColumns(0, m_model->columnCount(parent), index))
        return QPersistentModelIndex();

    return index;
}


QSharedPointer<Brand> Brands::addBrand(const QString& brand)
{
    qDebug() << "Adding new brand" << brand;
    const QPersistentModelIndex& index = addItem(brand, QModelIndex());
    auto iBrand = m_brands.insert(brand, QSharedPointer<Brand>(new Brand(brand, index)));
    return iBrand.value();
}


QSharedPointer<Model> Brands::addModel(const QString& model, const QSharedPointer<Brand>& brand)
{
    qDebug() << "Adding new model" << model;
    const QPersistentModelIndex& index = addItem(model, brand->getIndex());
    QSharedPointer<Model> modelPtr(new Model(model, index));
    brand->addModel(modelPtr);
    return modelPtr;
}


QPersistentModelIndex Brands::addYear(int year, const QSharedPointer<Model>& model)
{
    qDebug() << "Adding new year" << year;
    const QPersistentModelIndex& index = addItem(QString::number(year), model->getIndex());
    model->addYear(year, index);
    return index;
}
