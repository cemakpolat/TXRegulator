/*
 * IWConfig.cpp
 *
 *  Created on: Mar 1, 2013
 *      Author: cem
 */

#include "IWConfig.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>
std::string IWConfig::className="IWConfig";

IWConfig::IWConfig() {
	this->iwobj=new IWConfObject();
	this->selectedDevice = GENERIC;
	this->maxSignalThreshold=GENERIC_MAX_TX;
	this->minSignalThreshold=GENERIC_MIN_TX;
	this->interface = Utilities::getNetworkInterface();
	//this->setDeviceParameters();
}
IWConfig::IWConfig(int selectedDevice) {
	this->iwobj=new IWConfObject();
	this->interface = Utilities::getNetworkInterface();
	//cout<<"Interface: "<<this->interface <<endl;
	Utilities::writeOutputInFile(className+": Netgear Interface "+this->interface);
	this->selectedDevice = selectedDevice;
	this->setDeviceParameters();
}
IWConfig::~IWConfig() {
	delete iwobj;
}


void IWConfig::scanVoyage() {
	std::string cmd = "iwconfig " + this->interface;

	istringstream infile(Utilities::exec(cmd.c_str()));
	string iface, dontcare, cardType, mode,freq, txpower,limit, rts, frag, powMan;
	while (infile >> iface >> dontcare >> cardType >> mode >>freq>>dontcare>> txpower
			>> dontcare >> dontcare >>dontcare>>limit>>dontcare>> rts >> dontcare >> frag >> dontcare
			>> powMan) {
		cout <<"iface:"<< iface << "\n CardType:" << cardType << "\n Mode " << mode << "\n " << freq
				<< endl;
	}
	this->iwobj->mode = mode;
	this->iwobj->txpower = Utilities::split(txpower, '=')[1];
	this->iwobj->txPower=Utilities::convertStringToIntCPP(this->iwobj->txpower);
	//cout<<"Netgear Power:"<<this->iwobj->txpower<<endl;
	string io=className+": Voyage Current Transmission Power:"+this->iwobj->txpower;
	Utilities::writeOutputInFile(io);
}
/*
 * wlan0     IEEE 802.11bgn  Mode:Master  Frequency:2.412 GHz  Tx-Power=26 dBm
          RTS thr:off   Fragment thr:off
          Power Management:on
 *
 */
void IWConfig::scanNetgear() {

	std::string cmd = "iwconfig " +  this->interface;
	istringstream infile(Utilities::exec(cmd.c_str()));
	string iface, dontcare, cardType, mode, txpower,limit,freq, rts, frag, powMan;

	while (infile >> iface >> dontcare >> cardType >> mode >>freq>>dontcare>> txpower
			>> dontcare >>dontcare>> rts >> dontcare >> frag >> dontcare
			>> powMan) {
	//	cout <<"iface:"<< iface << "\n CardType:" << cardType << "\n Mode " << mode << "\n TXPower:" << txpower<< endl;
	}

	this->iwobj->mode = mode;
	this->iwobj->txpower = Utilities::split(txpower, '=')[1];
	this->iwobj->txPower=Utilities::convertStringToIntCPP(this->iwobj->txpower);

	if(this->iwobj->txPower==-1){
		this->iwobj->txPower=this->NETGEAR_MAX_TX;
	}

	//cout<<"Netgear Current Transmission Power:"<<this->iwobj->txpower<<endl;
	string io=className+": Netgear Current Transmission Power:"+this->iwobj->txpower;
	Utilities::writeOutputInFile(io);
}

void IWConfig::scanGeneric() {
	std::string cmd = "iwconfig " + this->interface;
	istringstream infile(Utilities::exec(cmd.c_str()));

	string iface, cardIns, cardType, essid, mode, freq, freqMetric, ac, point,
			mac, bit, bitrate, txpower, dontcare, sens, retryLim, rts, frag,
			powMan, link, linkQua, sig, sigLev, nois, noiseLev;
	while (infile >> iface >> cardIns >> cardType >> essid >> mode >> freq
			>> freqMetric >> ac >> point >> mac >> bit >> bitrate >> dontcare
			>> txpower >> dontcare >> sens >> dontcare >> retryLim >> dontcare
			>> rts >> dontcare >> frag >> dontcare >> powMan >> link >> linkQua
			>> sig >> sigLev >> dontcare >> nois >> noiseLev) {
		cout << iface << "\n " << cardType << "\n " << essid << "\n " << mode
				<< "\n " << freq << "\n " << mac << "\n " << bitrate << "\n "
				<< txpower << "\n " << retryLim << "\n " << link << linkQua
				<< "\n " << sig << sigLev << "\n " << nois << noiseLev << endl;
	}

	this->iwobj->mode = mode;
	this->iwobj->macAddr = mac;
	this->iwobj->ssid = Utilities::split(essid, ':')[1];
	this->iwobj->freq = Utilities::split(freq, ':')[1];
	this->iwobj->txpower = Utilities::split(txpower, '=')[1];
	this->iwobj->txPower=Utilities::convertStringToIntCPP(this->iwobj->txpower);

	this->iwobj->bitrate = Utilities::split(bitrate, '=')[1];

	this->iwobj->linkQuality = Utilities::split(linkQua, '=')[1];
	this->iwobj->signalLev = Utilities::split(sigLev, '=')[1];
	this->iwobj->noiseLevel = Utilities::split(noiseLev, '=')[1];

}

void IWConfig::setTransmissionPower(int value) {
	//cout<<"Min "<<this->minSignalThreshold<<" Max"<<this->maxSignalThreshold<<endl;
	//cout<<"TXPower will be increased "<<value<<endl;
	//Utilities::writeOutputInFile(className+": TXPower will be increased");
	if(value<=this->maxSignalThreshold && value>=this->minSignalThreshold){
		this->iwobj->txPower=value;

		std::string s="";
		if(geteuid()==0){
			s= "iwconfig " + interface + " txpower ";
		}else{
			s= "sudo iwconfig " + interface + " txpower ";
		}
		s = s + Utilities::convertIntToString(value);
		//cout << s << endl;
		Utilities::writeOutputInFile(className+": "+s);

		system(s.c_str());
	}
}

void IWConfig::scan() {
	switch (this->selectedDevice) {
	case GENERIC:
		scanGeneric();
		break;
	case NETGEAR:
		scanNetgear();
		break;
	case VOYAGE:
		scanVoyage();
		break;
	default:
		scanGeneric();
		break;
	};
}
void IWConfig::setDeviceParameters() {
	string device = "";
	switch (this->selectedDevice) {
	case GENERIC:
		Utilities::writeOutputInFile(className+": GENERIC");
		//cout<<"Selected Device: GENERIC"<<endl;
		this->maxSignalThreshold=GENERIC_MAX_TX;
		this->minSignalThreshold=GENERIC_MIN_TX;
		break;
	case NETGEAR:
		Utilities::writeOutputInFile(className+": NETGEAR");
		//cout<<"Selected Device: NETGEAR"<<endl;
		this->maxSignalThreshold=NETGEAR_MAX_TX;
		this->minSignalThreshold=NETGEAR_MIN_TX;
		break;
	case VOYAGE:
		Utilities::writeOutputInFile(className+": VOYAGE");
		//cout<<"Selected Device: VOYAGE"<<endl;
		this->maxSignalThreshold=VOYAGE_MAX_TX;
		this->minSignalThreshold=VOYAGE_MIN_TX;
		break;
	default:
		Utilities::writeOutputInFile(className+": GENERIC");
		//cout<<"Selected Device: GENERIC"<<endl;
		this->maxSignalThreshold=GENERIC_MAX_TX;
		this->minSignalThreshold=GENERIC_MIN_TX;
		break;
	};

//	string conf = Utilities::readFile2(IWConfig::file_configurations);
//
//	vector<string> v1, v2,v3;
//	v1 = Utilities::split(conf, ':');
//	for (unsigned j = 0; j < v1.size(); j++) {
//		v2 = Utilities::split(v1[j], ',');
//		v3= Utilities::split(v2[0], '=');
//
//		if (v3[1] == device) {
//			v3= Utilities::split(v2[1], '=');
//			this->minSignalThreshold =Utilities::convertStringToIntCPP(v3[1]);
//			cout<<"minSignalThreshold"<<this->minSignalThreshold<<endl;
//			v3= Utilities::split(v2[2], '=');
//			this->maxSignalThreshold =Utilities::convertStringToIntCPP( v3[1]);
//			cout<<"maxSignalThreshold"<<this->maxSignalThreshold<<endl;
//
//		}
//	}

}

