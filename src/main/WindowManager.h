
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
 * WindowManager.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef WINDOWMANAGER_H_
#define WINDOWMANAGER_H_
#include "PracticalSocket.h"
#include <vector>


/*! \struct QoSObject
 *  \brief This struct contains average delay, average jitter, average bitrate and packet loss values.
 */
struct QoSObject {
	double AverageDelay;
	double AverageJitter;
	double AverageBitRate;
	double AveragePacketLoss;
};

/*! \class Window Manager
 * \brief Window Manager is the platform where the QoS network indicators are collected and decided on whether the received
 * measurements are critical or not. \n
 *
 *
 *  \image latex /home/cem/Thesis_Doxygen/images/WindowManager.png "Window Manager Flow Chart" width=12cm
 */
class WindowManager {
public:
	pthread_mutex_t mutex_messageCounter;					/**< Pthread Mutex  */

	static  std::string className;							/**<  Class Name */

	std::vector<int> stateList_ADelay;						/**< List containing the critical and non-critical evaluations
																of average delay measurements.*/
	std::vector<int> stateList_ABitrate;					/**<List containing the critical and non-critical evaluations
																of average bitrate measurements.  */
	std::vector<int> stateList_AJitter;						/**< List containing the critical and non-critical evaluations
																of average jitter measurements. */
	//std::vector<int> stateList_APacketLoss;				/**<  */

	int critical_averageDelay_count;						/**< Total critical number of average delay state list.  */
	int critical_averageBitrate_count;						/**< Total critical number of average bitrate state list. */
//	int critical_averagePacketLoss_count;					/**<  */
	int critical_averageJitter_count;						/**<  Total critical number of average jitter state list.*/

	double critical_averageDelay_value;						/**< Threshold for the critical measurements of average delay*/
	double critical_averageBitrate_value;					/**<  Threshold for the critical measurements of average bitrate*/
//	double critical_averagePacketLoss_value;				/**<  */
	double critical_averageJitter_value;					/**<  Threshold for the critical measurements of average jitter*/

	unsigned int messageCounter;							/**<  The total received QoS messages.*/
	unsigned int listCount;									/**<  Window Size*/
	static WindowManager* m_mrInstance;						/**<  */

	int APPortNumber;										/**<  Port Number of the socket server on AP.*/
	TCPServerSocket *sock;									/**<  TCP Socket*/
	/**
	* Create only one instance of this class.
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static WindowManager* Instance();


	/**
	 *The assignment of the critical values is performed in this constructor.
	 */
	WindowManager();
	virtual ~WindowManager();

	/**
	 *This function starts sliding window and measurement taker components.
	 */
	static void* run(void* object);
	/**
	 * This function operates the sliding window mechanism.
	 */
	static void* runSlidingWindow(void*);

	/**
	 *D-ITG Measurements are received through the socket communication and its added in the related list.
	 */
	static void* runDITGMessageReceiver(void*);

	/**
	 *This method adds the total critical QoS measurements of State Lists into the related QoS network indicator's window.
	 */
	void addSample();

	/**
	 *  This function gets QoS Measurement and returns it in QoS Object form.
	 * \return QoSObject
	 */
	QoSObject getMessageFromClient();
	/**
	 * QoS Measurement is fetched from the TCP Socket through this method.
	 * \param sock
	 */
	std::string getMessageInSmallChunks(TCPSocket *sock);

	/**
	 * The critical status of the received OoS message is evaluated in this method
	 * \param obj Qos Object.
	 */
	void classifyMeasurement(QoSObject& obj);

	/**
	 * The half of the State List elements are removed with the help of this method.
	 */
	void removePartiallyMeasurments();

	/**
	 * The critical measurements of QoS network indicators are summed.
	 */
	void addSampleToExperimentList();


};

#endif /* WINDOWMANAGER_H_ */
