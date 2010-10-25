#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "FrameWidget.h"
#include <QTimer>
//#include "camera.h"
#include "mycamera.h"
#include "threadudp.h"
#include "ffdecode.h"

namespace Ui {
    class MainWindow;
}
class QTimer;
class MyCamera;
class ThreadUDP;
class CFFDecode;
class VideoWidget;

QT_BEGIN_NAMESPACE
class QAbstractVideoSurface;
QT_END_NAMESPACE


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();    
    void LogInfo(char *pBuf, ...);
    void DisplayFrame(CFbsBitmap* aFrame);
    void SendAudioFrame(TDesC8& aBuffer);
    void SendVideoFrame(unsigned char *p, int iSize);
    void DecodeAndShow(unsigned char *p, int iSize);

    MyCamera *_myCamera;

protected:
    void changeEvent(QEvent *e);
private slots:   
    void on_actionM2M_triggered();
    void on_actionEcho_triggered();
    void on_actionRear_triggered();
    void on_actionFront_triggered();
    void on_actionHigh_triggered();
    void on_actionMid_triggered();
    void on_actionLow_triggered();
    void on_actionHalf_triggered();
    void on_actionFull_triggered();
    void helpAction();
    void aboutAction();
    void DebugDecoder();
    void DebugEncoder();
    void OnRefresh(unsigned char *p);    

private:
    Ui::MainWindow *ui;
    bool _bClearLog;
    CFrame *_FrameImage;

    //Actions foe menu item
    QAction* menu_helpAction;
    QAction* menu_aboutAction;
    QAction* menu_versionAction;
    QAction* menu_exitAction;
    void createMyMenu();
    QTimer *_timer;
    ThreadUDP *_udp;
    CFFDecode *_decoder;    
    int _iCntLog;
    bool _bPause;
    int _iCodec;

    void StartCamera(bool bEnable);
};
extern MainWindow *iAppUi;

#endif // MAINWINDOW_H
