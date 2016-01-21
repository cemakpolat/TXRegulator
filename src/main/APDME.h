
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
 * APDME.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef APDME_H_
#define APDME_H_
#include <string>
#include <vector>
#include "TransmissionPowerControl.h"
#include "PracticalSocket.h"
#include "TCPConnection.h"
#include "QoSImprover.h"
#include "WindowManager.h"
//#include "BandwidthObserver.h"
#include "ConnTrack.h"
#include "SystemConfiguration.h"



/*! \class APDME
 *  \brief Access Point Decision Making Engine is the central point of this architecture, since all components and list are activated
 *  here. QoS Improver, Window Manager, Transmission Power Controller, Connection Tracker and QoS Network Indicators' windows are
 *  instantiated in this class.
 *
 *  This application is composed of two main processes being shown in different colors in Figure 1. In the first process,
 *  D-ITG Client takes measurements in collaboration with D-ITG Server (1) and Measurement Sender transmits them
 *  over TCP socket to Window Manager (2). On the AP side, the arrived measurements are separately evaluated
 *  for each QoS network indicator whether they are critical or non-critical measurements, and critical measurements
 *  are counted during a period of time. Once this period is over, the critical measurements are inserted
 *  into the QoS network indicators windows (3). QoS Improver analyze periodically the mentioned windows (4)
 *  through the proposed mathematical model with the aim to come to the decision whether the transmission power
 *  should be decreased/increased or done nothing (5).\n
 *
 * \image latex /home/cem/Thesis_Doxygen/images/ThesisWorkFlow.png "APDME Architecture Flow Chart" width=15cm
 */

class APDME {
private:
	static APDME* m_apdmeInstance;
public:
	static  std::string className;						/**< Class Name */
	TransmissionPowerControl *tpc;						/**< Transmission Power Controller */
	pthread_mutex_t mutex_window;						/**< Pthread Mutex for window synchronization */
	QoSImprover *exp;									/**< QoS Improver  */
	WindowManager *mr;									/**< WindowManager  */
	//BandwidthObserver *bw;							/**< BandwidthObserver  */
	ConnTrack *conntrack;								/**< ConnectionTracker  */
	SystemConfiguration *sysConf;

	static std::vector<double> DelayWindow;				/**< Average Delay Window */
	static std::vector<double> BitrateWindow;			/**< Average Bitrate Window */
	static std::vector<double> JitterWindow;			/**< Average Jitter Window */
	//static std::vector<double> PacketLossWindow;		/**< Average PacketLoss Window */


	static int selectedDevice;							/**< indicates the selected router */
	static int itgRestartDuration;						/**< ITG Restart Time */
	APDME();
	virtual ~APDME();
	/**
	* Create only one instance of this class
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static APDME* Instance();
	/**
	 * Activates all components: QoS Improver, Window Manager, Transmission Power Controller, Connection Tracker, D-ITG Tool.
	 */
	void run();

	/**
	 *Executes D-ITG Tool for measuring the network performance.
	 *\param void
	 *
	 */
	static void* runITGChecker(void*);

};


#endif /* APDME_H_ */
