#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>

#include "Core/common.h"
#include "Core/timemac.h"
#include "Core/readerman.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(TimeMachine &timemac, ReaderManager &readerman, ReaderInfo &ri, QWidget *parent = 0);
    ~LoginWindow();

public slots:
    void refreshTime(void);

private slots:
    void onLoginButtonClicked(void);

private:
    void showErrorInfo(const char *reason);

private:
    Ui::LoginWindow *ui;
    ReaderManager   &readerman;
    TimeMachine     &timemac;    // 时间机器指针
    ReaderInfo      &readerinfo; // 读者信息结构体指针
};

#endif // LOGINWINDOW_H
