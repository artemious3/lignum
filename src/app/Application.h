

#include <QApplication>
#include "DB.h"
#include "Config.h"

class Application {

		
	public:
		Application(const Application&) = delete;
		void operator=(const Application&) = delete;

		static Application& GetInstance();


		mftb::DB* DB();
		


	private:
		Application();

		mftb::DB* db;
		Config config;

};
