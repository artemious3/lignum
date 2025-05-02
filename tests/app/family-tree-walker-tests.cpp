#include "FamilyTreeSqlModel.h"
#include "family-tree-builder.h"
#include "family-tree-view.h"
#include <gtest/gtest.h>
#include <memory>

using namespace mftb;


class FamilyTreeWalkerTest : public ::testing::Test {
protected:
  void SetUp() override {
   
  }

  FamilyTreeWalkerTest() {

    for (int i = 0; i < 7; ++i) {
      trees[i] = new FamilyTreeView;
    }

    //
    //            1    0    2
    //            |____|____|
    //              |     |
    //            3---4   5   6
    //                    |___|
    //

    DB *db = mftb::SqlDB::getInstance();

    p[0] = db->insertPerson({'M', "0"});
    p[1] = db->addPartner({'F', "1"}, p[0]);
    p[2] = db->addPartner({'F', "2"}, p[0]);
    p[3] = db->addChild({'M', "3"}, p[0], p[1]);
    p[4] = db->addChild({'M', "4"}, p[0], p[1]);
    p[5] = db->addChild({'F', "5"}, p[0], p[2]);
    p[6] = db->addPartner({'M', "6"}, p[5]);

    std::vector<FamilyTreeBuilder> walkers;
    for (int i = 0; i < 7; ++i) {
      qDebug() << "creating walker for tree " << i;
      walkers.emplace_back(trees[i], db);
    }

    for (int i = 0; i < 7; ++i) {
      qDebug() << "processing tree " << i;
      walkers[i].build_tree_from(p[i]);
      qDebug() << "processed tree " << i;
    }
  }

  ~FamilyTreeWalkerTest() {
    // for (int i = 0; i < 8; ++i) {
    //   delete trees[i];
    // }
  }

  id_t p[7];
  FamilyTreeView *trees[7];
};

TEST_F(FamilyTreeWalkerTest, Basic) {

  const auto db = SqlDB::getInstance();

  auto couple10 = db->getCoupleIdByPersons(p[1], p[0]).value();
  auto couple20 = db->getCoupleIdByPersons(p[2], p[0]).value();
  auto couple56 = db->getCoupleIdByPersons(p[5], p[6]).value();

  for (int i = 0; i < 7; ++i) {
    auto tree = trees[i];
    auto family10 = tree->getFamily(couple10);
    auto family20 = tree->getFamily(couple20);
    auto family56 = tree->getFamily(couple56);

    ASSERT_TRUE(family10 != nullptr && family20 != nullptr &&
                family56 != nullptr);

    ASSERT_EQ(family10->getChildren().size(), 2);
    ASSERT_TRUE(family10->hasChild(tree->getPerson(p[3])));
    ASSERT_TRUE(family10->hasChild(tree->getPerson(p[4])));

    ASSERT_EQ(family20->getChildren().size(), 1);
    ASSERT_TRUE(family20->hasChild(tree->getPerson(p[5])));

    ASSERT_EQ(family56->getChildren().size(), 0);

    std::cout << i << " passed\n";
  }

  db->dropData();
}
