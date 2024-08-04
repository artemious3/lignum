#include "DB.h"
#include "Gender.h"
#include "Person.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <qcontainerfwd.h>
#include <qforeach.h>
#include <qhashfunctions.h>
#include <qlogging.h>

namespace mftb {

// Thhread-safe implementation of Singleton according to
// https://stackoverflow.com/questions/2576022/how-do-you-implement-a-singleton-efficiently-and-thread-safely
DB &mftb::DB::getInstance() {
  static DB instance{};
  return instance;
}

/*---------------------------------------*/

uint32_t DB::getNewId() { return last_id++; }

void DB::insertPerson(std::shared_ptr<Person> person) {
  std::lock_guard<db_mutex_t> lock(mutex);
  bool id_conversion_success;

  uint32_t pers_id = person->identifier.toUInt(&id_conversion_success);

  if(!id_conversion_success){
    qWarning("Unable to convert Person id %s to number.", qUtf8Printable(person->identifier));
  }

  if (persons.contains(pers_id)) {
    qFatal("Double insert person with id %u", pers_id);
  }

  persons[pers_id] = person;
  last_id = std::max(last_id.load(), pers_id);
}

bool DB::removePerson(uint32_t id) {
  std::lock_guard<db_mutex_t> lock(mutex);
  auto subject = persons[id];
  return persons.remove(id);
}

std::weak_ptr<Person> DB::getPersonById(uint32_t id) {
  std::lock_guard<db_mutex_t> lock(mutex);
  return  std::weak_ptr( persons[id] );
}

uint32_t DB::_createPerson(bool gender, QString first_name, QString surname) {
  std::lock_guard<db_mutex_t> lock(mutex);

  uint32_t id = getNewId();

  auto pers = std::make_shared<Person>();
  pers->identifier = QString::number(id);
  insertPerson(pers);

  pers->gender = gender ? &Gender::Male : &Gender::Female;

  auto * name = new Name(pers.get());

  NameForm *nf = new NameForm(name);
  NamePart *first = new NamePart(nf);
  first->value = first_name;
  NamePart *sur = new NamePart(nf);
  sur->value = surname;

  nf->parts.push_back(first);
  nf->parts.push_back(sur);

  name->nameForms.push_back(nf);
  pers->names.push_back(name);

  return id;
}

uint32_t DB::createRelationship(uint32_t id1, uint32_t id2, Relationship::Type type) {
  std::lock_guard<db_mutex_t> lock{mutex};

  auto new_relationship = std::make_shared<Relationship>();
  new_relationship->person1 = QString::number(id1);
  new_relationship->person2 = QString::number(id2);
  new_relationship->setType(type);

  uint32_t relationship_id = getNewId();
  new_relationship->identifier = QString::number(relationship_id);
  relationships[relationship_id] = new_relationship;

  return relationship_id;
}

void DB::insertRelationship(std::shared_ptr<Relationship> relationship) {   
  std::lock_guard<db_mutex_t> lock{mutex};

  bool id_conversion_success;
  uint32_t rel_id = relationship->identifier.toUInt(&id_conversion_success);

  if(!id_conversion_success){
    qWarning("Unable to convert Relationship id %s to number.", qUtf8Printable(relationship->identifier));
    return;
  }

  if(relationships.contains(rel_id)){
    qFatal("Double insert of Relationsip with id %u", rel_id);
  }

  relationships[rel_id] = relationship;
  last_id = std::max(last_id.load(), rel_id);
}

QList<std::shared_ptr<Relationship>> DB::getRelationships() {
  std::lock_guard<db_mutex_t> lock{mutex};
  return std::move(relationships.values());
}

QList<std::shared_ptr<Person>> DB::getPersons() {
  std::lock_guard<db_mutex_t> lock{mutex};
  return std::move(persons.values());
}


} // namespace mftb







