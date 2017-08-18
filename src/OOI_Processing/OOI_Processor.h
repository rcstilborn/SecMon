/*
 * OOIProcessor.h
 *
 *  Created on: Oct 23, 2015
 *      Author: richard
 */

#ifndef OOI_PROCESSING_OOI_PROCESSOR_H_
#define OOI_PROCESSING_OOI_PROCESSOR_H_

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/function/function_fwd.hpp>
#include <boost/function.hpp>
#include <vector>
#include <map>

#include "OOI.h"
//#include "Classifier.h"

class SceneInterface;
class FrameSequence;
class Frame;

class OOI_Processor  : private boost::noncopyable {
public:
	OOI_Processor(boost::asio::io_service& io_service, SceneInterface& sceneIf, FrameSequence& frameSequence, boost::function<void (const int)> next);
	virtual ~OOI_Processor();
	void processNextFrame(const int frameId);
	void classifyThis(boost::shared_ptr<Frame> frame0, OOI* newOOI);

private:
	/// Strand to ensure the connection's handlers are not called concurrently.
	boost::asio::io_service::strand strand;
//    Classifier caffeClassifier;

	SceneInterface& sceneIf;
	FrameSequence& frameSequence;
	boost::function<void (const int)> next;
	unsigned int nextOoiId = 0;
	std::map<unsigned int, boost::shared_ptr<OOI>> ooiList;
	void processFrame(const int frameId);
	void createNewOOI(std::vector<cv::Rect>::iterator rois_it,
			const int frameId, const boost::shared_ptr<Frame>& frame0,
			int& newOOIs, std::vector<cv::Rect>& rois);
};

#endif /* OOI_PROCESSING_OOI_PROCESSOR_H_ */
