/*
 * QoSImprover.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: cem
 */

#include "QoSImprover.h"
#include <iostream>
#include "Utilities.h"
#include "APDME.h"

using namespace std;
std::string QoSImprover::className="QoSImprover";

QoSImprover* QoSImprover::m_expInstance = NULL;
QoSImprover* QoSImprover::Instance() {
	if (!m_expInstance)   // Only allow one instance of class to be generated.
		m_expInstance = new QoSImprover();
	return m_expInstance;
}

QoSImprover::QoSImprover() {
	this->expWaitingDurationRate=1;
	this->experimentWaitingDuration=APDME::Instance()->sysConf->qos_experimentWaitingDuration;//2*60;//1 Hour for an experiment
	this->txAssignmentCountWillBeConsidered=APDME::Instance()->sysConf->qos_txAssignmentCountWillBeConsidered;//20;//Consider 20 Measurements
	this->initiateGMMComponentParameters();

	this->qos_min_well_state_value=APDME::Instance()->sysConf->qos_min_acceptable_prob_well_state;
	this->qos_min_normal_state_value=APDME::Instance()->sysConf->qos_min_acceptable_prob_normal_state;
	this->qos_min_critical_state_value=APDME::Instance()->sysConf->qos_min_acceptable_prob_critical_state;
	this->isThisFirstExtension=true;
	this->preExtensionForWhichTXPower=0;
}

QoSImprover::~QoSImprover() {

}
void QoSImprover::extendExperimentDuration(){
	Utilities::writeOutputInFile(className+": extendExperimentDuration  is called...");
	string io="Assigned List: ";
	bool converged=true;
	for(unsigned int j=0;j<assignedTXPowers.size();j++){
				io=io+Utilities::convertIntToString(assignedTXPowers[j]);
	}
	Utilities::writeOutputInFile(className+": "+io);

	if(this->assignedTXPowers.size()==this->txAssignmentCountWillBeConsidered){
		double mean=0;
		for(unsigned int i=0;i<this->assignedTXPowers.size();i++){
			mean=mean+this->assignedTXPowers[i];
		}
		mean=mean/this->assignedTXPowers.size();
		cout<<"Assigned List Mean "<<mean<< endl;
		for(unsigned int i=0;i<this->assignedTXPowers.size();i++){
			double difference=(double)this->assignedTXPowers[i]-mean;
			cout<<"difference: "<<difference<<endl;
			if(difference>2){
				converged=false;
				Utilities::writeOutputInFile(className+": difference->"+Utilities::convertDoubleToString(difference));
			}
		}
		Utilities::writeOutputInFile(className+": convergence->"+Utilities::convertIntToString(converged));
		//second part, choose the minimum message count of one of the mostly assigned TX Power.
		vector<int> mostlyAssignedTXPowerList;
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
		int valueWillBeAssigned=APDME::Instance()->conntrack->findOptimalTXPower(mostlyAssignedTXPowerList);
		if (valueWillBeAssigned != 0) {
			if (APDME::Instance()->conntrack->currentTXPower< valueWillBeAssigned) {
				int increaseRate = valueWillBeAssigned- APDME::Instance()->conntrack->currentTXPower;
				APDME::Instance()->tpc->increaseTXPower(increaseRate);
			} else 	if (APDME::Instance()->conntrack->currentTXPower> valueWillBeAssigned) {
				int decreaseRate = valueWillBeAssigned- APDME::Instance()->conntrack->currentTXPower;
				APDME::Instance()->tpc->decreaseTXPower(decreaseRate);
			}
		} else {
			converged = false;
		}
		//part three
		if(converged){ //if the values are residing in the close proximity
			Utilities::writeOutputInFile(className+" Assigned transmission power list is converged.");
			if(preExtensionForWhichTXPower==valueWillBeAssigned || preExtensionForWhichTXPower==valueWillBeAssigned+1 || preExtensionForWhichTXPower==valueWillBeAssigned-1 || isThisFirstExtension){
				isThisFirstExtension=false;
				this->experimentWaitingDuration=experimentWaitingDuration/this->expWaitingDurationRate;
				this->expWaitingDurationRate++;
				this->experimentWaitingDuration=this->experimentWaitingDuration*this->expWaitingDurationRate;
				io="Experiment Duration is increased, new experiment duration ->"+Utilities::convertIntToString(this->experimentWaitingDuration);
				Utilities::writeOutputInFile(className+": "+io);
			}else{
				Utilities::writeOutputInFile(className+": Transmission power is changed, \ntherefore new time extension will not be taken place.");
			}
			preExtensionForWhichTXPower=valueWillBeAssigned;

		}else {
			Utilities::writeOutputInFile(className+" Assigned transmission power list isn't converged.");
			if(this->expWaitingDurationRate>1){
				this->experimentWaitingDuration=experimentWaitingDuration/this->expWaitingDurationRate;
				this->expWaitingDurationRate--;
				this->experimentWaitingDuration=this->experimentWaitingDuration*this->expWaitingDurationRate;
				io="Experiment Duration is decreased, new experiment duration ->"+Utilities::convertIntToString(this->experimentWaitingDuration);
				Utilities::writeOutputInFile(className+": "+io);
			}
		}
		this->assignedTXPowers.clear();

	}
}
void* QoSImprover::runExperimenter(void* data) {

	Utilities::writeOutputInFile("QoSImprover is activated...");

	QoSImprover *ex = (QoSImprover*) data;

	int decision = 1;
	int mesCounterValue=0;
	while (true) {
		//QoSImprover should know the bandwidth case
		sleep(ex->experimentWaitingDuration);

		int counterValue=APDME::Instance()->mr->messageCounter;

		if (mesCounterValue == counterValue) {
			Utilities::writeOutputInFile(className+ ": Window Manager receives any measurements, users have probably left the connection area,\n"
									"therefore, the QoS Improver process will not be executed");
		}else{

			if (APDME::Instance()->DelayWindow.size() > 0) {
				decision = ex->commonDecisionOnTXState(ex->aDelayExperiment(),ex->aBitrateExperiment(), ex->aJitterExperiment());
				string io;
				io="Taken decision by QoSImprover: "+decision;
				Utilities::writeOutputInFile(className+":"+io);

				if (decision == DECREASE) {
					Utilities::writeOutputInFile(className+": DECISION: Decrease Instruction is sent to Transmission Power Changer Module");
					APDME::Instance()->tpc->performAction(DECREASE);
				} else if (decision == INCREASE) {
					Utilities::writeOutputInFile(className+": DECISION: Increase Instruction is sent to Transmission Power Changer Module");
					APDME::Instance()->tpc->performAction(INCREASE);

				} else if (decision == DONOTHING) {
					Utilities::writeOutputInFile(className+": DECISION: Do Nothing, normal level");
					//cout<<className<<": DECISION: " << "Do Nothing, normal level" << endl;
				}
				ex->assignedTXPowers.push_back(APDME::Instance()->tpc->currentTXPower);
				ex->extendExperimentDuration();
				//cout << " \n" << endl;

				decision = 1;

			}
		}
		mesCounterValue=counterValue;
	}
	delete ex;
	pthread_exit(NULL);
}

// TODO:: Organize here
int QoSImprover::commonDecisionOnTXState(int ad, int ab, int aj) {
	int result = 0;
//	result = ad + ab + aj + apl;
	result = ad + ab + aj;

	if (ad == 2) {
		return INCREASE;
	}
	if (result <=1) {
		result = DECREASE;
	} else if (result > 1 && result <= 3) {
		result = DONOTHING;
	} else if (result > 3) {
		result = INCREASE;
	}
	return result;
}
//TODO:find best value for the evaluation and try to convert the static value below to a variable that might be assigned in System Configurations.
int QoSImprover::txStateDecisionToMixCoef(vector<long double>& tetas) {
	int state = 0;	//
	///TODO: Which parameters
	//cout<<tetas[0]<<" "<<tetas[1]<<" "<<tetas[2]<<endl;
	if (tetas[0] >= qos_min_well_state_value) {//0.8
		state = DECREASE;
	} else if (tetas[1] >= qos_min_normal_state_value) {//0.8
		state = DONOTHING;
	} else if (tetas[2] >= qos_min_critical_state_value) {//0.3
		state = INCREASE;
	} else if(tetas[0]==0 && tetas[1]==0 && tetas[2]==0){
		state = DECREASE;
	} else  {
		state = DONOTHING;
	}
	return state;
}

int QoSImprover::aDelayExperiment() {

	std::vector<double> sourceList;
	std::vector<long double> newTaos;
	newTaos.resize(this->componentCount);
	EM *em = new EM();
	string io="";
	pthread_mutex_lock(&APDME::Instance()->mutex_window);
	sourceList = APDME::Instance()->DelayWindow;
	pthread_mutex_unlock(&APDME::Instance()->mutex_window);

	//EM
	//sourceList=this->generateList();
	for(unsigned int j=0;j<sourceList.size();j++){
		io=io+Utilities::convertIntToString(sourceList[j]);
	}
	Utilities::writeOutputInFile(className+": Delay List-> "+io);

	em->initiateParameters(sourceList, this->componentCount,this->means,this->sigmas);
	em->runEM(epsilon);
	em->getMixtureCoefficients(newTaos);

	io = "ADelay: "+Utilities::convertLongDoubleToString(newTaos[0]) + " "
			+ Utilities::convertLongDoubleToString(newTaos[1]) + " "
			+ Utilities::convertLongDoubleToString(newTaos[2]);
	Utilities::writeOutputInFile(className+":"+io);

	int decision = this->txStateDecisionToMixCoef(newTaos);
	io="ADelay: Decision "+Utilities::convertIntToString(decision);
	Utilities::writeOutputInFile(className+":"+io);
	delete em;
	return decision;
}

int QoSImprover::aBitrateExperiment() {
	std::vector<double> sourceList;
	std::vector<long double> newTaos;
	newTaos.resize(this->componentCount);

	EM *em = new EM();
	string io="";
	pthread_mutex_lock(&APDME::Instance()->mutex_window);
	sourceList = APDME::Instance()->BitrateWindow;
	pthread_mutex_unlock(&APDME::Instance()->mutex_window);
	for(unsigned int j=0;j<sourceList.size();j++){
			io=io+Utilities::convertIntToString(sourceList[j]);
	}
	Utilities::writeOutputInFile(className+": Bitrate List-> "+io);

	em->initiateParameters(sourceList, this->componentCount,this->means,this->sigmas);
	em->runEM(epsilon);
	em->getMixtureCoefficients(newTaos);

	io = "ABitrate: "+Utilities::convertLongDoubleToString(newTaos[0]) + " "
			+ Utilities::convertLongDoubleToString(newTaos[1]) + " "
			+ Utilities::convertLongDoubleToString(newTaos[2]);
	Utilities::writeOutputInFile(className+":"+io);

	int decision = this->txStateDecisionToMixCoef(newTaos);
	io="ABitrate: Decision "+Utilities::convertIntToString(decision);
	Utilities::writeOutputInFile(className+":"+io);
	delete em;

	return decision;
}

int QoSImprover::aJitterExperiment() {
	std::vector<double> sourceList;
	std::vector<long double> newTaos;
	newTaos.resize(this->componentCount);

	EM *em = new EM();
	string io="";

	pthread_mutex_lock(&APDME::Instance()->mutex_window);
	sourceList = APDME::Instance()->JitterWindow;
	pthread_mutex_unlock(&APDME::Instance()->mutex_window);
	for(unsigned int j=0;j<sourceList.size();j++){
			io=io+Utilities::convertIntToString(sourceList[j]);
	}
	Utilities::writeOutputInFile(className+": Jitter List-> "+io);

	em->initiateParameters(sourceList, this->componentCount,this->means,this->sigmas);
	em->runEM(epsilon);
	em->getMixtureCoefficients(newTaos);

	io = "AJitter: "+Utilities::convertLongDoubleToString(newTaos[0]) + " "
				+ Utilities::convertLongDoubleToString(newTaos[1]) + " "
				+ Utilities::convertLongDoubleToString(newTaos[2]);
	Utilities::writeOutputInFile(className+":"+io);

	int decision = this->txStateDecisionToMixCoef(newTaos);
	io="AJitter: Decision "+Utilities::convertIntToString(decision);
	Utilities::writeOutputInFile(className+":"+io);
	delete em;
	return decision;
}

int QoSImprover::aPacketLossExperiment() {
	/*std::vector<double> sourceList;
	std::vector<long double> newTaos;
	newTaos.resize(this->componentCount);

	EM *em = new EM();
	string io="";
	pthread_mutex_lock(&APDME::Instance()->mutex_window);
	sourceList = APDME::Instance()->PacketLossWindow;
	pthread_mutex_unlock(&APDME::Instance()->mutex_window);
	for(unsigned int j=0;j<sourceList.size();j++){
		io=io+Utilities::convertIntToString(sourceList[j]);
	}
	Utilities::writeOutputInFile(className+": PaketLost List-> "+io);
	em->initiateParameters(sourceList, this->componentCount,this->means,this->sigmas);
	em->runEM(epsilon);
	em->getMixtureCoefficients(newTaos);
	//this->pktlLosstetas = newTaos;

	int decision = this->txStateDecisionToMixCoef(newTaos);
	io="APacketLoss: Decision "+Utilities::convertIntToString(decision);
	Utilities::writeOutputInFile(className+":"+io);
	delete em;
	io="";

	return decision;
*/
	return 0;
}

void QoSImprover::initiateGMMComponentParameters(){

	this->componentCount =APDME::Instance()->sysConf->qos_componentCount;// 3;
	this->epsilon =1e-6;//APDME::Instance()->sysConf->qos_epsilon;// 1e-6;
	this->means.resize(this->componentCount);
	this->sigmas.resize(this->componentCount);

	this->means[0]=APDME::Instance()->sysConf->qos_mean_1;//8;
	this->means[1]=APDME::Instance()->sysConf->qos_mean_2;//16;
	this->means[2]=APDME::Instance()->sysConf->qos_mean_3;//24;
	this->sigmas[0]=APDME::Instance()->sysConf->qos_sigma_1;//1;
	this->sigmas[1]=APDME::Instance()->sysConf->qos_sigma_2;//1;
	this->sigmas[2]=APDME::Instance()->sysConf->qos_sigma_3;//1;

}


