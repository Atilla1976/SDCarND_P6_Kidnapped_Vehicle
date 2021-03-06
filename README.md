![](images/Animation.gif)

______________________________________________________________________________________

# Overview
This repository contains all the code needed to complete the project for the Localization course in Udacity's Self-Driving Car Nanodegree.<br>
<br>
A robot has been kidnapped and transported to a new location! Luckily it has a map of this location, a (noisy) GPS estimate of its initial location, and lots of (noisy) sensor and control data.

In this project I implemented a 2 dimensional particle filter in C++. My particle filter will be given a map and some initial localization information (analogous to what a GPS would provide). At each time step my filter will also get observation and control data.

## Algorithm

The following diagram shows the pseudocode of the particle filter with the following four main steps:

[image1]: ./images/02-l-pseudocode.00-00-47-13.still006.png "PseudoCode"

![alt text][image1]

*Initialization step, prediction step, update step and resample step*


### Initialization Step:
+ Setting the number of particles (num_particles) to 20
  + increasing the number of particles will increase the computation time significantly 
+ All particles are initialized to the first position based on estimates of x [m], y [m], theta [rad] and their uncertainties from GPS (provided by the Term 2 Simulator)
+ Random Gaussian noise to each particle
+ Setting the initial weight (particle.weight) of all particles to 1.0



### Prediction Step:
+ Adding meassurements and random Gaussian noise to each particle
+ The calculation of the location of each particle after the time step delta t depends on the yaw rate - if it is close or equal to 0 or not:

  [image2]: ./images/prediction.png "prediction"

  ![alt text][image2]


### Update Step:

In this step, particles are assigned with weights corresponding to their prediction:

+ The vehicle uses LIDAR to sense the distances to landmarks in the enviroment (given from the simulatior in a list with x, y coordinates). The measurements include noise as standard deviation.

+ In order to map the observation into the global coordinate system, transformation and rotation are requiert, but no scaling. This is done by Homogenous Transformation:


  [image3]: ./images/homogenous_transformation.PNG "homogenous transformation"

  ![alt text][image3]
+ nearest neighbour technique:
  + Finding the landmark with the lowest euclidean distance to an observation
+ Once all observations are associated to a landmark, weights of particles are calculated by Multivariante-Gaussian standard deviation:

    [image4]: ./images/multiv_gaussian.PNG "Multivariate-Gaussian's standard deviation"

    ![alt text><][image4]
      
+ The higher the weight, the more accurate is the particle's prediction


### Resample Step:

+ Resampling take particles more into account with higher weights and neglects particles with lower weight. Once resampling is done, the particle with highest weight is chosen. This particle gives most accurate prediction of vehicle's location.

+ The location provided by particle with highest weight is then compared with the ground truth and error in the system is calculated.


## Project Output
Below is the result with 20 particles:

[image5]: ./images/Success.PNG "success"

![alt text][image5]




## Running the Code
This project involves the Term 2 Simulator which can be downloaded [here](https://github.com/udacity/self-driving-car-sim/releases)

This repository includes two files that can be used to set up and install uWebSocketIO for either Linux or Mac systems. For windows you can use either Docker, VMware, or even Windows 10 Bash on Ubuntu to install uWebSocketIO.

Once the install for uWebSocketIO is complete, the main program can be built and ran by doing the following from the project top directory.

1. mkdir build
2. cd build
3. cmake ..
4. make
5. ./particle_filter

Alternatively some scripts have been included to streamline this process, these can be leveraged by executing the following in the top directory of the project:

1. ./clean.sh
2. ./build.sh
3. ./run.sh

Tips for setting up your environment can be found [here](https://classroom.udacity.com/nanodegrees/nd013/parts/40f38239-66b6-46ec-ae68-03afd8a601c8/modules/0949fca6-b379-42af-a919-ee50aa304e6a/lessons/f758c44c-5e40-4e01-93b5-1a82aa4e044f/concepts/23d376c7-0195-4276-bdf0-e02f1f3c665d)

Note that the programs that need to be written to accomplish the project are src/particle_filter.cpp, and particle_filter.h

The program main.cpp has already been filled out, but feel free to modify it.

Here is the main protocol that main.cpp uses for uWebSocketIO in communicating with the simulator.

INPUT: values provided by the simulator to the c++ program

// sense noisy position data from the simulator

["sense_x"]

["sense_y"]

["sense_theta"]

// get the previous velocity and yaw rate to predict the particle's transitioned state

["previous_velocity"]

["previous_yawrate"]

// receive noisy observation data from the simulator, in a respective list of x/y values

["sense_observations_x"]

["sense_observations_y"]


OUTPUT: values provided by the c++ program to the simulator

// best particle values used for calculating the error evaluation

["best_particle_x"]

["best_particle_y"]

["best_particle_theta"]

//Optional message data used for debugging particle's sensing and associations

// for respective (x,y) sensed positions ID label

["best_particle_associations"]

// for respective (x,y) sensed positions

["best_particle_sense_x"] <= list of sensed x positions

["best_particle_sense_y"] <= list of sensed y positions


Your job is to build out the methods in `particle_filter.cpp` until the simulator output says:

```
Success! Your particle filter passed!
```

# Implementing the Particle Filter
The directory structure of this repository is as follows:

```
root
|   build.sh
|   clean.sh
|   CMakeLists.txt
|   README.md
|   run.sh
|
|___data
|   |   
|   |   map_data.txt
|   
|   
|___src
    |   helper_functions.h
    |   main.cpp
    |   map.h
    |   particle_filter.cpp
    |   particle_filter.h
```

The only file you should modify is `particle_filter.cpp` in the `src` directory. The file contains the scaffolding of a `ParticleFilter` class and some associated methods. Read through the code, the comments, and the header file `particle_filter.h` to get a sense for what this code is expected to do.

If you are interested, take a look at `src/main.cpp` as well. This file contains the code that will actually be running your particle filter and calling the associated methods.

## Inputs to the Particle Filter
You can find the inputs to the particle filter in the `data` directory.

#### The Map*
`map_data.txt` includes the position of landmarks (in meters) on an arbitrary Cartesian coordinate system. Each row has three columns
1. x position
2. y position
3. landmark id

### All other data the simulator provides, such as observations and controls.

> * Map data provided by 3D Mapping Solutions GmbH.

## Success Criteria
If your particle filter passes the current grading code in the simulator (you can make sure you have the current version at any time by doing a `git pull`), then you should pass!

The things the grading code is looking for are:


1. **Accuracy**: your particle filter should localize vehicle position and yaw to within the values specified in the parameters `max_translation_error` and `max_yaw_error` in `src/main.cpp`.

2. **Performance**: your particle filter should complete execution within the time of 100 seconds.

## How to write a README
A well written README file can enhance your project and portfolio.  Develop your abilities to create professional README files by completing [this free course](https://www.udacity.com/course/writing-readmes--ud777).
