/*
 * SystemConfiguration.cpp
 *
 *  Created on: Aug 2, 2013
 *      Author: cem
 */

#include "SystemConfiguration.h"

std::string SystemConfiguration::className = "BandwidthObserver";

/**
 * singleton pattern required
 */
SystemConfiguration* SystemConfiguration::sysConf_Instance = NULL;
SystemConfiguration* SystemConfiguration::Instance() {
	if (!sysConf_Instance) {
		sysConf_Instance = new SystemConfiguration();
	}
	return sysConf_Instance;
}


SystemConfiguration::SystemConfiguration() {
	// TODO Auto-generated constructor stub
	apdme_windowSize=30;//Default
	//apdme_windowSize=30;
	setRouterConfiguration();
	setTransmissionPowerParameters();
	setConnTrackParameters();
	setQoSImproverParameters();
	//setBandwidthObserverParameters();
	setWindowManagerParameters();
}

SystemConfiguration::~SystemConfiguration() {
	// TODO Auto-generated destructor stub
}
void SystemConfiguration::setRouterConfiguration(){
	//Routers
	//1-Generic,2-Netgear,3-Voyage
	selectedRouter=2;
}
void SystemConfiguration::shortWindowsScenario(){

	qos_experimentWaitingDuration = 1 * 80; //For the first case is 2 Minutes and it means 6 Measurement since a measurement is taken in 20 s.
	qos_txAssignmentCountWillBeConsidered = 20;

	//GMM
	qos_componentCount = 3;
	qos_epsilon = 1e-6;
	qos_mean_1 = 2;
	qos_mean_2 = 4;
	qos_mean_3 = 6;
	qos_sigma_1 = .5;
	qos_sigma_2 = .5;
	qos_sigma_3 = .5;

	//window Manager List Size, it can maximally gather 8 critical values
	winManager_stateListSize=6;

}
void SystemConfiguration::longWindowsScenario(){
	//Default Configurations

	qos_experimentWaitingDuration = 10 * 60; //For the first case is 2 Minutes and it means 6 Measurement since a measurement is taken in 20 s.
	qos_txAssignmentCountWillBeConsidered = 20;

	//GMM
	qos_componentCount = 3;
	qos_epsilon = 1e-6;

	qos_mean_1 = 8;//2
	qos_mean_2 = 16;//4
	qos_mean_3 = 24;//6
	qos_sigma_1 = 0.5;//0.5
	qos_sigma_2 = 0.5;//0.5
	qos_sigma_3 = 0.5;//0.5

	//window Manager List Size, it can maximally gather 30 critical values
	winManager_stateListSize=30;
}
void SystemConfiguration::setConnTrackParameters() {
	conntrack_table_threshold=2;
	conntrack_experimentWaitingDuration = 1 * 60; //For the first case is 1 hour
	conntrack_txAssignmentCountWillBeConsidered = 20;
	conntrack_highestExperimentDuration = 10;//10 minutes
	//GMM
	conntrack_componentCount = 3;
	conntrack_epsilon = 1e-6;
	conntrack_mean_1 = 1;
	conntrack_mean_2 = 14;
	conntrack_sigma_1 = 0.3;
	conntrack_sigma_2 = 3;
	//window size
	conntrack_window_size=15;

	//Mixture thresholds
	conntrack_min_acceptable_message_prob=0.75;
}
//QoSImprover Configurations
void SystemConfiguration::setQoSImproverParameters() {
	//longWindowsScenario();
	shortWindowsScenario();

	//Mixture thresholds

	qos_min_acceptable_prob_well_state=0.8;
	qos_min_acceptable_prob_normal_state=0.8;
	qos_min_acceptable_prob_critical_state=0.3;

}
//Bandwidth Observer
void SystemConfiguration::setBandwidthObserverParameters() {

	bandwidthObserver_waitForObservationCycle = 20;
	bandwidthObserver_bandwidthFilePath = "/proc/net/dev";
	bandwidthObserver_loadThreshold = 1250000;//125000;
	tpc_criticalBandwidhtObserCount=20;
}
//Window Manager
void SystemConfiguration::setWindowManagerParameters() {
	APPortNumber = 13132;
	//maximumListSize = 30;
	//Default Window State List
	winManager_stateListSize=30;
	//winManager_stateListSize=6;//Each measurement lasts ca. 20 seconds //TODO:change the name here

	winManager_averageBitrate_value = 10;//50
	winManager_averageDelay_value = 0.6;//0.6
	winManager_averageJitter_value = 0.1;//0.1
	winManager_averagePacketLoss_value = 0.15;
}

void SystemConfiguration::setTransmissionPowerParameters(){

	 /*Loaded Case*/
	 tpc_increaseRateAtBWLoaded=1;
	 tpc_decreaseRateAtBWLoaded=2;
	 /*Under loaded case*/
	 tpc_increaseRateAtUnderLoaded=2;
	 tpc_decreaseRateAtUnderLoaded=1;
}
