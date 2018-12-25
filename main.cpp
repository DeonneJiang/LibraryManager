#include "Core/common.h"
#include "Core/timemac.h"
#include "Core/reader.h"
#include "GUI/loginwindow.h"
#include "GUI/mainwindow.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    /* 建立访问数据库 */
    BookManager bookman;
    ReaderManager readerman;
    BorrowingRecordManager recordman;

    /* 建立时间机器 */
    TimeMachine tm;

    /* 建立时间刷新器 */
    QTimer timeDriver(nullptr);
    timeDriver.start(1000);

    /* 应用程序循环 */
    while(true)
    {
        // 建立登录窗口
        ReaderInfo ri;
        LoginWindow lw(tm, readerman, ri);
        lw.setWindowState(Qt::WindowFullScreen);

        // 绑定时间刷新
        QObject::connect(&timeDriver, SIGNAL(timeout()), &lw, SLOT(refreshTime()));

        if(lw.exec() == QDialog::Accepted)
        {
            // 准备资源
            qDebug() << "Initializing Reader Information...";
            Reader reader(bookman, recordman, readerman, ri);
            qDebug() << "Finished.";

            // 创建主窗口
            MainWindow mw(tm, reader);
            mw.showFullScreen();

            // 绑定时间刷新
            QObject::connect(&timeDriver, SIGNAL(timeout()), &mw, SLOT(refreshTime()));

            // 进入主窗口消息循环
            app.exec();
        }
        else
            break; // 退出循环
    }

    /* 停止时间刷新器 */
    timeDriver.stop();

    return 0;
}
