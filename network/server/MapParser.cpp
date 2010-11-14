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

#include <QDebug>
#include <QDateTime>

#include "MapParser.h"
#include "BlockMapProperty.h"

MapParser::MapParser(MapServer* m)
{
    width = height = -1;
    map = m;
    counterRows = 0;
}

MapParser::~MapParser()
{
}

bool MapParser::characters(const QString & ch)
{
    qstr_buffer = ch;
    return true;
}

bool MapParser::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes& atts)
{
    if (qName == "Arena")
    {
        // Initialize the number of rows and columns
        for (int i = 0; i < atts.count(); ++i)
        {
            if (atts.qName(i) == "rowCount")
                height = atts.value(i).toInt();

            if (atts.qName(i) == "colCount")
                width = atts.value(i).toInt();

            //TODO:check for the right arenaFileVersion
            //if (atts.qName(i) == "arenaFileVersion")
            //{
            //}
        }
        // Initialize the map
        if (width >= 0 && height >= 0)
            map->setDim(width, height);
        qDebug() << "map parser width x height = " << width << height;
        // initialize random generator
        qsrand(QDateTime::currentDateTime().toTime_t());
    }
    
    return true;
}

bool MapParser::endElement(const QString &, const QString &, const QString & qName)
{
    if(qName == "Row")
    {
        if( width <= 0 || height <= 0 )
            return false;

        int bs = map->getBlockSize();
        for (int i = 0; i < qstr_buffer.length(); ++i)
        {
            switch(qstr_buffer.at(i).toAscii())
            {
                //Ground
                //Not handled types, treaded as empty blocks
                default:
                case 'o': //BombMortar
                case '-': //Ice
                case 'u': //ArrowUp
                case 'r': //ArrowRight
                case 'd': //ArrowDown
                case 'l': //ArrowLeft
                case '_':
                    map->setType(BlockMapProperty::empty,i,counterRows);
                    break;
                //Not handled types, treated as walls
                case ' ': //Hole
                case '=':
                    map->setType(BlockMapProperty::wall,i,counterRows);
                    break;
                case '+':
                    map->setType(BlockMapProperty::brick,i,counterRows);
                    break;
                case 'x':
                    // create a random block
                    if((qrand()/1.0)/RAND_MAX > 0.25)
                        map->setType(BlockMapProperty::brick,i,counterRows);
                    else
                        map->setType(BlockMapProperty::empty,i,counterRows);
                    break; 
                case 'p':
                    map->setType(BlockMapProperty::empty,i,counterRows);
                    map->addPlayerSlot(i * bs + bs / 2, counterRows * bs + bs / 2);
            }
        }
        counterRows++;
    }
    return true;
}
