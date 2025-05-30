
#include "mongocxx/client-fwd.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/database-fwd.hpp"
#include "mongocxx/instance.hpp"
#include "mongocxx/uri-fwd.hpp"
#include "BiographiesModel.h"

int main(){
	const std::string db_url = "mongodb://127.0.0.1:27017";


	BiographiesModel model {db_url};
	model.set_biography(1, "sample bio");
	auto bio = model.get_biography(1);
	if(bio){
		std::cout << *bio;
	} else {
		std::cout << "no bio!";
	}

	model.set_biography(1, "sample bio new");
	bio = model.get_biography(1);
	if(bio){
		std::cout << *bio;
	} else {
		std::cout << "no bio!";
	}



}
