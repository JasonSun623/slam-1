//
// Created by matzipan on 30/01/17.
//

#ifndef SLAM_GUI_PARTICLESLAMWRAPPER_H
#define SLAM_GUI_PARTICLESLAMWRAPPER_H

#include <Particle.h>
#include "slamwrapper.h"

class ParticleSLAMWrapper : public SLAMWrapper {
public:
    ParticleSLAMWrapper(Conf *conf, NetworkPlot *plot);

    ~ParticleSLAMWrapper();

protected:
    void initializeParticles();
    void initializeDataAssociationTable();

    void drawParticles();
    void drawFeatureParticles();

    /**
     * Compute estimated position over all the particles.
     *
     * @param [out] x - x coordinate
     * @param [out] y - y coordinate
     * @param [out] t - angle
     */
    void computeEstimatedPosition(double &x, double &y, double &t);

    vector<Particle> particles;
    VectorXf dataAssociationTable;

    void insert_sorted(vector<double> &vec, double item);
};

#endif //SLAM_GUI_PARTICLESLAMWRAPPER_H
