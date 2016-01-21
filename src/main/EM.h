
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
 * EM.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#ifndef EM_H_
#define EM_H_
#include <string>
#include <vector>
/*! \class EM
 *  \brief This class is responsible for executing the Gaussian Mixture Model experimentations for other classes. Its name comes from
 *  Expectation-Maximization algorithm, since GMM is constructed on the basis of EM algorithm.
 */
class EM {
public:
	static  std::string className;								/**< Class Name  */

	EM();
	virtual ~EM();

	int k; 														/**< Component Number */
	int n;														/**< Given data count to GMM*/
	int iteration;												/**< Total EM iteration count*/
	long double TINY;											/**<  */
	long double TINYFORPROB;									/**<  */

	std::vector<double> data;									/**< Data being provided to GMM */
	std::vector<long double> pis;								/**< GMM Mixture coefficients */
	std::vector<long double> means;								/**< GMM Means*/
	std::vector<long double> sigmas;							/**< GMM Sigmas*/
	std::vector<long double> probs;								/**< GMM Probability List of each data*/

	/**
	* Initiate the first values for means, mixture coefficient and standard deviation.
	* \param data source Data
	* \param componentNumber GMM component count
	* \param mu Estimated Means
	* \param sigm Estimated Sigmas
	*/
	void initiateParameters(std::vector<double>& dataSource, int componentNumber,std::vector<double>& mu,std::vector<double>& sigm);

	/**
	 *Update Probabilities of each data point in the data set.
	 *This step is called as Expectation(E) step of EM-Algorithm.
	 *
	 */
	void updateProbs();
	/**
	 *Update mixture coefficients with new estimation
	 */
	void updatePis();
	/**
	 * Update means based on new mixture coefficients.
	 */
	void updateMeans();
	/**
	 * Update Sigma or Covariance Matrix based on new mixture coefficient
	 */
	void updateSigmas();
	/**
	 * Run EM algorithm with the input of epsilon signifying the minimum required convergence value.
	 * \param eps epsilon
	 * \return last log likelihood value
	 */
	double runEM(long double eps);
	/**
	 * Check the convergence through the given epsilon by subtracting the last log likelihood from the previous log likelihood.
	 * \param llk log likelihood
	 * \param prevllk previous log likelihood
	 * \param eps epsilon
	 * \return true if the subtraction result is smaller than the given epsilon, otherwise false
	 */

	bool check_tol(long double llk,long double prevllk,long double eps);
	/**
	 * Provides the mixture coefficients to the given vector
	 * \param tetas vector to which the calculated mixture coefficients will be assigned
	 *
	 */
	void getMixtureCoefficients(std::vector<long double>& tetas);
	/**
	* Provides the means to the given vector
	* \param means vector to which the calculated GMM means will be assigned
	*/
	void getMeans(std::vector<long double>& means);
	/**
	* Provides the sigmas to the given vector
	* \param sigmas vector to which the calculated GMM sigmas will be assigned
	*/
	void getSigmas(std::vector<long double>& sigmas);




};

#endif /* EM_H_ */
