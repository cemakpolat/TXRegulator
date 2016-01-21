/*
 * BandwidthObserver.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: cem
 */

#include "BandwidthObserver.h"
#include <fstream>
#include "Utilities.h"
#include <vector>
#include <stdlib.h>
#include "APDME.h"
using namespace std;
std::string BandwidthObserver::className="BandwidthObserver";
std::vector<int> BandwidthObserver::bandwidthLoadList;

/**
 * singleton pattern required
 */
BandwidthObserver* BandwidthObserver::bandwidthObserver_Instance = NULL;
BandwidthObserver* BandwidthObserver::Instance() {
	if (!bandwidthObserver_Instance) {
		bandwidthObserver_Instance = new BandwidthObserver();
	}
	return bandwidthObserver_Instance;
}

BandwidthObserver::BandwidthObserver() {

	this->bandwidthRX=0;
	this->bandwidthTX=0;
	this->waitForCycle=APDME::Instance()->sysConf->bandwidthObserver_waitForObservationCycle;//20;
	this->filePath=APDME::Instance()->sysConf->bandwidthObserver_bandwidthFilePath;//"/proc/net/dev";
	this->loaded=false;
	this->threshold_loaded=APDME::Instance()->sysConf->bandwidthObserver_loadThreshold;//125000;//TODO:Sys Configuration
	//double totalBW=750000;//6MBits
	this->previousReceived=0;
	this->previousTransmitted=0;
}

BandwidthObserver::~BandwidthObserver() {

}

long BandwidthObserver::getBandwidthRate(){
	//cout<<"\n"<<endl;
	this->bandwidthRX=0;
	this->bandwidthTX=0;
	this->currentTransmitted=0;
	this->currentReceived=0;

	try
	{
		std::ifstream infile;
		infile.open(filePath.c_str());
		std::string sLine;
		char del=',';
		while (!infile.eof())
		{
			getline(infile, sLine);
			//cout<<"sLine: "<<sLine<<endl;
			space2comma(sLine);
			std::vector<std::string> temp=Utilities::split(sLine,del);

			for(unsigned int i=0;i<temp.size();i++){

				if (temp[i]=="wlan0:")// if (std::string::npos != temp[i].find("wlan0"))
				 {
					//remove comma
					 removeComma(temp,",");

					  this->currentTransmitted=atol(temp[i+9].c_str());
					  this->currentReceived=atol(temp[i+1].c_str());
					  long bytesPerSecondTx=this->currentTransmitted-this->previousTransmitted;
					  long bytesPerSecondRx=this->currentReceived-this->previousReceived;
					  this->previousReceived=this->currentReceived;
					  this->previousTransmitted=this->currentTransmitted;
					//  if(bytesPerSecondTx>this->bandwidthTX){
						  this->bandwidthTX=bytesPerSecondTx;
					 // }
					//  if(bytesPerSecondRx>this->bandwidthRX){
						  this->bandwidthRX=bytesPerSecondRx;
					 // }
					  string io;
					  io=className+": Received bytes on wlan0: "+Utilities::long_to_String(this->bandwidthRX);
					  Utilities::writeOutputInFile(io);
					  io=className+": Transmitted bytes from wlan0:"+Utilities::long_to_String(this->bandwidthTX);
					  Utilities::writeOutputInFile(io);

				 }
			}
		}
		infile.close();

	}
	catch(std::exception& e){
		std::cerr<<this->className<<e.what()<<std::endl;
		 Utilities::writeOutputInFile(className+":"+e.what());

	}

	long total=this->bandwidthRX+this->bandwidthTX;//TODO:received or transmitted bandwidth or together
	//long total=this->bandwidthRX;
	return total;
}
int BandwidthObserver::getBWCriticalMeasObservation(){
	Utilities::writeOutputInFile(className+": Critical Observation is probably invoked by Experimenter!");
	int criticalMesCount=0;
	pthread_mutex_lock(&BandwidthObserver::Instance()->mutex_bandwidthCounter);
	for(unsigned int i=0;i<bandwidthLoadList.size();i++){
		criticalMesCount+=bandwidthLoadList[i];
	}
	bandwidthLoadList.clear();
	//this->loaded_count=0; //Set the counter zero after this function is invoked.
	pthread_mutex_unlock(&BandwidthObserver::Instance()->mutex_bandwidthCounter);

	Utilities::writeOutputInFile(className+": Critical Observation Count :"+Utilities::convertIntToString(criticalMesCount));
	return criticalMesCount;

}

//short Term observer
void* BandwidthObserver::runObserver(void* obj){
	Utilities::writeOutputInFile("BandwidthObserver is activated..");

	BandwidthObserver *bw=(BandwidthObserver *)obj;
	bw->mutex_bandwidthCounter = PTHREAD_MUTEX_INITIALIZER;

	vector<long> bwList;
	double shortTerm_loadThreshold=bw->threshold_loaded;//20000;
	double consumedBW=0;
	int tenMinutesCounter=0;
	sleep(5);//for initialization

	while(true){
		consumedBW=bw->getBandwidthRate();
		if(consumedBW>=shortTerm_loadThreshold)
		{
			Utilities::writeOutputInFile(className+": Traffic Loaded!");
			bw->loaded_count++;
		}

		sleep(bw->waitForCycle);
		tenMinutesCounter++;
		if(tenMinutesCounter*bw->waitForCycle==600){//equivalent to 10 minutes
			tenMinutesCounter=0;
			pthread_mutex_lock(&BandwidthObserver::Instance()->mutex_bandwidthCounter);
			bw->bandwidthLoadList.push_back(bw->loaded_count);//shortTerm loaded
			pthread_mutex_unlock(&BandwidthObserver::Instance()->mutex_bandwidthCounter);
			bw->loaded_count=0; //Set the counter zero after this function is invoked.
			Utilities::writeOutputInFile(className+": Periodic Bandwidth Load Checker is invoked!");

		}
	}
	delete bw;
	pthread_exit(NULL);
}

 void BandwidthObserver::removeComma(std::vector<std::string>& list,std::string delimiter){
	 std::vector<std::string> temp;
	 for(unsigned int i=0;i<list.size();i++){
		 std::string a=list[i];
		 if(a!=delimiter){
			 temp.push_back(list[i]);
		 }
	 }
	 list=temp;
}

void BandwidthObserver::space2comma(std::string& sLine) {
	int i = 0;
	for (std::string::iterator it = sLine.begin(); it != sLine.end(); ++it) {
		if (*it == ' ' && i == 0) {
			*it = ',';
			i = 1;
		} else if (*it != ' ') {
			i = 0;
		}
	}
}


