/*
 * NettiautoFetcher.h
 *
 *  Created on: 24 Aug 2014
 *      Author: SNiLD
 */

#ifndef NETTIAUTOFETCHER_H_
#define NETTIAUTOFETCHER_H_

#include <QObject>


class NettiautoFetcher : public QObject
{
    Q_OBJECT

public:
    NettiautoFetcher();
    virtual ~NettiautoFetcher();

public slots:
    void fetch();
};

#endif /* NETTIAUTOFETCHER_H_ */
