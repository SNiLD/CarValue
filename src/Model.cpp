/*
 * Model.cpp
 *
 *  Created on: 21 Aug 2016
 *      Author: SNiLD
 */

#include "Model.h"

Model::Model(const QString& model, const QPersistentModelIndex& index) :
    m_model(model),
    m_index(index)
{
}


Model::~Model()
{
}


const QString& Model::getModel() const
{
    return m_model;
}


const QPersistentModelIndex& Model::getIndex() const
{
    return m_index;
}


QPersistentModelIndex Model::getYear(int year) const
{
    return m_years.value(year);
}


QList<QPersistentModelIndex> Model::getYearsBefore(int year) const
{
    QList<QPersistentModelIndex> years;

    for(auto iYear = m_years.constBegin(); iYear != m_years.constEnd(); ++iYear)
    {
        if (iYear.key() < year)
            years << iYear.value();
    }

    return years;
}


void Model::addYear(int year, const QPersistentModelIndex& index)
{
    m_years.insert(year, index);
}
