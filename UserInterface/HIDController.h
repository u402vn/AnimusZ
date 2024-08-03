#ifndef HIDCONTROLLER_H
#define HIDCONTROLLER_H

#include <Joystick.h>
#include <QKeyEvent>
#include <QObject>
#include "ApplicationSettings.h"

class HIDController;

class HIDMapItem : public QObject
{
    friend class HIDController;
    Q_OBJECT

    int _joystickButtonIdx;
    bool _joystickButtonWasPressed;
    QKeySequence _keySequence;
    bool _processAutoRepeatKey;

    explicit HIDMapItem(HIDController *parent,
                        void(HIDController::*onPressMethod)(), void(HIDController::*onReleaseMethod)(),
                        ApplicationPreferenceInt *joystickButtonPref,
                        ApplicationPreferenceString *keyboardPref, bool processAutoRepeatKey);
public:
    void processJoystick(const QList<bool> &buttons);
    bool processKeyboard(QKeyEvent *keyEvent);
signals:
    void processPressEvent();
    void processReleaseEvent();
};

class HIDController : public QObject
{
    Q_OBJECT

    Joystick *_joystick;
    bool _keyboardUsing;
    CameraControlModes _controlMode;

    QList<HIDMapItem*> _HIDMap;
    QList<HIDMapItem*> _forcedKeyboardHIDMapItems;

    quint32 _camZoomMin, _camZoomMax, _camZoom;
    qreal _prevJoystickZoom;

    qreal _joystickCameraX, _joystickCameraY, _joystickCameraZoom, _joystickCursorX, _joystickCursorY;
    qreal _keyboardCameraX, _keyboardCameraY, _keyboardCursorX, _keyboardCursorY;
    bool _joystickCameraAxesInZeroPoint, _joystickCursorAxesInZeroPoint;
    qint32 _joystickAxisCameraXIndex, _joystickAxisCameraYIndex, _joystickAxisZoomIndex;
    qint32 _joystickAxisCursorXIndex, _joystickAxisCursorYIndex;
    qreal _joystickCameraAxisMultiplier, _joystickCameraEmulationFromKeyboard, _joystickCameraAxisSensitivity;
    qreal _joystickCursorAxisMultiplier, _joystickCursorEmulationFromKeyboard, _joystickCursorAxisSensitivity;

    QTimer *_joystickFreqTimer;
    quint32 _joystickEventNumber, _joystickEventsCount;

    HIDMapItem *makeHIDMapItem(HIDButton prefIndex, void(HIDController::*onPressMethod)(), void(HIDController::*onReleaseMethod)(),
                               bool processAutoRepeatKey, bool forceUseKeyboard = false);

    bool processKeyboard(QKeyEvent *keyEvent, QObject *senderObj);

    void processCamZoomUp();
    void processCamZoomDown();

    void processRollUpPress();
    void processRollUpRelease();
    void processRollDownPress();
    void processRollDownRelease();
    void processPitchUpPress();
    void processPitchUpRelease();
    void processPitchDownPress();
    void processPitchDownRelease();

    void updateCamZoomInternal(quint32 zoom);
    void processAxesChanges();\
    void processPOVChanges(const QList<int> &povs);
protected:
    bool virtual eventFilter(QObject *obj, QEvent *event);
public:
    explicit HIDController(QObject *parent);
    void setCamZoomRange(quint32 camZoomMin, quint32 camZoomMax);
private slots:
    void processJoystick(const QList<int> &povs, const QList<double> &axes, const QList<bool> &buttons);
public slots:
    void doSetZoomFromUI(quint32 zoom);
signals:
    void onMapZoomInClicked();
    void onMapZoomOutClicked();
    void onFollowThePlaneClicked();
    void onMapMoveClicked(int directionAngle);

    void onOpenApplicationSettingsEditorClicked();
    void onOpenDataConsoleClicked();
    void onOpenEmulatorConsoleClicked();
    void onOpenHelpViewerClicked();
    void onForceStartNewSessionClicked();
    void onForceDisplayOnlyClicked();
    void onChangeVideo2MapClicked();
    void onSelectSessionsClicked();

    void onChangeActiveCamClicked();
    void onEnableSoftwareStabClicked();

    void onCamDriversOffClicked();
    void onCamLandingPosClicked();
    void onCamBeginingPosClicked();
    void onCamVerticalPosClicked();

    void onColorModeUpClicked();
    void onColorModeDownClicked();

    void onLaserActivationClicked();

    void onScreenshotClicked();
    void onSnapshotClicked();
    void onSnapshotSeriesClicked();

    void onTargetUnlockClicked();
    void onTargetLockInCursorClick();
    void onCamRecordingClicked();
    void onEnableAutomaticTracerClicked();
    void onMagnifierClick();

    void onChangeBombingSightClicked();

    void onDropBombClicked();
    void onSendHitCoordinatesClicked();
    void onSendWeatherClicked();
    void onNewMarkerForTargetClicked();
    void onNewMarkerForLaserClicked();
    void onNewMarkerForUAVClicked();

    void onNormalFlightClicked();
    void onPatrolMovingTargetModeClicked();
    void onPatrolStaticTargetModeClicked();
    void onManualFlightModeClicked();

    void onRelativeCamPositionChange(float deltaRoll, float deltaPitch, float deltaYaw);
    void onAbsoluteCamZoomChange(quint32 zoom);
    void onCamMovingSpeedChange(float speedRoll, float speedPitch, float deltaYaw);
    void onTargetLockCursorSpeedChange(float speedX, float speedY);

    void onJoystickStateTextChanged(const QString stateText);
};

#endif // HIDCONTROLLER_H
