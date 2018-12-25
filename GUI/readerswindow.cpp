#include "readerswindow.h"
#include "ui_readerswindow.h"
#include <QMenu>
#include <QMessageBox>

ReadersWindow::ReadersWindow(const ReaderInfo *currentReader, ReaderManager &rm, MainWindow *parent) :
    ui(new Ui::ReadersWindow), QWidget(parent), readerman(rm),
    currentReader(currentReader), parent(parent), isEditMode(false), searchType(0)
{
    qDebug() << "Start loading Readers Window...";

    ui->setupUi(this);

    // 初始化搜索菜单
    emit initSearchTypeMenu();

    // 初始化列表表头
    emit initReaderListHeaders();

    // 列出所有读者信息
    emit listAllReaders();

    // 初始化借阅权限列表框
    QStringList qsl;
    qsl.append("学生权限");
    qsl.append("教师权限");
    ui->Permission->addItems(qsl);

    // 默认选择第一项
    ui->ReadersList->selectRow(0);
    emit onReaderSelected(0, 0);

    // 加载成功
    qDebug() << "Finished.";
}

ReadersWindow::~ReadersWindow()
{
    qDebug() << "Destroying Readers Window...";
    delete ui;
    qDebug() << "Finished.";
}

/* 初始化搜索菜单 */
void ReadersWindow::initSearchTypeMenu(void)
{
    QMenu* searchMenu = new QMenu();

    QActionGroup *searchSetGroup = new QActionGroup(this);

    QString typeString[] = {
        QString("编号"), QString("姓名"), QString("权限"), QString("学院"),
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

    ui->SearchType->setMenu(searchMenu);
    ui->SearchType->setText(typeString[0]);

    //
    connect(searchMenu, SIGNAL(triggered(QAction*)), this, SLOT(onSearchTypeChanged(QAction*)));
}

/* 初始化列表表头 */
void ReadersWindow::initReaderListHeaders(void)
{
    QStringList horizontalHeader;
    horizontalHeader.append(tr("读者编号"));
    horizontalHeader.append(tr("姓名"));
    horizontalHeader.append(tr("学院"));
    horizontalHeader.append(tr("权限"));
    ui->ReadersList->setColumnCount(4);
    ui->ReadersList->setHorizontalHeaderLabels(horizontalHeader);
    ui->ReadersList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/* 设置列表项 */
void ReadersWindow::setItemContents(int currentRow, ReaderInfo *ri)
{
    //准备容器与内容
    std::string * p_permission = NULL;
    if( ri->readerPermission == student_permission )
    {
        p_permission = new std::string("学生权限");
    }
    else
    {
        p_permission = new std::string("教师权限");
    }

    std::string *contents[] = {
        new std::string(ri->userid), new std::string(ri->name),
        new std::string(ri->college), p_permission,
    };


    // 逐项添加内容
    for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString(contents[i]->c_str()));

        // 染色
        if(currentRow % 2)
            item->setBackgroundColor(QColor(235, 235, 235));
        else
            item->setBackgroundColor(QColor(255, 255, 255));

        ui->ReadersList->setItem(currentRow, i, item);
    }

    //释放内存
    for( int i = 0 ; i<3 ; i++) delete contents[i];

    // 选择已添加的行
    ui->ReadersList->selectRow(currentRow);
}

/* 显示所有读者信息 */
void ReadersWindow::listAllReaders(void)
{
    ui->ReadersList->setRowCount(readerman.Size());

    readerman.GetAllReadersInfo( [this](int index, ReaderInfo* rei)->bool
    {
        setItemContents(index-1, rei); // 当前行数

        return true; // 返回true以继续加载

     });
}

/* 更改搜索类型 */
void ReadersWindow::onSearchTypeChanged(QAction* action)
{
    searchType = action->data().toInt();
    ui->SearchType->setText(action->text());
}

/* 按下添加新读者按钮 */
void ReadersWindow::onAddClicked(void)
{
    // 设置编辑模式
    isEditMode = false;

    // 允许编辑 userid
    ui->UserID->setEnabled(true);

    // 获取当前选择的读者号
    std::string userid;
    auto targetRow = ui->ReadersList->currentRow();
    if(targetRow != -1)
    {
        userid = ui->ReadersList->item(targetRow, 0)->text().toStdString();
        userid = std::to_string(std::stoi(userid)+1);
    }
    else
        userid = std::to_string(readerman.Size());

    // 初始化表单
    ui->UserID->setText(QString(userid.c_str()));
    ui->UserName->setText(QString(userid.c_str()));
    ui->Password->clear();
    // 保留学院与权限；不设置
    // ui->College
    // ui->Permission

    // 显示详情页
    ui->ReaderDetailsWidget->show();
}

/* 按下保存读者信息按钮 */
void ReadersWindow::onSaveClicked(void)
{
    // 获取所填信息
    ReaderInfo ri;

    ri.college          = ui->College->text().toStdString();
    ri.name             = ui->UserName->text().toStdString();
    ri.password         = ui->Password->text().toStdString();
    ri.readerPermission = ui->Permission->currentIndex() == 1 ? teacher_permission : student_permission;
    ri.userid           = ui->UserID->text().toStdString();

    // 检查是否使用了相同的 UserID
    std::string userid;
    ReaderInfo finderInfo;
    userid = ui->ReadersList->item(ui->ReadersList->currentRow(), 0)->text().toStdString();

    if(ri.userid != userid && readerman.SearchReadersByuserid(ri.userid, &finderInfo) == SUCCESS)
    {
        // 新指定的编号已存在
        ui->Status->setText(QString("不能使用读者编号 %1，因为它已存在，并被 %1 使用。")
                            .arg(QString(finderInfo.userid.c_str()), QString(finderInfo.name.c_str())));
        ui->Status->setStyleSheet("color:red;");

        return;
    }

    // 检测是否正在操作已登录用户
    bool isLogin = currentReader->userid == userid && isEditMode;
    if(isLogin)
    {
        // 正在操作已登录用户：询问
        if(QMessageBox::No == QMessageBox::warning(this, QString("警告：您正在操作一个已登录读者"),
                             QString("读者 %1 目前已登录图书馆。\n\n如果您修改它的信息，当操作成功完成后，读者将会被强制注销。\n\n是否继续？")
                             .arg(userid.c_str()),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::Yes))
        {
            // 不操作
            return;
        }
    }

    // 操作
    Status result;
    int currentRow;
    if(isEditMode)
    {
        // 修改模式
        result = readerman.ModifyReadersInfo(userid, &ri);
        currentRow = ui->ReadersList->currentRow();
    }
    else
    {
        // 添加模式
        userid = ri.userid;
        result = readerman.AddReader(&ri);
        currentRow = 0;

        ui->ReadersList->insertRow(0);
        ui->ReadersList->selectRow(0);
    }

    // 检测状态
    if(result == SUCCESS)
    {
        // 修改成功
        ui->Status->setText(QString("%1 的读者信息已保存。").arg(QString(userid.c_str())));
        ui->Status->setStyleSheet("color:green;");

        // 检测强制注销状态
        if(isLogin)
            emit parent->forceToLogout(QString("读者信息已修改。"));
    }
    else
    {
        // 出现问题
        ui->Status->setText(QString("未知错误。%1 的读者信息未能保存。").arg(QString(userid.c_str())));
        ui->Status->setStyleSheet("color:red;");

        // 取消强制注销
        isLogin = false;
    }

    // 更新列表信息
    setItemContents(currentRow, &ri);
    emit onReaderSelected(currentRow, 0);
}

/* 按下删除读者信息按钮 */
void ReadersWindow::onDeleteClicked(void)
{
    // 获取所选用户
    auto userid = ui->ReadersList->item(ui->ReadersList->currentRow(), 0)->text().toStdString();

    // 检测登录状态
    bool isLogin = currentReader->userid == userid;

    // 询问是否删除
    if(QMessageBox::Yes == QMessageBox::question(this, QString("即将删除读者"),
                            QString("要删除读者 %1 吗？\n\n注意：该操作不能恢复。").arg(QString(userid.c_str())),
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::No))
    {
        if(isLogin && QMessageBox::No == QMessageBox::warning(this, QString("即将删除一个已经登录的读者"),
            QString("读者 %1 当前已登录。\n\n如果删除该读者，当操作成功完成后，该读者将会被强制注销。\n\n是否继续？")
            .arg(QString(userid.c_str())),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No))
        {
            return; // 停止操作
        }

        // 删除该用户
        if(readerman.RemoveReader(userid) == SUCCESS)
        {
            ui->ReaderDetailsWidget->hide();

            // 删除行
            ui->ReadersList->removeRow(ui->ReadersList->currentRow());

            // 检测强制注销
            if(isLogin)
                parent->forceToLogout(QString("该读者已被删除。请联系图书馆管理员以获取更多信息。"));
        }
        else
            QMessageBox::critical(this, QString("删除失败"),
                                  QString("发生了未知错误。未能删除读者 %1 。").arg(QString(userid.c_str())));

    }
}

/* 按下搜索按钮 */
void ReadersWindow::onSearchButtonClicked(void)
{
    // 搜索回调函数
    int currentRow = 0;
    auto callbackFunc = [&, this](int index, ReaderInfo* ri) -> bool
    {
        ui->ReadersList->insertRow(currentRow);
        setItemContents(currentRow++, ri);
        return true;
    };

    // 清空列表
    ui->ReadersList->setRowCount(0);

    // 绑定搜索函数
    typedef int(ReaderManager::*SearchFunc)(const std::string, std::function<ReaderCallbackFunc>);
    SearchFunc func[] = {
        nullptr,
        &ReaderManager::SearchReadersByname,
        nullptr,
        &ReaderManager::SearchReadersBycollege
    };

    // 搜索读者编号：另外处理
    if(searchType == 0)
    {
        ReaderInfo buffer;
        if(readerman.SearchReadersByuserid(ui->Keywords->text().toStdString(), &buffer) == SUCCESS)
            callbackFunc(0, &buffer);
    }
    else // 其它
        if(func[searchType] != nullptr)
            (readerman.*func[searchType])(ui->Keywords->text().toStdString(), callbackFunc);
}

/* 按下关闭详细信息按钮 */
void ReadersWindow::onCloseDetailsClicked(void)
{
    // 隐藏详情页
    ui->ReaderDetailsWidget->hide();
}

/* 选择项 */
void ReadersWindow::onReaderSelected(int row, int col)
{
    // 设置编辑模式
    isEditMode = true;

    // 禁止编辑 userid
    ui->UserID->setEnabled(false);

    // 获取 userid
    QTableWidgetItem* item = ui->ReadersList->item(row, 0);

    // 查找并显示
    ReaderInfo ri;
    if(readerman.SearchReadersByuserid(item->text().toStdString(), &ri) == SUCCESS)
    {
        // 显示
        ui->UserID->setText(QString(ri.userid.c_str()));
        ui->UserName->setText(QString(ri.name.c_str()));
        ui->Password->setText(QString(ri.password.c_str()));
        ui->College->setText(QString(ri.college.c_str()));
        ui->Permission->setCurrentIndex(ri.readerPermission);
    }
    else
        QMessageBox::critical(this, QString("不能显示详细信息"),
                              QString("因为我们发生了一个错误，所以没有找到 %1 读者的信息。").arg(QString(item->text().toStdString().c_str()))
                              );

    // 显示详情页
    ui->ReaderDetailsWidget->show();
}
