

#include "BiographiesModel.h"
#include <optional>
#include <string>
#include "bsoncxx/builder/basic/document.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/instance.hpp"
#include "mongocxx/uri-fwd.hpp"




using bsoncxx::v_noabi::builder::basic::make_document;
using bsoncxx::v_noabi::builder::basic::kvp;

	BiographiesModel::BiographiesModel(std::string db_url)
  :  connection(mongocxx::uri{db_url}, {}){

		this->db = connection["local"];
		if(!db.collection("biographies")){
			db.create_collection("biographies");
		}
	}

	std::optional<std::string> BiographiesModel::get_biography(db_id_t id){

		auto biographies = db.collection("biographies");

		auto doc = biographies.find_one(make_document(kvp("person_id", (std::int64_t)id)));
		if(!doc){
			return std::nullopt;
		}
		return std::string { doc.value()["bio"].get_string().value };
	}


	void BiographiesModel::set_biography(db_id_t id, std::string bio){
		auto biographies = db.collection("biographies");
		auto maybe_biography = biographies.find_one(make_document(kvp("person_id", (std::int64_t)id)));

		if(!maybe_biography){
			biographies.insert_one(make_document(kvp("person_id", (std::int64_t)id),
						                               kvp("bio", bio)));

		} else {
			biographies.update_one(make_document(kvp("person_id", (std::int64_t)id)),
															make_document( kvp("$set", 
																make_document(	kvp("bio", bio)))) );

		}
	}


	void BiographiesModel::drop(){
		db.drop();
	}



	BiographiesModel::~BiographiesModel(){

	}
