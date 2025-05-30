
#include <gtest/gtest.h>
#include <optional>
#include "BiographiesModel.h"

const std::string db_url = "mongodb://127.0.0.1:27017";

TEST(bio, drop_test){
	BiographiesModel db(db_url);
	db.set_biography(1, "BIO1");
	db.drop();
	ASSERT_EQ(db.get_biography(1), std::nullopt);
}

TEST(bio, create_and_get){
	BiographiesModel db(db_url);

	db.set_biography(1, "BIO1");
	db.set_biography(2, "BIO2");
	db.set_biography(312, "BIO312");


	ASSERT_EQ(db.get_biography(1), "BIO1");
	ASSERT_EQ(db.get_biography(2), "BIO2");
	ASSERT_EQ(db.get_biography(312), "BIO312");
	db.drop();
}


TEST(bio, get_nullopt){
	BiographiesModel db(db_url);
	ASSERT_EQ(db.get_biography(0), std::nullopt);
	ASSERT_EQ(db.get_biography(1), std::nullopt);
	ASSERT_EQ(db.get_biography(312), std::nullopt);
	db.drop();
}


TEST(bio, create_and_update){
	BiographiesModel db(db_url);
	db.set_biography(1, "BIO1 old");
	ASSERT_EQ(db.get_biography(1), "BIO1 old");
	db.set_biography(1, "BIO1 new");
	ASSERT_EQ(db.get_biography(1), "BIO1 new");
	db.drop();
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
