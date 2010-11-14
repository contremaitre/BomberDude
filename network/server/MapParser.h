/*
    Copyright (C) 2010 Sébastien Escudier

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QTB_MAPPARSER_H
#define QTB_MAPPARSER_H


#include <QtXml/QXmlDefaultHandler>

#include "MapServer.h"

/**
 * @brief This class handles XML reader events in order to initialize the Arena properties.
 */
class MapParser : public QXmlDefaultHandler
{

private:

    int width, height;
    int counterRows;

    /** The Map to initialize */
    MapServer* map;

    /** The parser's buffer */
    QString qstr_buffer;

public:
    
    /**
      * Creates a new GameParser.
      */
    MapParser(MapServer* map);
    
    /**
      * Deletes the GameParser instance.
      */
    ~MapParser();

    /**
    * Implement QXmlDefaultHandler::characters
    */
    bool characters(const QString & ch );

    /**
      * Implements QXmlDefaultHandler::startElement()
      */
    bool startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& atts);
    
    /**
    * Implements QXmlDefaultHandler::endElement()
    */
    bool endElement(const QString &, const QString &, const QString & qName );

};


#endif // QTB_MAPPARSER_H
