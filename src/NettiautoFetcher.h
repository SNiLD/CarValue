/*
 * NettiautoFetcher.h
 *
 *  Created on: 24 Aug 2014
 *      Author: SNiLD
 */

#ifndef NETTIAUTOFETCHER_H_
#define NETTIAUTOFETCHER_H_

#include <QAbstractItemModel>
#include <QByteArray>
#include <QFile>
#include <QObject>


typedef void CURL;


class NettiautoFetcher : public QObject
{
    Q_OBJECT

public:
    NettiautoFetcher();
    virtual ~NettiautoFetcher();

    void setPath(const QString& path);
    void setModel(QAbstractItemModel* model);
    void setSourceModel(QAbstractItemModel* model);

    void fetch(const QString& brand, const QString& model, int year);

    QFile& getCurrentFile();

public slots:
    void fetchAll();

private:
    CURL* m_curlHandle;
    QByteArray m_errorMessage;
    QString m_path;
    QAbstractItemModel* m_model;
    QAbstractItemModel* m_sourceModel;
    QFile m_file;
};

#endif /* NETTIAUTOFETCHER_H_ */
