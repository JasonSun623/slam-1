#ifndef SLAM_THREAD_H
#define SLAM_THREAD_H

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <Eigen/Dense>
#include <plotting/NetworkPlot.h>

#include "core.h"


using namespace std;
using namespace Eigen;

class SLAMWrapper {
public:
    enum RunMode {
        /// Move along waypoints
        SLAM_WAYPOINT,
        /// User interactive
        SLAM_INTERACTIVE
    };

    SLAMWrapper(Conf *conf, NetworkPlot *plot);

    ~SLAMWrapper();

    /// Set run mode
    void setRunMode(RunMode mode);
    /// Set map filename
    void loadMap();

    virtual void run() = 0;
/*public slots:
    virtual void commandRecieve(int command);*/

protected:

    void initializeLandmarkIdentifiers();

    /// Loads information and performs initial adjustment on the plot
    void configurePlot();
    /// Adds the waypoints and landmarks to the plot
    void addWaypointsAndLandmarks();
    /// Adjusts the range of the plot to show all information
    void setPlotRange();

    /**
     *     Integrate steering to predict control
     *
     *     @return -1 if there are no more waypoints, 0 if there are no commands, otherwise 1.
     */
    int control();

    /**
     * Available commands:
     *   - 1 - Forward
     *   - 2 - Backward
     *   - 3 - Turn Left
     *   - 4 - Turn Right
     *
     *   @return command value according to above list
     */
    //int getCurrentCommand();

    Conf *conf;
    NetworkPlot *plot;

    int commandId;
    /// Timestamp when command was recieved
    uint64_t commandTime;
    RunMode runMode;

    /// Landmark positions
    MatrixXf landmarks;
    /// Waypoints
    MatrixXf waypoints;

    int currentIteration = 0;

    /// True velocity
    float Vtrue;
    /// True steer angle
    float Gtrue;

    /// Velocity with control noise
    float Vnoisy;
    /// Steer angle with control noise
    float Gnoisy;

    int nLoop;
    /// Index to first waypoint
    int indexOfFirstWaypoint = 0;
    /// True position
    VectorXf xTrue;
    /// Predicted position
    VectorXf xEstimated;
    /// Control covariance
    MatrixXf Q;
    /// Observation covariance
    MatrixXf R;
    /// Noise covariance matrix. Used for inflated control covariance.
    MatrixXf Qe;
    /// Noise covariance matrix. Used for inflated observation covariance.
    MatrixXf Re;
    /// Change in time between predicts
    float dt;
    /// Change in time since last observation
    float dtSum = 0;

    /// Identifier for each landmark
    vector<int> landmarkIdentifiers;
    MatrixXf plines; // Old comment: will later change to list of point

    /// Range and bearing for each of the observed landmarks
    vector<VectorXf> landmarksRangeBearing;

    uint32_t updateMicrotimeMark();

    long getCurrentMicrotime();

    long microtimeMark;
};

#endif // SLAM_THREAD_H
