#include "Relationship.h"

Relationship::Relationship(QObject* par) : Subject(par) {}

void Relationship::setType(Relationship::Type type) {
    switch (type) {
        case Type::ParentChild:
            this->type = ParentChildType;
            break;
        case Type::Couple:
            this->type = CoupleType;
            break;
    }    
}
