#include <string>
#include <optional>
#include "mongocxx/client.hpp"
#include "mongocxx/database-fwd.hpp"
#include "mongocxx/instance.hpp"

using db_id_t = unsigned long;

class BiographiesModel {

public:
	//const std::string db_url = "mongodb://127.0.0.1:27017";
	BiographiesModel(std::string db_url);
	~BiographiesModel();

	std::optional<std::string> get_biography(db_id_t id);
	void set_biography(db_id_t id, std::string bio);
	void drop();

private:
	mongocxx::database db;
	// mongocxx::instance inst;
	mongocxx::client connection ;
};
