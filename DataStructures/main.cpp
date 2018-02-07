#include <iostream>
#include <ctime>

#include "myDatabase.h"
#include "student.h"

#include "names.h"
void printDbFile(std::string dbFile);

int main(){
	createDatabase("studata.sdb");
	int studentsNum = 100;	
	Student* aStudent;
	int previousAM = 0;
	for(int i=0; i<studentsNum; i++){
		getRandomStudent(aStudent, previousAM);
		insertInDatabase(*aStudent, "studata.sdb");
		delete aStudent;
	}
	//printDbFile("studata.sdb");

	createPrimaryIndex("studata.sdb", "indexAM.idx");
	createSecondaryIndex("studata.sdb", "indexFN.idx", ATTR_FIRST_NAME);
	
	readFromDatabase( findRange("0000", "0100" , "indexAM.idx") , "studata.sdb");
	readFromDatabase( find("Dimitris", "indexFN.idx") , "studata.sdb");
	
	std::cin.get();
    return 0;
}

void printDbFile(std::string dbFile){
	static Student* newStudent = new Student();
	std::ifstream dbStream;	
    dbStream.open(dbFile.c_str(), std::ios::binary);
    if( !dbStream ){
        std::cerr << "Database file "<< dbFile << " does not exist" << std::endl;
        return;
    }
	if( !dbStream.is_open()){
		std::cerr << "Error opening database file" << std::endl;
		return;
	}	
	dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	while(!dbStream.eof()){		
		std::cout << newStudent->getInfo() << std::endl;
		dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	}  	
	dbStream.close();
    return;
}