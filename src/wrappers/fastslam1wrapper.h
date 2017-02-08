#ifndef __FASTSLAM_1_H__
#define __FASTSLAM_1_H__

#include <QtGui>
#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>

#include <Eigen/Dense>

#include "src/core.h"
#include "slamwrapper.h"
#include "src/algorithms/fastslam1.h"
#include "ParticleSLAMWrapper.h"

using namespace std;
using namespace Eigen;

class FastSLAM1Wrapper : public ParticleSLAMWrapper {
    Q_OBJECT

public:
    FastSLAM1Wrapper(Conf *conf, NetworkPlot *plot, QObject *parent = 0);

    ~FastSLAM1Wrapper();

protected:
    void run();

    FastSLAM1* algorithm;
};

#endif // __FASTSLAM_1_H__
