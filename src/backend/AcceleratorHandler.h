//
// Created by matzipan on 11/03/17.
//

#ifndef SLAM_OCMHANDLER_H
#define SLAM_OCMHANDLER_H

#include "xtoplevel.h"

class AcceleratorHandler {
public:
    AcceleratorHandler();
    ~AcceleratorHandler();

    void *getMemoryPointer();
    void setN(uint n);
    void start();
    uint isDone();

private:
    /// Memory file descriptor
    int memd = 0;

    /// Pointer to OCM memory block
    void* ocm = 0;

    XToplevel xToplevelInstance;
};

#endif //SLAM_OCMHANDLER_H
