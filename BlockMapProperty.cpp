#include "BlockMapProperty.h"

BlockMapProperty::BlockMapProperty()
{
    type = empty;
    option = none;
}

BlockMapProperty::~BlockMapProperty()
{
}

void BlockMapProperty::setType(BlockType t)
{
    type = t;
}

BlockMapProperty::BlockType BlockMapProperty::getType()
{
    return type;
}

