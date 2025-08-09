#include "GedcomLoader.h"
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <format>
#include <iterator>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "entities.h"
#include "qcontainerfwd.h"
#include "qlocale.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include "tree-traversal.h"



static Person from_gedcom_individual(const Individual* ind) {
	Person person;
	if(ind->name.data != NULL) {
		if(ind->name.data->value.data != NULL) {
			person.first_name = QString{ind->name.data->value.data};
		}
	}
	switch(ind->sex) {
		case Gender::Male:
			person.gender = 'M';
			break;
		case Gender::Female:
			person.gender = 'F';
			break;
		default:
			person.gender = 'U';
	}
	return person;
}


GedcomLoaderException::GedcomLoaderException(const char* _msg) 
	: std::runtime_error(_msg)
{}

GedcomLoaderException::GedcomLoaderException(const std::string& _msg) 
	: std::runtime_error(_msg)
{}


// person_data::person_data(const Individual* gedcom_individual) : gedcom_data(gedcom_individual) {}






GedcomLoader::GedcomLoader(mftb::FamilyTreeModel* model) : db(model){}


std::string istream_read_all(std::istream &in)
{
    std::string ret;
    char buffer[4096];
    while (in.read(buffer, sizeof(buffer))){
        ret.append(buffer, sizeof(buffer));
    }
    ret.append(buffer, in.gcount());
    return ret;
}


void GedcomLoader::load(mftb::FamilyTreeModel* model, std::istream& is){
	std::string text = istream_read_all(is);


	char* err_str = (char*)malloc(256*sizeof(char));

	SPDLOG_DEBUG("I'm going to parse GEDCOM data");

	GedcomData * gedcom_data = parse(text.c_str(), err_str, 256);
	if(gedcom_data == NULL){
		std::string err_str_string{err_str};
		throw GedcomLoaderException(err_str_string);
	}

	free(err_str);

	SPDLOG_DEBUG("Performed GEDCOM parse successfully");

	// DBG_dump_gedcom_stats(gedcom_data);
	GedcomLoader loader(model);
	loader.traverse_gedcom_data(gedcom_data);

	model->setRenderData(RenderData{
			.center_couple = 1
			});
	if(gedcom_data != NULL) {
		free_parse(gedcom_data);
	}

}


void GedcomLoader::add_candidate_if_neccessary(std::string xref, const person_data& person_data) {
	if(!person_data.ancestors_processed || !person_data.descendants_processed) {
		canditates.push(xref);
	}
}


void GedcomLoader::process_ancestors(std::string gxref) {
	
	if(xref_to_person[gxref].ancestors_processed){
		return;
	}
	std::cout << "ANCESTORS " << gxref << '\n';

	auto get_parents = [&](std::string xref) -> std::vector<std::string>{
		auto &person = xref_to_person.at(xref);

		if(!person.parents_family.has_value()){
			return {};
		}

		const auto &parents_family = families[person.parents_family.value()];
		std::vector<std::string> parents_xrefs;
		if(parents_family.partner1.has_value()) {
			parents_xrefs.push_back(parents_family.partner1.value());
			std::cout << "p1 : " << parents_xrefs[0] << '\n';
		}
		if(parents_family.partner2.has_value()) {
			parents_xrefs.push_back(parents_family.partner2.value());
			std::cout << "p2 : " << parents_xrefs[1] << '\n';
		}
		return parents_xrefs;
	};


	auto add_parents = [&](std::string A_xref) {
		std::cout << "ancestors : adding " << A_xref << '\n';
		person_data* A = &xref_to_person.at(A_xref);
		assert(A->added_in_db);
		assert(A->db_id != 0);

		if(!A->parents_family.has_value()){
			return;
		}


		const auto A_family = families[A->parents_family.value()];

		if(auto A_parent1_maybe_xref = A_family.partner1;
			A_parent1_maybe_xref.has_value())	{

			person_data* A_parent1 = &xref_to_person.at(*A_parent1_maybe_xref);
			if(!A_parent1->added_in_db) {
				A_parent1->db_id = db->addParent(A->db_id, from_gedcom_individual(A_parent1->gedcom_data));
				A_parent1->added_in_db = true;
			}
			add_candidate_if_neccessary(*A_parent1_maybe_xref, *A_parent1);
		}
		if(auto A_parent2_maybe_xref = A_family.partner2;
			A_parent2_maybe_xref.has_value()) {

			person_data* A_parent2 = &xref_to_person.at(*A_parent2_maybe_xref);
			if(!A_parent2->added_in_db) {
				A_parent2->db_id = db->addParent(A->db_id, from_gedcom_individual(A_parent2->gedcom_data));
				A_parent2->added_in_db = true;
			}
			add_candidate_if_neccessary(*A_parent2_maybe_xref, *A_parent2);

		}
		A->ancestors_processed = true;
	};
	TreeTraversal<std::string>::breadth_first(gxref, get_parents, add_parents);
}

void GedcomLoader::process_descendants_and_self(std::string gxref) {
	if(xref_to_person[gxref].descendants_processed){
		return;
	}
	std::cout << "DESCENDANTS: " << gxref << '\n';

	auto get_descendants = [&](std::string xref) -> std::vector<std::string>{
		auto person = xref_to_person.at(xref);
	
		std::vector<std::string> children_xrefs;

		for(auto family_id : person.families) {
			const family_data& family = families.at(family_id);
			std::copy(family.children_xrefs.begin(),
				  family.children_xrefs.end(),
				  std::back_inserter(children_xrefs));
		}
		return children_xrefs;
	};

	auto add_partners_and_children_to_db = [&](std::string A_xref){
		// person A
		std::cout << "descendsnts : adding " << A_xref << '\n';
		person_data * A = &xref_to_person.at(A_xref);
		assert(A->added_in_db);
		assert(A->db_id != 0);
		for(auto family_id : A->families) {
			const family_data& family = families.at(family_id);

			std::optional<std::string> xref_A_partner;
			if(family.partner1 == A_xref) {
				xref_A_partner = family.partner2;
			} else if (family.partner2 == A_xref){
				xref_A_partner = family.partner1;
			} else {
				throw std::logic_error(std::format("Person has family_id {} but the family has no this person", family_id));
			}


			id_t A_partner_db_id;
			if(xref_A_partner.has_value()){

				person_data* A_partner = &xref_to_person[*xref_A_partner];


				if(!A_partner->added_in_db) {
					A_partner->db_id = db->addPartner(from_gedcom_individual(A_partner->gedcom_data), A->db_id);
					A_partner->added_in_db = true;
				}
				// partner could have other partners and descendants (and ancestors)
				// that's why it is to be processed same way further
				add_candidate_if_neccessary(*xref_A_partner, *A_partner);

				A_partner_db_id = A_partner->db_id;
			} else {
				A_partner_db_id = 0;
			}

			for(const auto& A_child_xref : family.children_xrefs) {

				person_data* A_child = &xref_to_person.at(A_child_xref);
				if(!A_child->added_in_db){
					A_child->db_id = db->addChild(from_gedcom_individual(A_child->gedcom_data), 
								      A->db_id, A_partner_db_id);
					A_child->added_in_db = true;
				}

			}
		}

		A->descendants_processed = true;
	};


	TreeTraversal<std::string>::breadth_first(gxref, get_descendants, add_partners_and_children_to_db);
}

/*
 * Add any person from parsed gedcom into family tree. 
 *
 * @return : xref of inserted person
 */
std::string add_first_person_into_db(mftb::FamilyTreeModel* db, std::unordered_map<std::string, person_data>& persons) {
	auto beg = persons.begin();
	if(beg == persons.end()){
		throw GedcomLoaderException("The tree is empty");
	}
	person_data* person = &beg->second;
	person->db_id = db->insertPerson(from_gedcom_individual(person->gedcom_data));
	person->added_in_db = true;
	return beg->first;
}

void GedcomLoader::traverse_gedcom_data(const GedcomData* ged){
	parse_into_internal_representation(ged);

	std::string first_xref = add_first_person_into_db(db, xref_to_person);

	process_descendants_and_self(first_xref);
	process_ancestors(first_xref);

	while(!canditates.empty()) {
		std::string candidate = canditates.front();
		canditates.pop();
		process_descendants_and_self(candidate);
		process_ancestors(candidate);

	}
}



/*
 * Parse GedcomData into temporary internal representation, that has all connections,
 * neccessary to load the tree into database
 */
void GedcomLoader::parse_into_internal_representation(const GedcomData* data){

	for(int i = 0; i < data->individuals.len; ++i){
		Individual * individual = &data->individuals.data[i];
		MaybeString mb_xref = individual->xref;
		if(mb_xref.data != NULL){
			std::string xref (mb_xref.data);
			xref_to_person.insert_or_assign(xref, person_data{individual});
			std::cout << "INSRTED person name is : " << individual->name.data->value.data << '\n';
			std::cout << "INSERTED person xref is : " << xref << '\n';
		}
	}


	families.resize(data->families.len);
	for(std::size_t fam_id = 0; fam_id < data->families.len; ++fam_id){
		const Family* gedcom_fam = &data->families.data[fam_id];
		family_data internal_fam;

		internal_fam.gedcom_data = gedcom_fam;
		internal_fam.children_xrefs.resize(gedcom_fam->children.len);
		for(int j = 0; j < gedcom_fam->children.len; ++j){
			const char* child_xref = gedcom_fam->children.data[j];
			if(std::string{child_xref} == "@I13002836@"){
				std::cout << "found";;
			}
			xref_to_person.at(child_xref).parents_family = fam_id;
			internal_fam.children_xrefs[j] = std::string{child_xref};
		}
		if(gedcom_fam->individual1.data != NULL) {
			auto xref =  std::string{gedcom_fam->individual1.data};
			internal_fam.partner1 = xref;
			xref_to_person.at(xref).families.push_back(fam_id);
		}
		if(gedcom_fam->individual2.data != NULL) {
			auto xref =  std::string{gedcom_fam->individual2.data};
			internal_fam.partner2 = xref;
			xref_to_person.at(xref).families.push_back(fam_id);
		}
		families[fam_id] = internal_fam;
	}


	DBG_dump();
}






void GedcomLoader::DBG_dump_gedcom_stats(GedcomData* data){
	SPDLOG_DEBUG("Dumping GEDCOM data...");
	SPDLOG_DEBUG("--------INDIVIDUALS--------");
	SPDLOG_DEBUG("Parsed {} individuals.", data->individuals.len);
	SPDLOG_DEBUG("----------FAMILIES--------");
	SPDLOG_DEBUG("Parsed {} families.", data->families.len);
}


void GedcomLoader::DBG_dump() {
	SPDLOG_DEBUG("Family dump");
	SPDLOG_DEBUG("\n");
	for(int i = 0; i < families.size(); ++i){
		auto fam = families[i];
		SPDLOG_DEBUG("ID {}", i);
		SPDLOG_DEBUG("P1 {}", fam.partner1.value_or(""));
		SPDLOG_DEBUG("P2 {}", fam.partner2.value_or(""));
		for(const auto& ch : fam.children_xrefs) {
			SPDLOG_DEBUG("C {}", ch);
		}
		SPDLOG_DEBUG("\n");
	}


	SPDLOG_DEBUG("Person dump");
	SPDLOG_DEBUG("\n");
	for(const auto& per : xref_to_person){
		SPDLOG_DEBUG("xref : {}", per.first);
		SPDLOG_DEBUG("Parents family {}", per.second.parents_family.value_or(-1));
		SPDLOG_DEBUG("\n");
	}
}




