
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
 * TXPowerChanger.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef TXPOWERCHANGER_H_
#define TXPOWERCHANGER_H_
#include "IWConfig.h"
#include <string>
#include <iostream>
#include <vector>

enum TXDecision{ TXPOWER_DECREASE=0,TXPOWER_INCREASE=2};

/*! \class TransmissionPowerControl
 * \brief Transmission Power Control class deals with the decrease and increase of the transmission power of the router. It is called by Conntrack and
 * QoSImprover components.
 *
 */

class TransmissionPowerControl {
private:
	static TransmissionPowerControl* txpowerChanger_Instance;/**<  */
public:
	static  std::string className;							 /**< Class Name. */

	IWConfig *iw;											 /**<  */
	static int selectedDevice;								 /**< Indicates the selected router. */
	int currentTXPower;										 /**< Current assigned transmission power. */
	int lastTXPower;										 /**< In case program is closed, last TX Power can show us the last situation. */
	static std::string file_TXPowerAssignement;				 /**<  */
	int connectionTrackingThresold;							 /**<  */

	static int minimumTXPowerDeterminedByConntrack;			 /**<  Minimum transmission power determined by Conntrack component.*/

	TransmissionPowerControl();
	virtual ~TransmissionPowerControl();

	/**
	* Create only one instance of this class.
	* \return the same reference of the created object in case it is invoked by other objects.
	*/
	static TransmissionPowerControl* Instance();
	/**
	 * Decrease the transmission power of the router with respect to given rate.
	 * \param decrease rate
	 */
	void decreaseTXPower(int decreaseRate);
	/**
	 *Increase the transmission power of the router with respect to given rate.
	 * \param increase rate
	 */
	void increaseTXPower(int increaseRate);
	/**
	 * Perform the given action, either increase or decrease the transmission power
	 * of the router.
	 * \param action
	 */
	void performAction(int action);
	/**
	 * Save the last Transmission Power Table.
	 */
	void saveLastTXPowerInFile();
	/**
	 * Read the last Transmission Power Table
	 */
	void readLastTXPowerFromFile();
	/**
	 * Set the transmission power to given value.
	 * \param lastValue
	 */
	void setConnectionTrackerTreshold(int lastValue);
	/**
	 * Set minimum transmission power to given recommended value.
	 * \param recommended transmission power.
	 */
	void setMinimumTXPower(int recommendedTXPower);
};

#endif /* TXPOWERCHANGER_H_ */
