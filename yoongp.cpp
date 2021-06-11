//Patrick, Kok Weng, Yoong
//CS452 Winterim 2021
//Professor: Jack Tan
//T.A: Craig Schunk
//I would like to apologize for any profanities in the comments/names beforehand. I tried to get rid of as many as I could find but I might have missed a few

//Known bugs: When Queues ==2, some inputs might cause an infinite loop, cause unknown
//When time quantum is <10, for some cases, some new inputs wont be put into the queue, cuase is memory filling up
//Print errors, due to the algorithm used to calculate when a process is printed out/run, some runtimes/finish times do not match, although the printing order matches 

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <limits>
#include <numeric>
#include <algorithm>
#include <bits/stdc++.h>
#include <queue>

using namespace std;

	struct CPUData //Struct Definition
{
	int pid;
	int burst;
	int arrival;
	int priority;
};

//Global Data
double waitTime;
double turnAroundTime; 
bool demotionInProgress;
bool promotionInProgress;
bool jobInProgress;
int structlinecounter; //total process scheduled, would be used to calculate average wait time and average turnaround time as well
int numQueues; //pre-inputted by user
int timeQuantum; //pre-inputted by user
int Ageing; //pre-inputted by user
struct CPUData* CurrentJobs;
char* copiedstring = (char *)malloc(256);
char fileinput[10];

int clocktime; //used to keep track of arrival time and to calculate burst times
int secondclocktime; //to keep track of the before and after clocktime, like when processes are due to be checked if theyre done
int ageingclock; //used to check ageing
int fcfstimer; //used to keep track of the fcfs process and when it will be done
int demotiontimer; //used to keep track on when to demote a process
bool single=false; //used only when there is 1 queue in total

//Methods
int RR(int TimeQuantumMultiplier, int QueueNumber, struct CPUData pullfrommain); //RoundRobin
int FCFS(struct CPUData pullfrommain);//First Come First Serve
bool compareforsort(CPUData a, CPUData b); //Used as a comparator to sort using the Sort STL
void retrieveData(); //used to pull data from the 1M file and sort it using Sort STl
void userInput(); //used to get the initial input from user;
void QueuesFunc(); //The most important part, handles multiple queues, deciding whether the roundrobin/fcfs is used 
void finaloutput();
CPUData tiebreaker(CPUData a, CPUData b);
void ageingFunc(int clocktime);


int main() //main method
{	
CurrentJobs = (struct CPUData*) malloc(100000000 * sizeof(struct CPUData));
userInput();
retrieveData();
QueuesFunc();
finaloutput();
free(CurrentJobs);
return 0;
}//end of main


ostream& operator<< (ostream& os, CPUData const& p)
{
	os << p.pid << "," << p.burst << "," << p.arrival << "," << p.priority;
	return os;
}

void QueuesFunc()
{	
	int counter=0;
	float processTurnAroundTime;
	float processWaitTime;
	bool jobInProcess = false;
	bool demotionInProcess = false;
	bool promotionInProcess = false;
	queue <CPUData> Jobs [numQueues]; //need max 5
	struct CPUData temp;
	struct CPUData temp1;
	struct CPUData temp2;
	struct CPUData temp3;
	int i;
	int prevclocktime=0;
	int locallinstructcounter;
	
	while(counter<=structlinecounter+10  )
	{
	
		if((clocktime-prevclocktime)==Ageing && !Jobs[numQueues-1].empty() && numQueues !=1) //promotion method
		{
		promotionInProgress=true;
		prevclocktime=clocktime;
		
		temp = Jobs[numQueues].front();
		Jobs[numQueues-1].pop();
		Jobs[numQueues-2].push(temp);
		cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
		}	
		
		for(i=0; i<structlinecounter; i++) //pulls all possible processses within that arrival time
		{
			if(CurrentJobs[i].arrival == clocktime && counter < structlinecounter) 
			{
				//push the current job into the queue
				cout<<"Process "<<CurrentJobs[i].pid<<": arrives @ "<<clocktime<<"\n";
				Jobs[0].push(CurrentJobs[i]);
				counter++;//used to cycle through numbers to add jobs to the queue, does not start the process in the queue	
			//	cout<<counter<<" this is countner but in th add method\n";
			}
		}
		if(counter>=structlinecounter || (numQueues==2 && counter==7)) //unknown bug exists here
			{
			//	cout<<counter<<" this is countner \n";
				counter++;
			}
			
		//Queue 1 Only uses FCFS, which is fully operational as of now
		if(numQueues == 1)
		{	
			if(!Jobs[0].empty() == true)
			{
				temp = Jobs[0].front();
				if(Jobs[0].front().burst>0)
				{
						clocktime--;
						Jobs[0].front().burst = FCFS(temp);
						clocktime++;
				}
					if(Jobs[0].front().burst == 0 && clocktime==fcfstimer )
					{
						
						for(i=0; i<structlinecounter; i++)
						{
						
							if(Jobs[0].front().pid == CurrentJobs[i].pid)
							{
								
								if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
								
						//	clocktime = clocktime + CurrentJobs[i].burst;
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";
							
							processTurnAroundTime = 0;
							
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							Jobs[0].front();
							Jobs[0].pop();
	
							fcfstimer=0;
							} //end of if condition
						}	//end of for loop
					}//end of fcfs condition
			}//end of condition to check if the queue is empty	
		}//end of numQueues ==1 
		
		if(numQueues ==2)
		{
				//first queue
			if(!Jobs[0].empty()==true)
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[0].front();
			
					Jobs[0].front();
					Jobs[0].pop();
					Jobs[1].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
						
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[0].front();
				Jobs[0].front().burst = RR(1,0,temp);
			}
				
					temp = Jobs[0].front();
					if(Jobs[0].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[0].front();
							Jobs[0].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of first queue
			
		 if(!Jobs[1].empty() == true && Jobs[0].empty())
			{
				temp = Jobs[1].front();
			
			if(jobInProgress ==false)
			{	
				Jobs[1].front().burst = FCFS(temp);
			}	
					 if(Jobs[1].front().burst == 0 )
					{
						for(i=0; i<structlinecounter; i++ && fcfstimer==clocktime)
						{
							if(temp.pid == CurrentJobs[i].pid )
							{
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime-1)<<" in the fcfs queue \n";
							
							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							Jobs[1].front();
							Jobs[1].pop();
							
							jobInProgress=false;
							demotionInProgress=false;
						
							clocktime=fcfstimer;
							clocktime--;
							fcfstimer=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							} //end of if condition
						}	//end of for loop
					}//end of fcfs condition		
			}//end of condition to check if the queue is empty	
		}//end of numQueues ==2
		
		if(numQueues ==3)
		{
			//first queue
			if(!Jobs[0].empty()==true)
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[0].front();
			
					Jobs[0].front();
					Jobs[0].pop();
					Jobs[1].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
						
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[0].front();
				Jobs[0].front().burst = RR(1,0,temp);
			}
				
					temp = Jobs[0].front();
					if(Jobs[0].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[0].front();
							Jobs[0].pop();				
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of first queue
			
			//2nd Queue, RR
			if(!Jobs[1].empty()==true && Jobs[0].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[1].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[1].front();
					Jobs[1].pop();
					Jobs[2].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[1].front();
				Jobs[1].front().burst = RR(2,1,temp);
			}	
					temp = Jobs[1].front();
					if(Jobs[1].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[1].front();
							Jobs[1].pop();
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of first queue
				
			//3rd Queue aka Last Queue
			 if(!Jobs[2].empty() == true && Jobs[1].empty() && Jobs[0].empty())
			{
				temp = Jobs[2].front();
			//	cout<<temp<<"this is the fucking data im sending in \n";
			
			if(jobInProgress ==false)
			{	
				Jobs[2].front().burst = FCFS(temp);
			}
				
					 if(Jobs[2].front().burst == 0 )
					{
						for(i=0; i<structlinecounter; i++ && fcfstimer==clocktime)
						{
							if(temp.pid == CurrentJobs[i].pid )
							{
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime-1)<<" in the fcfs queue \n";
							
							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							Jobs[2].front();
							Jobs[2].pop();
							
							jobInProgress=false;
							demotionInProgress=false;
						
							clocktime=fcfstimer;
							clocktime--;
							fcfstimer=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							} //end of if condition
						}	//end of for loop
					}//end of fcfs condition		
			}//end of condition to check if the queue is empty		
		}//end of numQueues ==3
	
		if(numQueues ==4)
		{
			//first queue
			if(!Jobs[0].empty()==true)
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[0].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[0].front();
					Jobs[0].pop();
					Jobs[1].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
							
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[0].front();
				Jobs[0].front().burst = RR(1,0,temp);
			}	
					temp = Jobs[0].front();
					if(Jobs[0].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[0].front();
							Jobs[0].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of first queue
			
			//2nd Queue, RR
			if(!Jobs[1].empty()==true && Jobs[0].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[1].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[1].front();
					Jobs[1].pop();
					Jobs[2].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
				
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[1].front();
				Jobs[1].front().burst = RR(2,1,temp);
			}
				temp = Jobs[1].front();
					if(Jobs[1].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[1].front();
							Jobs[1].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of 2nd Queue
			
			//3rd Queue, RR
			if(!Jobs[2].empty()==true && Jobs[0].empty() && Jobs[1].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[2].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[2].front();
					Jobs[2].pop();
					Jobs[3].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
				
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[2].front();
				Jobs[2].front().burst = RR(3,2,temp);
			}
					
					if(Jobs[2].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[2].front();
							Jobs[2].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of 3rd Queue
						
			//4th Queue aka last queue
			 if(!Jobs[3].empty() == true && Jobs[0].empty() && Jobs[1].empty() && Jobs[2].empty())
			{
				temp = Jobs[3].front();
			//	cout<<temp<<"this is the fucking data im sending in \n";
			if(jobInProgress ==false)
			{	
				Jobs[3].front().burst = FCFS(temp);
			}
					 if(Jobs[3].front().burst == 0 && fcfstimer==clocktime)
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid  )
							{
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime-1)<<" in the fcfs queue \n";
							
							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							Jobs[3].front();
							Jobs[3].pop();
							
							jobInProgress=false;
							demotionInProgress=false;
						if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							clocktime=fcfstimer;
							clocktime--;
							fcfstimer=0;
							} //end of if condition
						}	//end of for loop
					}//end of fcfs condition		
			}//end of condition to check if the queue is empty		
		}//end of numQueues ==4
	
		if(numQueues ==5)
		{	
			//first queue
			if(!Jobs[0].empty()==true)
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[0].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[0].front();
					Jobs[0].pop();
					Jobs[1].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
							
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[0].front();
				Jobs[0].front().burst = RR(1,0,temp);
			}
					temp = Jobs[0].front();
					if(Jobs[0].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[0].front();
							Jobs[0].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of first queue
						
			//2nd Queue, RR
			if(!Jobs[1].empty()==true && Jobs[0].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[1].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[1].front();
					Jobs[1].pop();
					Jobs[2].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
				
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[1].front();
				Jobs[1].front().burst = RR(2,1,temp);
			}
					temp = Jobs[1].front();
					if(Jobs[1].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid== CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[1].front();
							Jobs[1].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of 2nd Queue	
			
			//3rd Queue, RR
			if(!Jobs[2].empty()==true && Jobs[0].empty() && Jobs[1].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[2].front();
				//	cout<<"pid: "<<temp.pid<<" burst: "<<temp.burst<<"\n";
					Jobs[2].front();
					Jobs[2].pop();
					Jobs[3].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}
				
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[2].front();
				Jobs[2].front().burst = RR(3,2,temp);
			}
					temp=Jobs[2].front();
					if(Jobs[2].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							
							jobInProgress=false;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[2].front();
							Jobs[2].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of 3rd Queue
				
			//4th Queue, RR
			if(!Jobs[3].empty()==true && Jobs[0].empty() && Jobs[1].empty() && Jobs[2].empty())
			{		
				if(demotionInProgress==true && clocktime==demotiontimer)
				{
					temp = Jobs[3].front();
	
					Jobs[3].front();
					Jobs[3].pop();
					Jobs[4].push(temp);
					secondclocktime=0;
					
					cout<<"Process "<<temp.pid<<": switched @ "<<(clocktime)<<"\n";	
					
					demotiontimer=0;
					demotionInProgress=false;
					jobInProgress=false;
				}		
				
			if(demotiontimer==clocktime || demotionInProgress==false || jobInProgress==false)
			{
				temp = Jobs[3].front();
				Jobs[3].front().burst = RR(4,3,temp);
			}
					temp=Jobs[3].front();
					if(Jobs[3].front().burst <= 0 && clocktime == secondclocktime )
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid)
							{
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							jobInProgress=false;
							
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime)<<"\n";

							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							
							clocktime=secondclocktime;
							secondclocktime=0;
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							Jobs[3].front();
							Jobs[3].pop();
											
							} //end of if condition
						}	//end of for loop
					}//end of condition to check if burst if over	
			}//end of 3rd Queue
					
			//4th Queue aka last queue
			 if(!Jobs[4].empty() == true && Jobs[0].empty() && Jobs[1].empty() && Jobs[2].empty() && Jobs[3].empty())
			{
				temp = Jobs[4].front();
			//	cout<<temp<<"this is the fucking data im sending in \n";
			if(jobInProgress ==false)
			{	
				Jobs[4].front().burst = FCFS(temp);
			}
					if(Jobs[4].front().burst == 0 && fcfstimer==clocktime)
					{
						for(i=0; i<structlinecounter; i++)
						{
							if(temp.pid == CurrentJobs[i].pid )
							{
							if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							cout<<"Process "<<CurrentJobs[i].pid<<": finishes @ "<<(clocktime-1)<<" in the fcfs queue \n";
							
							processTurnAroundTime = 0;
							processTurnAroundTime = clocktime - CurrentJobs[i].arrival;
							turnAroundTime += processTurnAroundTime;
			
							processWaitTime = 0;
							processWaitTime = (processTurnAroundTime - CurrentJobs[i].burst);
							waitTime += processWaitTime;
							Jobs[4].front();
							Jobs[4].pop();
							
							jobInProgress=false;
							demotionInProgress=false;
						if(CurrentJobs[i].burst > clocktime)
							{
								clocktime=CurrentJobs[i].burst;
							}
							clocktime=fcfstimer;
							clocktime--;
							fcfstimer=0;
							} //end of if condition
						}	//end of for loop
					}//end of fcfs condition		
			}//end of condition to check if the queue is empty		
		}//end of numQueues ==5
	
	clocktime++;	
	}//end of while loop
}//end of queue method

int FCFS(struct CPUData pullfrommain)
{
	struct CPUData temp = pullfrommain;

	 if(numQueues == 1 ) //only used for the first queue and if its only one queue
	{
		jobInProgress = true;
		if(clocktime<=0)
		{
				cout<<"Process "<<temp.pid<<": runs @ 0"<<" for "<<temp.burst<<" \n";
		}
	
		else 
		{
				cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<temp.burst<<"\n";
		}
	
		fcfstimer = clocktime + temp.burst;
		temp.burst=0;
		return temp.burst;
	}
	
	 if(fcfstimer == 0 && jobInProgress == false && numQueues != 1)
	{
		cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<temp.burst<<" in fcfs queue \n";
		
		fcfstimer = clocktime + temp.burst;
		demotionInProgress=false;
		jobInProgress=true;
	
		temp.burst=0;
		return temp.burst;
	}

}

int RR(int TimeQuantumMultiplier, int QueueNumber , struct CPUData pullfrommain)
{
	struct CPUData temp = pullfrommain;
	int localTimeQuantum = timeQuantum * TimeQuantumMultiplier;
	
	if(clocktime<=0 && temp.burst < localTimeQuantum && temp.burst!=0 && jobInProgress == false)  //no demotion, instantly finishes
	{
	 cout<<"Process "<<temp.pid<<": runs @ 0 for "<<temp.burst<<"\n";
	 secondclocktime = (temp.burst + clocktime); 
	 jobInProgress = true;
	temp.burst = (temp.burst-localTimeQuantum);
	return temp.burst;
	}
	
	else if(clocktime<=0 && temp.burst > localTimeQuantum && temp.burst!=0 && jobInProgress == false) //has demotion
	{
	cout<<"Process "<<temp.pid<<": runs @ 0 for "<<localTimeQuantum<<"\n";
	 demotionInProgress=true;
	 demotiontimer= clocktime + localTimeQuantum;
	 jobInProgress=true;
	 
	 temp.burst = (temp.burst-localTimeQuantum);
	 return temp.burst;
	}
	
	else if(clocktime<=0 && temp.burst == localTimeQuantum && temp.burst!=0 && jobInProgress == false) //no demotion, instantly finishes
	{
	cout<<"Process "<<temp.pid<<": runs @ 0"<<" for "<<localTimeQuantum<<"\n";
	 jobInProgress=true;
	 secondclocktime = (temp.burst + clocktime); 
	 
	 temp.burst = (temp.burst-localTimeQuantum);
	return temp.burst;
	}
	
    else if(clocktime!=0 && temp.burst - localTimeQuantum <= 0  && secondclocktime == 0 && temp.burst!=0 && jobInProgress == false) //the process only runs when the round robin is 1 only after the first proces is done 
	{
	 cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<temp.burst<<"\n";
	 secondclocktime = (temp.burst + clocktime); 
	 jobInProgress = true;
	 
	temp.burst = (temp.burst-localTimeQuantum);
	return temp.burst;
	}
	
	else if(clocktime!=0 && temp.burst - localTimeQuantum > 0  && secondclocktime == 0 && temp.burst!=0 && jobInProgress == false) //requires switching, only after the first process is done 
	{
	 cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<localTimeQuantum<<"\n";
	// cout<<"I am gonna demote the shit outta you \n";
	 secondclocktime = (temp.burst + clocktime);
	 demotionInProgress=true;
	 demotiontimer= clocktime + localTimeQuantum;
	 jobInProgress=true;
	 temp.burst = (temp.burst-localTimeQuantum);
	return temp.burst;
	}
	
	else if(clocktime!=0 && temp.burst - localTimeQuantum > 0  && temp.burst-localTimeQuantum != 0 && temp.burst!=0 && jobInProgress == false) //requires switching, only after the first process is done 
	{
	 cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<localTimeQuantum<<"\n";
	// cout<<"I am gonna demote the shit outta you \n";
	 secondclocktime = (temp.burst + clocktime);
	 demotionInProgress=true;
	 demotiontimer= clocktime + localTimeQuantum;
	 jobInProgress=true;
	 temp.burst = (temp.burst-localTimeQuantum);
	 return temp.burst;
	}
	
	else if(clocktime!=0 && temp.burst <= localTimeQuantum && temp.burst >= 0  && temp.burst!=0 && jobInProgress == false) //prints out the last run
	{
	cout<<"Process "<<temp.pid<<": runs @ "<<clocktime<<" for "<<temp.burst<<"\n";
	jobInProgress = true;
	
	temp.burst = (temp.burst-localTimeQuantum);
	return temp.burst;
	}
}

void finaloutput()
{
	double avWaitTime, avTurnaroundTime;
	avWaitTime = (waitTime / structlinecounter);
	avTurnaroundTime = (turnAroundTime/structlinecounter);
	if(avWaitTime>0)
	{
		avWaitTime * -1;
	}
	cout<<"Ave. wait time = " << avWaitTime << "\t Ave. turnaround time = " << avTurnaroundTime;
	cout<<"\nTotal processes scheduled = " << structlinecounter;
}

bool compareforsort(CPUData a, CPUData b)
{
    if (a.arrival == b.arrival && a.priority == b.priority && a.pid < b.pid)
	{
		return 1;
	}
	else if (a.arrival == b.arrival && a.priority == b.priority && a.pid > b.pid)
	{
		return 0;
	}
	else if(a.arrival == b.arrival && a.priority<b.priority)
	{
		return 1;
	}
	else if(a.arrival == b.arrival && a.priority>b.priority)
	{
		return 0;
	}
	else if(a.arrival<b.arrival)
	{
		return 1;
	}
	else if(a.arrival>b.arrival)
	{
		return 0;
	}
}//end of compareforsort

void retrieveData() //Retrieves the Data and Sorts it 
{
char firstline[100];
int pid, burst, arrival, priority,tempnum;
int i=0;
structlinecounter=structlinecounter=0;
bool negative=false;

FILE* readfile;
strcpy(copiedstring, fileinput);

if((readfile = fopen(copiedstring, "r")) == NULL)
{
	cout<<"File failed to open";
	exit(1);
}

fgets(firstline, 100, readfile);	//remove the first line of the file

while(fscanf(readfile, "%d", &tempnum) == 1 )
{
	i++;
	if(!negative)
	{
		if(tempnum < 0) //remooves negative number from queue
		{
			negative=true;
		}//sets the negative to true, skipping this line overall
			else
			{
				if(i==1)
				{
					pid = tempnum;
				}//end of i==1
				else if(i==2)
				{
					burst = tempnum;
				}//emd pf i==2
				else if(i==3)
				{
					arrival = tempnum;
				}//end of i==3
				else
				{
					priority = tempnum;
				}
				if(i==4)
				{
			 		CurrentJobs[structlinecounter].pid = pid;
					CurrentJobs[structlinecounter].burst = burst;
					CurrentJobs[structlinecounter].arrival = arrival;
					CurrentJobs[structlinecounter].priority = priority;
			   	 	structlinecounter++; //keeps track of structs
			 		i=0; //resets i to c to restart the processs
				}//end of i==4
			}//end of adding numbers to struct
}//end if if(negative)
	else if(i==4)
	{
		negative=false;
		i=0; //resets i to 0 to restart the process
		}	
}//end of while loop
sort(CurrentJobs,CurrentJobs+structlinecounter, compareforsort); //sorts all the structs using the Sort STl
free(copiedstring);
fclose(readfile);
} //end of retrive data

void userInput() //gets the user input for the initial prompt
{

bool temp=false;
 cout<<"\nSpecify file/file path to open: ";
cin >> fileinput;

cout<<"\nRunning MFQS ..... \n\n";

while(temp == false)
{
	cout<<"Enter number of queues: ";
	cin>>numQueues;
	if(numQueues > 0 && numQueues < 6 )
	{
		temp=true;
	}
	else
	{
		cout<<"Please input a number between 1 -5\n";
	}
}
cout<<"Enter time quantum for top queue: ";
cin>>timeQuantum;
cout<<"Enter the ageing interval: ";
cin>>Ageing;
cout<<"\n";

}
