/*
 * ImageSource.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 */

#ifndef OOI_PROCESSING_IMAGESOURCE_H_
#define OOI_PROCESSING_IMAGESOURCE_H_

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <boost/system/error_code.hpp>
#include <cstdbool>
#include <string>

#include "Camera.h"

class FrameSequence;
class SceneInterface;

class GUI_Interface;

class ImageSource : private boost::noncopyable {
public:
    ImageSource(const std::string& name, const std::string& url, boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence, boost::function<void (const int)> next, const int fps = 1);
    virtual ~ImageSource();

    Camera& getCamera();
    const std::string& getName() const;
    void shutdown();
    void togglePause();

    void setFPS(const int fps);

private:
    // Make sure we can't copy them
//    Scene(const Scene&);
//    Scene& operator=(Scene);

    const std::string& name;
    int interval;
    Camera camera;

    SceneInterface& sceneIf;
    FrameSequence& frameSequence;

    boost::function<void (const int)> next;

    boost::asio::deadline_timer timer;
    boost::asio::strand strand;

    bool shuttingDown;
    void getNextFrame(const boost::system::error_code& ec);

    void restartTimer();
    void startTimer();

    bool isPaused_ = false;
};

#endif /* OOI_PROCESSING_IMAGESOURCE_H_ */
