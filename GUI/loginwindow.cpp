#include "loginwindow.h"
#include "ui_loginwindow.h"

using std::string;

LoginWindow::LoginWindow(TimeMachine &timemac, ReaderManager &readerman, ReaderInfo &ri, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginWindow), readerman(readerman), timemac(timemac), readerinfo(ri)
{
    qDebug() << "Start loading Login Window...";
    ui->setupUi(this);
    qDebug() << "Finished.";
}

LoginWindow::~LoginWindow()
{
    qDebug() << "Destroying Login Window...";
    delete ui;
    qDebug() << "Finished.";
}

void LoginWindow::onLoginButtonClicked(void)
{
    // 准备登录数据
    string username(ui->username->text().toStdString());
    string password(ui->password->text().toStdString());

    if(readerman.SearchReadersByuserid(username, &readerinfo) == SUCCESS) // 获取用户信息
    {
        if(readerinfo.password == password)
        {
            // 登录成功
            this->accept();
        }
        else
        {
            // 登录失败：由于密码错误
            this->showErrorInfo("用户名或密码错误");
        }
    }
    else
    {
        // 登录失败：由于用户错误
        this->showErrorInfo("用户名或密码错误。");
    }
}

void LoginWindow::showErrorInfo(const char* reason)
{
    ui->errorInfo->setText(QString(reason));
}

void LoginWindow::refreshTime(void)
{
    ui->Time->setText(QString(timemac.ConvertDate(timemac.Now())));
}
