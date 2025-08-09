#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include "Gedcom.h"
#include "FamilyTreeModel.h"
#include "qnamespace.h"



using fam_id_t = std::size_t;

struct family_data {
	const Family* gedcom_data;
	std::optional<std::string> partner1;
	std::optional<std::string> partner2;
	std::vector<std::string> children_xrefs;
};

struct person_data {
	const Individual * gedcom_data;
	std::vector<fam_id_t> families;
	std::optional<fam_id_t> parents_family;

	bool descendants_processed = false;
	bool ancestors_processed = false;
	bool added_in_db = false;
	id_t db_id = 0;


	// person_data(const Individual* gedcom_individual);
};


class GedcomLoader {

public:
	static void load(mftb::FamilyTreeModel* model, std::istream& reader);

private: 
	GedcomLoader(mftb::FamilyTreeModel* db);
	void parse_into_internal_representation(const GedcomData* data);
	void add_candidate_if_neccessary(std::string xref, const person_data& person_data);

	void process_descendants_and_self(std::string xref);
	void process_ancestors(std::string xref);


	// void addPartner(const Individual*, id_t db_id);

	void traverse_gedcom_data(const GedcomData* ged);

	void DBG_dump_gedcom_stats(GedcomData* ged);
	void DBG_dump();


	mftb::FamilyTreeModel* db;

	std::unordered_map<std::string, person_data> xref_to_person;
	// std::unordered_set<std::string> unprocessed_persons;
	std::vector<family_data> families;

	std::queue<std::string> canditates;
};

class GedcomLoaderException : public std::runtime_error {
	public:
	GedcomLoaderException(const char* _msg);
  GedcomLoaderException(const std::string& _msg);
};
