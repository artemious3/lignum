#include <entities.h>
#include <FamilyTreeModel.h>
#include <fstream>

struct gedcom_line {
	int level;
	std::string id;
	std::string tag;
	std::string line_value;
};

class GedcomWriter{
public:
	static void write(const mftb::FamilyTreeModel * db, std::ostream& ofs);

private:
	GedcomWriter(const mftb::FamilyTreeModel* db);

	const mftb::FamilyTreeModel* db;


};
