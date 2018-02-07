#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

enum{ ATTR_AM, ATTR_LAST_NAME, ATTR_FIRST_NAME, ATTR_YEAR, ATTR_AGE};

const int maxAM   =  4 ;
const int maxLN   = 20 ;
const int maxFN   = 15 ;
const int maxYear =  2 ;
const int maxAge  =  2 ;

class Student{
    public:
        Student(char*, char*, char*, char*, char*);
		Student();

        char* getAM();
        char* getLastName();
        char* getFirstName();
        char* getYear();
        char* getAge();
		char* getAttribute(int attribute);
        std::string getInfo();

    private:
        char mAM[maxAM +1];
        char mLastName[maxLN +1];
        char mFirstName[maxFN +1];
        char mYear[maxYear +1];
        char mAge[maxAge +1];
};

#endif // STUDENT_H