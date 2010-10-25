#ifndef FRAMEWIDGET_H
#define FRAMEWIDGET_H

#include <QWidget>
#include <QPixmap>

class CFrame : public QWidget
{
    QPixmap *_pix;
    QImage *_video;
    bool _bDrawVideo;
    int _iW, _iH;

protected:
    virtual void paintEvent(QPaintEvent *pe);
public:
    CFrame(QWidget *Parent);
    ~CFrame();
    void DrawVideo(bool bDraw);
    void RefreshVideo(unsigned char *p);
    void SetSize(int iW, int iH);
  };


#endif // FRAMEWIDGET_H
