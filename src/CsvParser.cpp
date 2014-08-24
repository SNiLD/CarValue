/*
 * CsvParser.cpp
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#include "CsvParser.h"

#include <QDebug>
#include <QDir>
#include <QPair>
#include <QTextStream>

#include <algorithm>
#include <cassert>


bool operator==(const QPair<QString, uint>& first, const QString& second)
{
    return first.first == second;
}


bool operator<(const QPair<QString, uint>& first, const QPair<QString, uint>& second)
{
    return first.second > second.second;
}


CsvParser::CsvParser() :
    m_model(NULL)
{
}


CsvParser::~CsvParser()
{
}


void CsvParser::setItemModel(QAbstractItemModel* model)
{
    m_model = model;
}


void CsvParser::parseTrafi(const QString& path)
{
    assert(m_model);

    if (!m_model->insertColumns(0, COLUMN_COUNT))
    {
        assert(false);
        return;
    }

    m_model->setHeaderData(COLUMN_MODEL, Qt::Horizontal, QObject::tr("Brand / Model"));
    m_model->setHeaderData(COLUMN_REGISTRATION_YEAR, Qt::Horizontal, QObject::tr("Registration year"));
    m_model->setHeaderData(COLUMN_RUST_PROTECTION, Qt::Horizontal, QObject::tr("Rust protection"));
    m_model->setHeaderData(COLUMN_REJECTION_PERCENT, Qt::Horizontal, QObject::tr("Rejection %"));
    m_model->setHeaderData(COLUMN_1_COMMON_REASON, Qt::Horizontal, QObject::tr("Most common reason"));
    m_model->setHeaderData(COLUMN_2_COMMON_REASON, Qt::Horizontal, QObject::tr("Second most common reason"));
    m_model->setHeaderData(COLUMN_3_COMMON_REASON, Qt::Horizontal, QObject::tr("Third most common reason"));

    QDir dir(path);
    const QStringList& files = dir.entryList(QStringList("*.csv"));

    for (auto iFile = files.constBegin(); iFile != files.constEnd(); ++iFile)
        parseTrafiCsv(dir.absoluteFilePath(*iFile));
}


void CsvParser::parseRust(const QString& path)
{
    QDir dir(path);
    const QStringList& files = dir.entryList(QStringList("*.csv"));

    for (auto iFile = files.constBegin(); iFile != files.constEnd(); ++iFile)
        parseRustCsv(dir.absoluteFilePath(*iFile));
}


void CsvParser::calculateAverages()
{
    assert(m_model);

    for (int i = 0; i < m_model->rowCount(); ++i)
    {
        const QModelIndex& parent = m_model->index(i, COLUMN_MODEL);
        const int rowCount = m_model->rowCount(parent);
        QList<int> years;
        QList<int> rustProtections;
        QList< QPair<QString, uint> > reasons;
        double rejections(0.0);

        for (int j = 0; j < rowCount; ++j)
        {
            years << m_model->data(m_model->index(j, COLUMN_REGISTRATION_YEAR, parent)).toInt();
            rejections += m_model->data(m_model->index(j, COLUMN_REJECTION_PERCENT, parent)).toDouble();

            const QVariant& rustProtection = m_model->data(m_model->index(j, COLUMN_RUST_PROTECTION, parent));

            if (rustProtection.isValid() && rustProtection.canConvert(QVariant::Int))
                rustProtections << rustProtection.toInt();

            for (int k = 0; k < 3; ++k)
            {
                const QVariant& reasonVariant = m_model->data(m_model->index(j, COLUMN_1_COMMON_REASON + k, parent));

                if (!reasonVariant.isValid() || reasonVariant.isNull() || !reasonVariant.canConvert(QVariant::String))
                    continue;

                const QString& reason = reasonVariant.toString();

                if (reason.isEmpty())
                    continue;

                auto iReason = std::find(reasons.begin(), reasons.end(), reason);

                if (iReason == reasons.end())
                    iReason = reasons.insert(iReason, qMakePair(reason, 0));

                ++((*iReason).second);
            }
        }

        if (years.size() > 0)
        {
            std::sort(years.begin(), years.end());
            m_model->setData(m_model->index(i, COLUMN_REGISTRATION_YEAR), years.at(years.size() / 2));
        }

        if (rustProtections.size() > 0)
        {
            std::sort(rustProtections.begin(), rustProtections.end());
            m_model->setData(m_model->index(i, COLUMN_RUST_PROTECTION), rustProtections.at(rustProtections.size() / 2));
        }

        if (rowCount > 0)
            m_model->setData(m_model->index(i, COLUMN_REJECTION_PERCENT), rejections / (double)rowCount);

        std::sort(reasons.begin(), reasons.end());

        if (reasons.size() > 0)
            m_model->setData(m_model->index(i, COLUMN_1_COMMON_REASON), reasons.at(0).first);

        if (reasons.size() > 1)
            m_model->setData(m_model->index(i, COLUMN_2_COMMON_REASON), reasons.at(1).first);

        if (reasons.size() > 2)
            m_model->setData(m_model->index(i, COLUMN_3_COMMON_REASON), reasons.at(2).first);
    }
}


void CsvParser::parseTrafiCsv(const QString& fileName)
{
    qDebug() << "Parsing file:" << fileName;

    QFile file(fileName);

    if (!file.exists())
    {
        qWarning() << "Skipping" << fileName << "because it does not exist!";
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open" << fileName << "for reading.";
        return;
    }

    const QStringList& names = QDir(fileName).dirName().split("_");
    int year = -1;

    if (names.size() == 2)
        year = names.at(0).toInt();

    QTextStream stream(&file);
    int rowCount = 0;

    while (!stream.atEnd())
    {
        const QString& row = stream.readLine();

        if (row.isEmpty())
        {
            qDebug() << "Skipping empty row.";
            continue;
        }

        const QStringList& columns = row.split(";");

        if (columns.size() != 6 || columns.at(0) == "Merkki" || columns.at(0).startsWith("Kaikki"))
        {
            qDebug() << "Skipping invalid row" << row;
            continue;
        }

        const QString& brand = columns.at(0);

        auto iBrand = m_brands.constFind(brand);

        if (iBrand == m_brands.constEnd())
        {
            const int rowId(m_model->rowCount());
            m_model->insertRow(rowId);
            qDebug() << "Adding new brand" << brand;
            iBrand = m_brands.insert(brand, QPersistentModelIndex(m_model->index(rowId, 0)));
            m_model->setData(m_model->index(rowId, COLUMN_MODEL), brand);

            const QModelIndex& index = iBrand.value();

            if (!m_model->insertColumns(0, COLUMN_COUNT, index))
            {
                assert(false);
                return;
            }
        }

        const QModelIndex& parent = iBrand.value();

        const int rowId(m_model->rowCount(parent));

        if (!m_model->insertRow(rowId, parent))
        {
            qWarning() << "Failed to add row with ID" << rowId;
            continue;
        }

        m_model->setData(m_model->index(rowId, COLUMN_MODEL, parent), columns.at(1));
        m_model->setData(m_model->index(rowId, COLUMN_REGISTRATION_YEAR, parent), year);

        for (int i = 2; i < columns.size(); ++i)
            m_model->setData(m_model->index(rowId, i + 1, parent), columns.at(i));

        rowCount++;
    }

    qDebug() << "Added" << rowCount << "rows.";
}


void CsvParser::parseRustCsv(const QString& fileName)
{
    qDebug() << "Parsing file:" << fileName;

    QFile file(fileName);

    if (!file.exists())
    {
        qWarning() << "Skipping" << fileName << "because it does not exist!";
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open" << fileName << "for reading.";
        return;
    }

    QTextStream stream(&file);
    int rowCount = 0;

    while (!stream.atEnd())
    {
        const QString& row = stream.readLine();

        if (row.isEmpty())
        {
            qDebug() << "Skipping empty row.";
            continue;
        }

        const QStringList& columns = row.split(";");

        if (columns.size() != 5 || columns.at(0) == "Merkki")
        {
            qDebug() << "Skipping invalid row" << row;
            continue;
        }

        QString brand = columns.at(0);
        brand.replace("é","e");
        brand.replace("ë","e");
        brand.replace("Mercedes","Mercedes-Benz");

        if (brand == "VW")
            brand = "Volkswagen";

        auto iBrand = m_brands.constFind(brand);

        if (iBrand == m_brands.constEnd())
        {
            qWarning() << "Did not find brand" << brand;
            continue;
        }

        const QModelIndex& parent = iBrand.value();

        const int rowId(m_model->rowCount(parent));
        int foundCount = 0;

        QString model = columns.at(1).toLower().split(" ").at(0);
        model.replace("é", "e");
        model.replace("´", "");
        model.replace("-luokka","");

        for (int i = 0; i < rowId; ++i)
        {
            if (model != m_model->data(m_model->index(i, COLUMN_MODEL, parent)).toString().toLower().split(" ").at(0))
                continue;

            // Make sure the car is not registered after the test year so we narrow down the false positives.
            if (columns.at(2).toInt() < m_model->data(m_model->index(i, COLUMN_REGISTRATION_YEAR, parent)).toInt())
                continue;

            m_model->setData(m_model->index(i, COLUMN_RUST_PROTECTION, parent), columns.at(4));
            foundCount++;
        }

        if (foundCount == 0)
            qWarning() << "Did not find brand" << brand << "model" << columns.at(1) << "row:" << row;
        else
        {
            qDebug() << "Added brand" << brand << "model" << columns.at(1) << "rust" << columns.at(4) << "to" << foundCount << "rows.";
            rowCount++;
        }
    }

    qDebug() << "Added" << rowCount << "rows.";
}
