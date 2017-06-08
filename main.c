#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SPACE_SIZE 1000
#define GALAXY_NUM 500
#define CIVIL_NUM 100
#define ACTIONS_EACH_ROUND 10
#define DANGER_TECH 1000
#define MAX_TECH DANGER_TECH*10

int livecivils = CIVIL_NUM;

typedef struct position{
	int x;
	int y;
	int z;
}Position;

typedef struct galaxy{
	Position position;
	int owner;
}Galaxy;

Galaxy space[GALAXY_NUM];//space

typedef struct galaxys{
	int id;
	struct galaxys* next;
}Galaxys;

typedef struct friends{
	int id;
	struct friends* next;
}Friends;


typedef struct enermys{
	int id;
	//int galaxy_id;
	struct enermys* next;
}Enermys;

typedef struct civil{
	int  id;
	int  tech_step;//0-100
	int  motherland;
	int  tech;//0-10000
	int  faction;//0-100
	int  success_count;
	int  failure_count;
	int  Commu_count;
	struct civil* next;
	Galaxys* galaxys;
	Friends*  friends;
	Enermys* enermys;
}Civil;

Position InitPosition(void){

	Position position;

	position.x = rand()%SPACE_SIZE;
	position.y = rand()%SPACE_SIZE;
	position.z = rand()%SPACE_SIZE;
	return position;

}

void InitSpace(){

	int i;
	for(i=0;i<GALAXY_NUM;i++){

		space[i].position=InitPosition();
		space[i].owner=0;
	
	}
}


void PrintSpace(){

	int i;
	for(i=0;i<GALAXY_NUM;i++){

		printf("Galaxy:%d--%d,%d,%d ",i,space[i].position.x,space[i].position.y,space[i].position.z);
		printf("owned by civil %d\n",space[i].owner);
	
	}
}


void SysInit(){

	srand(time(NULL));

}

Civil* InitCivils(Civil* civils){

	int i;
	int home;
	Civil* temp;

	civils=(Civil*)malloc(sizeof(Civil));
	civils->next = NULL;
	for(i=1;i<CIVIL_NUM+1;i++){

		temp=(Civil*)(malloc(sizeof(Civil)));
		if(temp==NULL){
			printf("malloc failed\n");
			return civils;
		}

		temp->id = i;
		temp->friends = (Friends*)malloc(sizeof(Friends));
		temp->friends->next = NULL;
		temp->enermys = (Enermys*)malloc(sizeof(Enermys));
		temp->enermys->next = NULL;
		temp->tech = rand()%20;
		temp->tech_step = rand()%10;
		temp->success_count=0;
		temp->failure_count=0;
		temp->Commu_count=0;
		temp->galaxys = (Galaxys*)malloc(sizeof(Galaxy));
		temp->galaxys->next = (Galaxys*)malloc(sizeof(Galaxy));
refind:
		home = rand()%GALAXY_NUM;
		if(space[home].owner!=0)
			goto refind;
		temp->galaxys->next->id=home;
		temp->motherland=temp->galaxys->next->id;
		space[temp->galaxys->next->id].owner=temp->id;
		temp->galaxys->next->next=NULL;
		temp->faction = rand()%100;
		temp->next=civils->next;
		civils->next=temp;
	}
	return civils;

}

Civil* FindCivil(Civil* civils,int id){

	Civil* temp = civils->next;

	while(temp->id!=id && temp->next!=NULL){
		temp = temp->next;
	}

	if(temp->id==id)
		return temp;
	else{
		//printf("civilization %d does not exist\n",id);
		return NULL;
	}

}


Civil* DeleteCivil(Civil* civils,int id){

	Civil* temp1 = civils;
	Civil* temp2 = NULL;
	Friends* friends = NULL;
	Galaxys* galaxys = NULL;
	Enermys* enermys = NULL;

	while(temp1->next->id!=id && temp1->next!=NULL){
		temp1 = temp1->next;
	}

	friends = temp1->next->friends->next;
	galaxys = temp1->next->galaxys->next;
	enermys = temp1->next->enermys->next;
	while(friends!=NULL){
		temp1->next->friends->next=friends->next;
		free(friends);
		friends=temp1->next->friends->next;
	}
	while(enermys!=NULL){
		temp1->next->enermys->next=enermys->next;
		free(enermys);
		enermys=temp1->next->enermys->next;
	}
	while(galaxys!=NULL){
		temp1->next->galaxys->next=galaxys->next;
		space[galaxys->id].owner=0;
		free(galaxys);
		galaxys=temp1->next->galaxys->next;
	}
	free(temp1->next->galaxys);
	free(temp1->next->friends);
	free(temp1->next->enermys);
	temp2=temp1->next;
	temp1->next=temp2->next;
	free(temp2);

	return civils;

}

void PrintCivil(Civil* civil){


	Galaxys* galaxys = NULL;
	Friends* friends = NULL;
	Enermys* enermys = NULL;
	if(civil==NULL)
		return;
	printf("*%d号文明:motherland=%d,tech=%d,tech_step=%d,faction=%d,succeeded=%d,failed=%d,commu=%d,galaxys:",civil->id,civil->motherland,civil->tech,civil->tech_step,civil->faction,civil->success_count,civil->failure_count,civil->Commu_count);
	galaxys = civil->galaxys->next;
	while(galaxys!=NULL){
		printf("%d ",galaxys->id);
		if(galaxys->next!=NULL)
			galaxys = galaxys->next;
		else
			break;
	}

	printf("friends:");
	friends = civil->friends->next;
	while(friends!=NULL){
		printf("%d ",friends->id);
		if(friends->next!=NULL)
			friends = friends->next;
		else
			break;
	
	}

	printf("enermys:");
	enermys = civil->enermys->next;
	while(enermys!=NULL){
		printf("%d ",enermys->id);
		if(enermys->next!=NULL)
			enermys = enermys->next;
		else
			break;
	
	}
	printf("\n");
}

int isHappen(int chance){

	if(rand()%chance == 1)
		return 1;
	else
		return 0;

}


int LoseGalaxy(Civil* civils, Civil* civil1,Civil* civil2,int galaxy_id){//2 lose id to 1

	Galaxys* galaxys = NULL;
	Galaxys* temp = NULL;

	printf("*%d号文明失去星系%d\n",civil2->id,galaxy_id);
	galaxys = civil2->galaxys;
	while(galaxys->next!=NULL && galaxys->next->id!=galaxy_id)
		galaxys = galaxys->next;

	if(galaxys->next==NULL){
		printf("error,galaxy[%d] is owned by %d, not %d\n",galaxy_id,space[galaxy_id].owner,civil2->id);
		//PrintCivil(civil2);
		return 2;	
	}
	if(galaxys->next->id == galaxy_id){
		temp = galaxys->next;
		galaxys->next = temp->next;

		temp->next = civil1->galaxys->next;
		civil1->galaxys->next = temp;
		space[galaxy_id].owner= civil1->id;
	}
	if(civil2->galaxys->next == NULL){
		printf("*%d号文明失去最后星系，灭亡\n",civil2->id);
		civils=DeleteCivil(civils,civil2->id);	
		livecivils--;
		return 1;// a civil is eliminated
	}
	else
		return 0;

}


void add_enermy(Civil* civil1,Civil* civil2){


	Enermys* enermy1 = (Enermys*)malloc(sizeof(Enermys));
	Enermys* enermy2 = (Enermys*)malloc(sizeof(Enermys));

	enermy1->id = civil2->id;
	enermy2->id = civil1->id;

	enermy1->next = civil1->enermys->next;
	civil1->enermys->next = enermy1;

	enermy2->next = civil2->enermys->next;
	civil2->enermys->next = enermy2;

}

int isEnermys(Civil* civil1,Civil* civil2){

	if(civil1==NULL || civil2==NULL){
		printf("error\n"); 
		return 0;
	}
	if(civil1->enermys->next==NULL)
		return 2;
	Enermys* enermy = NULL;
	enermy = civil1->enermys->next;
	if(enermy!=NULL){
		while(enermy->id!=civil2->id && enermy->next!=NULL)
			enermy=enermy->next;
	}
	if(enermy->id==civil2->id)
		return 1;
	else
		return 2;

}

void War(Civil* civils, Civil* civil1, Civil* civil2, int galaxy_id){

	long int chance = 0;
	int result =0;

	if(isEnermys(civil1,civil2)==2){
		add_enermy(civil1,civil2);
		if(civil1->faction>0)
			civil1->faction--;
		if(civil2->faction>0)
			civil2->faction--;
		
	}

	if(civil1->tech==0 && civil2->tech==0)
		return;
	chance = civil1->tech*100/(civil1->tech+civil2->tech);
	printf("*%d号文明取胜机会%ld\n",civil1->id,chance);
	
	if((rand()%100) < chance){
	//if(civil1->tech > civil2->tech){
		printf("战前:\n");
		PrintCivil(civil1);
		PrintCivil(civil2);

		printf("*%d号文明胜利，*%d号文明失败\n",civil1->id,civil2->id);
		civil1->tech=civil1->tech*1.05;
		civil2->tech=civil2->tech*0.9;
		civil1->success_count++;
		civil2->failure_count++;
		//civil2->faction++;

		if(space[galaxy_id].owner==civil1->id){
			return;
		}
		else if(space[galaxy_id].owner==civil2->id){
			result = LoseGalaxy(civils,civil1,civil2,galaxy_id);
			if(result==1){
				printf("*%d号文明消灭对手，获得对方1/10科技点\n",civil1->id);
				civil1->tech+=civil2->tech/10;
				if(civil1->faction>10)
					civil1->faction-=10;
			}
		}
		else{
			printf("error,owner=%d\n",space[galaxy_id].owner);
		}
	}
	else{
		printf("战前:\n");
		PrintCivil(civil1);
		PrintCivil(civil2);

		printf("*%d号文明胜利，*%d号文明失败\n",civil2->id,civil1->id);
		civil2->tech=civil2->tech*1.05;
		civil1->tech=civil1->tech*0.9;
		civil2->success_count++;
		civil1->failure_count++;
		//civil1->faction++;

		if(space[galaxy_id].owner==civil2->id){
			return;
		}
		else if(space[galaxy_id].owner==civil1->id){
			result = LoseGalaxy(civils,civil2,civil1,galaxy_id);
			if(result==1){
				printf("*%d号文明消灭对手，获得对方1/10科技点\n",civil2->id);
				civil2->tech+=civil1->tech/10;
				if(civil2->faction>10)
					civil2->faction-=10;
			}
		}
		else
			printf("error,owner=%d\n",space[galaxy_id].owner);

	}

}

int isFriends(Civil* civil1,Civil* civil2){


	if(civil1==NULL || civil2==NULL){
		printf("error\n"); 
		return 0;
	}
	if(civil1->friends->next==NULL)
		return 2;
	Friends* friend = NULL;
	friend = civil1->friends->next;
	if(friend!=NULL){
		while(friend->id!=civil2->id && friend->next!=NULL)
			friend=friend->next;
	}
	if(friend->id==civil2->id)
		return 1;
	else
		return 2;

}


void Communicate(Civil* civil1,Civil* civil2){

	int temp = 0;

	Friends* friend1 = (Friends*)malloc(sizeof(Friends));
	Friends* friend2 = (Friends*)malloc(sizeof(Friends));

	friend1->id = civil2->id;
	friend2->id = civil1->id;

	friend1->next = civil1->friends->next;
	civil1->friends->next = friend1;

	friend2->next = civil2->friends->next;
	civil2->friends->next = friend2;

	temp = civil1->tech;
	

	civil1->tech+=1;
	civil2->tech+=1;
	civil1->Commu_count++;
	civil2->Commu_count++;
	//civil1->tech=civil1->tech+(civil2->tech/1000);
	//printf("*%d号文明获得*%d号文明科技加成%d\n",civil1->id,civil2->id,civil2->tech/10);
	//civil2->tech=civil2->tech+temp/1000;
	//printf("*%d号文明获得*%d号文明科技加成%d\n",civil2->id,civil1->id,civil1->tech/10);
	
}

void Meet(Civil* civils, int id1, int id2, int galaxy_id){

	Civil* civil1 = NULL;
	Civil* civil2 = NULL;
	
	civil1 = FindCivil(civils,id1);
	if(civil1==NULL){
		printf("文明%d不存在\n",id1);
		return;
	}
	civil2 = FindCivil(civils,id2);
	if(civil2==NULL){
		printf("文明%d不存在\n",id2);
		return;
	}


	if(civil1->faction+civil2->faction<100){
		if(civil1->tech < civil2->tech){
			printf("*%d号文明遇到强大对手*%d号文明，努力发展科技\n",civil1->id,civil2->id);
                        if(rand()%20==1){
                                printf("*%d号文明在科学探索中引发灾难，科技倒退\n",civil1->id);
                                civil1->tech = civil1->tech/2;
                                civil1->tech_step = civil1->tech_step/10;
                        }

			civil1->tech_step++;
			civil1->tech=civil1->tech+civil1->tech_step*2;
			return;
		}
		printf("*%d号文明与*%d号文明在%d发生战争\n",id1,id2,galaxy_id);
		War(civils,civil1,civil2,galaxy_id);
	}
	else{
		if(isFriends(civil1,civil2)==2){
			printf("*%d号文明与*%d号文明发生友好交流\n",id1,id2);
			if(civil1->faction<100)
				civil1->faction++;
			if(civil2->faction<100)
				civil2->faction++;
			Communicate(civil1,civil2);
		}
	}
}


int main(int argc,char* argv[]){


	int Index=0;
	int i =0;
	int actions=0;

	SysInit();
	InitSpace();
	PrintSpace();
	Civil* civils = NULL;
	civils=InitCivils(civils);
	Civil* temp = NULL;
	Civil* temp1 = NULL;
	Enermys* enermy = NULL;
	Enermys* enermy1 = NULL;
	Friends* friend = NULL;
	Friends* friend1 = NULL;
	Galaxys* temp_g = NULL;


	for(Index=1;Index<CIVIL_NUM;Index++){
		temp = FindCivil(civils,Index);
		PrintCivil(temp);
	
	}

	int round=1;
	while(1){
		printf("round:%d=======================================================\n",round);		
		//sleep(1);
		for(i=1;i<CIVIL_NUM;i++){
			while((temp=FindCivil(civils,i))==NULL){
				i++;
				if(i>CIVIL_NUM)
					goto end;
			}
			if(livecivils==1){
				printf("%d号文明成为宇宙霸主\n",i);
				exit(1);
			}
			else
				printf("存活文明数量%d\n",livecivils);
			//actions when new round start===============================================================
			temp->tech+=temp->tech_step;
			if(temp->tech > MAX_TECH*2){
				printf("*%d号文明成为神级文明\n",i);
				sleep(5);
				//temp->id=0-temp->id;
				DeleteCivil(civils,temp->id);
				livecivils--;
				goto end;
				//exit(1);
			}
			if(temp->tech > MAX_TECH){
				printf("*%d号文明触及科学禁区\n",i);
				//temp->tech = MAX_TECH;
				temp->tech = temp->tech*0.65;
				temp->tech_step=1;
				if(temp->faction > 50)
					temp->faction-=50;
			}
			if(temp->tech>DANGER_TECH && rand()%10==1){
				printf("*%d号文明在科学探索中引发灾难，科技倒退\n",i);
				temp->tech = temp->tech/2;
				temp->tech_step = temp->tech_step/10;
			}
			if(temp->tech > DANGER_TECH && temp->faction>0)
				temp->faction--;
			if(temp->tech < DANGER_TECH/3 && temp->faction <100)
				temp->faction++;
			if(isHappen(50)==1){
				printf("*%d号文明发生科技爆炸\n",i);
				temp->tech_step=temp->tech_step*2+10;
			}
			printf("*%d号科技能力自然增长%d\n",i,temp->tech_step);
	
			//check friend list=========================================================================
			friend = temp->friends;
			while(friend->next!=NULL){
				temp1=FindCivil(civils,friend->next->id);
				if(temp1==NULL || temp->faction+temp1->faction<100){
					friend1=friend->next;
					if(friend1->next!=NULL)
						friend->next=friend1->next;
					else
						friend->next=NULL;
					free(friend1);
					friend1=NULL;
	
				}
				else{
					friend=friend->next;
				}
			}

			//start to expand
			actions=0;	
			while(actions < ACTIONS_EACH_ROUND){
				//fight with enermys=============================================================================
				enermy = temp->enermys;
				while(enermy->next!=NULL && actions<ACTIONS_EACH_ROUND){
					temp1 = FindCivil(civils,enermy->next->id);
					if(temp1!=NULL && temp->faction+temp1->faction<100){
						Meet(civils,temp->id,temp1->id,temp1->galaxys->next->id);	
						actions++;	
						enermy = enermy->next;
					}
					else{
						enermy1=enermy->next;
						if(enermy1->next!=NULL)
							enermy->next=enermy1->next;
						else
							enermy->next=NULL;
						free(enermy1);	
						enermy1 = NULL;
						//enermy=enermy->next;
					}
				}

				//=========search for new galaxys======================================================================
				Index = rand()%GALAXY_NUM;
				if(( space[Index].position.x < (space[temp->galaxys->next->id].position.x + temp->tech)) && space[Index].position.x > (space[temp->galaxys->next->id].position.x - temp->tech)){
					if(( space[Index].position.y < (space[temp->galaxys->next->id].position.y + temp->tech)) && space[Index].position.y > (space[temp->galaxys->next->id].position.y - temp->tech)){
						if(( space[Index].position.z < (space[temp->galaxys->next->id].position.z + temp->tech)) && space[Index].position.z > (space[temp->galaxys->next->id].position.z - temp->tech)){
							if(space[Index].owner==0){
								space[Index].owner=temp->id;
								temp_g = (Galaxys*)malloc(sizeof(Galaxy));
								temp_g->id = Index;
								temp_g->next = temp->galaxys->next;
								temp->galaxys->next= temp_g;					
								temp->tech=temp->tech*1.05;
								printf("*%d号文明发现新星系%d\n",i,Index);
								
							}
							else if(space[Index].owner==temp->id)
								NULL;
							else{
								Meet(civils,temp->id,space[Index].owner,Index);
							}
						}
							
					}
				}
				actions++;
				//=================================================================================

			}	
			actions=0;
			temp=FindCivil(civils,i);
			if(temp!=NULL)	
				PrintCivil(temp);


		}		
end:
		round++;

	}



	return 1;

}
