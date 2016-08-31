/*
 * CsvParser.cpp
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#include "CsvParser.h"
#include "Brands.h"

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
    m_model(nullptr),
    m_brands(new Brands(m_model))
{
}


CsvParser::~CsvParser()
{
}


void CsvParser::setItemModel(QAbstractItemModel* model)
{
    m_model = model;
    m_brands.reset(new Brands(model));
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
    m_model->setHeaderData(COLUMN_INSPECTION_YEAR, Qt::Horizontal, QObject::tr("Inspection year"));
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

    QList<int> rustProtections;
    QList< QPair<QString, uint> > reasons;
    double rejections(0.0);
    int validRowCount(0);

    calculateAverages(QModelIndex(), rustProtections, reasons, rejections, validRowCount);
}


bool CsvParser::calculateAverages(
        const QModelIndex& parent,
        QList<int>& rustProtections,
        QList< QPair<QString, uint> >& reasons,
        double& rejections,
        int& validRowCount)
{
    const int rowCount(m_model->rowCount(parent));

    if (rowCount <= 0)
        return false;

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex& index = m_model->index(row, COLUMN_MODEL, parent);

        if (!index.isValid())
            continue;

        QList<int> currentRustProtections;
        QList< QPair<QString, uint> > currentReasons;
        double currentRejections(0.0);
        int currentValidRowCount(0);

        if (m_model->hasChildren(index))
        {
            calculateAverages(index, currentRustProtections, currentReasons, currentRejections, currentValidRowCount);

            if (currentRustProtections.size() > 0)
            {
                // Calculate median
                std::sort(currentRustProtections.begin(), currentRustProtections.end());
                m_model->setData(m_model->index(row, COLUMN_RUST_PROTECTION, parent), currentRustProtections.at(currentRustProtections.size() / 2));
            }

            if (currentValidRowCount > 0)
                m_model->setData(m_model->index(row, COLUMN_REJECTION_PERCENT, parent), currentRejections / (double)currentValidRowCount);

            std::sort(currentReasons.begin(), currentReasons.end());

            if (currentReasons.size() > 0)
                m_model->setData(m_model->index(row, COLUMN_1_COMMON_REASON, parent), currentReasons.at(0).first);

            if (currentReasons.size() > 1)
                m_model->setData(m_model->index(row, COLUMN_2_COMMON_REASON, parent), currentReasons.at(1).first);

            if (currentReasons.size() > 2)
                m_model->setData(m_model->index(row, COLUMN_3_COMMON_REASON, parent), currentReasons.at(2).first);
        }
        else
        {
            const QVariant& rustProtection = m_model->data(m_model->index(row, COLUMN_RUST_PROTECTION, parent));

            if (rustProtection.isValid() && rustProtection.canConvert(QVariant::Int))
                currentRustProtections << rustProtection.toInt();

            bool wasOk = false;
            double rejection = m_model->data(m_model->index(row, COLUMN_REJECTION_PERCENT, parent)).toDouble(&wasOk);

            if (wasOk)
            {
                currentRejections += rejection;
                ++currentValidRowCount;
            }

            for (int l = 0; l < 3; ++l)
            {
                const QVariant& reasonVariant = m_model->data(m_model->index(row, COLUMN_1_COMMON_REASON + l, parent));

                if (!reasonVariant.isValid() || reasonVariant.isNull() || !reasonVariant.canConvert(QVariant::String))
                    continue;

                const QString& reason = reasonVariant.toString();

                if (reason.isEmpty())
                    continue;

                auto iReason = std::find(currentReasons.begin(), currentReasons.end(), reason);

                if (iReason == currentReasons.end())
                    iReason = currentReasons.insert(iReason, qMakePair(reason, 0));

                ++((*iReason).second);
            }
        }

        rustProtections << currentRustProtections;
        reasons << currentReasons;
        rejections += currentRejections;
        validRowCount += currentValidRowCount;
    }

    return true;
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

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "Failed to open" << fileName << "for reading.";
        return;
    }

    const QStringList& names = QDir(fileName).dirName().split("_");
    int year = -1;
    int inspectionYear = -1;

    if (names.size() == 2)
    {
        year = names.at(0).toInt();

        const QStringList& nameEnd = names.at(1).split(".");

        if (!nameEnd.isEmpty())
            inspectionYear = nameEnd.at(0).toInt();
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    int rowCount = 0;

    while (!stream.atEnd())
    {
        const QString& row = stream.readLine();

        if (row.isEmpty())
        {
            qDebug() << "Skipping empty row.";
            continue;
        }

        QStringList columns;
        int lastIndex = 0;
        bool isQuoteStart = false;

        for (int i = 0; i < row.size(); ++i)
        {
            if (row.at(i) == "\"")
            {
                isQuoteStart = !isQuoteStart;
            }
            else if (row.at(i) == "," && !isQuoteStart)
            {
                columns << row.mid(lastIndex, i - lastIndex).remove("\"").replace(",", ".").trimmed();
                lastIndex = i + 1;
            }
        }

        columns << row.mid(lastIndex, row.size() - lastIndex).remove("\"").trimmed();

        if (columns.size() < 5 || columns.at(0) == "Merkki" || columns.at(0).startsWith("Kaikki"))
        {
            qDebug() << "Skipping invalid row" << row << " (columns " << columns.join("|") << ")";
            continue;
        }

        const QString& brand = columns.at(0);
        const QString& model = columns.at(1);

        const QPersistentModelIndex& index = m_brands->createIndex(brand, model, year);

        const int rowId(m_model->rowCount(index));

        if (!m_model->insertRow(rowId, index))
        {
            qWarning() << "Failed to add row with ID" << rowId;
            continue;
        }

        m_model->setData(m_model->index(rowId, COLUMN_INSPECTION_YEAR, index), inspectionYear);

        for (int i = COLUMN_REJECTION_PERCENT; i < columns.size() + 1; ++i)
            m_model->setData(m_model->index(rowId, i, index), columns.at(i - 1));

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

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
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

        QString brand = columns.at(0).trimmed();
        brand.replace("é","e");
        brand.replace("ë","e");
        brand.replace("Mercedes","Mercedes-Benz");

        if (brand == "VW")
            brand = "Volkswagen";

        brand = brand.toLower();

        int foundCount = 0;

        QString model = columns.at(1).toLower().split(" ").at(0).trimmed();
        model.replace("é", "e");
        model.replace("´", "");
        model.replace("-luokka","");

        const int year(columns.at(2).toInt());
        const QString& rustProtection = columns.at(4);

        const auto& years = m_brands->getIndexes(brand, model, year);

        for (auto iYear = years.constBegin(); iYear != years.constEnd(); ++iYear)
        {
            const int rows = m_model->rowCount(*iYear);

            for (int row = 0; row < rows; ++row)
            {
                m_model->setData(m_model->index(row, COLUMN_RUST_PROTECTION, *iYear), rustProtection);
            }

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
