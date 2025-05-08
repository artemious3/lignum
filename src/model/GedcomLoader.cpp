#include "GedcomLoader.h"
#include <cstddef>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include "spdlog/spdlog.h"



GedcomLoaderException::GedcomLoaderException(const char* _msg) 
	: std::runtime_error(_msg)
{}




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


void GedcomLoader::load(mftb::FamilyTreeModel* db, std::istream& is){
	std::string text = istream_read_all(is);

	SPDLOG_DEBUG("I'm going to parse GEDCOM data");

	GedcomData * gedcom_data = parse(text.c_str());
	if(gedcom_data == NULL){
		throw GedcomLoaderException("");
	}

	SPDLOG_DEBUG("Performed GEDCOM parse successfully");

	DBG_dump_gedcom_stats(gedcom_data);
	// traverse_gedcom_data(db, gedcom_data);

	if(gedcom_data != NULL) {
		free_parse(gedcom_data);
	}

}

void GedcomLoader::DBG_dump_gedcom_stats(GedcomData* data){
	SPDLOG_DEBUG("Dumping GEDCOM data...");
	SPDLOG_DEBUG("--------INDIVIDUALS--------");
	SPDLOG_DEBUG("Parsed {} individuals.", data->individuals.len);
	SPDLOG_DEBUG("----------FAMILIES--------");
	SPDLOG_DEBUG("Parsed {} families.", data->families.len);
}




void GedcomLoader::traverse_gedcom_data(mftb::FamilyTreeModel* db, GedcomData* ged){
	SPDLOG_DEBUG("");
}
