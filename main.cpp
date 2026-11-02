#include <QtWidgets/QApplication>
#include <QDebug>
#include <QIcon>

#include "mainwindow.h"
#include "src/database/DatabaseManager.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    QIcon appIcon;
    for (int size : {16, 32, 48, 64, 128, 256})
        appIcon.addFile(QString(":/icons/app_icon_%1.png").arg(size), QSize(size, size));
    app.setWindowIcon(appIcon);

    if (!DatabaseManager::instance().initialize()) {
        qDebug() << "error occured while initializing the database";
        return 0;
    }

    qDebug() << "Database Initilized successfully";

    MainWindow window;
    window.show();
    return app.exec();
}
