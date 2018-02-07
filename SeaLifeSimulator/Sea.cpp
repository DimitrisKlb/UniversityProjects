#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "Sea.h"

Sea::Sea(int y,int x,bool random){
	width = x;
	height = y;              // Midenismos twn statistikwn olwn twn eidwn
	for(int i=0;i<10;i++){
		getStatisticks(i)->setPopulation(0);            
		getStatisticks(i)->setDeaths(0);
		getStatisticks(i)->setDeathPercentage(0);
		getStatisticks(i)->setSizeAv(0);
		getStatisticks(i)->setFoodAv(0);
		getStatisticks(i)->setAgeAv(0);
	}

	map = new Organism**[height];
	for(int i=0;i<height;i++){
		map[i] = new Organism*[width];
	}
	contamination = new int*[height];
	for(int i=0;i<height;i++){
		contamination[i] = new int[width];
	}
	
	for( int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			map[i][j] = NULL;
			contamination[i][j]=0;
		}
	}

	hours12 = days = weeks = years = 1;
	if(random){
		srand ((int)time(NULL) );	                  // Tixaia dimiourgia organismwn me pososta dia ka8e eidos
		for(int i=0;i<height;i++){ 
			for(int j=rand()%3;j<width;j+=rand()%3+1){
				int classRand = rand()%130;
				if(classRand<45)
					classRand = PHYTO_ID;
				else if(classRand<70)
					classRand = ZOO_ID;
				else if(classRand<80)
					classRand = SARDINE_ID;
				else if(classRand<91)
					classRand = SHRIMP_ID;
				else if(classRand<102)
					classRand = SQUID_ID;
				else if(classRand<110)
					classRand = TUNA_ID;
				else if(classRand<120)
					classRand = WOLFEEL_ID;
				else if(classRand<124)
					classRand = OCTOPUS_ID;
				else if(classRand<127)
					classRand = DOLPHIN_ID;
				else
					classRand = SHARK_ID;
				
				int size,age;
				if(classRand == PHYTO_ID)	
					size = rand()%getStatisticks(classRand)->getMaxSize()+10;
				else
					size = rand()%getStatisticks(classRand)->getMaxSize();
				age =  size/ getStatisticks(classRand)->getGrowthRate();
				
				createOrganism(i,j,classRand,size,age);

			}
		}
	}
}
Sea::~Sea(){
	for(int i=0;i<height;i++){
		delete[] map[i];
	}
	delete[] map;
	
	for(int i=0;i<height;i++){
		delete[] contamination[i];
	}
	delete[] contamination;
}

int Sea::getWidth(){return width;}
int Sea::getHeight(){return height;}
int Sea::getContaminationAt(int y,int x){return contamination[y][x];}
Organism* Sea::getOrganismAt(int y,int x){return map[y][x];}
string Sea::getDate(){
	std::stringstream temp;
	temp << "Year: "<<years << " Week: "<<weeks << " Day: "<<days;
	return temp.str();
}

void Sea::addContaminationAt(int y,int x,int contamLvl){
	contamination[y][x]=contamLvl;
}
void Sea::addOrganism(int y,int x,Organism* org){
	
		map[y][x] = org;
		org->setPosition(y,x);
	
}

void Sea::update(Organism** selected){
	selectedOrg = *selected;

	srand ((int)time(NULL) );
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			if(map[i][j]!= NULL){
				map[i][j]->setActed(false);
			}
		}
	}
	updateMap();
	updateTime();
	updateStatisticks();
	*selected = selectedOrg;
}
void Sea::updateMap(){
	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			
													// Algori8smos e3aplwsis tis molinsis
			if(contamination[i][j]>1 ){
				for(int dy=-1;dy<=1;dy++){
					for(int dx=-1;dx<=1;dx++){
						if( (i+dy==-1)||(i+dy==height)||(j+dx==-1)||(j+dx==width) ) continue;
						if(contamination[i][j]>contamination[i+dy][j+dx] && rand()%10<3)
							contamination[i+dy][j+dx] = contamination[i][j] - 1;
					}
				}
			}
			                                      // Molinsi organismou 
			if((map[i][j]!= NULL)&&(contamination[i][j]>map[i][j]->getContamLvl()) )
								map[i][j]->setContamLvl( contamination[i][j] );

			if( (map[i][j]!= NULL)&&(map[i][j]->getAttr()->getFoodLevel()!=PHYTO_ID )&&(map[i][j]->hasActed()==false)){			
				map[i][j]->setActed(true);

				Point target = canReproduce(i,j);
				if(target.x!=-1){				// Mixanismos anaparagwgis kai dimiourgia apogonwn analoga me to epipedo diatrofikis alisidas 
  					for(int z=0; z<(9-(map[i][j]->getAttr()->getFoodLevel() ))*3-4; z++){
						int di = rand()%7 - 3;
						int dj = rand()%7 - 3;
						if( (i+di<0)||(i+di>=height)||(j+dj<0)||(j+dj>=width) ) continue;	
						if(map[i+di][j+dj] == NULL){
							createOrganism(i+di,j+dj,map[i][j]->getAttr()->getID(),20,0);
					
						}
					}
				}         
				                              // Mixanismos diatrofis
				target = canEat(i,j);
				if( target.x!=-1){								
					map[i][j]->eat(*map[target.y][target.x]); 
					die(target.y,target.x);
					map[i][j]->setPosition(target.y,target.x);
					map[target.y][target.x] = map[i][j];				
					map[i][j] = NULL;
					                          // Mixanismos kinisis
				}else if( (map[i][j]->getAttr()->getMovementFreq()!=0)&&( (hours12+days*2)%map[i][j]->getAttr()->getMovementFreq() == 0)){  
					for(int k=0;k<3;k++){		
						int dy = rand()%3-1;
						int dx = rand()%3-1;
						if( (i+dy==-1)||(i+dy==height)||(j+dx==-1)||(j+dx==width) ){		
							k--;															
							continue;
						}
						if(map[i+dy][j+dx] == NULL){						
							map[i][j]->setPosition(i+dy,j+dx);
							map[i+dy][j+dx] = map[i][j];					
							map[i][j] = NULL;

							if(contamination[i+dy][j+dx]>map[i+dy][j+dx]->getContamLvl())
								map[i+dy][j+dx]->setContamLvl( contamination[i+dy][j+dx] );
							break;
						}
					
					}			
				}		
			
			}		
		}
	}
}
void Sea::updateTime(){
	hours12++;                 // Au3isi tou xronou proxomoiwsis kata misi mera kai ipologismos twn allagwn se sigkerkimena xronika diastimata
	if(hours12 == 3){
		hours12 = 1;
		days++;
	}
	if(days%3==0){            // Dinatotita anaparagwgis organismwn meta apo 3 meres afou anaparax8ike
		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				if( map[i][j] != NULL){
					map[i][j]->setCanReproduce(true);
				}
			}
		}
	}
	
	
	if(days == 8){
		days = 1;
		weeks++;

 		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				if( (map[i][j] != NULL)&&(map[i][j]->getAttr()->getFoodLevel()!=0) ){     //Ipologismos an enas organismos prepei na pe8anei logo ilikias  - Ola ta psaria pera apo ta Phytoplankton
					map[i][j]->setAge(map[i][j]->getAge()+1);
					if( map[i][j]->getAge()>=map[i][j]->getDyingAge()){
						die(i,j);
					}else if(!map[i][j]->isHungry()){                                    // Ipologismos an enas organismos prepei na megalwsei i na mikrinei analoga me tin katanalwsi tou kata tin teleutaia ebdomada
						if(map[i][j]->getSize()+map[i][j]->getAttr()->getGrowthRate() <= map[i][j]->getAttr()->getMaxSize() ){
							map[i][j]->setSize( map[i][j]->getSize()+map[i][j]->getAttr()->getGrowthRate());						
						}					
					}else{
						map[i][j]->setSize( map[i][j]->getSize() - (map[i][j]->getSize()/10 - map[i][j]->getFoodEaten() ) ); //Meiwsi tou size kata to poso pou den efage
						int normalSize = map[i][j]->getAge()*map[i][j]->getAttr()->getGrowthRate() + 20;
						if(map[i][j]->getSize()< normalSize/10 ){
							die(i,j);
						}						
						
					}
					
				}else if((map[i][j] != NULL)&&(map[i][j]->getAttr()->getFoodLevel()==0) ){     // Ipologismos gia au3isi size kai anaparagwgis me dixotimisi twn PhytoPlankon
					map[i][j]->setAge(map[i][j]->getAge()+1);
					if( map[i][j]->getAge()>=map[i][j]->getDyingAge()){
						die(i,j);
					}else{
						if( map[i][j]->getSize() < map[i][j]->getAttr()->getMaxSize() )
							map[i][j]->setSize( map[i][j]->getSize() + map[i][j]->getAttr()->getGrowthRate());
						if( (map[i][j]->getSize() >= map[i][j]->getAttr()->getMaxSize() )){         // Dixotomisi tou an 3eperase to megisto mege8os
							bool partitioned = false;
							for(int di=-1;di<=1;di++){
								for(int dj=-1;dj<=1;dj++){
									if( (i+di==-1)||(i+di==height)||(j+dj==-1)||(j+dj==width) ) continue;	
									if( (map[i+di][j+dj]==NULL)&&(!partitioned)  ){
										addOrganism(i+di,j+dj,new PhytoPlankton(map[i][j]->getAttr()->getMaxSize()/2,0) );
										map[i][j]->setSize(map[i][j]->getAttr()->getMaxSize()/2);									
										partitioned = true;
									}
								}
							}
						}
					}
				}
			}
		}

		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				if(map[i][j] != NULL){
					map[i][j]->setFoodEaten(0);
				}
			}
		}
	}
	
	if( ((weeks*7+days)%20  == 0) &&(hours12==1)){          // Miwsi tou epipedou molinsis ka8e 20 meres peripou
		for(int i=0;i<height;i++){
			for(int j=0;j<width;j++){
				if(contamination[i][j]>0)
					contamination[i][j]--;
			}
		}
	}
	if(weeks == 51){
		weeks = 1;
		years++;
	}
	
}

void Sea::updateStatisticks(){          // Alagi twn statistikwn gia ka8e eidos ogranismou
	int totalDeaths=0;
	for(int i=0;i<10;i++){
		getStatisticks(i)->setSizeAv(0);
		getStatisticks(i)->setFoodAv(0);
		getStatisticks(i)->setAgeAv(0);
		totalDeaths += getStatisticks(i)->getDeaths();
	}

	for(int i=0;i<height;i++){
		for(int j=0;j<width;j++){
			if( map[i][j] != NULL){
				map[i][j]->getAttr()->setSizeAv( map[i][j]->getAttr()->getSizeAv() + map[i][j]->getSize());
				map[i][j]->getAttr()->setFoodAv( map[i][j]->getAttr()->getFoodAv() + map[i][j]->getFoodEaten());
				map[i][j]->getAttr()->setAgeAv( map[i][j]->getAttr()->getAgeAv() + map[i][j]->getAge());
			}
		}
	}
	for(int i=0;i<10;i++){
 		Attributes* temp = getStatisticks(i);
		if(totalDeaths!=0){
			temp->setDeathPercentage( temp->getDeaths()/(float)totalDeaths * 100 );
		}		
		if(temp->getPopulation()!=0){
			temp->setSizeAv(temp->getSizeAv()/temp->getPopulation());
			temp->setFoodAv(temp->getFoodAv()/temp->getPopulation());
			temp->setAgeAv(temp->getAgeAv()/temp->getPopulation());
		}
	}
}

Point Sea::canEat(int I,int J){
	Point target;
    target.x=-1;
	target.y=-1;
	if(!map[I][J]->isHungry() ) return target;

	for(int dy=-1;dy<=1;dy++){
		for(int dx=-1;dx<=1;dx++){
			if( (I+dy==-1)||(I+dy==height)||(J+dx==-1)||(J+dx==width) ) continue;	
						
			if( (map[I+dy][J+dx]!=NULL) && (map[I][J]->getAttr()->getFoodLevel()-map[I+dy][J+dx]->getAttr()->getFoodLevel()<=5)&& (map[I][J]->getAttr()->getFoodLevel()-map[I+dy][J+dx]->getAttr()->getFoodLevel()>0) &&( ((float)(map[I+dy][J+dx][0].getSize()) )/map[I][J][0].getSize() <= 1.25 ) ){  // elegxos gia neededFood
				target.x = J+dx; 
				target.y = I+dy;
				return target;
			}		
		}
	}
	return target;

}
Point Sea::canReproduce(int I,int J){
	Point target;
    	target.x=-1;
		target.y=-1;
	
	if(map[I][J]->getSize()<30 || !map[I][J]->CanReproduce())
		return target;

	for(int dy=-1;dy<=1;dy++){
		for(int dx=-1;dx<=1;dx++){
			if( (I+dy==-1)||(I+dy==height)||(J+dx==-1)||(J+dx==width) ) continue;	
			if( (dx!=0 || dy!=0 )&&(map[I+dy][J+dx]!=NULL) && (map[I][J]->getAttr()->getID() == map[I+dy][J+dx]->getAttr()->getID()) && (map[I+dy][J+dx]->getSize()>=30) && map[I+dy][J+dx]->CanReproduce()){  
				map[I][J]->setCanReproduce(false);
				map[I+dy][J+dx]->setCanReproduce(false);
				target.x = J+dx; 
				target.y = I+dy;
				return target;
			}			

		}
	}
	return target;

}

void Sea::die(int I,int J){
	if(map[I][J] == NULL) return;
	if(map[I][J] == selectedOrg) selectedOrg = NULL;

	map[I][J]->getAttr()->setPopulation( map[I][J]->getAttr()->getPopulation() - 1 );    // Meiwsi tou population tou antistixou eidous
	map[I][J]->getAttr()->setDeaths( map[I][J]->getAttr()->getDeaths() + 1 );            // Meiwsi twn 8anatwn tou antistoixou eidous
	delete map[I][J];
	map[I][J] = NULL; 
}

void Sea::throwNets(int y1,int x1,int y2,int x2){
	srand((int)time(NULL)); 
	
	for(int i=y1;i<=y2;i++){
		for(int j=x1;j<=x2;j++){
			if(map[i][j]!=NULL && map[i][j]->getAttr()->getID()>1){	
				if( rand()%10 < 8){            // Pi8anotita piasimatos enos psariou 80%
					die(i,j);
	
				}
			}
		}	
	}

}
Attributes* Sea::getStatisticks(int fishType){
	switch(fishType){
	case 0:
		return PhytoPlankton::att;
	case 1:
		return ZooPlankton::att;
	case 2:
		return Sardine::att;
	case 3:
		return Shrimp::att;
	case 4:
		return Squid::att;
	case 5:
		return Tuna::att;
	case 6:
		return WolfEel::att;
	case 7:
		return Octopus::att;
	case 8:
		return Dolphin::att;
	case 9:
		return Shark::att;
	default:
		return NULL;
	}


}
void Sea::createOrganism(int y,int x,int fishType,int size,int age){
	switch(fishType){
	case 0:
		addOrganism(y,x, new PhytoPlankton(size,age));
		break;	
	case 1:
		addOrganism(y,x, new ZooPlankton(size,age));
		break;
	case 2:
		addOrganism(y,x, new Sardine(size,age));
		break;
	case 3:
		addOrganism(y,x, new Shrimp(size,age));
		break;
	case 4:
		addOrganism(y,x, new Squid(size,age));
		break;
	case 5:
		addOrganism(y,x, new Tuna(size,age));
		break;
	case 6:
		addOrganism(y,x, new WolfEel(size,age));
		break;	
	case 7:
		addOrganism(y,x, new Octopus(size,age));
		break;
	case 8:
		addOrganism(y,x, new Dolphin(size,age));
		break;
	case 9:
		addOrganism(y,x, new Shark(size,age));
		break;
	}
}