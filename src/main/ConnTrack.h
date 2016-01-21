
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
 * ConnTrack.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef CONNTRACK_H_
#define CONNTRACK_H_
#include <string>
#include <iostream>
#include <vector>

/*! \class ConnTrack
 * \brief Connection Tracker attempts to maintain the established connection between AP and Clients by means of HOSTAPD connection
 * status messages.
 *
 * This class has a responsibility for tracking the connection stability between AP and clients.Therefore,
 * the following algorithm in Figure 3.2 is applied here. It basically, counts the message number of the Connection-Disconnection messages sent
 * by HOSTAPD and then analyze them with the aid of the Gaussian Mixture Component. The final decision could be wither increase the
 * transmission power or assigning lower transmission power according to the data analysis.\n
 *
 *
 * \image latex /home/cem/Thesis_Doxygen/images/Conntrack1.png " " width=14cm
 * \image latex /home/cem/Thesis_Doxygen/images/Conntrack2.png "Connection Tracker Flow Chart " width=14cm
 * \n
 * \image latex /home/cem/Thesis_Doxygen/images/ConListener.png "Connection Tracker HOSTADP Lister Flow Chart" width=4cm
 */

class ConnTrack {
	/*! \struct TXState
	 *  \brief TX State is consisted of the transmission power, connection state list (messages), last state (state) and
	 *  total received message count (mesCount).
	 */
	struct TXState {
		int txpower;										/**< View the transmission power value */
		std::vector<double> measurements;					/**< This contains the non-critical and critical measurements */
		bool state;											/**< View whether the current transmission power is critical value or not */
		int mesCount;
	};

	/*! \struct HAPMessage1
	 *  \brief HOSTAPD Message contains MAC and the incoming message from HOSTAPD.
	 */
	struct HAPMessage1 {
		std::string mac;
		std::string message;
	};
private:
	static ConnTrack* conntrack_Instance;/**<  */
public:
	static  std::string className;								/**<  Class name*/

	//HOSTAPD MESSAGES AND DISCONNECTION MESSAGE COUNTER
	static const std::string DISASSOCIATED;						/**<  */
	static const  std::string DEAUTHENTICATED;					/**<  */
	static const std::string AUTHENTICATED;

	pthread_mutex_t mutex_disconnectionCounter;					/**< Message list mutex parameter*/
	static std::vector<std::string> disconnectedUserList;		/**< Message List */

	int txIncreaseRatioForConnTrack;							/**< The Transmission Power increase ratio  */
	int currentTXPower;											/**< Current Transmission Power Value */
	std::vector<int> assignedTXPowers; 							/**< Assigned Transmission Powers*/
	unsigned int txAssignmentCountWillBeConsidered; 			/**< The permitted size for Assigned TX List */
	int experimentWaitingDurationIncreaseRate;  				/**< Message observation duration increase rate*/

	int expWaitingDurationRate;  								/**< Message observation duration rate */

	int experimentWaitingDuration;  							/**< Total duration for observation in s.*/
	int highestExperimentDurationRate; 							/**< Maximum possible observation duration*/

	//GMM-EM Module in which the current transmission power state is determined such as critical or non critical state.
	static std::vector<TXState> txStateList;  					/**< Transmission power list*/
	int waitTimeForExperiment; 									/**< Taken measurement number, will be shortened and expanded after a while */
	int waitTimeForExperimentCounter;
	double epsilon;												/**< Required value for the EM convergence */
	double componentCount;										/**< GMM component counts*/
	std::vector<double> means; 									/**< GMM means*/
	std::vector<double> sigmas;									/**< GMM sigmas*/
	//std::vector<int> experimentStateCollector;/**< GMM component counts*/

	unsigned int preExtentedDurationforTXPower;					/**< Transmission Power where the observation duration previously extended.*/
	bool isThisFirstExtension;									/**< Show First Observation Extension */

	int window_size;											/**< The size of the messages will be observed. */

	double min_acceptable_message_prob;							/**< Minimum acceptable GMM non-critical component ratio*/

	/**
	* Create only one instance of this class
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static ConnTrack* Instance();
	/**
	 * The value of some parameters of SystemConfiguration are assigned to the class parameters.
	 */
	ConnTrack();
	virtual ~ConnTrack();

	/**
	 * HOSTAPD Messages are listened by UNIX Socket.
	 */
	static void* listenHostapdMessage(void*);

	/**
	 * The collected HOSTAPD messages are handled through this function.
	 */
	static void* disconnectionCounterTimer(void*);
	/**
	 * HOSTAPD Message Listener and data analyzer components are stated.
	 */
	static void* run(void*);

	/**
	 * The incoming HOSTAPD messages are converted to an understandable form for the system.
	 */
	void convertMessageToHAPForm(std::string str,HAPMessage1& message);

	/**
	 * After the conversion of HOSTAPD message, two type of messages (Disconnected and Authenticated) are accepted by this function and
	 * the MAC address being included in the received Message is inserted in the Message List.
	 */
	void interpretMessage(HAPMessage1& message) ;

	/**
	 *Transmission Table is started. This table is composed of the elements below:
	 *Transmission Power: All Transmission Power (TP) value of the router assigned in this column.
	 *Connection State List: This illuminates the received messages at the related TP. All values are assigned to zero.
	 *Last State: This shows the assignability of the related TP. It is derived from Connection State List.
	 *Message Count: The received total message at the related TP.
	 */

	void startTXStateTable();

	/**
	 *
	 * All received Messages and the maximum message count being related to unique client over a period of observation time is added
	 * into Message List.
	 * \param messageCount Maximum Message Number belonging to a user.
	 * \param totalMessageCount Total Message Number received by all users residing in the vicinity of AP.
	 */

	void addMeasurement(int messageCount,int totalMessageCount);

	/**
	 *
	 *\return The value of last assignable transmission power
	 */
	int getLastNonCriticalTX();
	/**
	 * Check whether the result of the Gaussian Mixture Model (GMM) is critical or not with regard to the defined some thresholds.
	 * \param results GMM Results obtained through EM Object.
	 * \return GMM Result state.
	 */
	bool isGMMCritical(std::vector<long double>& results);

	/**
	 * Initiate GMM components parameters.
	 */
	void initiateGMMComponentParameters();

	/**
	 * Execute the experiment after the end of the observation duration. Message List is handled by GMM.
	 * \return the experiment result as true or false
	 */
	int runExperiment();

	/**
	 *The Current State List of the current transmission power is assigned to the given parameter.
	 *\param sourceList
	 *
	 */
	void getCurentTXList(std::vector<double>& sourceList);

	/**
	 * Change the Last State of the transmission power given parameter
	 * \param state
     */
	void changeTXPowerState(bool state);
	/**
	 * This function checks the possibility for the extension of experiment observation duration. If it is possible,
	 * the extension would occur.
	 */
	void experimentStabilityControl();

	/**
	*
	 */
	void computeDisconnectionThreshold();

	/**
	 * Given the transmission power list, the optimal transmission power is selected and returned.
	 * \return the optimal transmission power
	 */
	int findOptimalTXPower(std::vector<int>& mostlyAssignedTXPowerList );

	/**
	 * All Last State of the transmission powers before the given threshold are set to false
	 */
	void setAllValueFalseBeforeThis(int threshold);
	/**
	* This method reassigns the values of GMM parameters.
	*/
	void reAssignGMMParameters();

};

#endif /* CONNTRACK_H_ */
