#include "Gender.h"
#include <qobject.h>
#include "core.h"

const Gender Gender::Male {g_GEDCOMX_PREFIX + "Male"};
const Gender Gender::Female {g_GEDCOMX_PREFIX + "Female"};
const Gender Gender::Intersex {g_GEDCOMX_PREFIX + "Intersex"};
const Gender Gender::Unknown {g_GEDCOMX_PREFIX + "Unknown"};

Gender::Gender(QString atype, QObject *parent) : type(atype), QObject(parent) {}

Gender::Gender(QObject* parent) : type(g_GEDCOMX_PREFIX + "Unknown") {}