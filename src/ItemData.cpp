#include <ItemData.h>

bool ItemData::operator<(const ItemData &other)
{
    return priority < other.priority;
}

bool ItemData::operator==(const ItemData &other)
{
    priority == other.priority;
}

bool ItemData::operator > (const ItemData &other)
{
    return priority > other.priority;
}

bool ItemData::operator!=(const ItemData &other)
{
    priority != other.priority;
}

bool ItemData::operator<=(const ItemData &other)
{
    priority <= other.priority;
}

bool ItemData::operator>=(const ItemData &other)
{
    priority >= other.priority;
}