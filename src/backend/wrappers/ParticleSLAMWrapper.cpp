//
// Created by matzipan on 30/01/17.
//
#include <src/backend/core.h>

#include "ParticleSLAMWrapper.h"

ParticleSLAMWrapper::ParticleSLAMWrapper(Conf *conf, NetworkPlot *plot) : SLAMWrapper(conf, plot) {
    particles = vector<Particle>(conf->NPARTICLES);
}

ParticleSLAMWrapper::~ParticleSLAMWrapper() { }

void ParticleSLAMWrapper::initializeParticles() {
    // Vector of particles (their count will change)
    for (unsigned long i = 0; i < particles.size(); i++) {
        particles[i] = Particle();
    }

    // Initialize particle weights as uniform
    float uniformw = 1.0 / (float) particles.size();
    for (int p = 0; p < particles.size(); p++) {
        particles[p].setW(uniformw);
    }
}

void ParticleSLAMWrapper::initializeDataAssociationTable() {
    dataAssociationTable = VectorXf(landmarks.cols());
    for (int s = 0; s < dataAssociationTable.size(); s++) {
        dataAssociationTable[s] = -1;
    }
}

void ParticleSLAMWrapper::drawParticles() {
    std::vector<double> particleXs, particleYs;

    for (int i = 0; i < particles.size(); i++) {
        particleXs.push_back(particles[i].xv()(0));
        particleYs.push_back(particles[i].xv()(1));
    }
    plot->setParticles(particleXs, particleYs);
}

void ParticleSLAMWrapper::drawFeatureParticles() {
    std::vector<double> featureParticleXs, featureParticleYs;

    for (int i = 0; i < particles.size(); i++) {
        for (unsigned long j = 0; j < particles[i].landmarkXs().size(); j++) {
            featureParticleXs.push_back(particles[i].landmarkXs()[j](0));
            featureParticleYs.push_back(particles[i].landmarkXs()[j](1));
        }
    }
    plot->setFeatureParticles(featureParticleXs, featureParticleYs);
}

void ParticleSLAMWrapper::computeEstimatedPosition(double &x, double &y, double &t) {
    // Compute xEstimated and y as the mean of xEstimated and y over all the particles and the t is the angle corresponding to the particle
    // with the highest weight.
#define ESTIMATE_WITH_MEAN
#if !defined(ESTIMATE_WITH_MEDIAN) && !defined(ESTIMATE_WITH_WEIGHTS)
    x = 0;
    y = 0;
    t = 0;

    double wMax = -1e30;
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].w() > wMax) {
            wMax = particles[i].w();
            t = particles[i].xv()(2);
        }
        x += particles[i].xv()(0);
        y += particles[i].xv()(1);
    }

    x = x / particles.size();
    y = y / particles.size();
#endif
#ifdef ESTIMATE_WITH_MEDIAN
    x = 0;
    y = 0;
    t = 0;

    std::vector<double> xVec;
    std::vector<double> yVec;

    double wMax = -1e30;
    for (int i = 0; i < particles.size(); i++) {
        if (particles[i].w() > wMax) {
            wMax = particles[i].w();
            t = particles[i].xv()(2);
        }

        insert_sorted(xVec, particles[i].xv()(0));
        insert_sorted(yVec, particles[i].xv()(1));
    }

    x = xVec[xVec.size()/2];
    y = yVec[yVec.size()/2];

#endif
#ifdef ESTIMATE_WITH_WEIGHTS
    x = 0;
    y = 0;
    t = 0;

    double wMax = -1e30;
    for (int i = 0; i < particles.size(); i++) {
        double w = particles[i].w();

        if (w > wMax) {
            wMax = w;
            t = particles[i].xv()(2);
        }

        x += particles[i].xv()(0) * w;
        y += particles[i].xv()(1) * w;
    }
#endif
}

void ParticleSLAMWrapper::insert_sorted(std::vector<double> & vec, double item) {
    vec.insert(std::upper_bound(vec.begin(), vec.end(), item), item);
}

