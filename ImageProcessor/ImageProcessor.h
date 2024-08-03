#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>
#include <QPointF>
#include <QRectF>
#include "Common/CommonData.h"
#include "ImageCorrector.h"
#include "ImageStabilazation.h"
#include "TelemetryDataFrame.h"
#include "CoordinateCalculator.h"
#include "ImageProcessor/ImageTracker.h"

class ImageProcessorThread final: public QThread
{
    Q_OBJECT

    QQueue<QImage> *_videoFrames;
    QQueue<TelemetryDataFrame> *_telemetryFrames;
    QMutex _mutex;
    bool _quit;
    QWaitCondition _waitCondition;

    StabilizationType _stabilizationType;

    ImageCorrector *_imageCorrector;
    ImageStabilazation  *_imageStabilazation;
    ImageTracker *_imageTracker;
public:
    ImageProcessorThread(QObject *parent, bool verticalMirror, ObjectTrackerTypeEnum trackerType);
    ~ImageProcessorThread();

    void run();

    void processData(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame);
    void lockTarget(const QPoint &targetCenter);
    void unlockTarget();
    void setTargetSize(int targetSize);
    void setStabilizationType(StabilizationType stabType);
    void setTuneImageSettings(qreal brightness, qreal contrast, qreal gamma, bool grayscale);
    void getTuneImageSettings(qreal &brightness, qreal &contrast, qreal &gamma, bool &grayscale);
signals:
    void dataProcessedInThread(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame);
};

class ImageProcessor final: public QObject
{
    Q_OBJECT

    ImageProcessorThread *_procThread;

    CoordinateCalculator *_coordinateCalculator;
public:
    explicit ImageProcessor(QObject *parent, CoordinateCalculator *coordinateCalculator, bool verticalMirror, ObjectTrackerTypeEnum trackerType);
    ~ImageProcessor();

    void processDataAsync(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame);
    void setTuneImageSettings(qreal brightness, qreal contrast, qreal gamma, bool grayscale);
    void getTuneImageSettings(qreal &brightness, qreal &contrast, qreal &gamma, bool &grayscale);
signals:
    void onDataProcessed(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame);
public slots:
    void lockTarget(const QPoint &targetCenter);
    void unlockTarget();
    void setTargetSize(int targetSize);
    void setStabilizationType(StabilizationType stabType);
private slots:
    void dataProcessedInThread(const TelemetryDataFrame &telemetryFrame, const QImage &videoFrame);
};

#endif // IMAGEPROCESSOR_H
