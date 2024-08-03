#include "ImageProcessor.h"
#include "ImageProcessor/CorrelationVideoTracker/ImageTrackerCorrelation.h"

ImageProcessor::ImageProcessor(QObject *parent, CoordinateCalculator *coordinateCalculator, bool verticalMirror, ObjectTrackerTypeEnum trackerType) : QObject(parent),
    _coordinateCalculator(coordinateCalculator)
{    
    _procThread = new ImageProcessorThread(nullptr, verticalMirror, trackerType);

    connect(_procThread, &ImageProcessorThread::dataProcessedInThread, this, &ImageProcessor::dataProcessedInThread);
}

ImageProcessor::~ImageProcessor()
{
    delete _procThread;
}

void ImageProcessor::processDataAsync(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame)
{
    _procThread->processData(telemetryFrame, videoFrame);
}

void ImageProcessor::lockTarget(const QPoint &targetCenter)
{    
    _procThread->lockTarget(targetCenter);
}

void ImageProcessor::unlockTarget()
{
    _procThread->unlockTarget();
}

void ImageProcessor::setTargetSize(int targetSize)
{
    _procThread->setTargetSize(targetSize);
}

void ImageProcessor::setStabilizationType(StabilizationType stabType)
{
    _procThread->setStabilizationType(stabType);
}

void ImageProcessor::setTuneImageSettings(qreal brightness, qreal contrast, qreal gamma, bool grayscale)
{
    _procThread->setTuneImageSettings(brightness, contrast, gamma, grayscale);
}

void ImageProcessor::getTuneImageSettings(qreal &brightness, qreal &contrast, qreal &gamma, bool &grayscale)
{
    _procThread->getTuneImageSettings(brightness, contrast, gamma, grayscale);
}

void ImageProcessor::dataProcessedInThread(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame)
{    
    TelemetryDataFrame frame = telemetryFrame;
    _coordinateCalculator->processTelemetryDataFrame(&frame);

    emit onDataProcessed(frame, videoFrame);
}

ImageProcessorThread::ImageProcessorThread(QObject *parent, bool verticalMirror, ObjectTrackerTypeEnum trackerType): QThread(parent)
{
    _quit = false;
    _videoFrames = new QQueue<QImage>();
    _telemetryFrames = new QQueue<TelemetryDataFrame>();
    _imageCorrector = new ImageCorrector();
    _imageStabilazation = new ImageStabilazation(verticalMirror);

    _stabilizationType = StabilizationType::StabilizationByFrame;

    switch (trackerType)
    {
    case ObjectTrackerTypeEnum::InternalCorrelation:
        _imageTracker = new ImageTrackerCorrelation();
        break;
    case ObjectTrackerTypeEnum::External:
        _imageTracker = nullptr;
        break;
    default:
        _imageTracker = nullptr;
    }
}

ImageProcessorThread::~ImageProcessorThread()
{
    _quit = true;
    _waitCondition.wakeOne();
    wait();
    delete _videoFrames;
    delete _telemetryFrames;
    delete _imageCorrector;
    delete _imageStabilazation;
    if ( _imageTracker != nullptr)
        delete _imageTracker;
}

void ImageProcessorThread::run()
{
    while (!_quit)
    {
        _mutex.lock();
        if (_videoFrames->isEmpty())
            _waitCondition.wait(&_mutex);
        if (_videoFrames->isEmpty())
        {
            _mutex.unlock();
            break;
        }

        QImage videoFrame = _videoFrames->dequeue();
        TelemetryDataFrame telemetryFrame = _telemetryFrames->dequeue();

        _mutex.unlock();

        if ( !videoFrame.isNull() )
        {
            videoFrame = _imageCorrector->ProcessFrame(videoFrame);

            _imageStabilazation->ProcessFrame(videoFrame);
            FrameShift2D correctionFrameShift = _imageStabilazation->getLastFrameCorrectionShift();

            if (_imageTracker != nullptr)
            {
                QImage gsImage = videoFrame.convertToFormat(QImage::Format_Grayscale8);
                QRect targetRect = _imageTracker->doProcessFrame((uint8_t *)gsImage.constBits(), gsImage.width(), gsImage.height());

                telemetryFrame.TrackedTargetState = targetRect.width() > 0 ? 1 : 0;

                if (telemetryFrame.TrackedTargetState > 0)
                {
                    telemetryFrame.TrackedTargetCenterX = targetRect.center().x();
                    telemetryFrame.TrackedTargetCenterY = targetRect.center().y();
                    telemetryFrame.TrackedTargetRectWidth = targetRect.width();
                    telemetryFrame.TrackedTargetRectHeight = targetRect.height();
                }
                else
                {
                    telemetryFrame.TrackedTargetCenterX = 0;
                    telemetryFrame.TrackedTargetCenterY = 0;
                    telemetryFrame.TrackedTargetRectWidth = 0;
                    telemetryFrame.TrackedTargetRectHeight = 0;
                }
            }

            if ((_stabilizationType == StabilizationType::StabilizationByTarget) && telemetryFrame.targetIsVisible())
            {
                telemetryFrame.StabilizedCenterX = telemetryFrame.TrackedTargetCenterX;
                telemetryFrame.StabilizedCenterY = telemetryFrame.TrackedTargetCenterY;
                telemetryFrame.StabilizedRotationAngle = 0;
            }
            else
            {
                telemetryFrame.StabilizedCenterX = videoFrame.width() / 2 + correctionFrameShift.X;
                telemetryFrame.StabilizedCenterY = videoFrame.height() / 2 + correctionFrameShift.Y;
                telemetryFrame.StabilizedRotationAngle = correctionFrameShift.A;
            }
        }
        emit dataProcessedInThread(telemetryFrame, videoFrame);
    }
}

void ImageProcessorThread::processData(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame)
{
    _mutex.lock();
    _videoFrames->enqueue(videoFrame);
    _telemetryFrames->enqueue(telemetryFrame);
    _mutex.unlock();
    if (!isRunning())
        start();
    else
        _waitCondition.wakeOne();
}

void ImageProcessorThread::lockTarget(const QPoint &targetCenter)
{
    if (_imageTracker != nullptr)
    {
        _mutex.lock();
        _imageTracker->lockTarget(targetCenter);
        _mutex.unlock();
    }
}

void ImageProcessorThread::unlockTarget()
{
    if (_imageTracker != nullptr)
    {
        _mutex.lock();
        _imageTracker->unlockTarget();
        _mutex.unlock();
    }
}

void ImageProcessorThread::setTargetSize(int targetSize)
{
    if (_imageTracker != nullptr)
    {
        _mutex.lock();
        _imageTracker->setTargetSize(targetSize);
        _mutex.unlock();
    }
}

void ImageProcessorThread::setStabilizationType(StabilizationType stabType)
{
    _stabilizationType = stabType;
}

void ImageProcessorThread::setTuneImageSettings(qreal brightness, qreal contrast, qreal gamma, bool grayscale)
{
    _mutex.lock();
    _imageCorrector->setTuneImageSettings(brightness, contrast, gamma, grayscale);
    _mutex.unlock();
}

void ImageProcessorThread::getTuneImageSettings(qreal &brightness, qreal &contrast, qreal &gamma, bool &grayscale)
{
    _mutex.lock();
    _imageCorrector->getTuneImageSettings(brightness, contrast, gamma, grayscale);
    _mutex.unlock();
}
