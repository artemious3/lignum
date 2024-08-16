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

  uint32_t id = person->identifier.toUInt(&id_conversion_success);

  if(!id_conversion_success){
    qWarning("Unable to convert Person id %s to number.", qUtf8Printable(person->identifier));
  }

  if (persons.contains(id)) {
    qFatal("Double insert person with id %u", id);
  }

  persons[id] = person;
  last_id = std::max(last_id.load(), id);
}

bool DB::removePerson(uint32_t id) {
  std::lock_guard<db_mutex_t> lock(mutex);
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

void DB::insertRelationship(std::shared_ptr<Relationship> relationship) {   
  std::lock_guard<db_mutex_t> lock{mutex};
  family_db.addRelationship(relationship);
}

QList<std::shared_ptr<Person>> DB::getPersons() {
  std::lock_guard<db_mutex_t> lock{mutex};
  return std::move(persons.values());
}

} // namespace mftb







