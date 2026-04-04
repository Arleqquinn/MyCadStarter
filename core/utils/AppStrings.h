#pragma once

#include <QString>

namespace AppStrings {

/**
 * @brief Builds a default entity name using a human readable kind and numeric identifier.
 * @param kind Primitive or operation kind, for example Box or Cut.
 * @param id Positive numeric identifier of the object.
 * @return Name in the form Kind_id.
 */
QString makeDefaultObjectName(const QString& kind, int id);

/**
 * @brief Creates a user-friendly text for translation vector values.
 * @param dx Translation along X axis.
 * @param dy Translation along Y axis.
 * @param dz Translation along Z axis.
 * @return Formatted string with two fractional digits.
 */
QString formatVector3(double dx, double dy, double dz);

/**
 * @brief Produces a duplicate suffix while preserving the original base name.
 * @param sourceName Original object name.
 * @return Name with _copy suffix.
 */
QString makeDuplicateName(const QString& sourceName);

}
