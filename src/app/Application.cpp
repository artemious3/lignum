#include "Application.h"
#include "SqlDB.h"

Application::Application(){
	db = mftb::SqlDB::getInstance();	
	
}
