/*
 * NettiautoFetcher.cpp
 *
 *  Created on: 24 Aug 2014
 *      Author: SNiLD
 */

#include "NettiautoFetcher.h"

#include "NettiautoParser.h"

#include <curl/curl.h>

#include <QDir>
#include <QtDebug>
#include <QUrl>

#include <cassert>


static size_t WriteHttpData(void* data, size_t dataSize, size_t dataTypeSize, void* userData)
{
    try
    {
        NettiautoFetcher* fetcher = reinterpret_cast<NettiautoFetcher*>(userData);

        QFile& file = fetcher->getCurrentFile();
        qint64 dataWritten = file.write((const char*)data, dataSize * dataTypeSize / sizeof(char));

        return dataWritten;
    }
    catch (const std::exception& e)
    {
        qCritical() << "Failed to fetch data:" << e.what();
    }

    return 0;
}


NettiautoFetcher::NettiautoFetcher() :
        QObject(),
    m_curlHandle(NULL),
    m_model(NULL),
    m_sourceModel(NULL)
{
    m_curlHandle = curl_easy_init();
    m_errorMessage.resize(CURL_ERROR_SIZE);
    curl_easy_setopt(m_curlHandle, CURLOPT_ERRORBUFFER, m_errorMessage.data());

}


NettiautoFetcher::~NettiautoFetcher()
{
    curl_easy_cleanup(m_curlHandle);
    m_curlHandle = NULL;
}


void NettiautoFetcher::setPath(const QString& path)
{
    m_path = path;

    QDir dir(m_path);

    if (!dir.exists())
        dir.mkdir(".");
}


void NettiautoFetcher::setModel(QAbstractItemModel* model)
{
    m_model = model;
}


void NettiautoFetcher::setSourceModel(QAbstractItemModel* model)
{
    m_sourceModel = model;
}


void NettiautoFetcher::fetch(const QString& brand, const QString& model, int year)
{
    assert(m_curlHandle);

    if (!m_curlHandle)
        return;

    // Brand only:
    // http://m.nettiauto.com/audi/vaihtoautot?sortCol=price&ord=ASC&id_make=4&autoload=true&page=1
    // Brand + model:
    // http://m.nettiauto.com/audi/a4?sortCol=price&ord=ASC&id_make=4&id_model=45&autoload=true&page=1
    // Brand + model + year
    // http://m.nettiauto.com/audi/a4?sortCol=price&ord=ASC&id_make=4&id_model=45&yfrom=2004&autoload=true&page=1
    // http://m.nettiauto.com/audi/a4?sortCol=price&ord=ASC&id_make=4&id_model=45&yfrom=2004&yto=2004&autoload=true&page=1
    int page(1);

    QDir path(m_path);

    if (!path.cd(brand))
    {
        path.mkdir(brand);
        path.cd(brand);
    }

    if (!path.cd(model))
    {
        path.mkdir(model);
        path.cd(model);
    }

    curl_easy_setopt(m_curlHandle, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_curlHandle, CURLOPT_WRITEFUNCTION, WriteHttpData);

    m_file.close();
    bool wasValidPage = true;

    while (wasValidPage)
    {
        const QString& fileName = path.absoluteFilePath("%1_%2.html").arg(QString::number(year), QString::number(page));
        m_file.setFileName(fileName);

        if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qWarning() << "Failed to open file" << fileName;
            m_file.close();
            return;
        }

        QUrl url(QString("http://m.nettiauto.com/%1/%2?sortCol=price&ord=ASC&yfrom=%3&yto=%3&autoload=true&page=%4").arg(brand.toLower(), model.toLower(), QString::number(year), QString::number(page)));
        qDebug() << "Fetching" << brand << "model" << model << "registration year" << year << "URL:" << url.toString();
        curl_easy_setopt(m_curlHandle, CURLOPT_URL, qPrintable(url.toString()));
        CURLcode code = curl_easy_perform(m_curlHandle);

        if (code != CURLE_OK)
        {
            qWarning() << "Failed to retrieve:" << QString::fromLocal8Bit(m_errorMessage);
            wasValidPage = false;
        }
        else
        {
            wasValidPage = NettiautoParser::isValid(m_file.fileName());
        }

        page++;
        m_file.close();
    }
}


QFile& NettiautoFetcher::getCurrentFile()
{
    return m_file;
}


void NettiautoFetcher::fetchAll()
{
    assert(m_sourceModel);
    assert(m_model);

    const int brandCount(m_sourceModel->rowCount());

    for (int i = 0; i < brandCount; ++i)
    {
        const QModelIndex& parent = m_sourceModel->index(i, 0);
        const QString& brand = m_sourceModel->data(parent).toString();
        const int childCount(m_sourceModel->rowCount(parent));

        for (int j = 0; j < childCount; ++j)
        {
            const QString& model = m_sourceModel->data(m_sourceModel->index(j, 0, parent)).toString();
            int year = m_sourceModel->data(m_sourceModel->index(j, 1, parent)).toInt();

            fetch(brand, model, year);
        }
    }
}
