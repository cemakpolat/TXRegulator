/*
 * WindowManager.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: cem
 */

#include "WindowManager.h"
#include "Utilities.h"
#include "APDME.h"

std::string WindowManager::className="WindowManager";

//int WindowManager::APPortNumber=APDME::Instance()->sysConf->APPortNumber;//13132;
//unsigned int WindowManager::listCount=APDME::Instance()->sysConf->maximumListSize;//30;
WindowManager* WindowManager::m_mrInstance = NULL;
WindowManager* WindowManager::Instance() {
	if (!m_mrInstance)   // Only allow one instance of class to be generated.
		m_mrInstance = new WindowManager();
	return m_mrInstance;
}
WindowManager::WindowManager() {
	this->APPortNumber=APDME::Instance()->sysConf->APPortNumber;//13132;
	this->listCount=APDME::Instance()->sysConf->winManager_stateListSize;//30;
	 this->critical_averageDelay_count=0;
	 this->critical_averageBitrate_count=0;
	 this->critical_averageJitter_count=0;
	// this->critical_averagePacketLoss_count=0;

	 this->critical_averageBitrate_value=APDME::Instance()->sysConf->winManager_averageBitrate_value;//10;
	 this->critical_averageDelay_value=APDME::Instance()->sysConf->winManager_averageDelay_value;//0.4;
	 this->critical_averageJitter_value=APDME::Instance()->sysConf->winManager_averageJitter_value;//0.2;
	// this->critical_averagePacketLoss_value=0.3;//APDME::Instance()->sysConf->winManager_averagePacketLoss_value;//0.25;
}

WindowManager::~WindowManager() {
}
/**
 * Main Component which starts every sub elements
 */
void* WindowManager::run(void* object){
	Utilities::writeOutputInFile("WindowManager is activated...");

	pthread_t mreceiver,slidingWindow;
	WindowManager *mr = (WindowManager*) object;
	int rc1;
	if ((rc1 = pthread_create(&mreceiver, NULL,&mr->runDITGMessageReceiver, NULL))) {
		printf("WindowManager:Thread creation failed: %d\n", rc1);
	}
	if ((rc1 = pthread_create(&slidingWindow, NULL,&mr->runSlidingWindow, NULL))) {
			printf("WindowManager:Thread creation failed: %d\n", rc1);
		}
	pthread_join(slidingWindow,NULL);
	pthread_join(mreceiver,NULL);

	pthread_exit(NULL);
}

void WindowManager::addSample(){
	Utilities::writeOutputInFile("WindowManager: New sample is added...");

	pthread_mutex_lock(&APDME::Instance()->mutex_window);//mutex list

	APDME::Instance()->DelayWindow.erase(APDME::Instance()->DelayWindow.begin() + 0);
	APDME::Instance()->DelayWindow.push_back(critical_averageDelay_count);

	APDME::Instance()->BitrateWindow.erase(APDME::Instance()->BitrateWindow.begin() + 0);
	APDME::Instance()->BitrateWindow.push_back(critical_averageBitrate_count);

	APDME::Instance()->JitterWindow.erase(APDME::Instance()->JitterWindow.begin() + 0);
	APDME::Instance()->JitterWindow.push_back(critical_averageJitter_count);

	//APDME::Instance()->PacketLossWindow.erase(APDME::Instance()->PacketLossWindow.begin() + 0);
	//APDME::Instance()->PacketLossWindow.push_back(critical_averagePacketLoss_count);
	pthread_mutex_unlock(&APDME::Instance()->mutex_window);

	critical_averageDelay_count=0;
	critical_averageBitrate_count=0;
	critical_averageJitter_count=0;
	//critical_averagePacketLoss_count=0;

}

//First List

void WindowManager::removePartiallyMeasurments(){
	Utilities::writeOutputInFile("WindowManager:  removePartiallyMeasurments is called...");

	int listSize=this->stateList_ADelay.size();
	int elementsWillBeRemoved=listSize/2;
	this->stateList_ADelay.erase(this->stateList_ADelay.begin(),this->stateList_ADelay.begin()+elementsWillBeRemoved);
	this->stateList_ABitrate.erase(this->stateList_ABitrate.begin(),this->stateList_ABitrate.begin()+elementsWillBeRemoved);
	this->stateList_AJitter.erase(this->stateList_AJitter.begin(),this->stateList_AJitter.begin()+elementsWillBeRemoved);
	//this->stateList_APacketLoss.erase(this->stateList_APacketLoss.begin(),this->stateList_APacketLoss.begin()+elementsWillBeRemoved);
}
void WindowManager::addSampleToExperimentList(){
	Utilities::writeOutputInFile("WindowManager: addSampleToExperimentList is called...");

	for(unsigned int i=0;i<this->stateList_ADelay.size();i++){
		if (this->stateList_ADelay[i] == 1) {

			this->critical_averageDelay_count++;
		}
		if (this->stateList_ABitrate[i] == 1) {
			this->critical_averageBitrate_count++;
		}
		if (this->stateList_AJitter[i] == 1) {
			this->critical_averageJitter_count++;
		}
//		if (this->stateList_APacketLoss[i] == 1) {
//			this->critical_averagePacketLoss_count++;
//		}
	}
	this->addSample();
}

void* WindowManager::runSlidingWindow(void*){
	//sleep(1*60);//first awaiting the element which will be stored during 4 minutes
	Utilities::writeOutputInFile("WindowManager:  Sliding Window Component starts...");
	unsigned int listSize=APDME::Instance()->sysConf->winManager_stateListSize;
	while(true){
		if(WindowManager::Instance()->stateList_ADelay.size()>=listSize){//30 //Element number in the list
			Utilities::writeOutputInFile("WindowManager:  New sample is added in QoS Lists...");
			WindowManager::Instance()->addSampleToExperimentList();
			WindowManager::Instance()->removePartiallyMeasurments();
		}
		string io="";
		io="Current State List Size : "+Utilities::convertIntToString(WindowManager::Instance()->stateList_ADelay.size());
		Utilities::writeOutputInFile(className+":"+io);

		sleep(30);
	}
	pthread_exit(NULL);
}
void* WindowManager::runDITGMessageReceiver(void* ){
	Utilities::writeOutputInFile("WindowManager:  QoS Measurement Receiver starts...");

	WindowManager *mr=WindowManager::Instance();
	WindowManager::Instance()->sock=new TCPServerSocket(mr->APPortNumber);
	mr->messageCounter=0;
	while (true) {
		sleep(2);
		QoSObject obj = mr->getMessageFromClient();
		//temporary code about the QoSObect, remove the following 5 lines
		if(!(obj.AverageBitRate==0 && obj.AverageDelay==0 && obj.AverageJitter==0 && obj.AveragePacketLoss==0)){
			mr->classifyMeasurement(obj);

		}
	//	pthread_mutex_lock(&WindowManager::Instance()->mutex_messageCounter);//mutex list
		mr->messageCounter++;
		if(mr->messageCounter==10000){
			mr->messageCounter=0;
		}
	//	pthread_mutex_unlock(&WindowManager::Instance()->mutex_messageCounter);

	}
	delete mr;
	pthread_exit(NULL);
}

void WindowManager::classifyMeasurement(QoSObject& obj) {


	//cout<<"average delay "<<obj.AverageDelay<<" "<<this->critical_averageDelay_value<<endl;
	if(obj.AverageDelay >= this->critical_averageDelay_value) {
	//	cout<<"critical "<<endl;
		this->stateList_ADelay.push_back(1);
	}else{
		//cout<<"not critical "<<endl;
		this->stateList_ADelay.push_back(0);
	}
	if(obj.AverageBitRate <= this->critical_averageBitrate_value) {
		this->stateList_ABitrate.push_back(1);
	}else{
		this->stateList_ABitrate.push_back(0);

	}
	if (obj.AverageJitter >= this->critical_averageJitter_value){
		this->stateList_AJitter.push_back(1);

	}else{
		this->stateList_AJitter.push_back(0);

	}
//	if (obj.AveragePacketLoss >= this->critical_averagePacketLoss_value) {
//		this->stateList_APacketLoss.push_back(1);
//
//	}else{
//		this->stateList_APacketLoss.push_back(0);
//
//	}

}

QoSObject WindowManager::getMessageFromClient() {

	string message = "";
	message = this->getMessageInSmallChunks(sock->accept()); //This will give a failure
	QoSObject o;
	o.AverageBitRate = 0;
	o.AverageDelay = 0;
	o.AverageJitter = 0;
	o.AveragePacketLoss = 0;
	if (message != "") {
		//cout << "Received Message From Client:\n" << message << endl;
		vector<string> v0, v2;
		v0 = Utilities::split(message, ',');

		for (unsigned int i = 0; i < v0.size(); i++) {
			//cout << "" << v0[i] << endl;
			v2 = Utilities::split(v0[i], ':');
			if (v2.size() > 0) {
				if (Utilities::contains(v2[0], "AverageDelay")) {
					o.AverageDelay = Utilities::convertStringDouble(v2[1]);
				} else if (Utilities::contains(v2[0], "AverageJitter")) {
					o.AverageJitter = Utilities::convertStringDouble(v2[1]);
				} else if (Utilities::contains(v2[0], "AveragePacketLoss")) {
					o.AveragePacketLoss = Utilities::convertStringDouble(v2[1]);
				} else if (Utilities::contains(v2[0], "AverageBitrate")) {

					o.AverageBitRate = Utilities::convertStringDouble(v2[1]);
				}
			} else {
				Utilities::writeOutputInFile("WindowManager:  QoS: Message does not contain network measurement");

			}
		}

		Utilities::writeOutputInFile("----------------------------------------");
		Utilities::writeOutputInFile(className+ ": Incoming QoS Measurements");
		Utilities::writeOutputInFile("-----------------------------------------");
		Utilities::writeOutputInFile(className+ ": AverageDelay: "+Utilities::convertDoubleToString( o.AverageDelay));
		Utilities::writeOutputInFile(className+ ": AverageBitRate: "+Utilities::convertDoubleToString( o.AverageBitRate));
		Utilities::writeOutputInFile(className+ ": AverageJitter: "+Utilities::convertDoubleToString( o.AverageJitter));
		Utilities::writeOutputInFile(className+ ": AveragePacketLoss: "+Utilities::convertDoubleToString( o.AveragePacketLoss));
		Utilities::writeOutputInFile("-----------------------------------------");


	}
	return o;

}
//Temporary Function, we benefit from the same function defined under TCPConnection
std::string WindowManager::getMessageInSmallChunks(TCPSocket *sock) {
	const unsigned int RCVBUFSIZE = 32;
	std::string str = "";
	try {
		// Establish connection with the echo server
		//	TCPSocket sock(servAddress, echoServPort);
		//cout<<className  << sock->getForeignAddress() << ":";
		//cout<<className  << sock->getForeignPort() << endl;
		char echoBuffer[RCVBUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = sock->recv(echoBuffer, RCVBUFSIZE)) > 0) { // Zero means// end of transmission

			// Echo message back to client
			//cout<<":received Message Size "<<recvMsgSize<<endl;
			echoBuffer[recvMsgSize] = '\0';
			//cout << ":echo " << echoBuffer << endl;
			str.append(echoBuffer);
			//sock->send(echoBuffer, recvMsgSize);
		}
		//cout << ":all " << str << endl;
		delete sock;
	} catch (SocketException &e) {
		cerr << e.what() << endl;

	}
	return str;
}



