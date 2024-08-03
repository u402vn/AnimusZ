#ifndef CAMPREFERENCES_H
#define CAMPREFERENCES_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QMatrix4x4>

const qint32 MAXIMAL_CAMERA_ZOOM = 100;

class OpticalDevicePreferences final : public QObject
{
    Q_OBJECT
    friend class CamAssemblyPreferences;

    quint32 _zoomMin, _zoomMax;
    qint32 _frameWidth, _frameHeight;
    quint32 _magnifierSize;
    qreal _magnifierScale;
    QList<double> _fovHorizontalAngles;
    QList<double> _fovVerticalAngles;
    QList<double> _automaticTracerSpeedMultipliers;
    QList<double> _manualSpeedMultipliers;
    bool _forceSetResolution;
    bool _useVerticalFrameMirrororing;
    quint32 _videoConnectionId;

    quint32 zoomIndex(quint32 zoom);
public:
    explicit OpticalDevicePreferences(QObject *parent);
    ~OpticalDevicePreferences();

    void init(qint32 farmeWidth, qint32 frameHeight,
              quint32 zoomMinValue, quint32 zoomMaxValue,
              quint32 magnifierSize, qreal magnifierScale,
              const QList<double> &fovHorizontalAngles, const QList<double> &fovVerticalAngles,
              const QList<double> &automaticTracerSpeedMultipliers, const QList<double> &manualSpeedMultipliers,
              bool forceSetResolution, bool useVerticalFrameMirrororing, quint32 videoConnectionId);

    double fovHorizontalAngle(qint32 zoom);
    double fovVerticalAngle(qint32 zoom);
    qint32 frameWidth();
    qint32 frameHeight();
    double automaticTracerSpeedMultiplier(qint32 zoom);
    double manualSpeedMultipliers(qint32 zoom);
    const QMatrix4x4 projection(qint32 zoom);

    quint32 zoomMin();
    quint32 zoomMax();
    bool forceSetResolution();
    bool useVerticalFrameMirrororing();

    quint32 videoConnectionId();

    quint32 magnifierSize();
    qreal magnifierScale();
    void incMagnifierScale(qreal delta);

    void getScreenPointAnglesRad(const quint32 zoom, const qint32 screenX, const qint32 screenY, double &angleXRad, double &angleYRad);
    void getScreenPointAnglesDegree(const quint32 zoom, const qint32 screenX, const qint32 screenY, double &angleXDegree, double &angleYDegree);
};

class CamAssemblyPreferences final : public QObject
{
    Q_OBJECT

    QMap<qint32, OpticalDevicePreferences*> _opticalDevices;
    double _encoderAutomaticTracerMultiplier = 0;
public:
    explicit CamAssemblyPreferences(QObject *parent);
    ~CamAssemblyPreferences();

    void initGimbal(double encoderAutomaticTracerMultiplier);

    void initOpticalDevice(qint32 opticalSystemId,
                 qint32 farmeWidth, qint32 frameHeight,
                 quint32 zoomMinValue, quint32 zoomMaxValue, quint32 magnifierSize, qreal magnifierScale,
                 const QList<double> &fovHorizontalAngles,
                 const QList<double> &fovVerticalAngles,
                 const QList<double> &automaticTracerSpeedMultipliers,
                 const QList<double> &manualSpeedMultipliers,
                 bool forceSetResolution, bool useVerticalFrameMirrororing, quint32 videoConnectionId);
    OpticalDevicePreferences *opticalDevice(qint32 opticalDeviceId);

    double encoderAutomaticTracerMultiplier();
};

#endif // CAMPREFERENCES_H
