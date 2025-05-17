#include "GedcomWriter.h"
#include <fstream>
#include <string>
#include "FamilyTreeModel.h"
#include "qlocale.h"


GedcomWriter::GedcomWriter(const mftb::FamilyTreeModel* db) : db(db) {}


static void write_gedcom_line(std::ostream& ofs, const gedcom_line& line){
	ofs << line.level << " " << line.id << " " << line.tag << " " << line.line_value << '\n';
}


static void write_head(std::ostream& os) {
	os << "0 HEAD\n";
	os << "1 SOUR Lignum\n";
	os << "2 VERS 1.1\n";
	os << "1 SUBM @I1@\n";
	os << "1 GEDC \n";
	os << "2 VERS 5.0\n";
	os << "2 FORM LINEAGE-LINKED\n";

}

void GedcomWriter::write(const mftb::FamilyTreeModel *db, std::ostream &ofs) {

	auto persons_ids = db->getPeopleIds();
	auto couple_ids = db->getCoupleIds();


	write_head(ofs);

	for(id_t person_id : persons_ids) {
		auto person = db->getPersonById(person_id);
		
		write_gedcom_line(ofs, gedcom_line {
				.level = 0,
				.id = "@I"+std::to_string(person_id)+"@",
				.tag = "INDI",
				.line_value = ""
				});
		write_gedcom_line(ofs, gedcom_line {
				.level = 1,
				.id = "",
				.tag = "NAME",
				.line_value = person->fullName().toStdString()
				});
		write_gedcom_line(ofs, gedcom_line {
				.level = 1,
				.id = "",
				.tag = "SEX",
				.line_value = QString{person->gender}.toStdString()
				});


		auto parents_couple = db->getParentsCoupleId(person_id);
		if(parents_couple.has_value() && parents_couple != 0) {
			write_gedcom_line(ofs, gedcom_line {
					.level = 1,
					.id = "",
					.tag = "FAMC",
					.line_value = "@F"+std::to_string(*parents_couple)+"@"					});
		}


		auto couples = db->getPersonCouplesId(person_id);
		for(auto couple_id : couples) {
			write_gedcom_line(ofs, gedcom_line {
					.level = 1,
					.id = "",
					.tag = "FAMS",
					.line_value = "@F"+std::to_string(couple_id)+"@"
					});

		}
	}


	for(id_t couple_id : couple_ids) {
		auto couple = *db->getCoupleById(couple_id);

		write_gedcom_line(ofs, gedcom_line {
				.level = 0,
				.id = "@F"+std::to_string(couple_id)+"@",
				.tag = "FAM",
				.line_value = ""
				});

		if(couple.person1_id != 0) {
			write_gedcom_line(ofs, gedcom_line {
					.level = 1,
					.id = "",
					.tag = "HUSB",
					.line_value = "@I"+std::to_string(couple.person1_id)+"@"
					});
		}
		if(couple.person2_id != 0) {
			write_gedcom_line(ofs, gedcom_line {
					.level = 1,
					.id = "",
					.tag = "WIFE",
					.line_value = "@I"+std::to_string(couple.person2_id)+"@"
					});
		}


		auto childen = db->getCoupleChildren(couple_id);
		for(auto child_id : childen) {
			write_gedcom_line(ofs, gedcom_line {
					.level = 1,
					.id = "",
					.tag = "CHIL",
					.line_value = "@I"+std::to_string(child_id)+"@"
					});
		}


	}

	ofs << "0 TRLR";

}

