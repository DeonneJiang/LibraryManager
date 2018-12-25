#include "recordswindow.h"
#include "ui_recordswindow.h"
#include "Core\timemac.h"
#include <QMenu>
RecordsWindow::RecordsWindow(const ReaderInfo *currentReader, BorrowingRecordManager &brm, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordsWindow), recordman(brm), searchType(0), currentReader(currentReader), isRootMode(false)
{
    qDebug() << "Start loading Records Window...";

    ui->setupUi(this);

    // 初始化搜索类型菜单
    emit initSearchTypeMenu();

    // 初始化列表表头
    emit initRecordListHeader();

    // 列出所有记录内容
    emit listAllRecords();

    // 检查权限按钮是否可以显示
    ui->SwitchMode->setVisible(currentReader->readerPermission == teacher_permission);

    // 加载成功
    qDebug() << "Finished.";
}

RecordsWindow::~RecordsWindow()
{
    qDebug() << "Destroying Records Window...";
    delete ui;
    qDebug() << "Finished.";
}

/* 往表格添加内容 */
void RecordsWindow::appendNewItem(int currentRow, BorrowingRecordInfo* bri)
{
    //准备容器与内容
    std::string * p_recordType = NULL;
    if( bri->recordType == 0 )
    {
        p_recordType = new std::string("预约");
    }
    else
    {
        if( bri->recordType == 1 )
        {
            p_recordType = new std::string("已借阅未归还");
        }
        else
        {
            p_recordType = new std::string("已借阅已归还");
        }
    }
    std::string * p_dataofB = new std::string(bri->dateofBorrowing ? TimeMachine::ConvertDate(bri->dateofBorrowing) : "-");
    std::string * p_EDR     = new std::string(bri->ExpectedDateofReturning ? TimeMachine::ConvertDate(bri->ExpectedDateofReturning) : "-");
    std::string * p_dataofR = new std::string(bri->dateofReturning ? TimeMachine::ConvertDate(bri->dateofReturning) : "-");

    std::string *contents[] = {
        new std::string(std::to_string(bri->recid)), p_recordType,
        p_dataofB, p_EDR,
        p_dataofR, &bri->userid,
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

        ui->RecordsList->setItem(currentRow, i, item);
    }

    //释放内存
    for( int i = 0 ; i<5 ; i++) delete contents[i];

    // 选择已添加的行
    ui->RecordsList->selectRow(currentRow);
}

/* 列出所有记录内容 */
void RecordsWindow::listAllRecords(void)
{
    //。。。。
    ui->RecordsList->setRowCount(0);

    int currentRow = 0;
    recordman.GetAllRecordInfo( [&, this](int index, BorrowingRecordInfo* bri)->bool
    {
        if(isRootMode || bri->userid == currentReader->userid)
        {
            ui->RecordsList->insertRow(currentRow);
            appendNewItem(currentRow++, bri);
        }

        return true; // 返回true以继续加载

     });
}

/* 初始化搜索类型菜单 */
void RecordsWindow::initSearchTypeMenu(void)
{
    QMenu* searchMenu = new QMenu();

    QActionGroup *searchSetGroup = new QActionGroup(this);

    const QString typeString[] = {
        QString("借阅日期"), QString("归还日期"), QString("图书编号"), QString("读者编号"), QString("借阅种类"),
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
    connect(searchMenu, SIGNAL(triggered(QAction*)), this, SLOT(onSearchTypeChanged(QAction*)));

    // 设置按钮字体
    ui->SearchType->setText(typeString[0]);
}

/* 初始化列表表头 */
void RecordsWindow::initRecordListHeader(void)
{
    QStringList horizontalHeader;
    horizontalHeader.append(tr("编号"));
    horizontalHeader.append(tr("种类"));
    horizontalHeader.append(tr("借阅日期"));
    horizontalHeader.append(tr("预计归还日期"));
    horizontalHeader.append(tr("归还日期"));
    horizontalHeader.append(tr("读者编号"));
    ui->RecordsList->setColumnCount(6);
    ui->RecordsList->setHorizontalHeaderLabels(horizontalHeader);
    ui->RecordsList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/* 按下搜索按钮时，或输入文字触发的搜索事件 */
void RecordsWindow::SearchEvent(void)
{
    // 记录个数
    int total = 0;

    // 显示结果的Lambda函数
    auto lambdaFunc = [&, this](int index, BorrowingRecordInfo* bri) -> bool
    {
        if(isRootMode || bri->userid == currentReader->userid)
        {
            ui->RecordsList->insertRow(total);
            appendNewItem(total++, bri);
        }

        return true;
    };

    // 清空列表
    ui->RecordsList->setRowCount(0);

    // 检测搜索类型
    switch(searchType)
    {
    case 0: // 借阅日期
        // recordman.SearchRecordBydateofBorrowing
        break;
    case 1: // 归还日期
        // recordman.SearchRecordByExpectedDateofReturning
        break;
    case 2: // 图书编号
        recordman.SearchRecordBybookid(ui->Keywords->text().toStdString(), lambdaFunc);
        break;
    case 3: // 读者编号
        recordman.SearchRecordByuserid(ui->Keywords->text().toStdString(), lambdaFunc);
        break;
    case 4: // 借阅种类
        recordman.SearchRecordByrecordType(std::stoi(ui->Keywords->text().toStdString()), lambdaFunc);
        break;
    }
}

/* 改变搜索方式 */
void RecordsWindow::onSearchTypeChanged(QAction* action)
{
    searchType = action->data().toInt();

    // 设置按钮文字
    ui->SearchType->setText(action->text());
}

/* 切换浏览模式 */
void RecordsWindow::onSwitchModeClicked(void)
{
    // 切换状态
    isRootMode = !isRootMode;

    // 修改按钮文字
    ui->SwitchMode->setText(QString(isRootMode ? "切换到读者模式" : "切换到管理员模式"));

    // 刷新列表
    emit listAllRecords();
}
