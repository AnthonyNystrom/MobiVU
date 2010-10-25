#include "FrameWidget.h"
#include <QPainter>
#include <QDebug>
#include "mainwindow.h"

CFrame::CFrame(QWidget *Parent)
    :QWidget(Parent)
{
    _video = NULL;
    _pix = new QPixmap(":/images/Sfondo.bmp");
    resize(_pix->width(), _pix->height());
    _bDrawVideo = false;    
}
CFrame::~CFrame(){
    delete _pix;
    delete _video;        
}

void CFrame::paintEvent(QPaintEvent *pe)
{ 
    QPainter p(this);
    if (!_bDrawVideo)
        p.drawPixmap(0,0,_pix->width(), _pix->height(), *_pix);
    else if (_video!=NULL)
    {
        QPoint pt(0,0);
        p.drawImage(pt,*_video);
    }
}

void CFrame::DrawVideo(bool bDraw){
    _bDrawVideo=bDraw;
}

void CFrame::RefreshVideo(unsigned char *p)
{
    if (_video==NULL)
        _video = new QImage(p, _iW, _iH, QImage::Format_RGB16);    
    DrawVideo(true);
    repaint(0,0,_iW,_iH);
}

void CFrame::SetSize(int iW, int iH){

    _iW = iW;
    _iH = iH;
    iAppUi->LogInfo("CFrame::SetSize %d x %d", _iW, _iH);
    if (_video){
        delete _video;
        _video=NULL;
    }
}
