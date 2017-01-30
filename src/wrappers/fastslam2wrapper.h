#ifndef __FASTSLAM_2_H__
#define __FASTSLAM_2_H__

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
#include "src/algorithms/fastslam2.h"
#include "ParticleSLAMWrapper.h"

using namespace std;
using namespace Eigen;

class FastSLAM2Wrapper : public ParticleSLAMWrapper {
Q_OBJECT

public:
    FastSLAM2Wrapper(QObject *parent = 0);

    ~FastSLAM2Wrapper();

protected:
    void run();

    FastSLAM2 *algorithm;
};

#endif // __FASTSLAM_2_H__
