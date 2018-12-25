#ifndef BOOKLISTWINDOW_H
#define BOOKLISTWINDOW_H

#include "Core/common.h"
#include "Core/bookman.h"
#include "Core/readerman.h"
#include "Core/borrowingrecordman.h"
#include "Core/timemac.h"
#include "Core/reader.h"
#include <QWidget>

#include "mainwindow.h"

namespace Ui {
class BookListWindow;
}

class BookListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BookListWindow(TimeMachine &timemac, Reader &reader, MainWindow *parent);
    ~BookListWindow();

public slots:
    void searchBooks(QString type, QString keywords);

private slots:
    void onRefreshDetails(void);
    void onCommandButtonClicked(void);
    void onChangedMode(bool state);
    void onSelectedBook(int row, int col);
    void onClickedCloseDetailsBox(void);
    void showAllBooks(void);
    void toggleDetailsBox(bool state);

private:
    void showDetails(BookInfo* bi);

private:
    /* 应用程序窗口 */
    Ui::BookListWindow *ui;
    MainWindow*         parent;
    /* 数据库指针 */
    Reader &reader;
    TimeMachine &timemac;
    /* 状态变量 */
    bool isLoaded;
    /* 图书信息 */
    BookInfo bookinfo;
};

#endif // BOOKLISTWINDOW_H
