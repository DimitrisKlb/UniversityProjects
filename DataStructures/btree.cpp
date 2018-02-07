#include "btree.h"

BTreeHandler::BTreeHandler(std::fstream &idxStream, bool creatingTree, std::fstream &uidxStream, int attr){
	attribute = attr;
	if( attr<ATTR_AM || attr>ATTR_AGE ){
		attribute = ATTR_AM;
	}	
	indexStream = &idxStream;
	utilityIndexStream = &uidxStream;
	if( creatingTree ){
		root = noChild;      
		recordsNum = 0;
		nodesNum = 0;		
		currentNode.children[0] = recordsNum;
		currentNode.children[1] = nodesNum;
		currentNode.children[2] = root;
		currentNode.children[3] = attribute;
		indexStream->seekp(0, std::ios::beg);
		indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	}else{
		indexStream->seekg(0, std::ios::beg);
		indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
		recordsNum = currentNode.children[0];
		nodesNum = currentNode.children[1];
		root = currentNode.children[2];	
		attribute = currentNode.children[3];
	}
	primaryKey = (attribute == ATTR_AM)?  true : false;
}
BTreeHandler::BTreeHandler(std::fstream &idxStream, bool creatingTree){
	attribute = ATTR_AM;
	indexStream = &idxStream;
	primaryKey = true;

	if( creatingTree ){
		root = noChild;      
		recordsNum = 0;
		nodesNum = 0;		
		currentNode.children[0] = recordsNum;
		currentNode.children[1] = nodesNum;
		currentNode.children[2] = root;
		currentNode.children[3] = attribute;
		indexStream->seekp(0, std::ios::beg);
		indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	}else{
		indexStream->seekg(0, std::ios::beg);
		indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
		recordsNum = currentNode.children[0];
		nodesNum = currentNode.children[1];
		root = currentNode.children[2];	
		attribute = currentNode.children[3];
	}
}
BTreeHandler::~BTreeHandler(){
	currentNode.children[0] = recordsNum;
	currentNode.children[1] = nodesNum;
	currentNode.children[2] = root;
	currentNode.children[3] = attribute;
	indexStream->seekp(0, std::ios::beg);
	indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
}
bool BTreeHandler::buildForPrimaryKey(){
	return primaryKey;
}
void BTreeHandler::setUidxStream(std::fstream &uidxStream){
	utilityIndexStream = &uidxStream;
}

bool BTreeHandler::insert(StudentInfo &newStudentRecord){
	bool moveUp;
	long newRight;
	StudentInfo newRecord;

	int code = pushDown(newStudentRecord, root, moveUp, newRecord, newRight);
	if( code == DOUBLE_RECORD){		
		std::cout << "Error: Record with primary key AM = ";
		std::cout << newStudentRecord.theKey << " already exists in the specified";
		std::cout << " index file" << std::endl;
		return false;
	}
	if (moveUp){
      currentNode.count = 1;
      currentNode.keys[0] = newRecord;
      currentNode.children[0] = root;
      currentNode.children[1] = newRight;
      nodesNum++;
      root = nodesNum;
	  
	  indexStream->seekp(nodesNum * sizeof(currentNode), std::ios::beg);
      indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	}
	recordsNum++;
	return true;
}
std::list<StudentInfo> BTreeHandler::find(char* key){
	int location;
	bool found = false;
	long currentRoot = root;
	std::list<StudentInfo> studentsFound; 

	while( (currentRoot != noChild) && (!found) ){
		indexStream->seekg(currentRoot * sizeof(currentNode), std::ios::beg);
		indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
		if( searchNode(key, location) ){
			found = true;
			if(primaryKey == true){
				studentsFound.push_back(currentNode.keys[location]);
			}else{
				int number = currentNode.keys[location].theNumber;
				StudentInfo tempStudent;
				strcpy(tempStudent.theKey, currentNode.keys[location].theKey);
				tempStudent.theNumber = number;				
				long *offsets = new long[number];
				utilityIndexStream->seekg((currentNode.keys[location].thePosition)*sizeof(long),std::ios::beg);
				utilityIndexStream->read (reinterpret_cast <char *> (offsets), (sizeof(long)*number));	
				for(int i=0;i<number;i++){
					tempStudent.thePosition = offsets[i];
					studentsFound.push_back(tempStudent);
				}
				delete offsets;
			}
		}else{
			currentRoot = currentNode.children[location + 1];
		}
	}
	return studentsFound;
}
void BTreeHandler::searchSubTree(long currentRoot, char* key1, char* key2, std::list<StudentInfo> &studentsFound){
	if(currentRoot == 0)
		currentRoot = root;
	if(currentRoot == noChild)
		return;
	
	indexStream->seekg(currentRoot*sizeof(currentNode), std::ios::beg);
	indexStream->read (reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	int count = currentNode.count;
	for(int i=0; i < count; i++){
		searchSubTree(currentNode.children[i],key1,key2,studentsFound);	
		indexStream->seekg(currentRoot*sizeof(currentNode), std::ios::beg);
		indexStream->read (reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
		char* currentKey = currentNode.keys[i].theKey;
		if(strcmp(currentKey,key1)>=0 && strcmp(currentKey,key2)<=0 ){		
			if(primaryKey == true){
				studentsFound.push_back(currentNode.keys[i]);
			}else{
				int number = currentNode.keys[i].theNumber;
				StudentInfo tempStudent;
				strcpy(tempStudent.theKey, currentNode.keys[i].theKey);
				tempStudent.theNumber = number;				
				long *offsets = new long[number];
				utilityIndexStream->seekg((currentNode.keys[i].thePosition)*sizeof(long),std::ios::beg);
				utilityIndexStream->read (reinterpret_cast <char *> (offsets), (sizeof(long)*number));
				for(int k=0;k<number;k++){
					tempStudent.thePosition = offsets[k];
					studentsFound.push_back(tempStudent);
				}
				delete offsets;			
			}
		}	
	}
	searchSubTree(currentNode.children[currentNode.count],key1,key2,studentsFound);
}

int BTreeHandler::pushDown(const StudentInfo &currentRec, long currentRoot,
	bool &moveUp, StudentInfo &newRec, long &newRight)
{
	int location=0;
	if(currentRoot == noChild){
		moveUp = true;
		newRec = currentRec; 
		newRight = noChild;	
		if(primaryKey == false)
			addSecondaryRecord(newRec);
	}else{
		indexStream->seekg(currentRoot * sizeof(currentNode), std::ios::beg);
		indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
		if(searchNode(currentRec.theKey, location)){
			if(primaryKey == true){
				return DOUBLE_RECORD;
			}else{
				addExistingKeyItem(currentRec,currentRoot,location);
				moveUp = false;
				return NOT_UNIQUE_SECONDARY;
			}
		}		
		int code = pushDown(currentRec, currentNode.children[location+1], moveUp, newRec, newRight);
		if( code != CONTINUE ){
			return code;
		}	
		if(moveUp){
			indexStream->seekg(currentRoot * sizeof(currentNode), std::ios::beg);
			indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
			if(currentNode.count < bTreeOrder-1){
				moveUp = false;
				addItem(newRec, newRight, currentNode, location+1);
				indexStream->seekp(currentRoot * sizeof(currentNode), std::ios::beg);
				indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
			}else{
				moveUp = true;
				split(newRec, newRight, currentRoot, location, newRec, newRight);
			}			
		}		
	}
	return CONTINUE;
}
void BTreeHandler::split(const StudentInfo &currentRec, long currentRight, long currentRoot,
	int location, StudentInfo &newRec, long &newRight)
{
	int median;
	BTreeNode rightNode;
	if( location < minKeys )  // < Minimum number of keys
		median = minKeys;
	else
		median = minKeys +1 ;

	indexStream->seekg(currentRoot * sizeof(currentNode), std::ios::beg);
	indexStream->read(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	
	for(int i=median; i<maxKeys; i++){
		rightNode.keys[i - median] = currentNode.keys[i];
		rightNode.children[i - median +1] = currentNode.children[i+1];
	}
	rightNode.count = maxKeys - median;
	currentNode.count = median;

	if( location < minKeys )
		addItem(currentRec, currentRight, currentNode, location+1);
	else
		addItem(currentRec, currentRight, rightNode, location - median+1);
	
	newRec = currentNode.keys[currentNode.count - 1];
	rightNode.children[0] = currentNode.children[currentNode.count];
	currentNode.count--;

	indexStream->seekp(currentRoot * sizeof(currentNode), std::ios::beg);
	indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));

	nodesNum++;
	newRight = nodesNum;
	indexStream->seekp(newRight * sizeof(rightNode), std::ios::beg);
	indexStream->write(reinterpret_cast <char *> (&rightNode), sizeof(rightNode));

}
bool BTreeHandler::searchNode(const char* targetKey, int &location){
	bool found = false;
	if( strcmp(targetKey, currentNode.keys[0].theKey)<0 ){
		location = -1;
	}else{
		location = currentNode.count - 1;
		while( (strcmp(targetKey, currentNode.keys[location].theKey) < 0)&&(location>0) ) {
			location--;
		}
		if( strcmp(targetKey, currentNode.keys[location].theKey) == 0){
			found = true;
		}
	}
	return found;
}
void BTreeHandler::addItem(const StudentInfo &newRec, long newRight, BTreeNode &node, int location){
	
	for(int i=node.count; i>location ; i--){
		node.keys[i] = node.keys[i-1];
		node.children[i+1] = node.children[i];
	}
	node.keys[location] = newRec;
	node.children[location+1] = newRight;
	node.count++;
}
void BTreeHandler::addSecondaryRecord(StudentInfo &newRec){
	utilityIndexStream->seekp(0,std::ios::end);
	long uidxOffest = (utilityIndexStream->tellp()) / (sizeof(long));
	long dbOffset = newRec.thePosition;
	long noOffset = -1;
	newRec.thePosition = uidxOffest;	
	long *block = new long[uidxBlockSize];
	block[0] = dbOffset;
	for(int i=1; i<uidxBlockSize; i++){
		block[i] = noOffset;
	}
	utilityIndexStream->write(reinterpret_cast <char *> (block), (sizeof(long)*uidxBlockSize) );
	delete block;
}
void BTreeHandler::addExistingKeyItem(const StudentInfo &newRec,long nodePos, int location){
	int currentNumber = currentNode.keys[location].theNumber;
	long currentPosition = currentNode.keys[location].thePosition;
	if(currentNumber % uidxBlockSize == 0){ 
		long *currentOffsets = new long[currentNumber];
		utilityIndexStream->seekg(currentPosition * sizeof(long), std::ios::beg);
		utilityIndexStream->read(reinterpret_cast <char *> (currentOffsets), (sizeof(long)*currentNumber));		
		utilityIndexStream->seekp( 0, std::ios::end);
		long newUidxOffest = (utilityIndexStream->tellp()) / (sizeof(long));
		currentNode.keys[location].thePosition = newUidxOffest;
		utilityIndexStream->write(reinterpret_cast <char *> (currentOffsets),(sizeof(long)*currentNumber));
		delete currentOffsets;		
		currentOffsets = new long[uidxBlockSize];
		for(int i=1; i<uidxBlockSize; i++){
			currentOffsets[i] = -1;
		}
		utilityIndexStream->write(reinterpret_cast <char *> (currentOffsets), (sizeof(long)*uidxBlockSize) );
		delete currentOffsets;
	}
	long uidxOffset = currentNode.keys[location].thePosition + currentNode.keys[location].theNumber;
	currentNode.keys[location].theNumber++;
	long dbOffset = newRec.thePosition;
	indexStream->seekp(nodePos * sizeof(currentNode), std::ios::beg);
	indexStream->write(reinterpret_cast <char *> (&currentNode), sizeof(currentNode));
	utilityIndexStream->seekp( uidxOffset * sizeof(long), std::ios::beg);
	utilityIndexStream->write(reinterpret_cast <char *> (&dbOffset), sizeof(long) );
}