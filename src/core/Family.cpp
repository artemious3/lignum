#include "Family.h"
#include <cstdint>
#include <qassert.h>
#include <qpair.h>

Family::Family(uint32_t a_parent1, uint32_t a_parent2)
    : parent1_id(a_parent1), parent2_id(a_parent2) {}

const QList<uint32_t> &Family::getChildren() const { return this->children; }

QPair<uint32_t, uint32_t> Family::getParents() const {
  return qMakePair(parent1_id, parent2_id);
}

void Family::addChild(uint32_t child) {
  Q_ASSERT(!children.contains(child));
  children.append(child);
}

bool Family::isSingleParent() const {
    return parent2_id == 0;    
}

void Family::setEmptyParent(uint32_t id) {
    if(parent2_id == 0){
        parent2_id = id;
    }    
}

bool Family::hasParent(uint32_t id) const {
    return parent1_id == id || parent2_id == id;    
}
