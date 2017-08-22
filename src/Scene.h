/*
 * Scene.h
 *
 *  Created on: Jul 28, 2015
 *      Author: richard
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <boost/core/noncopyable.hpp>
#include <cstdbool>
#include <string>

#include "FrameSequence.h"
#include "ImageProcessing/ImageProcessor.h"
#include "ImageSource/ImageSource.h"
#include "OOI_Processing/OOI_Processor.h"
#include "SceneInterface.h"

class GUI_Interface;

class Scene : private boost::noncopyable {
public:
    Scene(const std::string& name, const std::string& url, boost::asio::io_service& io_service, GUI_Interface& gui, const int fps = 1);
    virtual ~Scene();

    Camera& getCamera();
    FrameSequence& getFrameSequence();
    const std::string& getName() const;
    void shutdown();
    void togglePause();
    void setFPS(const int fps);

private:
    const std::string& name;
//    int interval;
    FrameSequence frameSequence;
    SceneInterface sceneIf;
    OOI_Processor ooiProcessor;
    ImageProcessor imageProcessor;
    ImageSource imageSource;

    bool isPaused_ = false;
};

#endif /* SCENE_H_ */
