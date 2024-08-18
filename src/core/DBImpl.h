#pragma once
#include "DB.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlrecord.h>
#include <optional>
#include <qvariant.h>

namespace mftb {

class SqlDB : public DB {

public:
  static SqlDB *getInstance();

private:
  SqlDB();
  ~SqlDB();
  static Person extractPersonFromRecord(const QSqlRecord &);
  static Couple extractCoupleFromRecord(const QSqlRecord &);
  static QString getTemporaryDbName();
  id_t insertPersonWithParentsCoupleId(const Person &pers, id_t couple_id);
  static id_t convertToId(QVariant variand); 

public:
  virtual std::optional<Person> getPersonById(id_t) const override;
  virtual std::optional<Couple> getCoupleById(id_t) const override;

  virtual std::pair<id_t, id_t> getPersonParentsById(id_t) const override;
  virtual std::vector<id_t> getPersonPartners(id_t ids) const override;

  virtual std::vector<id_t> getPersonChildren(id_t parent1_id) const override;
  
  virtual std::vector<id_t>
  getParentsChildren(id_t parent1, id_t parent2) const override;

  virtual id_t insertPerson(const Person &pers) override;
  virtual id_t addChild(const Person &person, id_t parent1,
                        id_t parent2 = 0) override;
  virtual id_t addPartner(const Person &person, id_t partner) override;
  // virtual void removePerson(id_t) override;
  virtual std::vector<Person> getPeople(int max_amount) const override;
  void dropData() override;

private:
  static const inline QString DB_DRIVER = "QSQLITE";
  static const inline std::string TEMP_FILENAME = ".mftbdb";
  QString db_filename;
};

} // namespace mftb
