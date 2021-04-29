#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    QPixmap splashMap("resources/icons/splash.png");
    QSplashScreen splash(splashMap);
    splash.show();
    app.processEvents();
    QCoreApplication::setApplicationName("CAExplorer");
    QCoreApplication::setOrganizationName("SSHF");
    app.setWindowIcon(QIcon("resources/icons/logo.png"));

    MainWindow win;
    QString title = "CAExplorer v";
    win.setWindowTitle(title);
    win.show();

    splash.finish(&win);
    return app.exec();
}
/*
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}*/
