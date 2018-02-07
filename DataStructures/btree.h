#ifndef BTREE_H
#define BTREE_H

#include <fstream>
#include <list>

#include "student.h"
#include "cstring"

const int bTreeOrder = 10;			// Max num of children=10, Min=5
const int maxKeys = bTreeOrder - 1;
const int minKeys = maxKeys/2;
const int maxKeyChars = 20;
const int noChild = -1;

const int uidxBlockSize = 50;		// Size of a block of positions in the uidxFile
enum{	// Possible values returned by the pushDown() method
	CONTINUE, 
	DOUBLE_RECORD, 
	NOT_UNIQUE_SECONDARY
	}; 

typedef struct{
    char theKey[maxKeyChars];
    long thePosition;
	int theNumber;				// Number of records with the same secondary key
}StudentInfo;

typedef struct{
    int count;                      // Current number of keys
    StudentInfo keys[maxKeys];
    long children[bTreeOrder];
}BTreeNode;

class BTreeHandler{
    public:
		BTreeHandler(std::fstream &idxStream, bool creatingTree);
		BTreeHandler(std::fstream &idxStream, bool creatingTree, std::fstream &uidxStream, int attribute=ATTR_AM);
		~BTreeHandler();
		bool buildForPrimaryKey();
		void setUidxStream(std::fstream &uidxStream);

		bool insert(StudentInfo &newStudentRecord);
		std::list<StudentInfo> find(char* key);
		void searchSubTree(long currentRoot, char* key1, char* key2, std::list<StudentInfo> &studentsFound);
		
    private:
        long root;
        long nodesNum;
        long recordsNum;
		int attribute;
		bool primaryKey;
        BTreeNode currentNode;
		
		std::fstream* indexStream;
		std::fstream* utilityIndexStream;

	private:
		int pushDown(const StudentInfo &currentRec, long currentRoot,
			bool &moveUp, StudentInfo &newRec, long &newRight);
		void split(const StudentInfo &currentRec, long currentRight, long currentRoot,
			int location, StudentInfo &newRec, long &newRight);
		bool searchNode(const char* targetKey, int &location);
		void addItem(const StudentInfo &newRec, long newRight, BTreeNode &node, int location);
		void addSecondaryRecord(StudentInfo &newRec);
		void addExistingKeyItem(const StudentInfo &newRec, long nodePos, int location);
};

#endif // BTREE_H	