#include <ostream>
#include <stdexcept>
#include "Gedcom.h"
#include "FamilyTreeModel.h"



class GedcomLoader {

public:
	static void load(mftb::FamilyTreeModel* db, std::istream& reader);

private: 
	static void traverse_gedcom_data(mftb::FamilyTreeModel* db, GedcomData* ged);


	static void DBG_dump_gedcom_stats(GedcomData* ged);

};

class GedcomLoaderException : public std::runtime_error {
	public:
	GedcomLoaderException(const char* _msg);
};
