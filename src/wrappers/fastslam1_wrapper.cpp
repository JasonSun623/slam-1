#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iterator>

#include <Eigen/Dense>

#include "src/core.h"
#include "src/plot.h"
#include "src/particle.h"
#include "src/algorithms/fastslam1.h"
#include "fastslam1_wrapper.h"

using namespace std;
using namespace Eigen;

// global variable
extern Plot *g_plot;
extern Conf *g_conf;

FastSLAM1_Wrapper::FastSLAM1_Wrapper(QObject *parent) : Wrapper_Thread(parent) {
    algorithm = new FastSLAM1();
}

FastSLAM1_Wrapper::~FastSLAM1_Wrapper() {
    wait();
}

void FastSLAM1_Wrapper::run() {
    printf("FastSLAM 1\n\n");

    MatrixXf landmarks; //landmark positions
    MatrixXf waypoints; //way points

    int pos_i = 0;
    double time_all;

    int m, n;

    QString msgAll;

    QVector<double> arrParticles_x, arrParticles_y;
    QVector<double> arrParticlesFea_x, arrParticlesFea_y;

    QVector<double> arrWaypoints_x, arrWaypoints_y;
    QVector<double> arrLandmarks_x, arrLandmarks_y;
    double x_min, x_max, y_min, y_max;

    double w_max;
    double x_mean, y_mean, t_mean;

    int draw_skip = 4;

    g_conf->i("draw_skip", draw_skip);

    x_min = 1e30;
    x_max = -1e30;
    y_min = 1e30;
    y_max = -1e30;

    read_slam_input_file(fnMap, &landmarks, &waypoints);

    // draw waypoints
    if (runMode == SLAM_WAYPOINT) {
        m = waypoints.rows();
        n = waypoints.cols();

        for (int i = 0; i < n; i++) {
            arrWaypoints_x.push_back(waypoints(0, i));
            arrWaypoints_y.push_back(waypoints(1, i));

            if (waypoints(0, i) > x_max) { x_max = waypoints(0, i); }
            if (waypoints(0, i) < x_min) { x_min = waypoints(0, i); }
            if (waypoints(1, i) > y_max) { y_max = waypoints(1, i); }
            if (waypoints(1, i) < y_min) { y_min = waypoints(1, i); }
        }

        g_plot->setWaypoints(arrWaypoints_x, arrWaypoints_y);
    }

    // draw landmarks
    m = landmarks.rows();
    n = landmarks.cols();
    for (int i = 0; i < n; i++) {
        arrLandmarks_x.push_back(landmarks(0, i));
        arrLandmarks_y.push_back(landmarks(1, i));

        if (landmarks(0, i) > x_max) { x_max = landmarks(0, i); }
        if (landmarks(0, i) < x_min) { x_min = landmarks(0, i); }
        if (landmarks(1, i) > y_max) { y_max = landmarks(1, i); }
        if (landmarks(1, i) < y_min) { y_min = landmarks(1, i); }
    }

    g_plot->setLandmarks(arrLandmarks_x, arrLandmarks_y);

    g_plot->setCarSize(g_conf->WHEELBASE, 0);
    g_plot->setCarSize(g_conf->WHEELBASE, 1);
    g_plot->setPlotRange(x_min - (x_max - x_min) * 0.05, x_max + (x_max - x_min) * 0.05,
                         y_min - (y_max - y_min) * 0.05, y_max + (y_max - y_min) * 0.05);

    // FIXME: force predict noise on
    g_conf->SWITCH_PREDICT_NOISE = 1;

    //normally initialized configfile.h
    MatrixXf Q(2, 2), R(2, 2);

    Q << pow(g_conf->sigmaV, 2), 0, 0, pow(g_conf->sigmaG, 2);

    R << g_conf->sigmaR * g_conf->sigmaR, 0, 0, g_conf->sigmaB * g_conf->sigmaB;


    //vector of particles (their count will change)
    vector<Particle> particles(g_conf->NPARTICLES);
    for (unsigned long i = 0; i < particles.size(); i++) {
        particles[i] = Particle();
    }

    //initialize particle weights as uniform
    float uniformw = 1.0 / (float) particles.size();
    for (int p = 0; p < particles.size(); p++) {
        particles[p].setW(uniformw);
    }

    VectorXf xtrue(3);
    xtrue.setZero();

    float dt = g_conf->DT_CONTROLS; //change in time btw predicts
    float dtsum = 0; //change in time since last observation

    vector<int> ftag; //identifier for each landmark
    for (int i = 0; i < landmarks.cols(); i++) {
        ftag.push_back(i);
    }

    //data ssociation table
    VectorXf data_association_table(landmarks.cols());
    for (int s = 0; s < data_association_table.size(); s++) {
        data_association_table[s] = -1;
    }

    int iwp = 0; //index to first waypoint
    int nloop = g_conf->NUMBER_LOOPS;
    float V = g_conf->V; // default velocity
    float G = 0; //initial steer angle
    MatrixXf plines; //will later change to list of points

    if (g_conf->SWITCH_SEED_RANDOM != 0) {
        srand(g_conf->SWITCH_SEED_RANDOM);
    }

    MatrixXf Qe = MatrixXf(Q);
    MatrixXf Re = MatrixXf(R);

    if (g_conf->SWITCH_INFLATE_NOISE == 1) {
        Qe = 2 * Q;
        Re = 2 * R;
    }

    vector<VectorXf> z; //range and bearings of visible landmarks

    pos_i = 0;
    time_all = 0.0;

    // initial position
    g_plot->addPos(xtrue(0), xtrue(1));
    g_plot->setCarPos(xtrue(0), xtrue(1), xtrue(2), 0);

    g_plot->addPosEst(xtrue(0), xtrue(1));
    g_plot->setCarPos(xtrue(0), xtrue(1), xtrue(2), 1);

    emit replot();

    float *VnGn = new float[2];
    float Vn, Gn;
    float V_ori = V;
    int cmd;

    //Main loop
    while (isAlive) {
        if (runMode == SLAM_WAYPOINT) {
            if (iwp == -1) {
                break;
            }

            compute_steering(xtrue, waypoints, iwp, g_conf->AT_WAYPOINT, G, g_conf->RATEG, g_conf->MAXG, dt);

            if (iwp == -1 && nloop > 1) {
                iwp = 0;
                nloop--;
            }
        }
        if (runMode == SLAM_INTERACTIVE) {
            getCommand(&cmd);

            // no commands then continue
            if (cmd == -1) {
                continue;
            }

            switch (cmd) {
                case 1:
                    // forward
                    V = V_ori;
                    G = 0.0;
                    break;
                case 2:
                    // backward
                    V = -V_ori;
                    G = 0.0;
                    break;
                case 3:
                    // turn left
                    V = V_ori;
                    G = 30.0 * M_PI / 180.0;
                    break;
                case 4:
                    // turn right
                    V = V_ori;
                    G = -30.0 * M_PI / 180.0;
                    break;
                default:
                    V = V_ori;
                    G = 0.0;
            }
        }

        // get true position
        predict_true(xtrue, V, G, g_conf->WHEELBASE, dt);

        // add process noise
        add_control_noise(V, G, Q, g_conf->SWITCH_CONTROL_NOISE, VnGn);
        Vn = VnGn[0];
        Gn = VnGn[1];

        dtsum += dt;
        bool observe = false;

        if (dtsum >= g_conf->DT_OBSERVE) {
            observe = true;
            dtsum = 0;

            //Compute true data, then add noise
            vector<int> ftag_visible = vector<int>(ftag); //modify the copy, not the ftag

            //z is the range and bearing of the observed landmark
            z = get_observations(xtrue, landmarks, ftag_visible, g_conf->MAX_RANGE);
            add_observation_noise(z, R, g_conf->SWITCH_SENSOR_NOISE);

            plines = make_laser_lines(z, xtrue);

            algorithm->sim(particles, z, ftag_visible, data_association_table, R, g_conf->NEFFECTIVE, g_conf->SWITCH_RESAMPLE == 1);
        }


        // update status bar
        time_all = time_all + dt;
        pos_i++;

        // accelate drawing speed
        if (pos_i % draw_skip != 0) {
            continue;
        }

        msgAll.sprintf("[%6d] %7.3f", pos_i, time_all);
        emit showMessage(msgAll);

        // get mean x, y
        x_mean = 0;
        y_mean = 0;
        t_mean = 0;
        w_max = -1e30;
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i].w() > w_max) {
                w_max = particles[i].w();
                t_mean = particles[i].xv()(2);
            }
            x_mean += particles[i].xv()(0);
            y_mean += particles[i].xv()(1);
            //t_mean += pi_to_pi(particles[i].xv()(2));
        }

        x_mean = x_mean / particles.size();
        y_mean = y_mean / particles.size();
        //t_mean = t_mean / NPARTICLES;
        //printf("   x, y, t = %f %f %f\n", x_mean, y_mean, t_mean);

        // Draw particles
        arrParticles_x.clear();
        arrParticles_y.clear();
        for (int i = 0; i < particles.size(); i++) {
            arrParticles_x.push_back(particles[i].xv()(0));
            arrParticles_y.push_back(particles[i].xv()(1));
        }
        g_plot->setParticles(arrParticles_x, arrParticles_y);

        // Draw feature particles
        arrParticlesFea_x.clear();
        arrParticlesFea_y.clear();
        for (int i = 0; i < particles.size(); i++) {
            for (unsigned long j = 0; j < particles[i].xf().size(); j++) {
                arrParticlesFea_x.push_back(particles[i].xf()[j](0));
                arrParticlesFea_y.push_back(particles[i].xf()[j](1));
            }
        }
        g_plot->setParticlesFea(arrParticlesFea_x, arrParticlesFea_y);

        // add new position
        if (pos_i % 4 == 0) {
            g_plot->addPos(xtrue(0), xtrue(1));
            g_plot->addPosEst(x_mean, y_mean);
        }

        // draw current position
        g_plot->setCarPos(xtrue(0), xtrue(1), xtrue(2));
        g_plot->setCarPos(x_mean, y_mean, t_mean, 1);

        // set laser lines
        g_plot->setLaserLines(plines);

        emit replot();

        msleep(10);
    }

    delete[] VnGn;
}