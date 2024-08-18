#include <QDate>
#include <QString>
#include <cstdint>
typedef uint32_t id_t;

struct Person {
  QChar gender;

  QString first_name;
  QString middle_name;
  QString last_name;

  QDate birth_date;
  QDate death_date;

  bool operator==(const Person& pers) const = default;
  //   return gender == pers.gender &&
  //   first_name == pers.first_name &&
  //   middle_name == pers.middle_name &&
  //   last_name == pers.last_name &&
  //   birth_date == pers.birth_date &&
  //   death_date == pers.death_date;
  // }
};

struct Couple {
  id_t person1_id;
  id_t person2_id;
  QDate start_date;
};

struct IdentifiedObject {
  id_t id_;
};

class CouplePrivate : public Couple, public IdentifiedObject {};

class PersonPrivate : public Person, public IdentifiedObject {
public:
  id_t parents_couple_id_;
};
