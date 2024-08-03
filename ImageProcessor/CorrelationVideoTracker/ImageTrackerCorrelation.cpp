#include "ImageTrackerCorrelation.h"
#include <QDebug>

using namespace std;
using namespace vtracker;


ImageTrackerCorrelation::ImageTrackerCorrelation() : ImageTracker()
{
    _correlationTracker = nullptr;
    _targetSize = 20;
}

ImageTrackerCorrelation::~ImageTrackerCorrelation()
{
    delete _correlationTracker;
}

QRect ImageTrackerCorrelation::doProcessFrame(uint8_t *frame_mono8, int32_t width, int32_t height)
{
    if (_correlationTracker == nullptr)
    {
        _correlationTracker = new CorrelationVideoTracker();
        _correlationTracker->SetProperty(CorrelationVideoTrackerProperty::NUM_THREADS, 1);
        _correlationTracker->ExecuteCommand(vtracker::CorrelationVideoTrackerCommand::RESET);
        setTargetSize(_targetSize);
    };

    _correlationTracker->ProcessFrame(frame_mono8, width, height);

    CorrelationVideoTrackerResultData trackerData = _correlationTracker->GetTrackerResultData();
    if (trackerData.mode == CVT_TRACKING_MODE_INDEX)
    {
        //QRect result(trackerData.strobe_x - (trackerData.strobe_w / 2) + trackerData.substrobe_x - (trackerData.substrobe_w / 2),
        //                     trackerData.strobe_y - (trackerData.strobe_h / 2) + trackerData.substrobe_y - trackerData.substrobe_h / 2,
        //                     trackerData.substrobe_w, trackerData.substrobe_h);
        QRect result(trackerData.trackingRectangleCenterX - trackerData.trackingRectangleWidth / 2,
                     trackerData.trackingRectangleCenterY - trackerData.trackingRectangleHeight / 2,
                     trackerData.trackingRectangleWidth, trackerData.trackingRectangleHeight);

        return result;
    }
    else
        return QRect(0, 0, 0, 0);
}

void ImageTrackerCorrelation::lockTarget(const QPoint &targetCenter)
{
    if (_correlationTracker != nullptr)
    {
        unlockTarget();
        setTargetSize(_targetSize);
        _correlationTracker->ExecuteCommand(CorrelationVideoTrackerCommand::CAPTURE, targetCenter.x(), targetCenter.y(), -1, nullptr);
    }
}

void ImageTrackerCorrelation::unlockTarget()
{
    if (_correlationTracker != nullptr)
        _correlationTracker->ExecuteCommand(CorrelationVideoTrackerCommand::RESET, -1, -1, -1, nullptr);
}

void ImageTrackerCorrelation::setTargetSize(int size)
{
    _targetSize = size;
    if (_correlationTracker != nullptr)
    {
        _correlationTracker->SetProperty(CorrelationVideoTrackerProperty::TRACKING_RECTANGLE_WIDTH, _targetSize);
        _correlationTracker->SetProperty(CorrelationVideoTrackerProperty::TRACKING_RECTANGLE_HEIGHT, _targetSize);
    }
}
