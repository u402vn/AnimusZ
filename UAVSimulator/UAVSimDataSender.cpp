#include "UAVSimDataSender.h"
#include "HardwareLink/lz4.h"
#include "DataAccess/csv.h"
#include <QFileInfo>
#include <QStringList>
#include <QtMath>
#include <QTcpSocket>
#include <QPainter>
#include "Common/CommonWidgets.h"

const int CUDPPACKETVIDEODATALENGTH = 720;

#pragma pack(push, 1)
struct UDPSimpleVideoMessage {
    unsigned frameNumber;
    unsigned short line;
    unsigned short part;
    unsigned char frameData[CUDPPACKETVIDEODATALENGTH];
};
#pragma pack(pop)

const int  XPLANE_PACKET_CHUNKSIZE = 1400;
const int  XPLANE_IMAGE_WIDTH      =  720;
const int  XPLANE_IMAGE_HEIGHT     =  576;
const int  XPLANE_IMAGE_BPP        =    4;
const int  XPLANE_IMAGE_DATA_SIZE  = XPLANE_IMAGE_WIDTH * XPLANE_IMAGE_HEIGHT * XPLANE_IMAGE_BPP;


#pragma pack(push,1)
struct XPLANE_PACKET {
    unsigned       frameId        = 0;
    int            frameTotalSize = 0;
    unsigned short framePartNo    = 0;
    quint8         frameData[XPLANE_PACKET_CHUNKSIZE];
};
#pragma pack(pop)

void UAVSimDataSender::updateCamPitchRoll()
{

    if (_controlMode == AbsolutePosition)
    {
        if (_camX < _camXTarget)
            _camX++;
        else if (_camX > _camXTarget)
            _camX--;
        if (qAbs(_camX - _camXTarget) < 1)
            _camX = _camXTarget;

        if (_camY < _camYTarget)
            _camY++;
        else if (_camY > _camYTarget)
            _camY--;
        if (qAbs(_camY - _camYTarget) < 1)
            _camY = _camYTarget;
    }
    else
    {
        _camX += _camXTarget;
        _camY += _camYTarget;
    }

    if (_camZoom < _camZoomTarget)
        _camZoom++;
    else if (_camZoom > _camZoomTarget)
        _camZoom--;
    if (qAbs(_camZoom - _camZoomTarget) < 1)
        _camZoom = _camZoomTarget;
}

bool UAVSimDataSender::loadFromCSV(const QString &fileName)
{
    _currentMessageNumber = 5000; //???

    QFileInfo checkFile(fileName);
    if (!checkFile.exists() || !checkFile.isFile())
        return false;

    double roll, pitch, yaw, gps_lat, gps_lon, gps_hmsl, gps_course, gps_Vnorth, gps_Veast, airspeed, windSpd, windHdg;

    io::CSVReader<12> telemetryFile(fileName.toLocal8Bit().constData());
    telemetryFile.read_header(io::ignore_extra_column, "roll", "pitch", "yaw", "gps_lat", "gps_lon", "gps_hmsl", "gps_course", "gps_Vnorth", "gps_Veast", "airspeed", "windSpd", "windHdg");
    while(telemetryFile.read_row(roll, pitch, yaw, gps_lat, gps_lon, gps_hmsl, gps_course, gps_Vnorth, gps_Veast, airspeed, windSpd, windHdg))
    {
        UDPSimulatorTelemetryMessageV4 telemetryMessage;

        telemetryMessage.UavRoll = roll;
        telemetryMessage.UavPitch = pitch;
        telemetryMessage.UavYaw = yaw;
        telemetryMessage.UavLatitude_GPS = gps_lat;
        telemetryMessage.UavLongitude_GPS = gps_lon;
        telemetryMessage.UavAltitude_GPS = gps_hmsl;
        telemetryMessage.Course_GPS = gps_course;
        telemetryMessage.GroundSpeed_GPS = 0;
        telemetryMessage.AirSpeed = airspeed;

        telemetryMessage.GroundSpeedNorth_GPS = gps_Vnorth;
        telemetryMessage.GroundSpeedEast_GPS = gps_Veast;

        telemetryMessage.WindSpeed = windSpd;
        telemetryMessage.WindDirection = windHdg;

        telemetryMessage.CamRoll = 0;
        telemetryMessage.CamPitch = 0;
        telemetryMessage.CamYaw = 0;
        telemetryMessage.CamZoom = 1;

        telemetryMessage.RangefinderDistance = 0;

        _telemetryMessages.append(telemetryMessage);
    }
    return true;
}


void UAVSimDataSender::loadXPlaneVideo(const QString &fileName)
{
    _frameNumber = 0;


    _frame.load(fileName);
    _frame = _frame.convertToFormat(QImage::Format_ARGB32);


}

void UAVSimDataSender::sendTelemetryMassage()
{
    UDPSimulatorTelemetryMessageV4 telemetryMessage = _telemetryMessages.at(_currentMessageNumber);

    telemetryMessage.UavRoll =              _isFreezedUavRoll ?        _freezedUavRoll :        telemetryMessage.UavRoll;
    telemetryMessage.UavPitch =             _isFreezedUavPitch ?       _freezedUavPitch :       telemetryMessage.UavPitch;
    telemetryMessage.UavYaw =               _isFreezedUavYaw ?         _freezedUavYaw :         telemetryMessage.UavYaw;
    telemetryMessage.UavLatitude_GPS =      _isFreezedGPSLat ?         _freezedGPSLat :         telemetryMessage.UavLatitude_GPS;
    telemetryMessage.UavLongitude_GPS =     _isFreezedGPSLon ?         _freezedGPSLon :         telemetryMessage.UavLongitude_GPS;
    telemetryMessage.UavAltitude_GPS =      _isFreezedGPSHmsl ?        _freezedGPSHmsl :        telemetryMessage.UavAltitude_GPS;
    telemetryMessage.Course_GPS =           _isFreezedGPSCourse ?      _freezedGPSCourse :      telemetryMessage.Course_GPS;
    telemetryMessage.WindDirection =        _isFreezedWindDirection ?  _freezedWindDirection :  telemetryMessage.WindDirection;
    telemetryMessage.WindSpeed =            _isFreezedWindSpeed ?      _freezedWindSpeed :      telemetryMessage.WindSpeed;

    telemetryMessage.CamPitch =             _camY;
    telemetryMessage.CamYaw =               _camX;
    telemetryMessage.CamRoll =              _camZ;
    telemetryMessage.CamZoom =              _camZoom;

    telemetryMessage.CamEncoderPitch = telemetryMessage.CamPitch / 360.0 * 16384.0;
    telemetryMessage.CamEncoderYaw = telemetryMessage.CamYaw / 360.0 * 16384.0;
    telemetryMessage.CamEncoderRoll = telemetryMessage.CamRoll / 360.0 * 16384.0;

    telemetryMessage.BombState =            _bombState ? 000 : 111;

    telemetryMessage.RangefinderDistance =  _isRangefinderDistance ? _freezedRangefinderDistance : telemetryMessage.RangefinderDistance;

    // https://forum.qt.io/topic/37774/endianess-of-qbytearray/11
    // https://stackoverflow.com/questions/2782725/converting-float-values-from-big-endian-to-little-endian
    // https://betterexplained.com/articles/understanding-big-and-little-endian-byte-order/

    _udpTelemetrySocket.writeDatagram((char*)&telemetryMessage, sizeof(telemetryMessage),
                                      QHostAddress::LocalHost, _telemetryUDPPort);

    if (!_pause)
    {
        _currentMessageNumber++;
        if (_currentMessageNumber == _telemetryMessages.count())
            _currentMessageNumber = 0;
    }

    QByteArray msg = QByteArray::fromHex("0f0b00000000000000001a");
    _udpTelemetrySocket.writeDatagram(msg, QHostAddress::LocalHost, 50011);
    msg = QByteArray::fromHex("050f0000000041980000421000003f");
    _udpTelemetrySocket.writeDatagram(msg, QHostAddress::LocalHost, 50011);
    msg = QByteArray::fromHex("030fbf28c0003eca8000c09b640000");
    _udpTelemetrySocket.writeDatagram(msg, QHostAddress::LocalHost, 50011);
    msg = QByteArray::fromHex("100f06000606000000000000000031");
    _udpTelemetrySocket.writeDatagram(msg, QHostAddress::LocalHost, 50011);
}


void UAVSimDataSender::sendXPLaneVideoMessage()
{
    _frameNumber++;

    QPainter painter;
    painter.begin(&_frame);


    QPen pen = painter.pen();
    pen.setColor(QColor("#FFFFFF"));
    pen.setWidth(2);
    painter.setPen(pen);

    QFont font = painter.font();
    font.setPointSize(15);
    painter.setFont(font);

    CommonWidgetUtils::drawText(painter, QPoint(100, 100), Qt::AlignTop | Qt::AlignLeft, tr("Frame %1").arg(_frameNumber), true);


    painter.end();

    int compressedSizeEstimation = LZ4_compressBound(XPLANE_IMAGE_DATA_SIZE);
    _compressedXPlaneData = new quint8[compressedSizeEstimation];

    _compressedXPlaneDataSize = LZ4_compress_default((char*)_frame.bits(),
                                                     (char*)_compressedXPlaneData,
                                                     XPLANE_IMAGE_DATA_SIZE,
                                                     compressedSizeEstimation);






    XPLANE_PACKET videoPacket;
    int beginPos = 0, endPos = 0;
    videoPacket.framePartNo = 0;
    videoPacket.frameTotalSize = _compressedXPlaneDataSize;
    videoPacket.frameId = _frameNumber;

    int framePartCount = qCeil((float)videoPacket.frameTotalSize / XPLANE_PACKET_CHUNKSIZE);
    //int framePartCount = (videoPacket.frameTotalSize / XPLANE_PACKET_CHUNKSIZE);

    QTcpSocket *clientConnection = _tcpVideoServer->nextPendingConnection();
    if (clientConnection != nullptr)
        _clientConnection = clientConnection;

    if (_clientConnection == nullptr)
        return;

    for (beginPos = 0; endPos < _compressedXPlaneDataSize - 1; beginPos += XPLANE_PACKET_CHUNKSIZE)
    {
        endPos = beginPos + XPLANE_PACKET_CHUNKSIZE - 1;
        if (endPos >= _compressedXPlaneDataSize)
            endPos = _compressedXPlaneDataSize - 1;
        int frameSize = endPos - beginPos + 1;
        memcpy(videoPacket.frameData, _compressedXPlaneData + beginPos, frameSize);

        qint64 len = sizeof(videoPacket);
        if (_clientConnection->isValid())
            _clientConnection->write((char*)&videoPacket, len);
        _clientConnection->flush();
        videoPacket.framePartNo++;
    }
}

void UAVSimDataSender::timerEvent(QTimerEvent *event)
{
    if (_telemetryMessages.isEmpty())
        return;

    if (event->timerId() == _telemetryTimerId)
    {
        updateCamPitchRoll();
        sendTelemetryMassage();
    }
    else if (event->timerId() == _videoTimerId)
    {
        sendXPLaneVideoMessage();
    }
}

UAVSimDataSender::UAVSimDataSender(QObject *parent,
                                   const QString &telemetryFile, int telemetryUDPPort,
                                   const QString &videoFile, int videoTcpPort, CameraControlModes controlMode) : QObject(parent)
{
    _telemetryFile = telemetryFile;
    _telemetryUDPPort = telemetryUDPPort;
    _videoFile = videoFile;
    _clientConnection = nullptr;

    _controlMode = controlMode;

    _camX = 0;
    _camY = 0;
    _camZ = 0;
    _camZoom = 0;
    _camYTarget = 0;
    _camXTarget = 0;
    _camZoomTarget = 0;

    _bombState = true;

    _isFreezedUavRoll = false;
    _isFreezedUavPitch = false;
    _isFreezedUavYaw = false;
    _isFreezedGPSLat = false;
    _isFreezedGPSLon = false;
    _isFreezedGPSHmsl = false;
    _isFreezedGPSCourse = false;
    _isFreezedWindDirection = false;
    _isFreezedWindSpeed = false;
    _isRangefinderDistance = false;

    _pause = false;

    _udpTelemetrySocket.setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 2000000);

    _tcpVideoServer = new QTcpServer(this);
    _tcpVideoServer->listen(QHostAddress::Any, videoTcpPort);

    loadFromCSV(telemetryFile);

    loadXPlaneVideo(videoFile);

    _telemetryTimerId = this->startTimer(100);
    _videoTimerId = this->startTimer(40);
}

void UAVSimDataSender::setPause(bool pause)
{
    _pause = pause;
}

double UAVSimDataSender::camXTarget()
{
    return _camX;
}

double UAVSimDataSender::camYTarget()
{
    return _camY;
}

void UAVSimDataSender::setCamXTarget(double value)
{
    _camXTarget = value;
}

void UAVSimDataSender::setCamYTarget(double value)
{
    _camYTarget = value;
}

void UAVSimDataSender::setCamZoomTarget(double value)
{
    _camZoomTarget = value;
}

void UAVSimDataSender::setBombState(bool state)
{
    _bombState = state;
}

void UAVSimDataSender::setFreezedUavRoll(bool freezed, double value)
{
    _isFreezedUavRoll = freezed;
    _freezedUavRoll = value;
}

void UAVSimDataSender::setFreezedUavPitch(bool freezed, double value)
{
    _isFreezedUavPitch = freezed;
    _freezedUavPitch = value;
}

void UAVSimDataSender::setFreezedUavYaw(bool freezed, double value)
{
    _isFreezedUavYaw = freezed;
    _freezedUavYaw = value;
}

void UAVSimDataSender::setFreezedGPSLat(bool freezed, double value)
{
    _isFreezedGPSLat = freezed;
    _freezedGPSLat = value;
}

void UAVSimDataSender::setFreezedGPSLon(bool freezed, double value)
{
    _isFreezedGPSLon = freezed;
    _freezedGPSLon = value;
}

void UAVSimDataSender::setFreezedGPSHmsl(bool freezed, double value)
{
    _isFreezedGPSHmsl = freezed;
    _freezedGPSHmsl = value;
}

void UAVSimDataSender::setFreezedGPSCourse(bool freezed, double value)
{
    _isFreezedGPSCourse = freezed;
    _freezedGPSCourse = value;
}

void UAVSimDataSender::setFreezedWindDirection(bool freezed, double value)
{
    _isFreezedWindDirection = freezed;
    _freezedWindDirection = value;
}

void UAVSimDataSender::setFreezedWindSpeed(bool freezed, double value)
{
    _isFreezedWindSpeed = freezed;
    _freezedWindSpeed = value;
}

void UAVSimDataSender::setFreezeRangefinderDistance(bool freezed, double value)
{
    _isRangefinderDistance = freezed;
    _freezedRangefinderDistance = value;
}
