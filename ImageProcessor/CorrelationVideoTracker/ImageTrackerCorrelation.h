#ifndef IMAGETRACKERCORRELATION_H
#define IMAGETRACKERCORRELATION_H

#include <QPoint>
#include <QRect>
#include "Common/CommonData.h"
#include "ImageProcessor/CorrelationVideoTracker/CorrelationVideoTracker.h"
#include "ImageProcessor/ImageTracker.h"

class ImageTrackerCorrelation : public ImageTracker
{
private:
    vtracker::CorrelationVideoTracker *_correlationTracker;
    int _targetSize;
public:
    explicit ImageTrackerCorrelation();
    virtual ~ImageTrackerCorrelation();


    virtual QRect doProcessFrame(uint8_t* frame_mono8, int32_t width, int32_t height);
    virtual void lockTarget(const QPoint &targetCenter);
    virtual void unlockTarget();
    virtual void setTargetSize(int size);
};

#endif // IMAGETRACKERCORRELATION_H
