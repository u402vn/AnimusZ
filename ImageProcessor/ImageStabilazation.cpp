#include "ImageStabilazation.h"
#include <QtGlobal>
#include <QDebug>

using namespace std;

#define CORNER_RETRACK_FRAMES 30
#define EXPECTED_POINTS_COUNT 10
#define MAXIMAL_POINTS_COUNT 20
#define MINIMAL_DISTANCE_BETWEEN_POINTS 50
#define MAXIMAL_FRAME_WIDTH 720.0
#define MAXIMAL_OFFSET_COMPENSATION MAXIMAL_FRAME_WIDTH / 4
#define MAXIMAL_ROTATION_COMPENSATION 20
#define FILTER_UPDATE_K 0.9

ImageStabilazation::ImageStabilazation(bool verticalMirror) :
    _filterX(-MAXIMAL_OFFSET_COMPENSATION,   +MAXIMAL_OFFSET_COMPENSATION,   FILTER_UPDATE_K),
    _filterY(-MAXIMAL_OFFSET_COMPENSATION,   +MAXIMAL_OFFSET_COMPENSATION,   FILTER_UPDATE_K),
    _filterA(-MAXIMAL_ROTATION_COMPENSATION, +MAXIMAL_ROTATION_COMPENSATION, FILTER_UPDATE_K),
    _verticalMirror(verticalMirror)
{
    _frameNumber = 0;

    _correctionShift = {.X = 0, .Y = 0, .A = 0};
    _lastShift = {.X = 0, .Y = 0, .A = 0};
}

ImageStabilazation::~ImageStabilazation(void)
{

}

void ImageStabilazation::ProcessFrame(const QImage &sourceFrame)
{  

    _frameNumber++;
}

FrameShift2D ImageStabilazation::getLastFrameCorrectionShift() const
{
    return _correctionShift;
}

FrameShift2D ImageStabilazation::getLastFrameShift() const
{
    return _lastShift;
}

//------------------------------------------------------------------

FloatFilter::FloatFilter(double minValue, double maxValue, double k)
{
    _minValue = minValue;
    _maxValue = maxValue;
    _k = k;
    _sum = 0;
}

double FloatFilter::process(double value)
{
    value = qBound(_minValue, value, _maxValue);
    _sum = _sum * _k + value;
    return _sum;
}
