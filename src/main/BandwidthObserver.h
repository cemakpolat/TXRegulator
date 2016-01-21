
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
 * BandwidthObserver.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */

#ifndef BANDWIDTHOBSERVER_H_
#define BANDWIDTHOBSERVER_H_
#include <string>
#include <iostream>
#include <vector>
/*! \class BandwidthObserver
 *  \brief
 */
class BandwidthObserver {
private:
	static BandwidthObserver* bandwidthObserver_Instance;
public:
	static  std::string className;					/**<  */
	pthread_mutex_t mutex_bandwidthCounter;			/**<  */
	static BandwidthObserver* Instance();
	bool loaded;
	long threshold_loaded;							/**<  */
	std::string filePath;							/**<  */
	long previousTransmitted;						/**<  */
	long previousReceived;							/**<  */
	long currentTransmitted;						/**<  */
	long currentReceived;							/**<  */
	long bandwidthTX;								/**<  */
	long bandwidthRX;								/**<  */

	int waitForCycle;								/**<  */

	int loaded_count;								/**<  */
	static std::vector<int> bandwidthLoadList;		/**<  */

	/**
	 *
	 */
	BandwidthObserver();
	virtual ~BandwidthObserver();
	/**
	 *
	 */
	long getBandwidthRate();
	/**
	 *
	 */
	static void* run(void*);
	/**
	 *
	 */
	static void* runObserver(void*);
	//void longTermObserver();

	/**
	 *
	 */
	int getBWCriticalMeasObservation();
	/**
	 *
	 */
	void removeComma(std::vector<std::string>& list,std::string delimiter);
	/**
	 *
	 */
	void space2comma(std::string& sLine);

};

#endif /* BANDWIDTHOBSERVER_H_ */
