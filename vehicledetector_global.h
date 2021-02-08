#ifndef VEHICLEDETECTOR_GLOBAL_H
#define VEHICLEDETECTOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(VEHICLEDETECTOR_LIBRARY)
#  define VEHICLEDETECTORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define VEHICLEDETECTORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // VEHICLEDETECTOR_GLOBAL_H