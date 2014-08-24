/*
 * TrafiParser.h
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#ifndef TRAFIPARSER_H_
#define TRAFIPARSER_H_

#include <QAbstractItemModel>
#include <QHash>
#include <QPersistentModelIndex>
#include <QString>

class CsvParser
{
public:
    CsvParser();
    virtual ~CsvParser();

    void setItemModel(QAbstractItemModel* model);
    void parseTrafi(const QString& path);
    void parseRust(const QString& path);
    void calculateAverages();

private:
    enum Column
    {
        COLUMN_MODEL,
        COLUMN_REGISTRATION_YEAR,
        COLUMN_RUST_PROTECTION,
        COLUMN_REJECTION_PERCENT,
        COLUMN_1_COMMON_REASON,
        COLUMN_2_COMMON_REASON,
        COLUMN_3_COMMON_REASON,
        COLUMN_COUNT
    };

    QAbstractItemModel* m_model;
    QHash<QString, QPersistentModelIndex> m_brands;

    void parseTrafiCsv(const QString& fileName);
    void parseRustCsv(const QString& fileName);
};

#endif /* TRAFIPARSER_H_ */
