/*
 * ConnTrack.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: cem
 */

#include "ConnTrack.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include "Utilities.h"
#include "APDME.h"

using namespace std;

std::string ConnTrack::className="ConnTrack";

/*Socket Path for Hostapd Messages*/
#define SOCKET_PATH "/var/run/uloop/hostapd_socket"

/*Buffer Length for received hostapd messages*/
#define BUFFLEN 4000

//singleton pattern required
ConnTrack* ConnTrack::conntrack_Instance = NULL;
ConnTrack* ConnTrack::Instance() {
	if (!conntrack_Instance)   // Only allow one instance of class to be generated.
		conntrack_Instance = new ConnTrack();
	return conntrack_Instance;
}


std::vector<ConnTrack::TXState> ConnTrack::txStateList;
std::vector<std::string> ConnTrack::disconnectedUserList;
const string ConnTrack::DISASSOCIATED="Disassociation";
const string ConnTrack::DEAUTHENTICATED="Deauthentication";
const string ConnTrack::AUTHENTICATED="Authentication";

ConnTrack::ConnTrack() {
	system("mkdir /var/run/uloop");

	this->expWaitingDurationRate=1;
	this->experimentWaitingDuration=APDME::Instance()->sysConf->conntrack_experimentWaitingDuration;//2*60;//1 Hour for an experiment
	this->txAssignmentCountWillBeConsidered=APDME::Instance()->sysConf->conntrack_txAssignmentCountWillBeConsidered;//20;//Consider 20 Measurements
	this->highestExperimentDurationRate=APDME::Instance()->sysConf->conntrack_highestExperimentDuration;//300;//300 sn
	this->txIncreaseRatioForConnTrack=1; //Increase ration for transmission power
	this->min_acceptable_message_prob=APDME::Instance()->sysConf->conntrack_min_acceptable_message_prob;
	this->window_size=APDME::Instance()->sysConf->conntrack_window_size;
	this->startTXStateTable();
	this->initiateGMMComponentParameters();
	this->mutex_disconnectionCounter = PTHREAD_MUTEX_INITIALIZER;
	this->isThisFirstExtension=true;
	this->preExtentedDurationforTXPower=0;

}
/**
 * Start the TX State Table
 */
void ConnTrack::startTXStateTable(){

	int maxTXValue=APDME::Instance()->tpc->iw->maxSignalThreshold; //TODO:: uncomment here
	int minTXValue=APDME::Instance()->tpc->iw->minSignalThreshold;

	int threshold=APDME::Instance()->sysConf->conntrack_table_threshold;

	for (int i = minTXValue; i < maxTXValue; i++) {
		TXState st;
		st.txpower=i;
		st.mesCount=0;
		if(i<threshold){//		if(i<threshold+minTXValue){
			st.state=true;
			for (int i = 0; i < window_size; i++) {
				st.measurements.push_back(5);
			}
		}else{
			st.state=false;
			for (int i = 0; i < window_size; i++) {
				st.measurements.push_back(0);
			}
		}
		this->txStateList.push_back(st);

	}
	for (unsigned int i = 0; i < this->txStateList.size(); i++) {
		for (int k = 0; k < window_size; k++) {
			cout<< this->txStateList[i].measurements[k];
		}
		cout<<endl;
	}
}

void ConnTrack::initiateGMMComponentParameters(){
	this->componentCount =2;//APDME::Instance()->sysConf->conntrack_componentCount;// 2;	//GMM component number
	this->epsilon =1e-5;//APDME::Instance()->sysConf->conntrack_epsilon;// 1e-6; //EM epsilon for stopping the log likelihood function
	this->means.resize(this->componentCount); //GMM means
	this->sigmas.resize(this->componentCount); //GMM sigmas or covariance matrixes

	//this value would be later determined with the aid of a new EM algorithm
	this->means[0]=APDME::Instance()->sysConf->conntrack_mean_1;//1;
	this->means[1]=APDME::Instance()->sysConf->conntrack_mean_2;//5;
	this->sigmas[0]=APDME::Instance()->sysConf->conntrack_sigma_1;//0.2;
	this->sigmas[1]=APDME::Instance()->sysConf->conntrack_sigma_2;//0.5;
	Utilities::writeOutputInFile(className+": initiateGMMComponentParameters  ");

}
ConnTrack::~ConnTrack() {
}

void* ConnTrack::run(void* obj) {
	Utilities::writeOutputInFile("Connection Tracker is activated...");

	ConnTrack *ct = (ConnTrack *) obj;
	pthread_t listener, timer;

	int rc;
	if ((rc = pthread_create(&listener, NULL, &ct->listenHostapdMessage, NULL))) {
		printf("ITG Checker thread creation failed: %d\n", rc);
	}
	if ((rc = pthread_create(&timer, NULL, &ct->disconnectionCounterTimer, NULL))) {
		printf("ITG Checker thread creation failed: %d\n", rc);
	}
	pthread_join(listener, NULL);
	pthread_join(timer, NULL);

	pthread_exit(NULL);
}

int ConnTrack::runExperiment() {

	std::vector<double> sourceList;
	std::vector<long double> newTaos;
	newTaos.resize(this->componentCount);
	EM *em = new EM();
	this->getCurentTXList(sourceList);
	em->initiateParameters(sourceList,this->componentCount,this->means,this->sigmas);
	em->runEM(1e-6);
	em->getMixtureCoefficients(newTaos);
	//string io="";
	int decision =0;
	decision=this->isGMMCritical(newTaos);

	delete em;
	return decision;
}

bool ConnTrack::isGMMCritical(vector<long double>& tao) {
	bool state = false;
	double tao1=1-min_acceptable_message_prob;

	string io="";//="Tao 1 ->"+Utilities::long_to_String(tao[0])+" Tao 2->"+Utilities::long_to_String(tao[1]);
	for(unsigned int i=0;i<tao.size();i++){
			//cout<<tao[i]<<" ";
			io=io+"tao "+Utilities::convertIntToString(i)+"->"+Utilities::convertLongDoubleToString(tao[i])+" ";
	}
	Utilities::writeOutputInFile(className+": "+io);
	if(tao[0] == 0 && tao[1] == 0){
		state=false;
	}
	else if (tao[0]!=0 && tao[0] >= min_acceptable_message_prob) {
		io="non-critical";
		Utilities::writeOutputInFile(className+": "+io);
		state = false;//means the state is non critical
	} else if(tao[1] !=0 && tao[1] >tao1) {
		io="critical";
		state = true;//means state critical
		Utilities::writeOutputInFile(className+": "+io);

	}
	return state;
}

void ConnTrack::getCurentTXList(vector<double>& sourceList){
	for(unsigned int i=0;i<this->txStateList.size();i++){
		if(this->currentTXPower==this->txStateList[i].txpower){
			sourceList=this->txStateList[i].measurements;
		}
	}
}

void* ConnTrack::listenHostapdMessage(void*) {

	/**
	 * Unix Socket Communication
	 */
	struct sockaddr_un strAddr;
	socklen_t lenAddr;
	int fdSock;
	int fdConn;
	char str[BUFFLEN];
	try{
		/*Open a socket in UNIX domain*/
		if ((fdSock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			//exit(1);
		}
		/*Unlink the path in order to prevent the creation of the conflict on the same file*/
		unlink(SOCKET_PATH);

		/* Unix Domain */
		strAddr.sun_family = AF_UNIX;

		/*Insert Path in parameters*/
		strcpy(strAddr.sun_path, SOCKET_PATH);
		lenAddr = sizeof(strAddr.sun_family) + strlen(strAddr.sun_path);

		/*Bind socket*/
		if (bind(fdSock, (struct sockaddr*) &strAddr, lenAddr) != 0) {
			perror("bind");
			//exit(1);
		}
		/*Start to listen to the clients*/
		if (listen(fdSock, 5) != 0) {
			perror("listen");
			//exit(1);
		}
		int n=0;
		HAPMessage1 message;
		for (;;) {
			/*Accept client and build a connection between client and server, and continue to the listen*/
			if ((fdConn = accept(fdSock, (struct sockaddr*) &strAddr, &lenAddr))
					>= 0) {
				printf("\n------------| Hostapd Message|--------------- \n");
				/*Receive data*/
				n = recv(fdConn, str, sizeof(str), 0);
				if (n <= 0) {
					if (n < 0)
						perror("recv");
				}
				printf("ConnTrack: Message From Hostapd:\n%s", str);

				ConnTrack::Instance()->convertMessageToHAPForm(str,message);
				ConnTrack::Instance()->interpretMessage(message);
				printf("\n---------------------------------------------- \n");
				/*Close established connection*/
				close(fdConn);
			}
		}
		/*Close the bound connection*/
		close(fdSock);

	} catch (exception& e) {
		cerr<<className  << ": ConnTrack: " << e.what() << endl;
		Utilities::writeOutputInFile(className+":  e.what()");

		sleep(5);
		ConnTrack::listenHostapdMessage(NULL);

	}
	pthread_exit(NULL);

}
void* ConnTrack::disconnectionCounterTimer(void*) {
	//ConnTrack::Instance()->startTXStateTable();
	//ConnTrack::Instance()->initiateGMMComponentParameters();

	vector<string> userMACList;
	int totalMessageCount=0;
	int mesCounterValue=0;
	int mesCounter=0;
	int userDown=0;
	//int controlForTXPower=5;
	//int whatTXPower=0;
	int limit=0;
	//bool first=true;
	while (true) {

		totalMessageCount=0;
		sleep(ConnTrack::Instance()->experimentWaitingDuration);
		Utilities::writeConsole(className+":Connection Tracker is tracking the received Message Number");
		ConnTrack::Instance()->currentTXPower=APDME::Instance()->tpc->currentTXPower;

		pthread_mutex_lock(&ConnTrack::Instance()->mutex_disconnectionCounter);
		userMACList=ConnTrack::Instance()->disconnectedUserList;
		int elementsWillBeRemoved=userMACList.size()/2;
		ConnTrack::Instance()->disconnectedUserList.erase(
				ConnTrack::Instance()->disconnectedUserList.begin(),
				ConnTrack::Instance()->disconnectedUserList.begin()
						+ elementsWillBeRemoved);
		if(elementsWillBeRemoved==0){
			ConnTrack::Instance()->disconnectedUserList.clear();
		}
		pthread_mutex_unlock(&ConnTrack::Instance()->mutex_disconnectionCounter);

		int counterValue=APDME::Instance()->mr->messageCounter;

		if(userDown==2 ){
			Utilities::writeOutputInFile(className+": User is sensed but user has some trouble while transmitting packets!");
			if(limit<5){
				APDME::Instance()->tpc->increaseTXPower(1);
				limit++;
			}else {
				limit=0;
			}
			userDown=0;
		}

		if (mesCounterValue == counterValue && userMACList.size() == 0) {
			mesCounter++;
		} else if (mesCounterValue == counterValue && userMACList.size() != 0) {
			userDown++;
		} else {
			mesCounter = 0;
		}

		if(mesCounter>5){
			Utilities::writeOutputInFile(className+": Connection Tracker receives any measurements, users have probably left the connection area,\n"
								"therefore, the connection tracking process will not be executed.");
		}else{
			totalMessageCount=userMACList.size();
			//Count the disconnection and connection messages of the clients

			int previousUserCounter=0;
			for (unsigned int i = 0; i < userMACList.size(); i++) {
				int counterForEachUser = 0;
				for (unsigned int j = 0; j < userMACList.size(); j++) {
					if (userMACList[i] == userMACList[j]) {
						counterForEachUser++;
					}
				}
				//Get Maximum Message Number
				if(counterForEachUser>previousUserCounter)
					previousUserCounter=counterForEachUser;
			}

			//add Measurement into the list
			ConnTrack::Instance()->addMeasurement(previousUserCounter,totalMessageCount);
			string io="Maximum Received Message Number "+Utilities::convertIntToString(previousUserCounter);
			Utilities::writeConsole(className+":"+io);

			//run EM and get Decision
			int decision=ConnTrack::Instance()->runExperiment();

			//decide increase or not decrease
			io="EM: Decision "+Utilities::convertIntToString(decision);
			Utilities::writeConsole(className+":"+io);

			if(decision){
				ConnTrack::Instance()->changeTXPowerState(true);
				Utilities::writeConsole(className+":Connection Tracker recommends to increase TX Power");
				//APDME::Instance()->tpc->setMinimumTXPower(ConnTrack::Instance()->getLastNonCriticalTX()+1);
				APDME::Instance()->tpc->increaseTXPower(1);

			}else{
				ConnTrack::Instance()->changeTXPowerState(false);
		//		APDME::Instance()->tpc->decreaseTXPower(ConnTrack::Instance()->txIncreaseRatioForConnTrack);
				Utilities::writeConsole(className+":Connection Tracker recommends to decrease TX Power");
				APDME::Instance()->tpc->setMinimumTXPower(ConnTrack::Instance()->getLastNonCriticalTX()-1);
			}
			//extend the time of EM
			ConnTrack::Instance()->assignedTXPowers.push_back(ConnTrack::Instance()->currentTXPower);
			ConnTrack::Instance()->experimentStabilityControl();
			Utilities::writeOutputInFile(className+": Last Conntrack Table values");
			for(unsigned int j=0;j<ConnTrack::Instance()->txStateList.size();j++){
				if(ConnTrack::Instance()->currentTXPower>=(ConnTrack::Instance()->txStateList[j].txpower-2))
				{
					for(unsigned int i=0;i<ConnTrack::Instance()->txStateList[j].measurements.size();i++){
							io=io+Utilities::convertIntToString(ConnTrack::Instance()->txStateList[j].measurements[i]);
					}
				io="TX: "+Utilities::convertIntToString(ConnTrack::Instance()->txStateList[j].txpower)+" MList: "+io;
				Utilities::writeOutputInFile(className+": "+io);
				}
				io="";
			}
		}
		mesCounterValue=counterValue;
	}
	pthread_exit(NULL);

}
void ConnTrack::changeTXPowerState(bool state){
	for (unsigned int i = 0; i < this->txStateList.size(); i++) {
		if (this->currentTXPower == this->txStateList[i].txpower) {
			this->txStateList[i].state=state;
			break;
		}
	}
	string io=" changeTXPowerState "+Utilities::convertIntToString(state);
	/*//Utilities::writeOutputInFile(className+":"+io);
	for (int i = 0; i < this->txStateList.size(); i++) {

		cout<<"Current "<<this->currentTXPower <<
			" List"	<<this->txStateList[i].txpower<<" "<<this->txStateList[i].state<<endl;;
	}*/
}
void ConnTrack::addMeasurement(int messageCount,int totalMessageCount){
	string io=Utilities::convertIntToString(messageCount);
	Utilities::writeConsole(className+":add new Measurement-> "+io);
	for(unsigned int i=0;i<this->txStateList.size();i++){
		if(this->currentTXPower==this->txStateList[i].txpower){
			this->txStateList[i].measurements.erase(this->txStateList[i].measurements.begin()+0);
			this->txStateList[i].measurements.push_back(messageCount);
			this->txStateList[i].mesCount=this->txStateList[i].mesCount+totalMessageCount;
			io="";
			for(unsigned int j=0;j<this->txStateList[i].measurements.size();j++){
				io=io+Utilities::convertIntToString(this->txStateList[i].measurements[j]);
			}
			io = "TX: "+ Utilities::convertIntToString(this->txStateList[i].txpower) +
					" Message Count: "+ Utilities::convertIntToString(this->txStateList[i].mesCount) +
					" Message List:" + io;
			Utilities::writeOutputInFile(className+": "+io);
			i=this->txStateList.size();
		}
	}
}
int ConnTrack::getLastNonCriticalTX(){
	int lastNonCriticalValue=4;
	for(int i=ConnTrack::Instance()->txStateList.size();i>0;i--){
		if(ConnTrack::Instance()->txStateList[i].state==true){
			cout<<"TX Value: "<<ConnTrack::Instance()->txStateList[i].txpower<<" State:"<<ConnTrack::Instance()->txStateList[i].state<<endl;
			lastNonCriticalValue=ConnTrack::Instance()->txStateList[i+1].txpower;
			i=0;
		}
	}
	string io="lastNonCriticalValue: "+Utilities::convertIntToString(lastNonCriticalValue);
	Utilities::writeOutputInFile(className+": "+io);
	return lastNonCriticalValue;//Transmission power //get the table
}
int ConnTrack::findOptimalTXPower(std::vector<int>& mostlyAssignedTXPowerList ){
	int minimumMessageNumber = 0;
	int txCandidate = 0;
	bool first = true;
	for (unsigned int i = 0; i < this->txStateList.size(); i++) {
		for (unsigned int j = 0; j < mostlyAssignedTXPowerList.size(); j++) {
			if (mostlyAssignedTXPowerList[j] == this->txStateList[i].txpower) {
				if (first) {
					minimumMessageNumber = this->txStateList[i].mesCount;
					txCandidate = mostlyAssignedTXPowerList[j];
					first = false;
				} else {
					if (this->txStateList[i].mesCount < minimumMessageNumber) {
						minimumMessageNumber = this->txStateList[i].mesCount;
					} else if (this->txStateList[i].mesCount== minimumMessageNumber) {
						if (mostlyAssignedTXPowerList[j]  > txCandidate) {
							txCandidate = mostlyAssignedTXPowerList[j];
						}
					}
				}
			}
		}
	}
	Utilities::writeOutputInFile(className+": selected TX Candidate->"+Utilities::convertIntToString(txCandidate));
	return txCandidate;
}
void ConnTrack::setAllValueFalseBeforeThis(int threshold){
	for (unsigned int i = 0; i < this->txStateList.size(); i++) {
		if (threshold>this->txStateList[i].txpower) {
			this->txStateList[i].state=true;
		}else{
			this->txStateList[i].state=false;
		}
	}
}
void ConnTrack::reAssignGMMParameters() {
	if (this->expWaitingDurationRate < 4) {
		this->means[0] = 1;
		this->means[1] = 9;
	} else if (this->expWaitingDurationRate > 3
			&& this->expWaitingDurationRate < 7) {
		this->means[0] = 3;
		this->means[1] = 12;
	} else if (this->expWaitingDurationRate > 6) {
		this->means[0] = 5;
		this->means[1] = 15;
	}

}
void ConnTrack::experimentStabilityControl(){
	Utilities::writeOutputInFile(className+": extendExperimentDuration  is called...");
	string io="";
	//if experiment is stable and long
	bool converged=true;
	for (unsigned int j = 0; j < assignedTXPowers.size(); j++) {
		io = io + Utilities::convertIntToString(assignedTXPowers[j]);
	}
	Utilities::writeOutputInFile(className + ": Assigned List: " + io);

	if(this->assignedTXPowers.size()==this->txAssignmentCountWillBeConsidered){
		//first part
		double mean=0;
		for(unsigned int i=0;i<this->assignedTXPowers.size();i++){
			mean=mean+this->assignedTXPowers[i];
		}
		mean=(double)mean/this->assignedTXPowers.size();
		for(unsigned int i=0;i<this->assignedTXPowers.size();i++){
			double difference=(double)(this->assignedTXPowers[i]-mean);
			if(difference>3){
				converged=false;
				Utilities::writeOutputInFile(className+": difference->"+Utilities::convertDoubleToString(difference));
			}
		}
		Utilities::writeOutputInFile(className+": convergence->"+Utilities::convertIntToString(converged));

		vector<int> mostlyAssignedTXPowerList;
		//second part, choose the minimum message count of one of the mostly assigned TX Power.
		int numberOfTXPower=0;
		for(unsigned int i=0;i<this->assignedTXPowers.size();i++){
			for(unsigned int j=0;j<this->assignedTXPowers.size();j++){
				if(this->assignedTXPowers[i]==this->assignedTXPowers[j]){
					numberOfTXPower++;
				}
			}
			if(numberOfTXPower>6){
				mostlyAssignedTXPowerList.push_back(this->assignedTXPowers[i]);
			}
			numberOfTXPower=0;
		}
		//find the minimum message number among the selected transmission power
		int valueWillBeAssigned=this->findOptimalTXPower(mostlyAssignedTXPowerList);
		cout<<"Current Power "<<this->currentTXPower<<" valueWillBeassigned "<<valueWillBeAssigned<<endl;
		Utilities::writeOutputInFile(
				className + ": Current Power->"
						+ Utilities::convertIntToString(this->currentTXPower)
						+ ", valueWillBeAssigned->"
						+ Utilities::convertIntToString(valueWillBeAssigned));
		if(valueWillBeAssigned!=0){
			if(this->currentTXPower<valueWillBeAssigned){
				int increaseRate = valueWillBeAssigned - this->currentTXPower;
				APDME::Instance()->tpc->increaseTXPower(increaseRate);
			}else if(valueWillBeAssigned<this->currentTXPower){
				APDME::Instance()->tpc->setMinimumTXPower(valueWillBeAssigned);
			}
		}else{
			converged=false;
		}
		this->setAllValueFalseBeforeThis(valueWillBeAssigned);
		APDME::Instance()->tpc->setMinimumTXPower(valueWillBeAssigned);

		//convergence test and the extension of the experiment time
		if(converged){
			Utilities::writeOutputInFile(className+"Assigned transmission power list is converged.");
			if(this->expWaitingDurationRate<this->highestExperimentDurationRate){
				if((preExtentedDurationforTXPower==valueWillBeAssigned) || (preExtentedDurationforTXPower==valueWillBeAssigned-1) ||(preExtentedDurationforTXPower==valueWillBeAssigned+1) || isThisFirstExtension){
					isThisFirstExtension=false;
					this->experimentWaitingDuration=experimentWaitingDuration/this->expWaitingDurationRate;
					this->expWaitingDurationRate++;
					this->experimentWaitingDuration=this->experimentWaitingDuration*this->expWaitingDurationRate;
					io="Experiment Duration is increased, new experiment duration ->"+Utilities::convertIntToString(this->experimentWaitingDuration);
					Utilities::writeOutputInFile(className+": "+io);
					this->reAssignGMMParameters();
				}else{
					Utilities::writeOutputInFile(className+": New stable transmission power is selected,\ntherefore new time extension will not be taken place.");
				}
				preExtentedDurationforTXPower=valueWillBeAssigned;
			}else{
				Utilities::writeOutputInFile(className+": Highest experiment duration is reached, no more duration increase is needed.");
			}
		}else {
			if(this->expWaitingDurationRate>1){
				this->experimentWaitingDuration=experimentWaitingDuration/this->expWaitingDurationRate;
				this->expWaitingDurationRate--;
				this->experimentWaitingDuration=this->experimentWaitingDuration*this->expWaitingDurationRate;
				io="Experiment Duration is decreased, new experiment duration ->"+Utilities::convertIntToString(this->experimentWaitingDuration);
				Utilities::writeOutputInFile(className+": "+io);
				this->reAssignGMMParameters();
			}
		}
		this->assignedTXPowers.clear();
	}
}
void ConnTrack::convertMessageToHAPForm(string str,HAPMessage1& message) {
	message.mac = "";
	message.message = "";
	vector<string> v0, v1;
	v0 = Utilities::split(str, ',');

	for (unsigned int i = 0; i < v0.size(); i++) {
		v1 = Utilities::split(v0[i], '=');
		if (v1[0] == "MAC") {
			message.mac = v1[1];
		} else if (v1[0] == "Message") {
			message.message = v1[1];
		}
	}
}
void ConnTrack::interpretMessage(HAPMessage1& message) {
	try {
		//Utilities::writeOutputInFile(className+": Hostapd Message is received:"+message.message);

		if (message.message == ConnTrack::DEAUTHENTICATED|| message.message == ConnTrack::DISASSOCIATED) {
			Utilities::writeOutputInFile(className+": Disconnected Message->  "+message.mac);

			pthread_mutex_lock(&ConnTrack::Instance()->mutex_disconnectionCounter);
			this->disconnectedUserList.push_back(message.mac);
			pthread_mutex_unlock(&ConnTrack::Instance()->mutex_disconnectionCounter);
		}
		if (message.message == ConnTrack::AUTHENTICATED) {

			Utilities::writeConsole(className+": Connected Message-> "+message.mac);
			pthread_mutex_lock(&ConnTrack::Instance()->mutex_disconnectionCounter);
			this->disconnectedUserList.push_back(message.mac);
			pthread_mutex_unlock(&ConnTrack::Instance()->mutex_disconnectionCounter);
		}

	} catch (exception& e) {
		cerr<<className  << ": ConnTrack : " << e.what() << endl;
		Utilities::writeOutputInFile(className+":  e.what()");

	}
}

void ConnTrack::computeDisconnectionThreshold(){
	// This can be done with EM algorithm for defining the received Message from Hostapd, check whether the message count are generally more than the
	// average value being determined by Gaussian Normal distribution...

}
