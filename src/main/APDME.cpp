/*
 * APDME.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: cem
 */

#include "APDME.h"
#include "Utilities.h"


int main(int argc, char* argv[]) {

	APDME *apd = APDME::Instance();
	apd->run();
}

std::string APDME::className="APDME";

//STATIC ASSIGNEMENTS
std::vector<double> APDME::DelayWindow;
std::vector<double> APDME::BitrateWindow;
std::vector<double> APDME::JitterWindow;
//std::vector<double> APDME::PacketLossWindow;

//singleton pattern required
APDME* APDME::m_apdmeInstance = NULL;
APDME* APDME::Instance() {
	if (!m_apdmeInstance)   // Only allow one instance of class to be generated.
		m_apdmeInstance = new APDME();
	return m_apdmeInstance;
}

APDME::APDME() {
	this->sysConf=SystemConfiguration::Instance();
	Utilities::writeOutputInFile("===============================================");
	Utilities::writeOutputInFile("Access Point Decision Making Engine is started");
	Utilities::writeOutputInFile("===============================================");
	//Initiate Experiment Lists In Well Situation
	for( int i=0;i<this->sysConf->apdme_windowSize;i++){
		DelayWindow.push_back(0);
		BitrateWindow.push_back(0);
		JitterWindow.push_back(0);
	//	PacketLossWindow.push_back(0);

	}
	this->mutex_window = PTHREAD_MUTEX_INITIALIZER;
}

APDME::~APDME() {
	delete exp;
	delete mr;
	//delete bw;
	delete tpc;
	delete conntrack;
	delete sysConf;
}

void APDME::run() {
	this->tpc = TransmissionPowerControl::Instance();
	this->conntrack = ConnTrack::Instance();
	//this->bw = BandwidthObserver::Instance();
	this->exp = QoSImprover::Instance();
	this->mr = WindowManager::Instance();

	Utilities::writeOutputInFile("-----------------------------------------------");
	Utilities::writeOutputInFile(className+": The required components are being loaded ...");
	Utilities::writeOutputInFile("-----------------------------------------------");

	pthread_t experimenter, mesreceiver,ditg,conntrackTread;
	//pthread_t bwObs;
	int rc;

	//run SNR measurement of the users
	if ((rc = pthread_create(&experimenter, NULL, &QoSImprover::runExperimenter, exp))) {
		printf("APDME : Experimenter thread creation failed: %d\n", rc);
	}
	//run Traffic Measurement of the user
	if ((rc = pthread_create(&mesreceiver, NULL, &WindowManager::run,mr))) {
		printf("APDME : WindowManager thread creation failed: %d\n", rc);
	}
	//run Connection Tracker
	if ((rc = pthread_create(&conntrackTread, NULL, &ConnTrack::run, conntrack))) {
		printf("APDME : ConnectionTracker thread creation failed: %d\n", rc);
	}
	//run Bandiwdth Observer
//	if ((rc = pthread_create(&bwObs, NULL, &BandwidthObserver::runObserver, bw))) {
//		printf("APDME : BandwidthObserver thread creation failed: %d\n", rc);
//	}
	//run ITG
	if ((rc = pthread_create(&ditg, NULL, &APDME::runITGChecker, NULL))) {
		printf("APDME : ITG Checker thread creation failed: %d\n", rc);
	}

	pthread_join(experimenter, NULL);
	pthread_join(mesreceiver, NULL);
	pthread_join(ditg,NULL);
	//pthread_join(bwObs,NULL);//start bw observer
	pthread_join(conntrackTread,NULL);
	cout<<"-----------------------------------------"<<endl;
}

int APDME::itgRestartDuration=600;
void* APDME::runITGChecker(void*) {
	sleep(1);
	Utilities::writeOutputInFile("D-ITG is activated ..");

	system("killall ITGRecv");

	while (true) {
		try {
			system("ITGRecv &");
			sleep(APDME::Instance()->itgRestartDuration);
			system("killall ITGRecv");
			//cout<<className <<": ITG is restarting..."<<endl;
			Utilities::writeOutputInFile(className+": D-ITG is restarting ..");
		}  catch (std::exception& e) {
			std::cerr<<className  << ": ITG Error:" << e.what() << std::endl;
			Utilities::writeOutputInFile(className+"D-ITG Error .."+e.what());
		}
	}
	pthread_exit(NULL);
}
