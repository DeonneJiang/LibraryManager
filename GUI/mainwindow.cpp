#include <functional>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "homewindow.h"
#include "booklistwindow.h"
#include "bookswindow.h"
#include "readerswindow.h"
#include "recordswindow.h"

#include <QLabel>
#include <QMenu>
#include <QRect>
#include <QActionGroup>
#include <QPoint>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(TimeMachine &tm, Reader &reader, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), timemac(tm), reader(reader), searchType(0),
    timer(new QTimer(this)), runningTime(0), lastOperationTime(0)
{
    qDebug() << "Start loading Main Window...";

    ui->setupUi(this);

    // 初始化列表
    emit initWidget();

    // 设置搜索菜单
    emit initSearchMenu();

    // 设置用户菜单
    emit initUserMenu();

    // 显示用户信息
    ui->UserName->setText(QString(reader.ri.name.c_str()));

    // 隐藏时间
    ui->Time->hide();

    // 启动定时器
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerEvent()));

    // 加载成功
    qDebug() << "Finished.";
}

MainWindow::~MainWindow()
{
    qDebug() << "Destroying Main Window...";

    /* 停止定时器并复位时间 */
    timer->stop();
    timemac.SetElapsingRate(NORMAL);
    delete timer;

    delete ui;

    /* 销毁所有面板对象 */
    for(auto i = 0; i < panelCount; i++)
        if(wList[i] != noPermissionPanel)
            delete wList[i];
    delete wList;
    delete rbList;
    delete bVisible;
    delete noPermissionPanel;

    qDebug() << "Finished.";
}

/* 打开/关闭Banner */
void MainWindow::toggleBanner(const QWidget* object, bool toggle)
{
    // 定位到 QWidget
    auto id = 0;
    for(; wList[id]; id++)
        if(wList[id] == object) break;

    // 检查 id 是否有效
    if(!wList[id]) return;

    // 切换状态
    bVisible[id] = toggle;
    ui->BannerWidget->setVisible(toggle);
}

/* 初始化标签页 */
void MainWindow::initWidget(void)
{
    // 初始化面板分级
    static bool permission[][panelCount] = {
        true, true, true, false, false, false,
        true, true, true, true, true, true
    };

    // 初始化面板组
    static const int groupBound[] = {
        0, 1, 4, 5
    };
    QLabel* groupLabel[] = {
        nullptr, ui->LibraryLabel, ui->ManagerLabel, ui->StatisticsLabel
    };


    // 无权限提示文字
    noPermissionPanel = new QLabel(QString("没有合适的权限访问该面板。"));
    qobject_cast<QLabel*>(noPermissionPanel)->setAlignment(Qt::AlignHCenter);

    // 初始化 Widget 列表
    wList    = new QWidget*[panelCount];
    wList[0] = permission[reader.ri.readerPermission][0] ?
                (new HomeWindow(timemac, reader, this)) : noPermissionPanel;
    wList[1] = permission[reader.ri.readerPermission][1] ?
                new BookListWindow(timemac, reader, this) : noPermissionPanel;
    wList[2] = permission[reader.ri.readerPermission][2] ?
                new RecordsWindow(&reader.ri, reader.recman, this) : noPermissionPanel;
    wList[3] = permission[reader.ri.readerPermission][3] ?
                new ReadersWindow(&reader.ri, reader.readman, this) : noPermissionPanel;
    wList[4] = permission[reader.ri.readerPermission][4] ?
                new BooksWindow(reader.bookman, this) : noPermissionPanel;
    wList[5] = permission[reader.ri.readerPermission][5] ?
                new QLabel(QString("正在施工")) : noPermissionPanel;

    // 初始化对应 QRadioButton
    rbList = new QRadioButton*[6];
    rbList[0] = ui->HomeButton;
    rbList[1] = ui->BookListButton;
    rbList[2] = ui->RecordsButton;
    rbList[3] = ui->UsersButton;
    rbList[4] = ui->BooksButton;
    rbList[5] = ui->StatisticsButton;

    // 初始化各个面板的 Banner 打开状态
    bVisible = new bool[6];
    bVisible[0] = true;
    bVisible[1] = true;
    bVisible[2] = true;
    bVisible[3] = true;
    bVisible[4] = true;
    bVisible[5] = true;

    // 隐藏一些按钮
    for(auto i = 0; i < panelCount; i++)
        if(!permission[reader.ri.readerPermission][i])
            rbList[i]->hide();

    // 顺便把一些标签关掉
    auto usingPanel = false;
    for(auto i = 0, j = 0; i < panelCount && j < sizeof(groupLabel)/sizeof(groupLabel[0]); i++)
    {
        if(permission[reader.ri.readerPermission][i])
            usingPanel = true;

        if(groupBound[j] == i)
        {
            if(!usingPanel && groupLabel[j] != nullptr)
                groupLabel[j]->hide();
            usingPanel = false;
            j++;
        }
    }

    // 加载列表
    for(auto i = 0; i < panelCount; i++)
        if(wList[i] != nullptr)
            ui->stackedWidget->addWidget(wList[i]);

    // 切换到第一页
    changeWidget(0);
}

/* 切换标签页 */
void MainWindow::changeWidget(int i)
{
    if(wList[i] != nullptr)
    {
        // 切换面板
        ui->stackedWidget->setCurrentWidget(wList[i]);
        ui->TitleLabel->setText(wList[i]->windowTitle());
        ui->DescribeLabel->setText(wList[i]->whatsThis());

        // 检查这个面板的 Banner 状态
        ui->BannerWidget->setVisible(bVisible[i]);
    }
}

// slot function: The radiobutton of list changes
void MainWindow::onChangedWidget(bool state)
{
    // Process when the state is true
    if(!state)
        return;

    // Find the correct widget
    for(int i = 0; rbList[i]; i++)
        if(qobject_cast<QRadioButton*>(sender()) == rbList[i])
        {
            if(wList[i])
                changeWidget(i);
            break;
        }
}

/* 初始化搜索菜单 */
void MainWindow::initSearchMenu(void)
{
    QMenu* searchMenu = new QMenu();

    QActionGroup *searchSetGroup = new QActionGroup(this);

    QString typeString[] = {
        QString("书名"), QString("作者"), QString("出版社")
    };
    for(unsigned int i = 0; i < sizeof(typeString)/sizeof(typeString[0]); i++)
    {
        QAction* action = new QAction(typeString[i]);
        action->setCheckable(true);
        action->setChecked(i == 0);
        action->setData(i);
        searchSetGroup->addAction(action);
        searchMenu->addAction(action);
    }

    ui->TypeButton->setMenu(searchMenu);
    connect(searchMenu, SIGNAL(triggered(QAction*)), this, SLOT(onChangedSehType(QAction*)));
}

// slot function: 搜索菜单的响应函数
void MainWindow::onChangedSehType(QAction* action)
{
    searchType = action->data().toInt();
    ui->TypeLabel->setText(action->text());
}

/* 初始化用户菜单 */
void MainWindow::initUserMenu(void)
{
    QMenu* userMenu = new QMenu();

    // 添加时间控制
    QActionGroup *timeSetGroup = new QActionGroup(this);
    QAction *showNameAction = new QAction(QString("显示用户名"));
    QAction *showTimeAction = new QAction(QString("显示时间"));
    showNameAction->setCheckable(true);
    showNameAction->setData(0);
    showNameAction->setChecked(true);
    showTimeAction->setCheckable(true);
    showTimeAction->setData(1);
    timeSetGroup->addAction(showNameAction);
    timeSetGroup->addAction(showTimeAction);

    // 添加注销按钮
    QAction *logOut = new QAction(QString("注销"));
    logOut->setData(2);

    // 添加菜单
    userMenu->addAction(showNameAction);
    userMenu->addAction(showTimeAction);
    userMenu->addSeparator();
    userMenu->addAction(logOut);

    ui->UserButton->setMenu(userMenu);
    connect(userMenu, SIGNAL(triggered(QAction*)), this, SLOT(onSelectedUserMode(QAction*)));
}

// slot function: 用户菜单的响应函数
void MainWindow::onSelectedUserMode(QAction * action)
{
    std::function<void(void)> menuAction[] = {
        [this](void) { ui->UserName->show(); ui->Time->hide(); },
        [this](void) { ui->UserName->hide(); ui->Time->show(); },
        [this](void) { this->close(); }
    };

    (menuAction[action->data().toInt()])();
}

// slot function: 刷新时间
void MainWindow::refreshTime(void)
{
    ui->Time->setText(QString(timemac.ConvertDate(timemac.Now())));
}

// slot function: 定时器触发函数
void MainWindow::onTimerEvent(void)
{
    // 增加运行时间
    runningTime ++;

    emit checkMouseMove();

    if(runningTime - lastOperationTime > waitForOperator) // 一定时间内无操作
    {
        timemac.SetElapsingRate(FAST);
        ui->Time->setStyleSheet(QString("color: red"));
    }
    else
    {
        timemac.SetElapsingRate(NORMAL);
        ui->Time->setStyleSheet(QString("color: black"));
    }
}

// slot function: 强制注销
void MainWindow::forceToLogout(QString &reason)
{
    // 显示注销信息
    QMessageBox::warning(this, QString("强制注销"),
                         QString("管理员要求您重新登录，原因是：\n\n%1").arg(reason),
                         QMessageBox::Ok, QMessageBox::Ok);
    // 注销
    this->close();
}

// Keyboard Event: 按下键盘事件
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    lastOperationTime = runningTime;
}

// 鼠标移动事件
void MainWindow::checkMouseMove(void)
{
    static QPoint pt;

    if(this->isActiveWindow())
        if(pt != QCursor::pos())
        {
            lastOperationTime = runningTime;
            pt = QCursor::pos();
        }
}

// 搜索按钮按下
void MainWindow::onSearchButtonClicked(void)
{
    // 初始化类型字符串
    const QString typeString[] = {"Title", "Author", "Press"};

    // 切换面板
    this->changeWidget(1);
    ui->BookListButton->setChecked(true);

    // 调用搜索接口
    emit qobject_cast<BookListWindow*>(wList[1])->searchBooks(typeString[searchType], ui->SearchBox->text());
}

// 搜索文本改变
void MainWindow::onSearchTextChanged(QString string)
{
    if(ui->stackedWidget->currentWidget() == wList[1]) // 当前为图书窗口
        emit onSearchButtonClicked();
}
