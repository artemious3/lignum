#include "GedcomWriter.h"
#include <fstream>
#include <string>
#include "FamilyTreeModel.h"
#include "qlocale.h"


GedcomWriter::GedcomWriter(const mftb::FamilyTreeModel* db) : db(db) {}


static void write_gedcom_line(std::ofstream& ofs, const gedcom_line& line){
	ofs << line.level << " " << line.id << " " << line.tag << " " << line.line_value << '\n';
}

void GedcomWriter::write(const mftb::FamilyTreeModel *db, std::ofstream &ofs) {

	auto persons_ids = db->getPeopleIds();
	auto couple_ids = db->getCoupleIds();


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

}

