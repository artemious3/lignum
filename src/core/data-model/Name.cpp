#include "Name.h" 
#include "NamePart.h"
#include <qcontainerfwd.h>
#include <qobject.h>

QString Name::getFullName() const {   
    QString full_name{};
    foreach(const NamePart* part,  nameForms.first()->parts){
        full_name += part->value + " ";
    }
    return full_name;
}


Name::Name(QObject* par) : QObject(par) {}