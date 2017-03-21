//
// Created by matzipan on 21/03/17.
//

#ifndef SLAM_DATAGATHERER_H
#define SLAM_DATAGATHERER_H

#include <string>
#include <vector>

class DataGatherer {
public:
    DataGatherer();
    ~DataGatherer();

    void setSimulationName(std::string name);
    void saveData();
    void setCarTruePosition(double x, double y, double t);
    void setCarEstimatedPosition(double x, double y, double t);
    void nextTurn();


private:
    double truePositionX;
    double truePositionY;
    double truePositionT;

    double estimatedPositionX;
    double estimatedPositionY;
    double estimatedPositionT;

    std::vector<double> errors;
    std::string simulationName;
};


#endif //SLAM_DATAGATHERER_H
