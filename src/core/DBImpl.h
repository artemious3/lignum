#pragma once
#include "DB.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlrecord.h>

namespace mftb {

class SqlDB : public DB {

public:
  static SqlDB *getInstance();

private:
  SqlDB();
  ~SqlDB();
  static Person extractPersonFromRecord(const QSqlRecord &);
  static QString getTemporaryDbName();

public:
  // virtual Person getPersonById(id_t) const override;
  // virtual Couple getCoupleById(id_t) const override;

  // virtual std::pair<id_t, id_t> getPersonParentsById(id_t) const override;
  // virtual std::vector<id_t> getPersonPartners(id_t ids) const override;
  // virtual std::vector<id_t> getPersonChildren() const override;
  // virtual std::vector<id_t>
  // getPersonChildrenWithPartner(id_t parener_id) const override;

  virtual void insertPerson(const Person &pers) override;
  // virtual void addChild(const Person &person, id_t parent1,
  //                       id_t parent2 = 0) override;
  // virtual void addPartner(const Person &person, id_t partner) override;
  // virtual void removePerson(id_t) override;
  virtual std::vector<Person> getPeople(int max_amount) const override;

private:
  QString db_filename;
};

} // namespace mftb
