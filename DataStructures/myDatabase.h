#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>

#include "btree.h"

bool createDatabase(std::string newDbFile);
bool createPrimaryIndex(std::string dbFile, std::string newIndexFile);
bool createSecondaryIndex(std::string dbFile, std::string newIndexFile, int attribute);
StudentInfo insertInDatabase(Student newStudent, std::string dbFile);
bool insert(StudentInfo newStudentRecord, std::string indexFile);
std::list<StudentInfo> find(char* key, std::string indexFile);
std::list<StudentInfo> findRange(char* key1, char* key2, std::string indexFile);

void readFromDatabase(std::list<StudentInfo> students, std::string dbFile);
void readFromDatabase(StudentInfo student, std::string dbFile);

#endif // MYDATABASE