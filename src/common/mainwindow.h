/*
 * (c) 2017, Deepin Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
#ifndef _DMR_MAIN_WINDOW_H
#define _DMR_MAIN_WINDOW_H

#include <QObject>
#include <DMainWindow>
#include <DTitlebar>
#include <DPlatformWindowHandle>
//#include <QtWidgets>
#include <DFrame>
#include <QPainterPath>
#include <DPushButton>
#include <DFloatingMessage>
#include <QDBusAbstractInterface>
#include <QtX11Extras/QX11Info>

#include "widgets/titlebar.h"
#include "animationlabel.h"
#include "volumemonitoring.h"
#include "diskcheckthread.h"
#include "actions.h"
#include "online_sub.h"

class Presenter;

namespace Dtk {
namespace Widget {
class DImageButton;
class DSettingsDialog;
}
}

DWIDGET_USE_NAMESPACE

class MainWindowEventListener;

namespace dmr {
enum CornerEdge {
    TopLeftCorner = 0,
    TopEdge = 1,
    TopRightCorner = 2,
    RightEdge = 3,
    BottomRightCorner = 4,
    BottomEdge = 5,
    BottomLeftCorner = 6,
    LeftEdge = 7,
    NoneEdge = -1
};

class ToolboxProxy;
class EventMonitor;
class PlaylistWidget;
class PlayerEngine;
class NotificationWidget;
class MovieProgressIndicator;
class MainWindowPropertyMonitor;

class IconButton: public DPushButton
{
public:
    explicit IconButton(QWidget *parent = 0): DPushButton(parent), m_themeType(0) {}

    void setIcon(QIcon icon)
    {
        m_icon = icon;
        DPushButton::setIcon(m_icon);
    };

    void changeTheme(int themeType = 0)
    {
        m_themeType = themeType;
        update();
    }
protected:
    void paintEvent(QPaintEvent *event)
    {
        QPainter painter(this);
        QRect backgroundRect = rect();
        //QPainterPath bp1;
        //bp1.addRoundedRect(backgroundRect, 2, 2);
        painter.setPen(Qt::NoPen);
        if (m_themeType == 1) {
            painter.setBrush(QBrush(QColor(247, 247, 247, 220)));
        } else if (m_themeType == 2) {
            painter.setBrush(QBrush(QColor(42, 42, 42, 220)));
        } else {
            painter.setBrush(QBrush(QColor(247, 247, 247, 220)));
        }
        QPainterPath painterPath;
        painterPath.addRoundedRect(backgroundRect, 15, 15);
        painter.drawPath(painterPath);

        DPushButton::paintEvent(event);
    };
private:
    QIcon m_icon;
    int m_themeType;
};

class MainWindow: public DMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool inited READ inited WRITE setInit NOTIFY initChanged)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool inited() const
    {
        return _inited;
    }
    PlayerEngine *engine()
    {
        return _engine;
    }
    ToolboxProxy *toolbox()
    {
        return _toolbox;
    }
    PlaylistWidget *playlist()
    {
        return _playlist;
    }
    ///用于测试触屏效果
    void setTouched(bool touched)
    {
        _isTouch = touched;
    }
    /**
     * @brief firstPlayInit 第一次点击播放时，需要加载动态库函数指针然后进行构造未完成的初始化
     */
    //void firstPlayInit();
    ///判断鼠标是否在窗口内
    bool judgeMouseInWindow(QPoint pos);

    void requestAction(ActionFactory::ActionKind, bool fromUI = false,
                       QList<QVariant> args = {}, bool shortcut = false);

    bool insideResizeArea(const QPoint &global_p);
    QMargins dragMargins() const;

    void capturedMousePressEvent(QMouseEvent *me);
    void capturedMouseReleaseEvent(QMouseEvent *me);

    void syncStaysOnTop();
    void updateGeometryNotification(const QSize &sz);

    void updateContentGeometry(const QRect &rect);

    static QString lastOpenedPath();

    void reflectActionToUI(ActionFactory::ActionKind);

    bool set_playlistopen_clicktogglepause(bool playlistopen);
    NotificationWidget *get_nwComm();

    //在读取光盘的时候，直接把光盘挂载点的路径加入到播放列表中 thx
    bool addCdromPath();
    void loadPlayList();
    void setOpenFiles(QStringList &);

    void testMprisapp();
    void testCdrom();
    void setShowSetting(bool);
    void updateGeometry(CornerEdge edge, QPoint p);
    void setPresenter(Presenter *);
    int getDisplayVolume();
signals:
    void windowEntered();
    void windowLeaved();
    void initChanged();
    void frameMenuEnable(bool);
    void playSpeedMenuEnable(bool);
    void playlistchanged();

public slots:
    void play(const QUrl &url);
    void playList(const QList<QString> &l);
    void updateProxyGeometry();
    void suspendToolsWindow();
    void resumeToolsWindow();
    void checkOnlineState(const bool isOnline);
    void checkOnlineSubtitle(const OnlineSubtitle::FailReason reason);
    void checkErrorMpvLogsChanged(const QString prefix, const QString text);
    void checkWarningMpvLogsChanged(const QString prefix, const QString text);
    void slotdefaultplaymodechanged(const QString &key, const QVariant &value);
#if defined (__aarch64__) || defined (__mips__)
    void syncPostion();
#endif
    //设置窗口顶层
    void my_setStayOnTop(const QWidget *widget, bool on);
    void slotmousePressTimerTimeOut();
    void slotPlayerStateChanged();
    void slotFocusWindowChanged();
//    void slotElapsedChanged();
    void slotFileLoaded();
    void slotUrlpause(bool status);
    void slotFontChanged(const QFont &font);
    void slotMuteChanged(bool mute);
    void slotAwaacelModeChanged(const QString &key, const QVariant &value);     //改变硬解码模式
protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void closeEvent(QCloseEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void focusInEvent(QFocusEvent *fe) override;
    void wheelEvent(QWheelEvent *we) override;

    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void moveEvent(QMoveEvent *ev) override;
    void contextMenuEvent(QContextMenuEvent *cme) override;
    void paintEvent(QPaintEvent *) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    bool event(QEvent *event) override;
    void leaveEvent(QEvent *) override;

protected slots:
    void setInit(bool v);
    void menuItemInvoked(QAction *action);
#ifdef USE_DXCB
    void onApplicationStateChanged(Qt::ApplicationState e);
#endif
    void onBindingsChanged();
    void updateActionsState();
    void syncPlayState();
    void animatePlayState();
    void resizeByConstraints(bool forceCentered = false);
    void onWindowStateChanged();

    void miniButtonClicked(QString id);

    void startBurstShooting();
    void onBurstScreenshot(const QImage &frame, qint64 timestamp);
    void delayedMouseReleaseHandler();
//    void onDvdData(const QString &title);

#ifdef USE_DXCB
    void onMonitorButtonPressed(int x, int y);
    void onMonitorMotionNotify(int x, int y);
    _miniPlayBtn
    void onMonitorButtonReleased(int x, int y);

    void updateShadow();
#endif

//    void handleHelpAction();
//    void changedVolume(int);
    void changedVolumeSlot(int vol);
    void changedMute();
    void changedMute(bool);

    void updateMiniBtnTheme(int);
    void diskRemoved(QString strDiskName);

    void sleepStateChanged(bool bSleep);
private:
    void setupTitlebar();

    void handleSettings(DSettingsDialog *);
    DSettingsDialog *initSettings();
    void updateSizeConstraints();
    void toggleUIMode();

    bool insideToolsArea(const QPoint &p);
    void switchTheme();
    bool isActionAllowed(ActionFactory::ActionKind kd, bool fromUI, bool isShortcut);
    QString probeCdromDevice();
    void updateWindowTitle();
    void toggleShapeMask();
    void prepareSplashImages();
    void saveWindowState();
    void loadWindowState();
    void subtitleMatchVideo(const QString &fileName);
    void defaultplaymodeinit();
    void readSinkInputPath();
    void setAudioVolume(int);
    void setMusicMuted(bool muted);
    void popupAdapter(QIcon, QString);
    void setHwaccelMode(const QVariant &value = -1);

    //Limit video to mini mode size
    void LimitWindowize();
    void mipsShowFullScreen();
    //hide pop windows when dragging window
    void hidePopWindow();
    void adjustPlaybackSpeed(ActionFactory::ActionKind);
    void setPlaySpeedMenuChecked(ActionFactory::ActionKind);
    void setPlaySpeedMenuUnchecked();

private:
    DFloatingMessage *popup {nullptr};
    QLabel *_fullscreentimelable {nullptr};             //全屏时右上角的影片进度
    QHBoxLayout *_fullscreentimebox {nullptr};

    Titlebar *_titlebar {nullptr};
    ToolboxProxy *_toolbox {nullptr};
    PlaylistWidget *_playlist {nullptr};
    PlayerEngine *_engine {nullptr};
    AnimationLabel *_animationlable {nullptr};
    MovieProgressIndicator *_progIndicator {nullptr};   //全屏时右上角的系统时间

    QList<QPair<QImage, qint64>> _burstShoots;
    bool _inBurstShootMode {false};
    bool _pausedBeforeBurst {false};

#ifdef __mips__
    QAbstractButton *_miniPlayBtn {nullptr};
    QAbstractButton *_miniCloseBtn {nullptr};
    QAbstractButton *_miniQuitMiniBtn {nullptr};
#else
    DIconButton *_miniPlayBtn {nullptr};
    DIconButton *_miniCloseBtn {nullptr};
    DIconButton *_miniQuitMiniBtn {nullptr};
#endif

    QImage bg_dark;
    QImage bg_light;

    bool _miniMode {false};
    /// used to restore to recent geometry when quit fullscreen or minVolumeMonitoringi mode
    QRect _lastRectInNormalMode;

    // the first time a play happens, we consider it inited.
    bool _inited {false};

    DPlatformWindowHandle *_handle {nullptr};
    EventMonitor *_evm {nullptr};

    bool _pausedOnHide {false};
    // track if next/prev is triggered in fs/maximized mode
    bool _movieSwitchedInFsOrMaxed {false};
    bool _delayedResizeByConstraint {false};

    //toggle-able states
    bool _lightTheme {false};
    bool _windowAbove {false};
    bool _mouseMoved {false};
    bool _mousePressed {false};
    bool _isSettingMiniMode{false};
    bool _playlistopen_clicktogglepause {false};
    double _playSpeed {1.0};

    bool _quitfullscreenstopflag {false};
    bool _quitfullscreenflag{false};
    bool _maxfornormalflag {false};  //is the window maximized
    //add by heyi
    bool m_bMpvFunsLoad {false};
    QPoint posMouseOrigin;
    QPoint m_pressPoint;

    enum StateBeforeEnterMiniMode {
        SBEM_None = 0x0,
        SBEM_Above = 0x01,
        SBEM_Fullscreen = 0x02,
        SBEM_PlaylistOpened = 0x04,
        SBEM_Maximized = 0x08,
    };
    int _stateBeforeMiniMode {0};
    Qt::WindowStates _lastWindowState {Qt::WindowNoState};

    uint32_t _lastCookie {0};
    uint32_t _powerCookie {0};

    MainWindowEventListener *_listener {nullptr};
    NotificationWidget *_nwDvd {nullptr};
    NotificationWidget *_nwComm {nullptr};
    QTimer _autoHideTimer;
    QTimer _delayedMouseReleaseTimer;
    QUrl m_dvdUrl {QUrl()};
    QProcess *shortcutViewProcess {nullptr};

    VolumeMonitoring volumeMonitoring;
    QString sinkInputPath;
    int m_displayVolume;
    int m_oldDisplayVolume;
    bool m_isManual;

    bool m_IsFree = true;  //播放器是否空闲，和IDel的定义不同

    static int _retryTimes;
    bool _isJinJia = false;//是否是景嘉微显卡
    QTimer _progressTimer;
    //add by heyi 解决触屏右键菜单bug
    int nX = 0, nY = 0;     //左键按下时保存的点
    bool _isTouch = false;          //是否是触摸屏按下
    QTimer _mousePressTimer;
    qint64 oldDuration = 0;
    qint64 oldElapsed = 0;

    Diskcheckthread m_diskCheckThread;
    bool m_bClosed {false};      //用于景嘉微显卡下过滤metacall事件
    bool _isFileLoadNotFinished{false};
    QStringList m_openFiles;
    QString m_currentHwdec;
    bool m_bProgressChanged {false};        //进度条是否被拖动
    bool m_bFirstInit {false};
    bool m_bLastIsTouch {false};
    bool m_bTouchChangeVolume {false};
    bool m_bIsFullSreen {false};
    bool m_bisOverhunderd {false};
    bool m_bisShowSettingDialog {true};
    QDBusInterface *m_pDBus {nullptr};
    MainWindowPropertyMonitor *m_pMWPM {nullptr};
    bool m_isFirstLoadDBus {false};

    Presenter *m_presenter {nullptr};
};

//窗管返回事件过滤器
class MainWindowPropertyMonitor: public QAbstractNativeEventFilter
{
public:
    explicit MainWindowPropertyMonitor(MainWindow *);
    ~MainWindowPropertyMonitor();
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *);
    MainWindow *_mw {nullptr};
    xcb_atom_t _atomWMState;
    QList<unsigned int> m_list;
    bool m_start {false};
};
};


#endif /* ifndef _MAIN_WINDOW_H */


