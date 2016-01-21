
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
 * QoSImprover.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef EXPERIMENTER_H_
#define EXPERIMENTER_H_
#include <string>
#include <vector>
#include "EM.h"


enum TXChange{ DECREASE=0,DONOTHING=1,INCREASE=2};
/*! \class QoSImprover
 * \brief  QoS Improver is the platform where the QoS network indicators are processed and the related decision
 * such as increasing/decreasing the transmission power is taken. \n
 *
 *
 * \image latex /home/cem/Thesis_Doxygen/images/QoSImprover1.png " " width=14cm
 * \image latex /home/cem/Thesis_Doxygen/images/QoSImprover2.png " QoS Improver Flow Chart" width=14cm
 */

class QoSImprover {
public:
	static  std::string className;							/**< Class name.*/
	static QoSImprover* m_expInstance;						/**<  */

	double epsilon;											/**< Required value for the EM convergence.*/
	double componentCount;									/**< GMM component numbers.*/
	int experimentWaitingDuration;							/**< Total observation duration in s. */

	double qos_min_well_state_value;						/**< Minimum value for Well state. */
	double qos_min_normal_state_value;						/**< Minimum value for Normal state.*/
	double qos_min_critical_state_value;					/**< Minimum value for Critical state.*/

	//std::vector<double> banwidthStateList;/**<  */
	std::vector<int> assignedTXPowers;						/**< Assigned transmission powers.*/
	unsigned int preExtensionForWhichTXPower;				/**< Transmission power where the observation duration previously extended. */
	bool isThisFirstExtension;								/**< Show first observation extension.*/
	unsigned int txAssignmentCountWillBeConsidered;			/**< The permitted size for Assigned TX List.*/

	int expWaitingDurationRate;								/**< Observation duration increase/decrease rate.*/
	std::string file_decisionFile;							/**<*/

	std::vector<double> means;								/**< GMM Components means.*/
	std::vector<double> sigmas;								/**< GMM Components sigmas. */

	/**
	* Create only one instance of this class.
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static QoSImprover* Instance();

	QoSImprover();
	virtual ~QoSImprover();

	/**
	 * This function is periodically executed in order to process the QoS network indicators' windows.
	 */
	static void* runExperimenter(void* data);


	/**
	 * This method evaluates the given tetas parameters in terms of the threshold for well, normal and critical states and
	 * return the possible state.
	 *\param tetas
	 * \return the adequate state
	 *
	 */
	int txStateDecisionToMixCoef(std::vector<long double>& tetas);
	/**
	 * Average Delay Window is handled through EM Object.
	 * \return the decision obtained by EM.
	 */
	int aDelayExperiment();
	/**
	* Average Bitrate Window is handled through EM Object.
	* \return the decision obtained by EM.
	*/
	int aBitrateExperiment();
	/**
	* Average Jitter Window is handled through EM Object.
	* \return the decision obtained by EM.
	*/
	int aJitterExperiment();
	/**
	 *
	 */
	int aPacketLossExperiment();
	/**
	 * Initiate GMM Component Parameters.
	 */
	void initiateGMMComponentParameters();

	/**
	 * A method coming to the last decision on the action that will be taken. The common decision of Average Delay, Average Bitrate
	 * and Average Jitter is taken.
	 * \param ad average delay decision
	 * \param ab average bitrate decision
	 * \param aj average jitter decision
	 * \return the possible decisions
	 * 0 - Increase Transmission Power \n
	 * 1 - Do nothing \n
	 * 2 - Decrease Transmission Power \n
	 */
	int commonDecisionOnTXState(int ad,int ab,int aj);
	/**
	* This function checks the possibility for the extension of experiment observation duration. If it is possible,
	* the extension would occur.
	 */
	void extendExperimentDuration();

};

#endif /* QOSIMPROVER_H_ */
