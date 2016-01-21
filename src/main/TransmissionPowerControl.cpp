/*
 * TransmissionPowerControl.cpp
 *
 *  Created on: Jul 17, 2013
 *      Author: cem
 */

#include "TransmissionPowerControl.h"
#include "Utilities.h"
#include "APDME.h"
std::string TransmissionPowerControl::className="TransmissionPowerControl";
string TransmissionPowerControl::file_TXPowerAssignement = "TXPower.txt";
int TransmissionPowerControl::selectedDevice=2; //DEVICE 2 POINT OUT NETGEAR
int TransmissionPowerControl::minimumTXPowerDeterminedByConntrack=0;

//singleton pattern required
TransmissionPowerControl* TransmissionPowerControl::txpowerChanger_Instance = NULL;
TransmissionPowerControl* TransmissionPowerControl::Instance() {
	if (!txpowerChanger_Instance)   // Only allow one instance of class to be generated.
		txpowerChanger_Instance = new TransmissionPowerControl();
	return txpowerChanger_Instance;
}

TransmissionPowerControl::TransmissionPowerControl() {

	Utilities::writeOutputInFile("-----------------------------------------------");
	Utilities::writeOutputInFile(className+": Current IWConfig Information");
	Utilities::writeOutputInFile("-----------------------------------------------");

	this->selectedDevice=APDME::Instance()->sysConf->selectedRouter;
	this->iw = new IWConfig(this->selectedDevice);
	this->iw->scan();

	this->iw->setTransmissionPower(this->iw->maxSignalThreshold/2);
	this->currentTXPower=this->iw->iwobj->txPower; //get current tx power
	minimumTXPowerDeterminedByConntrack=iw->minSignalThreshold;

}
void TransmissionPowerControl::setMinimumTXPower(int recommendedTXByConnTrack){
	minimumTXPowerDeterminedByConntrack=recommendedTXByConnTrack;
	string io=""+Utilities::convertIntToString(minimumTXPowerDeterminedByConntrack);
	Utilities::writeOutputInFile(className+"Recommended Value by Conntrack Component -> "+io);
}
TransmissionPowerControl::~TransmissionPowerControl() {
}
void TransmissionPowerControl::setConnectionTrackerTreshold(int lastValue){
	this->connectionTrackingThresold=lastValue;
}
void TransmissionPowerControl::decreaseTXPower(int decreaseRate) {

	if(this->currentTXPower>(minimumTXPowerDeterminedByConntrack) && this->currentTXPower>iw->minSignalThreshold){
		currentTXPower = currentTXPower - decreaseRate;
		Utilities::writeOutputInFile(className+ ": Power is being decreasing... ");
		iw->setTransmissionPower(this->currentTXPower);
	}else{
		Utilities::writeOutputInFile(className+": AP was set to minimal transmission power");
	}
}

void TransmissionPowerControl::increaseTXPower(int increaseRate) {
	//Utilities::writeOutputInFile(className+": Transmission Power should be increased");
	if(this->currentTXPower<iw->maxSignalThreshold){
		currentTXPower = currentTXPower + increaseRate;
		Utilities::writeOutputInFile(className+ ": Power is being increasing... ");
		iw->setTransmissionPower(this->currentTXPower);
	}else{
		Utilities::writeOutputInFile(className+": AP was set to maximal transmission power");
	}

}

void TransmissionPowerControl::performAction(int action){
	int sensibleBWThresholdValue=APDME::Instance()->sysConf->tpc_criticalBandwidhtObserCount;//TODO:Sys Configuration
	int changeRate=1;
	if(action==TXPOWER_INCREASE){
		this->increaseTXPower(changeRate);
	}else if (action==TXPOWER_DECREASE){
		this->decreaseTXPower(changeRate);
	}
}

void TransmissionPowerControl::saveLastTXPowerInFile(){
	string output="";
	output="TXPower="+Utilities::convertIntToString(this->currentTXPower);
	Utilities::fileWrite(this->file_TXPowerAssignement,output);
}

void TransmissionPowerControl::readLastTXPowerFromFile(){
	string output="";
	string conf = Utilities::readFile2(this->file_TXPowerAssignement);
	vector<string> v1;
	v1 = Utilities::split(conf, '=');
	if (Utilities::contains(v1[0], "TXPower")) {
		this->lastTXPower=Utilities::convertStringToInt(v1[1]);
	}

}

