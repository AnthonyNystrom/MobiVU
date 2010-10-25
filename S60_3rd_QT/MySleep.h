#ifndef MYSLEEP_H
#define MYSLEEP_H
#include <QThread>

class MySleep : public QThread
{
    public:
        static void sleep(unsigned long secs)
        {
            QThread::sleep(secs);
        }
        static void msleep(unsigned long msecs)
        {
            QThread::msleep(msecs);
        }
        static void usleep(unsigned long usecs)
        {
            QThread::usleep(usecs);
        }
};


#endif // MYSLEEP_H
