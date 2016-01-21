/*
 * EM.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: cem
 */

#include "EM.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "NormMix.h"
#include "Utilities.h"

using namespace std;
std::string EM::className="EM";

EM::EM(){
	//iteration=0;
	TINYFORPROB=8.30359e-40;
}

void EM::initiateParameters(std::vector<double>& dataInput, int componentNumber,std::vector<double>& mu,std::vector<double>& sigm) {
	data = dataInput;
	k = componentNumber;
	n = (int) data.size();
	pis.resize(k);
	means.resize(k);
	sigmas.resize(k);
	probs.resize(k * data.size());

	for (int i = 0; i < k; ++i) {
		pis[i] = 1. / k; //uniform priors
		means[i]=mu[i];
		sigmas[i]=sigm[i];
	}
	/*string str;
	str="CC | Means | Sigmas | Mixture Coef \n";
	for (int i = 0; i < k; ++i) {
		str = str+ Utilities::convertIntToString(i) + " "
				+ Utilities::convertLongDoubleToString(means[i])+" "
				+ Utilities::convertLongDoubleToString(sigmas[i]) + " "
				+ Utilities::convertLongDoubleToString(pis[i]) + "\n";
	}
	Utilities::writeOutputInFile(className+" ------ EM First Values ---- \n"+str);*/
}
EM::~EM() {
}
void EM::getMixtureCoefficients(vector<long double>& tetas){
	bool state=true;
	for(unsigned int i=0;i<this->pis.size();i++){
		if(isnan(pis[i])){
			state=false;
			break;
		}
	}
	if(state){
		tetas=this->pis;
//		for (int i = 0; i < k; ++i) {
//			cout<<"EM: "<<i<<"	"<<pis[i]<<"	"<<tetas[i]<<endl;
//		}
	}
}
void EM::getMeans(vector<long double>& tetas){
	bool state=true;
	for(unsigned int i=0;i<this->means.size();i++){
		if(isnan(means[i])){
			state=false;
			break;
		}
	}
	if(state)
		tetas=this->means;
}
void EM::getSigmas(vector<long double>& tetas){
	bool state=true;
	for(unsigned int i=0;i<this->sigmas.size();i++){
		if(isnan(sigmas[i])){
			state=false;
			break;
		}
	}
	if(state)
		tetas=this->sigmas;
}

void EM::updateProbs(){
	for(int i=0;i<n;++i){
		long double cum=0;
		for(int j=0;j<k;++j){
			probs[i*k+j]=pis[j]*NormMix::dnorm(data[i],means[j],sigmas[j]);
			cum+=probs[i*k+j];
		}
		//normalizer
		for(int j=0;j<k;++j){
			probs[i*k+j]/=cum;
			//cout <<"Prop " <<probs[i*k+j]<<endl;
			if(probs[i*k+j]==0)
				probs[i*k+j]=8.30359e-40;
		}
	}
}

void EM::updatePis(){
	for(int j=0;j<k;++j){
		pis[j]=0;
		for(int i=0;i<n;++i){
			pis[j]+=probs[i*k+j];
		}
		pis[j]/=n;
		//cout<<"pis "<<j<<" "<<pis[j]<<endl;
	}
}

void EM::updateMeans() {
	for (int j = 0; j < k; ++j) {
		means[j] = 0;
		for (int i = 0; i < n; ++i) {
			means[j] += data[i] * probs[i * k + j];
		}
		means[j] /= (n * pis[j]); //(n*pis[j]+TINY)
		//cout<<"means "<<j<<" "<<means[j]<<endl;
	}
}
void EM::updateSigmas(){
	for (int j = 0; j < k; ++j) {
		sigmas[j] = 0;
		for (int i = 0; i < n; ++i) {
			sigmas[j] += (data[i]-means[j])*(data[i]-means[j])*probs[i*k+j];
		}
		sigmas[j] = sqrt(sigmas[j]/(n*pis[j]));//(n*pis[j]+TINY)
		//cout<<"sigmas "<<j<<" "<<sigmas[j]<<endl;

	}
}

double EM::runEM(long double eps){
	Utilities::writeOutputInFile("EM: EM is activated for new experimentation !");
	iteration=0;
	TINY=0.000000000001;
	long double llk=0,prevllk=0;
	bool state=true;
	string str;

	while(state && iteration<40  ){

		updateProbs();
		updatePis();
		updateMeans();
		updateSigmas();
		prevllk=llk;
		llk=NormMix::mixLLK(data,pis,means,sigmas);
		iteration++;
		//cout<<"Prevllk: "<<prevllk<<" LLK" << llk<<" diff "<<llk-prevllk<<endl;
		state=check_tol(llk,prevllk,eps);
	}
	str="Last Iteration Count:"+Utilities::convertIntToString(iteration);
	iteration=0;
	return llk;
}

bool EM::check_tol(long double llk,long double prevllk,long double eps){
	bool state=true;
	if ((-(llk - prevllk)) < (eps)&& ((llk - prevllk)) < (eps)) {
		state = false;
	}
	return state;
}



