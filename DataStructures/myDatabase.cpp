#include "myDatabase.h"

using std::ios;

// Function that creates the needed BTreeHandler *btree according to the other arguments while
// checking for errors regarding opening files and index file endings.
bool createTheBTree(BTreeHandler* &btree, std::fstream &idxStream, std::fstream &uidxStream, std::string indexFile, bool creatingTree, int attribute=ATTR_AM);
bool correctExtension(std::string file, char* extension);

bool createDatabase(std::string newDbFile){
	std::fstream dbStream;
	std::stringstream tempStream;
	tempStream << newDbFile;
	if( !correctExtension(newDbFile, ".sdb")){
		tempStream << ".sdb";
	}
    dbStream.open( tempStream.str().c_str(), ios::in| ios::out| ios::trunc );
    if( dbStream.is_open() ){
		dbStream.close();
        return true;
    }else{
        std::cerr << "Error creating database file" << std::endl;
       return false;
    }	
}
StudentInfo insertInDatabase(Student newStudent, std::string dbFile){
    std::fstream dbStream;
	StudentInfo addedRecord;
	strcpy(addedRecord.theKey, "-1");
	addedRecord.thePosition = -1;

	if( !correctExtension(dbFile, ".sdb")){
		std::cerr << "File format not recognized:" << std::endl;
		std::cerr << "Dabatase file extension must be .sdb" << std::endl; 
        return addedRecord;	
	}
	dbStream.open(dbFile.c_str(),ios::in);
    if( !dbStream ){
        std::cerr << "Database file "<< dbFile << " does not exist" << std::endl;
        return addedRecord;		
    }
    dbStream.close();
    dbStream.open(dbFile.c_str(),ios::out|ios::binary|ios::app);
    if( dbStream.is_open() ){
		dbStream.write( (char*)&newStudent, sizeof(newStudent));
		size_t filep = dbStream.tellp();
		int studentOffset = (filep - sizeof(newStudent) ) / (sizeof(newStudent)) ;
		std::cout << "Done: Record Insertion with primary key AM = " << newStudent.getAM();
        std::cout  << " into database file " << dbFile << std::endl;
        
		strcpy(addedRecord.theKey, newStudent.getAM());
		addedRecord.thePosition = studentOffset;

		dbStream.close();
        return addedRecord;
    }else{
        std::cerr << "Error opening database file" << std::endl;
        return addedRecord;
    }

}
bool createPrimaryIndex(std::string dbFile, std::string newIndexFile){
	std::fstream indexStream;
	std::fstream utilityIndexStream;
	BTreeHandler* btree;
	bool btreeCreated;
	static Student* newStudent = new Student();
	StudentInfo newStudentRecord;
    std::ifstream dbStream;
	int studentNo = 0;

	if( !correctExtension(dbFile, ".sdb")){
		std::cerr << "File format not recognized:" << std::endl;
		std::cerr << "Dabatase file extension must be .sdb" << std::endl; 
        return false;	
	}
    dbStream.open(dbFile.c_str(), ios::binary);
    if( !dbStream ){
        std::cerr << "Database file "<< dbFile << " does not exist" << std::endl;
        return false;
    }
	if( !dbStream.is_open()){
		std::cerr << "Error opening database file" << std::endl;
		return false;
	}
	btreeCreated = createTheBTree(btree, indexStream, utilityIndexStream, newIndexFile, true, ATTR_AM);
	if(btreeCreated == false){
		return false;
	}
	dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	while(!dbStream.eof()){
		strcpy(newStudentRecord.theKey, newStudent->getAM());  
		newStudentRecord.thePosition = studentNo;
		btree->insert(newStudentRecord);
		studentNo++;
		dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	}  	
	delete btree;

	indexStream.close();
	utilityIndexStream.close();
	dbStream.close();
    return true;
}
bool createSecondaryIndex(std::string dbFile, std::string newIndexFile, int attribute){
	std::fstream indexStream;
	std::fstream utilityIndexStream;
	BTreeHandler* btree;
	bool btreeCreated;
	static Student* newStudent = new Student();
	StudentInfo newStudentRecord;  
	std::ifstream dbStream;
	int studentNo = 0;

	if( !correctExtension(dbFile, ".sdb")){
		std::cerr << "File format not recognized:" << std::endl;
		std::cerr << "Dabatase file extension must be .sdb" << std::endl; 
        return false;	
	}
	dbStream.open(dbFile.c_str(), ios::binary);
    if( !dbStream ){
        std::cerr << "Database file "<< dbFile << " does not exist" << std::endl;
        return false;
    }
	if( !dbStream.is_open()){
		std::cerr << "Error opening database file" << std::endl;
		return false;
	}
	btreeCreated = createTheBTree(btree, indexStream, utilityIndexStream, newIndexFile, true, attribute);
	if(btreeCreated == false){
		return false;
	}

	dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	while(!dbStream.eof()){
		strcpy(newStudentRecord.theKey, newStudent->getAttribute(attribute)); 
		newStudentRecord.thePosition = studentNo;
		newStudentRecord.theNumber = 1;		
		btree->insert(newStudentRecord);		
		studentNo++;
		dbStream.read(reinterpret_cast <char *> (newStudent), sizeof(*newStudent));
	}  	
	delete btree;
	indexStream.close();
	dbStream.close();		
	return true; 
}
bool insert(StudentInfo newStudentRecord, std::string indexFile){
	BTreeHandler* btree;
	bool btreeCreated;
	bool recordInserted;
    std::fstream indexStream;	
	std::fstream utilityIndexStream;	

	btreeCreated = createTheBTree(btree, indexStream, utilityIndexStream, indexFile, false);
	if(btreeCreated == false){
		return true;
	}		
	recordInserted = btree->insert(newStudentRecord);	
	delete btree;
	indexStream.close();
	utilityIndexStream.close();
	return recordInserted;   
}	
std::list<StudentInfo> find(char* key, std::string indexFile){
	std::list<StudentInfo> studentsFound;
	BTreeHandler* btree;
	bool btreeCreated;
    std::fstream indexStream;
	std::fstream utilityIndexStream;
	
	btreeCreated = createTheBTree(btree, indexStream, utilityIndexStream, indexFile, false);
	if(btreeCreated == false){
		return studentsFound;
	}	
	studentsFound = btree->find(key);
	delete btree;
	indexStream.close();
	utilityIndexStream.close();
	if(!studentsFound.empty()){
		std::cout << "Record/s with key " <<  key;
		std::cout << " has/ve been found" << std::endl;
	}else{
		std::cout << "There is no record with key " << key;
		std::cout << " in the index " << indexFile << std::endl;
	}
	return studentsFound;
	
}
std::list<StudentInfo> findRange(char* key1, char* key2, std::string indexFile){
	std::list<StudentInfo> studentsFound;
	BTreeHandler* btree;
	bool btreeCreated;
    std::fstream indexStream;
	std::fstream utilityIndexStream;
	
	btreeCreated = createTheBTree(btree, indexStream, utilityIndexStream, indexFile, false);
	if(btreeCreated == false){
		return studentsFound;
	}	
	btree->searchSubTree(0,key1,key2,studentsFound);
	delete btree;
	indexStream.close();
	utilityIndexStream.close();
	if(!studentsFound.empty()){
		std::cout <<"Records in the space ["<<key1<<","<<key2; 
		std::cout <<"] have been found" << std::endl;
	}else{
		std::cout << "There is no record with key in the space [" ;
		std::cout << key1 << "," << key2 <<"]" ;
		std::cout << " in the index " << indexFile << std::endl;
	}
	return studentsFound;	
}

void readFromDatabase(std::list<StudentInfo> students, std::string dbFile){
	std::ifstream dbStream;
	Student student;
	if( !correctExtension(dbFile, ".sdb")){
		std::cerr << "File format not recognized:" << std::endl;
		std::cerr << "Dabatase file extension must be .sdb" << std::endl; 
        return;	
	}
	dbStream.open(dbFile.c_str(), ios::in|ios::binary);
    if( !dbStream ){
        std::cerr << "Database file "<< dbFile << " does not exist" << std::endl;
        return;		
    }
	if( dbStream.is_open() ){
		std::list<StudentInfo>::iterator i;
		for(i=students.begin(); i!=students.end();i++){
		dbStream.seekg( ((*i).thePosition) * sizeof(student),std::ios::beg);
		dbStream.read(reinterpret_cast <char *> (&student), sizeof(student));
		std::cout << student.getInfo() << std::endl;
		}
    }else{
        std::cerr << "Error opening database file" << std::endl;
    }  
}
void readFromDatabase(StudentInfo studentRecord, std::string dbFile){
	std::list<StudentInfo> students;
	students.push_back(studentRecord);
	readFromDatabase(students, dbFile);
}


bool createTheBTree(BTreeHandler* &btree, std::fstream &idxStream, std::fstream &uidxStream, std::string indexFile, bool creatingTree, int attribute){
	std::string uIndexFile;
	std::stringstream tempStream;
						
	if(creatingTree == true){		//Create the BTree of a new index file
		if( attribute<ATTR_AM || attribute>ATTR_AGE ){
			std::cerr << "Invalid Attribute ID" << std::endl;
			return false;
		}
		tempStream << indexFile;
		if( !correctExtension(indexFile, ".idx")){
			tempStream << ".idx";
			uIndexFile = indexFile;
		}else{
			uIndexFile = indexFile.substr(0,(indexFile.size()-4));		
		}		
		idxStream.open(tempStream.str().c_str(), ios::in|ios::out|ios::trunc|ios::binary);
		if( !idxStream.is_open()){
			std::cerr << "Error creating index file" << std::endl;
			return false;
		}
		if( attribute != ATTR_AM ){
			tempStream.str("");
			tempStream << uIndexFile << ".uidx";
			uidxStream.open(tempStream.str().c_str(), ios::in|ios::out|ios::trunc|ios::binary);
			if( !uidxStream.is_open() ){
				std::cerr << "Error creating utility index file" << std::endl;
				return false;
			}
			btree = new BTreeHandler(idxStream, true, uidxStream, attribute);
		}else{			
			btree = new BTreeHandler(idxStream, true);
		}
	}else{		//Create the BTree from an existing index file
		if( !correctExtension(indexFile, ".idx")){
			std::cerr << "File format not recognized:" << std::endl;
			std::cerr << "Index file extension must be .idx" << std::endl; 
			return false;	
		}
		idxStream.open(indexFile.c_str(),ios::in);
		if( !idxStream ){
			std::cerr << "Index file "<< indexFile << " does not exist" << std::endl;
			return false;
		}
		idxStream.close();
		idxStream.open(indexFile.c_str(), ios::in|ios::out|ios::binary);
		if(!idxStream.is_open()){
			std::cerr << "Error opening index file" << std::endl;
			return false;
		}
		btree = new BTreeHandler(idxStream, false, idxStream);
		if( btree->buildForPrimaryKey() == false){ //If the tree was created for a secondary key
			tempStream << indexFile.substr(0,(indexFile.size()-4)) << ".uidx";
			uidxStream.open(tempStream.str().c_str(),ios::in);
			if( !uidxStream ){
				std::cerr << "Utility index file "<< tempStream.str() << " is missing" << std::endl;
				return false;
			}
			uidxStream.close();
			uidxStream.open(tempStream.str().c_str(), ios::in|ios::out|ios::binary);
			if(!uidxStream.is_open()){
				std::cerr << "Error opening utility index file" << std::endl;
				return false;
			}
			btree->setUidxStream(uidxStream);
		}
	}
	return true;
}
bool correctExtension(std::string file, char* extension){
	char* fileExtension = 0;
	char* cFile = new char[file.size()+1];
	strcpy(cFile, file.c_str());
	fileExtension = strrchr(cFile,'.');	
	if ((fileExtension != 0) && (strcmp(fileExtension, extension) == 0) ){
		delete cFile;
		return true;
	}else{
		delete cFile;
		return false;
	}
}