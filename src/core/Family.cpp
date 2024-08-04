#include "Family.h"
#include <qassert.h>
#include <qpair.h>


const QList<Person*>& Family::getChildren() const {
    return this->children;
}

QPair<Person*, Person*> Family::getParents() const {
    return qMakePair(parent1, parent2);    
}

void Family::addChild(Person* child) {
    Q_ASSERT(!children.contains(child));
    children.append(child);    
}
