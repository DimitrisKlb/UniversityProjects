#include "student.h"
using std::setw;

std::string correctString(char* givenStr, int maxLetters, int exactNumber=0, char neutralChar='0');

Student::Student(){
	strcpy(mAM,"0000");
	strcpy(mLastName,"X");
	strcpy(mFirstName,"X");
	strcpy(mYear,"00");
	strcpy(mAge,"00");
}
Student::Student(char* AM, char* lastName, char* firstName, char* year, char* age){
	strcpy(mAM, correctString(AM, maxAM, 1, '0').c_str() );	
	strcpy(mLastName, correctString(lastName, maxLN).c_str() );
	strcpy(mFirstName, correctString(firstName, maxFN).c_str() );
	strcpy(mYear, correctString(year, maxYear, 1, '0').c_str() );
	strcpy(mAge, correctString(age, maxAge, 1, '0').c_str() );
}

char* Student::getAM(){ return mAM;}
char* Student::getLastName(){ return mLastName;}
char* Student::getFirstName(){ return mFirstName;}
char* Student::getYear(){ return mYear;}
char* Student::getAge(){ return mAge;}
char* Student::getAttribute(int attribute){
	switch(attribute){
	case ATTR_LAST_NAME:
		return mLastName;
		break;
	case ATTR_FIRST_NAME:
		return mFirstName;
		break;
	case ATTR_YEAR:
		return mYear;
		break;
	case ATTR_AGE:
		return mAge;
		break;
	default:
		return "";
		break;
	}
}

std::string Student::getInfo(){
    std::stringstream tempStream;

    tempStream << mAM <<' ';
    tempStream << mLastName  <<' ';
	if( strlen(mLastName) < maxLN ){
		for(int i=strlen(mLastName); i < maxLN; i++)
			tempStream << ' ';
	}
    tempStream << mFirstName <<' ';
	if( strlen(mFirstName) < maxFN ){
		for(int i=strlen(mFirstName); i < maxFN; i++)
			tempStream << ' ';
	}
    tempStream << mYear <<' ';
    tempStream << mAge       ;
    return tempStream.str();
}

std::string correctString(char* givenStr, int maxLetters, int exactNumber, char neutralChar){
	std::stringstream correctStr;
	if( strlen(givenStr) <= maxLetters ){
		if(exactNumber == 1 && strlen(givenStr) < maxLetters){
			for(int i=strlen(givenStr); i < maxLetters; i++){
				correctStr << neutralChar;
			}
			correctStr << givenStr;
			return correctStr.str();
		}else{
			correctStr << givenStr;
			return correctStr.str();
		}
	}else{
		correctStr << givenStr;
		return correctStr.str().substr(0, maxLetters);
	}
}