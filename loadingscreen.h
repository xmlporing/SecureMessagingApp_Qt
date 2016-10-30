#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

/*
 * Reference from:
 * http://www.codeprogress.com/cpp/libraries/qt/showQtExample.php?index=607&key=QSplashScreenAnimation
 */

#include <QtWidgets>

class LoadingScreen:public QSplashScreen
{
  Q_OBJECT
  public:
    LoadingScreen():movie(":/img/loader2.gif")
    {
        this->installEventFilter(this);
        connect(&movie,SIGNAL(updated(QRect)),this,SLOT(frameUpdate()));
    }

    ~LoadingScreen()
    {}

    void startLoading()
    {
        movie.start();
    }

    void stopLoading()
    {
        movie.stop();
    }

    bool eventFilter(QObject * watched, QEvent * event){
        switch(event->type()){
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonRelease:
            return true;
        default:
            return QObject::eventFilter(watched, event);
        }
    }

  private slots:
    void frameUpdate()
    {
      setPixmap(movie.currentPixmap().scaled(400,300,Qt::KeepAspectRatio));
    }

  private:
    QMovie movie;

};

#endif // LOADINGSCREEN_H
