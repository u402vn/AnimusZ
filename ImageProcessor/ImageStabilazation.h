#ifndef IMAGESTABILAZATION_H
#define IMAGESTABILAZATION_H

#include <QPoint>
#include "Common/CommonData.h"

class FloatFilter final
{
    double _minValue, _maxValue, _k;
    double _sum;
public:
    FloatFilter(double minValue, double maxValue, double k);
    double process(double value);
};

class ImageStabilazation final
{
private:
    quint32 _frameNumber;
    FloatFilter _filterX, _filterY, _filterA;
    FrameShift2D _lastShift, _correctionShift;

    bool _verticalMirror;
public:
    ImageStabilazation(bool verticalMirror);
    ~ImageStabilazation();

    void ProcessFrame(const QImage &sourceFrame);
    FrameShift2D getLastFrameCorrectionShift() const;
    FrameShift2D getLastFrameShift() const;
};

#endif // IMAGESTABILAZATION_H
