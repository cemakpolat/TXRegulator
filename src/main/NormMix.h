
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
 * NormMix.h
 *
 *  Created on: 01.10.2013
 *      Author: Cem Akpolat
 */


#include <math.h>
#include <string>
#include <vector>
/*! \class NormMix
 *  \brief Gaussian related mathematical calculation is performed through this class.
 */
class NormMix {
public:
	NormMix();
	~NormMix();

	/**
	 * Calculate the gaussian normal distribution with the given parameterss
	 * \param x data
	 * \param mu mean
	 * \param sigma sigma
	 * \return
	 */
	static long double dnorm(double x, long double mu, long double sigma) {
		return 1.0 / (sigma * sqrt(M_PI * 2.0))
				* exp(-0.5 * (x - mu) * (x - mu) / sigma / sigma);
	}
	/*
	 * A method calculating the total density of the each point
	 * \param x data point in the data set
	 * \param pis mixture coefficient
	 * \param means mean
	 * \param sigmas sigma
	 *\return density of the given x data point in the data set
	 */
	static long double dmix(double x, std::vector<long double>& pis,
			std::vector<long double>& means, std::vector<long double>& sigmas) {
		long double density = 0;
		for (int i = 0; i < (int) pis.size(); ++i) {
			density += pis[i]
					* dnorm(x, (long double) means[i], (long double) sigmas[i]);
		}
		//std::cout<<"Density: "<<density<<std::endl;
		return density;
	}
	/**
	 *This method log likelihood based on the provided parameters.
	 *\param xs data set
	 *\param pis mixture coefficients
	 *\param means mean
	 *\param sigmas sigma
	 *\return new log likelihood
	 */
	static long double mixLLK(std::vector<double>& xs,
			std::vector<long double>& pis, std::vector<long double>& means,
			std::vector<long double>& sigmas) {
		//int i=0;
		long double llk = 0.0;
		for (unsigned int i = 0; i < xs.size(); ++i) {
			llk += log(dmix(xs[i], pis, means, sigmas));
			//	return llk;
		}
		return llk;
	}
};

