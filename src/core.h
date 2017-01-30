#ifndef __FASTSLAM_CORE_H__
#define __FASTSLAM_CORE_H__

#include <iostream>
#include <vector>
#include <string>

#include <Eigen/Dense>

#include "utils.h"

#include "particle.h"

using namespace std;
using namespace Eigen;

////////////////////////////////////////////////////////////////////////////////
// SLAM config Variables
////////////////////////////////////////////////////////////////////////////////
class Conf : public CParamArray {
public:
    float V;
    float MAXG;
    float RATEG;
    float WHEELBASE;
    float DT_CONTROLS;

    float sigmaV;
    float sigmaG;

    float MAX_RANGE;
    float DT_OBSERVE;

    float sigmaR;
    float sigmaB;
    float sigmaT;


    float GATE_REJECT;
    float GATE_AUGMENT;

    float AT_WAYPOINT;
    int NUMBER_LOOPS;

    int NPARTICLES;
    int NEFFECTIVE;

    int SWITCH_CONTROL_NOISE;
    int SWITCH_SENSOR_NOISE;
    int SWITCH_INFLATE_NOISE;
    int SWITCH_PREDICT_NOISE;

    int SWITCH_SAMPLE_PROPOSAL;
    int SWITCH_HEADING_KNOWN;
    int SWITCH_RESAMPLE;
    int SWITCH_PROFILE;
    int SWITCH_SEED_RANDOM;

    int SWITCH_ASSOCIATION_KNOWN;
    int SWITCH_BATCH_UPDATE;
    int SWITCH_USE_IEKF;

public:
    // pase some data
    virtual int parse(void);
};

////////////////////////////////////////////////////////////////////////////////
// Common functions
////////////////////////////////////////////////////////////////////////////////
void compute_steering(VectorXf &x, MatrixXf &wp, int &iwp, float minD, float &G, float rateG, float maxG, float dt);

void predict_true(VectorXf &xv, float V, float G, float WB, float dt);

vector<VectorXf> get_observations(VectorXf &x, MatrixXf lm, vector<int> &idf, float rmax);

void get_visible_landmarks(VectorXf &x, MatrixXf &lm, vector<int> &idf, float rmax);

vector<VectorXf> compute_range_bearing(VectorXf &x, MatrixXf &lm);

vector<int> find2(vector<float> &dx, vector<float> &dy, float phi, float rmax);

void add_control_noise(float V, float G, MatrixXf &Q, int add_noise, float *VnGn);

void add_observation_noise(vector<VectorXf> &z, MatrixXf &R, int addnoise);

void KF_joseph_update(VectorXf &x, MatrixXf &P, float v, float R, MatrixXf &H);

void KF_cholesky_update(VectorXf &x, MatrixXf &P, VectorXf &v, MatrixXf &R, MatrixXf &H);


////////////////////////////////////////////////////////////////////////////////
// FastSLAM functions
////////////////////////////////////////////////////////////////////////////////
void compute_jacobians(Particle &particle, vector<int> &idf, MatrixXf &R, vector<VectorXf> &zp, vector<MatrixXf> *Hv, vector<MatrixXf> *Hf, vector<MatrixXf> *Sf);

void resample_particles(vector<Particle> &particles, int Nmin, bool do_resample);

void stratified_resample(VectorXf w, vector<int> &keep, float &Neff);

void cumulative_sum(VectorXf &w);

void data_associate_known(vector<VectorXf> &z, vector<int> &idz, VectorXf &data_association_table, int Nf, vector<VectorXf> &zf, vector<int> &idf, vector<VectorXf> &zn);

void add_feature(Particle &particle, vector<VectorXf> &z, MatrixXf &R);

void feature_update(Particle &particle, vector<VectorXf> &z, vector<int> &idf, MatrixXf &R);

////////////////////////////////////////////////////////////////////////////////
// Utils
////////////////////////////////////////////////////////////////////////////////
float pi_to_pi(float ang);

float pi_to_pi2(float ang);

MatrixXf make_symmetric(MatrixXf &P);

void transform_to_global(MatrixXf &p, VectorXf &b);

MatrixXf make_laser_lines(vector<VectorXf> &rb, VectorXf &xv);

MatrixXf line_plot_conversion(MatrixXf &lnes);

void make_covariance_ellipse(MatrixXf &x, MatrixXf &P, MatrixXf &lines);

void stratified_random(unsigned long N, vector<float> &di);

double unifRand();

VectorXf multivariate_gauss(VectorXf &x, MatrixXf &P, int n);

namespace nRandMat {
    MatrixXf randn(int m, int n);

    MatrixXf rand(int m, int n);
}

void read_slam_input_file(const string s, MatrixXf *lm, MatrixXf *wp);


#endif // end of __FASTSLAM_CORE_H__
