/**
 * particle_filter.cpp
 *
 */

#include "particle_filter.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <string>
#include <vector>

#include "helper_functions.h"

using std::string;
using std::vector;
using std::normal_distribution;

void ParticleFilter::init(double x, double y, double theta, double std[]) {
  /**
   * TODO: Set the number of particles. Initialize all particles to 
   *   first position (based on estimates of x, y, theta and their uncertainties
   *   from GPS) and all weights to 1. 
   * TODO: Add random Gaussian noise to each particle.
   * NOTE: Consult particle_filter.h for more information about this method 
   *   (and others in this file).
   */
  
  // number of particles
  num_particles = 20; 
  
  // list of particles
  // vector<Particle> particles;
  
  // Createing normal distributions for x, y and theta (std: array of dim 3 standart deviation of x, y and theta)
  normal_distribution<double> dist_x(x, std[0]);
  normal_distribution<double> dist_y(y, std[1]);
  normal_distribution<double> dist_theta(theta, std[2]);
  
  std::default_random_engine gen;
  
  // Generate "num_particles" of particles
  for (int i=0; i<num_particles; i++){
    Particle particle;
    particle.id = i;
    particle.x = dist_x(gen);
    particle.y = dist_y(gen);
    particle.theta = dist_theta(gen);
    particle.weight = 1.0;
    // particles[i] = particle;
    particles.push_back(particle);
    weights.push_back(particle.weight);
  }
  is_initialized = true;    
}

void ParticleFilter::prediction(double delta_t, double std_pos[], 
                                double velocity, double yaw_rate) {
  /**
   * TODO: Add measurements to each particle and add random Gaussian noise.
   * NOTE: When adding noise you may find std::normal_distribution 
   *   and std::default_random_engine useful.
   *  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
   *  http://www.cplusplus.com/reference/random/default_random_engine/
   */
  std::default_random_engine gen;
  
  for (int i = 0; i<num_particles; i++) {
    Particle particle = particles[i];
    
    // Checking if yaw rate is equal to zero
    if (fabs(yaw_rate) < 0.0001) {
      particle.x = particle.x + velocity * delta_t * cos(particle.theta);
      particle.y = particle.y + velocity * delta_t * sin(particle.theta);
      particle.theta = particle.theta;
    }
    else {
      particle.x = particle.x + (velocity /yaw_rate) * (sin(particle.theta + (yaw_rate * delta_t)) - sin(particle.theta));
      particle.y = particle.y + (velocity /yaw_rate) * (cos(particle.theta) - cos(particle.theta + (yaw_rate * delta_t)));
      particle.theta = particle.theta + (yaw_rate * delta_t);
    }
  
    // Createing normal distributions for x, y and theta (std: array of dim 3 standart deviation of x, y and theta) 
    normal_distribution<double> dist_x(particle.x, std_pos[0]);
    normal_distribution<double> dist_y(particle.y, std_pos[1]);
    normal_distribution<double> dist_theta(particle.theta, std_pos[2]);
    
    particles[i].x = dist_x(gen);
    particles[i].y = dist_y(gen);
    particles[i].theta = dist_theta(gen);
  }
}


void ParticleFilter::dataAssociation(vector<LandmarkObs> predicted, vector<LandmarkObs>& observations) {
  /**
   * TODO: Find the predicted measurement that is closest to each 
   *   observed measurement and assign the observed measurement to this 
   *   particular landmark.
   * NOTE: this method will NOT be called by the grading code. But you will 
   *   probably find it useful to implement this method and use it as a helper 
   *   during the updateWeights phase.
   */
  
  for(unsigned int i=0; i<observations.size(); i++){
    double minDist = std::numeric_limits<double>::max();
    for(unsigned int j=0; j<predicted.size(); j++){
      double distance = dist(observations[i].x, observations[i].y, predicted[j].x, predicted[j].y);
      if(distance < minDist){
        minDist = distance;
        observations[i].id = predicted[j].id;
      }
    }
  }
}


double multiv_prob(double sig_x, double sig_y, double x_obs, double y_obs,
                   double mu_x, double mu_y) {
  // calculate normalization term
  double gauss_norm;
  gauss_norm = 1 / (2 * M_PI * sig_x * sig_y);

  // calculate exponent
  double exponent;
  exponent = (pow(x_obs - mu_x, 2) / (2 * pow(sig_x, 2)))
               + (pow(y_obs - mu_y, 2) / (2 * pow(sig_y, 2)));
    
  // calculate weight using normalization terms and exponent
  double weight;
  weight = gauss_norm * exp(-exponent);
    
  return weight;
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], const vector<LandmarkObs> &observations, const Map &map_landmarks) {
  /**
   * TODO: Update the weights of each particle using a mult-variate Gaussian 
   *   distribution. You can read more about this distribution here: 
   *   https://en.wikipedia.org/wiki/Multivariate_normal_distribution
   * NOTE: The observations are given in the VEHICLE'S coordinate system. 
   *   Your particles are located according to the MAP'S coordinate system. 
   *   You will need to transform between the two systems. Keep in mind that
   *   this transformation requires both rotation AND translation (but no scaling).
   *   The following is a good resource for the theory:
   *   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
   *   and the following is a good resource for the actual equation to implement
   *   (look at equation 3.33) http://planning.cs.uiuc.edu/node99.html
   */
  
   for (unsigned int i = 0; i < particles.size(); ++i) {
    Particle particle = particles[i];
    double prob = 1.0;

  
    for (unsigned int j = 0; j < observations.size(); j++) {

      // Homogenous Transformation (from vehicle's coord. system to map's coordinate system)
      // transformed to map x coordinate
      double x_m = particle.x + (cos(particle.theta) * observations[j].x) - (sin(particle.theta) * observations[j].y);

      // transformed to map y coordinate
      double y_m = particle.y + (sin(particle.theta) * observations[j].x) + (cos(particle.theta) * observations[j].y);
  
      

      std::vector<Map::single_landmark_s> landmark_list = map_landmarks.landmark_list;
      double land_x; // x value of landmark
      double land_y; // y value of landmark
      double max_val = 2 * sensor_range;

      for (unsigned int k = 0; k < landmark_list.size(); k++) {
        // Calculate distance between particle and landmarks
        double local_land_x = landmark_list[k].x_f;
        double local_land_y = landmark_list[k].y_f;
        double distance = dist(x_m, y_m, local_land_x, local_land_y);

        if ((distance <= sensor_range) && (distance <= max_val))  {
          // Calculate multivariate Gaussian normal distribution
          land_x = local_land_x;
          land_y = local_land_y;
          max_val = distance;
          prob = multiv_prob(std_landmark[0], std_landmark[1], x_m, y_m, land_x, land_y);
          particles[i].weight = prob;
          weights[i] = prob;
        }
      }
    }
  }
}


void ParticleFilter::resample() {
  /**
   * TODO: Resample particles with replacement with probability proportional 
   *   to their weight. 
   * NOTE: You may find std::discrete_distribution helpful here.
   *   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
   */
  
    std::default_random_engine gen;
    std::discrete_distribution<> d(weights.begin(), weights.end());
    std::vector<Particle> resampled_particles;

    for (int n = 0; n < num_particles; ++n) 
    {
        Particle particle = particles[d(gen)];
        resampled_particles.push_back(particle);
    }
    particles = resampled_particles;

}


void ParticleFilter::SetAssociations(Particle& particle, 
                                     const vector<int>& associations, 
                                     const vector<double>& sense_x, 
                                     const vector<double>& sense_y) {
  // particle: the particle to which assign each listed association, 
  //   and association's (x,y) world coordinates mapping
  // associations: The landmark id that goes along with each listed association
  // sense_x: the associations x mapping already converted to world coordinates
  // sense_y: the associations y mapping already converted to world coordinates
  particle.associations= associations;
  particle.sense_x = sense_x;
  particle.sense_y = sense_y;
}

string ParticleFilter::getAssociations(Particle best) {
  vector<int> v = best.associations;
  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<int>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}

string ParticleFilter::getSenseCoord(Particle best, string coord) {
  vector<double> v;

  if (coord == "X") {
    v = best.sense_x;
  } else {
    v = best.sense_y;
  }

  std::stringstream ss;
  copy(v.begin(), v.end(), std::ostream_iterator<float>(ss, " "));
  string s = ss.str();
  s = s.substr(0, s.length()-1);  // get rid of the trailing space
  return s;
}
