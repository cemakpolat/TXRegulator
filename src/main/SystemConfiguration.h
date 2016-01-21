
/*
 *
 *  Version 1.0
 * Copyright Master Thesis project - 2013
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by  the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * SystemConfiguration.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef SYSTEMCONFIGURATION_H_
#define SYSTEMCONFIGURATION_H_
#include <string>
#include <iostream>
#include <vector>
/*! \class SystemConfiguration
 *  \brief  System Configuration class is devised as an interface in which all required parameters of all other main components could
 * be assigned. We attempt here to unify parameters at one location, so that we will be able to concentrate merely on our model parameters.
 */
class SystemConfiguration {
private:
	static SystemConfiguration* sysConf_Instance;
public:
	static  std::string className;/**<  Class name*/

	SystemConfiguration();
	virtual ~SystemConfiguration();

	//APDME
	int apdme_windowSize;/**< The window size of QoS network indicators. */
	/**
	* Create only one instance of this class.
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static SystemConfiguration* Instance();
	//Connection Tracker Configurations

	/**
	 * Assign all Conntrack related parameters.
	 */
	void setConnTrackParameters();
	 int conntrack_experimentWaitingDuration;		/**< Conntrack Experiment Observation Duration*/
	 int conntrack_txAssignmentCountWillBeConsidered;/**<Conntrack Transmission Power Assignment List Size*/
	 int conntrack_highestExperimentDuration;		/**< Conntrack Highest Possible Experiment Duration */
	 int conntrack_componentCount;					/**< Conntrack GMM Component Count*/
	 double conntrack_epsilon;						/**< Conntrack GMM Epsilon*/
	 double conntrack_mean_1;						/**< Connrack first mean value for non-critical state of GMM*/
	 double conntrack_mean_2;						/**< Connrack second mean value for critical state of GMM*/
	 double conntrack_sigma_1;						/**< Connrack first sigma value for non-critical state of GMM */
	 double conntrack_sigma_2;						/**< Connrack second sigma value for critical state of GMM */
	 double conntrack_min_acceptable_message_prob;	/**< Conntrack Minimum Acceptable Non-Critical Probability Rate   */
	 int conntrack_window_size;						/**< Conntrack Window Size*/
	 int conntrack_table_threshold;					/**< Conntrack Table Threshold */
	//QoSImprover Configurations

	 /**
	  * Assign all QoS Improver related parameters.
	  */
	void setQoSImproverParameters();
	int qos_experimentWaitingDuration;				/**< QoSImprover Experiment Observation Duration */
	int qos_txAssignmentCountWillBeConsidered;		/**< QoSImprover Transmission Power Assignment List Size */
	int qos_componentCount;							/**< QoSImprover GMM Component Count*  */
	int qos_epsilon;								/**< QoSImprover GMM Epsilon */
	double qos_mean_1;								/**< QoSImprover first mean value for well state of GMM */
	double qos_mean_2;								/**< QoSImprover second mean value for normal state of GMM */
	double qos_mean_3;								/**< QoSImprover third mean value for critical state of GMM */

	double qos_sigma_1;								/**< QoSImprover first mean value for well state of GMM */
	double qos_sigma_2;								/**< QoSImprover second mean value for normal state of GMM  */
	double qos_sigma_3;								/**< QoSImprover third mean value for critical state of GMM */

	double	qos_min_acceptable_prob_well_state;		/**< Minimum Acceptable Well Probability Rate */
	double	qos_min_acceptable_prob_critical_state;	/**< Minimum Acceptable Critical Probability Rate */
	double	qos_min_acceptable_prob_normal_state;	/**< Minimum Acceptable Normal Probability Rate */
	//Bandwidth Observer

	/**
	 *
	 */
	void setBandwidthObserverParameters();
	int bandwidthObserver_waitForObservationCycle;/**<  */
	std::string bandwidthObserver_bandwidthFilePath;/**<  */
	double bandwidthObserver_loadThreshold;/**<  */
	int tpc_criticalBandwidhtObserCount;/**<  */
	//Window Manager

	/**
	 * Assign all Window Manager related parameters.
	 */
	void setWindowManagerParameters();
	int APPortNumber; 								/**< Windows Manager Access Point Port Number  */
	unsigned int maximumListSize;
	unsigned int winManager_stateListSize;			/**<  Windows Manager State List Size*/
	double winManager_averageBitrate_value;			/**<  Windows Manager Threshold Value for Minimum Acceptable non-critical
													Average Bitrate Value */
	double winManager_averageDelay_value;			/**<  Windows Manager Threshold Value for Minimum Acceptable non-critical
	 	 	 	 	 	 	 	 	 	 	 	 	 Average Delay Value */
	double winManager_averageJitter_value;			/**<  Windows Manager Threshold Value for Minimum Acceptable non-critical
													Average Jitter Value */
	double winManager_averagePacketLoss_value;		/**<  Windows Manager Threshold Value for Minimum Acceptable non-critical
	 	 	 	 	 	 	 	 	 	 	 	 	 Average Packet Loss Value */

	/**
	 * Assign all Transmission Power related parameters.
	 */
	void setTransmissionPowerParameters();

	int tpc_increaseRateAtBWLoaded;/**<  */
	int tpc_decreaseRateAtBWLoaded;/**<  */
	int tpc_increaseRateAtUnderLoaded;/**<  */
	int tpc_decreaseRateAtUnderLoaded;/**<  */
	int selectedRouter;/**< Selected Router */
	void longWindowsScenario();
	void shortWindowsScenario();

	/**
	 * Set the configuration of the router parameters.
	*/
	void setRouterConfiguration();
};

#endif /* SYSTEMCONFIGURATION_H_ */
