#ifndef IMAGETRACKER_H
#define IMAGETRACKER_H

#include <QPoint>
#include <QRect>
#include "Common/CommonData.h"

class ImageTracker
{
public:
    ImageTracker();
    virtual ~ImageTracker();
    virtual QRect doProcessFrame(uint8_t* frame_mono8, int32_t width, int32_t height) = 0;
    virtual void lockTarget(const QPoint &targetCenter) = 0;
    virtual void unlockTarget() = 0;
    virtual void setTargetSize(int size) = 0;
};

#endif // IMAGETRACKER_H
