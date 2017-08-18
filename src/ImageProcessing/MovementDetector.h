//
// Created by richard on 10/7/15.
//

#ifndef SECURITYMONITOR_MOVEMENTDETECTOR_H
#define SECURITYMONITOR_MOVEMENTDETECTOR_H


#include <boost/smart_ptr/shared_ptr.hpp>

#include "../Frame.h"

class Frame;

class MovementDetector {

public:
    virtual void processFrame(boost::shared_ptr<Frame> frame0) = 0;
    virtual ~MovementDetector();

};


#endif //SECURITYMONITOR_MOVEMENTDETECTOR_H
