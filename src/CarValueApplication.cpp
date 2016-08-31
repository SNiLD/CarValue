/*
 * CarValueApplication.cpp
 *
 *  Created on: 23 Aug 2014
 *      Author: SNiLD
 */

#include "CarValueApplication.h"
#include "CsvParser.h"


CarValueApplication::CarValueApplication(int& argc, char** argv) :
        QApplication(argc, argv)
{
    m_window.setTrafiModel(&m_trafiModel);
    m_window.setNettiautoModel(&m_nettiautoModel);
    m_window.show();

    CsvParser csvParser;
    csvParser.setItemModel(&m_trafiModel);
    csvParser.parseTrafi("data/trafi");
    csvParser.parseRust("data");
    csvParser.calculateAverages();
}


CarValueApplication::~CarValueApplication()
{
}
