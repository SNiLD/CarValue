/*
 * BrandModel.cpp
 *
 *  Created on: 18 Aug 2016
 *      Author: SNiLD
 */

#include "Brand.h"
#include "Model.h"

Brand::Brand(const QString& brand, const QPersistentModelIndex& index) :
    m_brand(brand),
    m_index(index)
{
}


Brand::~Brand()
{
}


const QString& Brand::getBrand() const
{
    return m_brand;
}


const QPersistentModelIndex& Brand::getIndex() const
{
    return m_index;
}


QSharedPointer<Model> Brand::getModel(const QString& model) const
{
    return m_models.value(model);
}


QList< QSharedPointer<Model> > Brand::getModels(const QString& model) const
{
    QList< QSharedPointer<Model> > models;

    for (auto iModel = m_models.constBegin(); iModel != m_models.constEnd(); ++iModel)
    {
        if (iModel.key().toLower().startsWith(model))
            models << iModel.value();
    }

    return models;
}


void Brand::addModel(const QSharedPointer<Model>& model)
{
    m_models.insert(model->getModel(), model);
}
