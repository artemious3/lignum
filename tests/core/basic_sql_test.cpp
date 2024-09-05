#include "DB.h"
#include "SqlDB.h"
#include <QtWidgets/qapplication.h>
#include <algorithm>
#include <gtest/gtest.h>
#include <qassert.h>
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <utility>

using namespace mftb;

template <typename T> bool contains(std::vector<T> container, T val) {
  return std::ranges::find(container, val) != container.end();
}

TEST(SqlDb, InsertAndGet) {
  DB *db = mftb::SqlDB::getInstance();

  Person inserted_persons[] = {
      {'M', "John", "", "Doe", QDate(1970, 1, 1), QDate(2070, 1, 1)},
      {'F', "Anna", "", "Doe", QDate(1970, 1, 1), QDate(2060, 1, 1)},
      {'U', "Phillip", "John", "Doe", QDate(1995, 1, 1), QDate(2075, 1, 1)}};

  for (const auto &person : inserted_persons) {
    db->insertPerson(person);
  }

  auto get_persons = db->getPeople();
  ASSERT_EQ(get_persons.size(), std::size(inserted_persons));
  for (int i = 0; i < std::size(inserted_persons); ++i) {
    ASSERT_EQ(get_persons[i], inserted_persons[i]);
  }
  db->dropData();
}

TEST(SqlDb, GetAllIds) {
  auto db = SqlDB::getInstance();

  auto father = Person{'M', "A", "", "X"};
  auto mother1 = Person{'F', "B", "", "X"};
  auto mother2 = Person{'F', "D", "", "Y"};
  auto child1 = Person{'M', "C", "", "X"};
  auto child2 = Person{'M', "C", "", "Y"};

  const auto f_id = db->insertPerson(father);
  const auto m1_id = db->insertPerson(mother1);
  const auto m2_id = db->insertPerson(mother2);
  const auto ch1 = db->addChild(child1, f_id, m1_id);
  const auto ch2 = db->addChild(child2, f_id, m2_id);

  auto all_ids = db->getPeopleIds();

  ASSERT_EQ(all_ids.size(), 5);
  ASSERT_TRUE(contains(all_ids, f_id));
  ASSERT_TRUE(contains(all_ids, m1_id));
  ASSERT_TRUE(contains(all_ids, m2_id));
  ASSERT_TRUE(contains(all_ids, ch1));
  ASSERT_TRUE(contains(all_ids, ch2));
}
TEST(SqlDb, ReturnValidId) {

  DB *db = mftb::SqlDB::getInstance();
  Person person = {
      'M', "John", "", "Doe", QDate(1970, 1, 1), QDate(2070, 1, 1)};
  auto id = db->insertPerson(person);

  auto get_person_opt = db->getPersonById(id);
  ASSERT_TRUE(get_person_opt.has_value());
  ASSERT_EQ(get_person_opt.value(), person);
  db->dropData();
}

TEST(SqlDb, AddChildrenAndGetParents) {

  DB *db = mftb::SqlDB::getInstance();

  Person parent = {'M', "A", "", "X", QDate(1970, 1, 1), QDate(2070, 1, 1)};
  Person mother = {'F', "B", "", "X", QDate(1970, 1, 1), QDate(2070, 1, 1)};
  Person daughter = {'F', "C", "", "X", QDate(2000, 1, 1), QDate()};
  Person son = {'M', "D", "", "X", QDate(2005, 1, 1), QDate()};

  auto father_id = db->insertPerson(parent);
  auto mother_id = db->insertPerson(mother);
  auto son_id = db->addChild(son, father_id, mother_id);
  auto daughter_id = db->addChild(daughter, father_id, mother_id);

  ASSERT_EQ(db->getPersonParentsById(son_id),
            std::make_pair(father_id, mother_id));
  ASSERT_EQ(db->getPersonParentsById(daughter_id),
            std::make_pair(father_id, mother_id));
  db->dropData();
}

TEST(SqlDb, AddPartnersAndGetPartners) {
  auto *db = mftb::SqlDB::getInstance();

  Person main = {'M', "A", "", "X", QDate(), QDate()};
  Person partner1 = {'F', "B", "", "X", QDate(), QDate()};
  Person partner2 = {'M', "C", "", "X", QDate(), QDate()};
  Person partner3 = {'F', "C", "", "X", QDate(), QDate()};

  auto main_id = db->insertPerson(main);
  auto p1_id = db->addPartner(partner1, main_id);
  auto p2_id = db->addPartner(partner2, main_id);
  auto p3_id = db->addPartner(partner3, p2_id);

  auto main_partners = db->getPersonPartners(main_id);
  ASSERT_EQ(main_partners.size(), 2);
  ASSERT_TRUE(contains(main_partners, p1_id));
  ASSERT_TRUE(contains(main_partners, p2_id));

  auto p1_partners = db->getPersonPartners(p1_id);
  ASSERT_EQ(p1_partners.size(), 1);
  ASSERT_TRUE(contains(p1_partners, main_id));

  auto p2_partners = db->getPersonPartners(p2_id);
  ASSERT_EQ(p2_partners.size(), 2);
  ASSERT_TRUE(contains(p2_partners, main_id));
  ASSERT_TRUE(contains(p2_partners, p3_id));

  auto p3_partners = db->getPersonPartners(p3_id);
  ASSERT_EQ(p3_partners.size(), 1);
  ASSERT_TRUE(contains(p3_partners, p2_id));

  db->dropData();
}

TEST(SqlDb, AddChildrenAndGetPartners) {

  auto db = SqlDB::getInstance();

  auto father = Person{'M', "A", "", "X"};
  auto mother1 = Person{'F', "B", "", "X"};
  auto mother2 = Person{'F', "D", "", "Y"};
  auto child1 = Person{'M', "C", "", "X"};
  auto child2 = Person{'M', "C", "", "Y"};

  auto f_id = db->insertPerson(father);
  auto m1_id = db->insertPerson(mother1);
  auto m2_id = db->insertPerson(mother2);

  db->addChild(child1, f_id, m1_id);
  db->addChild(child2, f_id, m2_id);

  auto f_partners = db->getPersonPartners(f_id);
  ASSERT_EQ(f_partners.size(), 2);
  ASSERT_TRUE(contains(f_partners, m1_id));
  ASSERT_TRUE(contains(f_partners, m2_id));

  auto m1_partners = db->getPersonPartners(m1_id);
  ASSERT_EQ(m1_partners.size(), 1);
  ASSERT_TRUE(contains(m1_partners, f_id));

  auto m2_partners = db->getPersonPartners(m2_id);
  ASSERT_EQ(m2_partners.size(), 1);
  ASSERT_TRUE(contains(m2_partners, f_id));

  db->dropData();
}

TEST(SqlDb, AddChildrenAndGetChildren) {

  auto db = SqlDB::getInstance();

  auto father = Person{'M', "A", "", "X"};
  auto mother1 = Person{'F', "B", "", "X"};
  auto mother2 = Person{'F', "D", "", "Y"};
  auto child1 = Person{'M', "C", "", "X"};
  auto child2 = Person{'M', "C", "", "Y"};

  const auto f_id = db->insertPerson(father);
  const auto m1_id = db->insertPerson(mother1);
  const auto m2_id = db->insertPerson(mother2);

  const auto ch1 = db->addChild(child1, f_id, m1_id);
  const auto ch2 = db->addChild(child2, f_id, m2_id);

  auto f_children = db->getPersonChildren(f_id);
  ASSERT_EQ(f_children.size(), 2);
  ASSERT_TRUE(contains(f_children, ch1));
  ASSERT_TRUE(contains(f_children, ch2));

  auto m1_children = db->getPersonChildren(m1_id);
  ASSERT_EQ(m1_children.size(), 1);
  ASSERT_TRUE(contains(m1_children, ch1));

  auto m2_children = db->getPersonChildren(m2_id);
  ASSERT_EQ(m2_children.size(), 1);
  ASSERT_TRUE(contains(m2_children, ch2));

  db->dropData();
}

TEST(SqlDb, AddChildrenAndGetChildren2) {

  auto db = SqlDB::getInstance();

  auto father = Person{'M', "A", "", "X"};
  auto mother1 = Person{'F', "B", "", "X"};
  auto mother2 = Person{'F', "D", "", "Y"};
  auto child1 = Person{'M', "C", "", "X"};
  auto child2 = Person{'M', "C", "", "Y"};

  const auto f_id = db->insertPerson(father);
  const auto m1_id = db->insertPerson(mother1);
  const auto m2_id = db->insertPerson(mother2);

  const auto ch1 = db->addChild(child1, f_id, m1_id);
  const auto ch2 = db->addChild(child2, f_id, m2_id);

  auto children1 = db->getParentsChildren(f_id, m1_id);
  ASSERT_EQ(children1.size(), 1);
  ASSERT_TRUE(contains(children1, ch1));

  auto children2 = db->getParentsChildren(f_id, m2_id);
  ASSERT_EQ(children2.size(), 1);
  ASSERT_TRUE(contains(children2, ch2));

  db->dropData();
}

TEST(SqlDb, AddChildrenAndGetChildrenByCouple) {

  auto db = SqlDB::getInstance();

  auto father = Person{'M', "A", "", "X"};
  auto mother1 = Person{'F', "B", "", "X"};
  auto mother2 = Person{'F', "D", "", "Y"};
  auto child1 = Person{'M', "C", "", "X"};
  auto child2 = Person{'M', "C", "", "X"};
  auto child3 = Person{'F', "C", "", "X"};

  const auto f_id = db->insertPerson(father);
  const auto m1_id = db->insertPerson(mother1);
  const auto m2_id = db->insertPerson(mother2);

  const auto ch1 = db->addChild(child1, f_id, m1_id);
  const auto ch2 = db->addChild(child2, f_id, m2_id);
  const auto ch3 = db->addChild(child3, f_id, m2_id);

  auto couple1 = db->getCoupleIdByPersons(f_id, m1_id).value();
  auto couple2 = db->getCoupleIdByPersons(f_id, m2_id).value();

  auto children1 = db->getCoupleChildren(couple1);
  ASSERT_EQ(children1.size(), 1);
  ASSERT_TRUE(contains(children1, ch1));

  auto children2 = db->getCoupleChildren(couple2);
  ASSERT_EQ(children2.size(), 2);
  ASSERT_TRUE(contains(children2, ch2));
  ASSERT_TRUE(contains(children2, ch3));

  db->dropData();
}

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}