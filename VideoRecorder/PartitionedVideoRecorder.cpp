#include "PartitionedVideoRecorder.h"
#include <QTime>
#include <QDebug>
#include <QImage>
#include "Common/CommonData.h"


void PartitionedVideoRecorder::swapVideoFiles()
{
    QString fileName = getVideoFileNameForFrame(_fileDirecory, _recordName, _videoFileFrameCount, _frameNumber);

}

PartitionedVideoRecorder::PartitionedVideoRecorder(QObject *parent) : QObject(parent)
{
    _frameNumber = 0;
    _frameWidth = 0;
    _frameHeight = 0;
    _videoFileQuality = VIDEO_FILE_QUALITY_DEFAULT;
}

PartitionedVideoRecorder::~PartitionedVideoRecorder()
{
    stop();
}

void PartitionedVideoRecorder::start(const QString &fileDirecory, const QString &recordName, const quint32 videoFileFrameCount, const quint32 videoFileQuality)
{
    if (_frameNumber > 0)
        stop();

    _frameNumber = 0;
    _fileDirecory = fileDirecory;
    _recordName = recordName;
    _videoFileFrameCount = videoFileFrameCount;
    _videoFileQuality = videoFileQuality;
}

void PartitionedVideoRecorder::stop()
{

}

void PartitionedVideoRecorder::saveFrame(const QImage &frame)
{
    if (_frameNumber == 0)
    {
        _frameWidth = frame.width();
        _frameHeight = frame.height();
        qDebug() << "Frame Size: " << _frameWidth << " x " << _frameHeight;
    }

    if (_frameNumber == 0 || (_videoFileFrameCount > 0 && _frameNumber % _videoFileFrameCount == 0))
        swapVideoFiles();

    _frameNumber++;
}

quint32 PartitionedVideoRecorder::frameWidth()
{
    return _frameWidth;
}

quint32 PartitionedVideoRecorder::frameHeight()
{
    return _frameHeight;
}

QString getVideoFileNameForFrame(const QString &fileDirecory, const QString &recordName, quint32 videoFileFrameCount, quint32 frameNumber)
{
    quint32 partNumber = videoFileFrameCount > 0 ? frameNumber / videoFileFrameCount + 1 : 1;
    QString fileName = QString("%1/%2_%3.avi").arg(fileDirecory).arg(recordName).arg(partNumber);
    return fileName;
}
