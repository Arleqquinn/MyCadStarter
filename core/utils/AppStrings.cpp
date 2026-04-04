#include "AppStrings.h"

namespace AppStrings {

QString makeDefaultObjectName(const QString& kind, int id)
{
    return QString("%1_%2").arg(kind).arg(id);
}

QString formatVector3(double dx, double dy, double dz)
{
    return QString("X=%1, Y=%2, Z=%3")
        .arg(dx, 0, 'f', 2)
        .arg(dy, 0, 'f', 2)
        .arg(dz, 0, 'f', 2);
}

QString makeDuplicateName(const QString& sourceName)
{
    return sourceName + "_copy";
}

}
