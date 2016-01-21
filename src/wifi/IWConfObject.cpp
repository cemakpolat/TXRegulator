/*
 * IWConfObject.cpp
 *
 *  Created on: Mar 4, 2013
 *      Author: cem
 */

#include "IWConfObject.h"
#include "Utilities.h"
/*! \class IWConfObject
 *  \brief
 */

IWConfObject::IWConfObject() {

}

IWConfObject::~IWConfObject() {
}
std::string IWConfObject::getLinkQuality(){
	return this->linkQuality;
}
std::string IWConfObject::getSignalLevel(){
	return this->signalLev;
}
std::string IWConfObject::getMacAddr(){
	return this->macAddr;
}
std::string IWConfObject::getChannel(){
	return this->channel;
}

void IWConfObject::setChannel(std::string& val){
	this->channel=val;
}
string IWConfObject::getSNR() {
	int sigLev = Utilities::covertStringToIntC(this->signalLev);
	int noiseLev = Utilities::covertStringToIntC(this->noiseLevel);
	int res = sigLev - noiseLev;
	return Utilities::convertIntToString(res);
}
string IWConfObject::getSSID() {
	return this->ssid;
}
string IWConfObject::getTXPower() {
	return this->txpower;
}
string IWConfObject::getBitRate() {
	return this->bitrate;
}
string IWConfObject::getRetryLimit() {
	return this->retryLim;
}

string IWConfObject::getNoiseLevel() {
	return this->noiseLevel;
}
