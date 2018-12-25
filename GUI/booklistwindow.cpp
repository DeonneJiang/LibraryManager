#include <memory>
#include <functional>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QImage>
#include <QColor>

#include "booklistwindow.h"
#include "ui_booklistwindow.h"

/* 窗口构造函数 */
BookListWindow::BookListWindow(TimeMachine &timemac, Reader &reader, MainWindow *parent) :
    QWidget(parent),
    ui(new Ui::BookListWindow), parent(parent), reader(reader), timemac(timemac),
    isLoaded(false) // 列表加载状态
{
    qDebug() << "Start loading Book List Window...";

    ui->setupUi(this);

    // 初始化列表
    QStringList horizontalHeader;
    horizontalHeader.append(tr("图书编号"));
    horizontalHeader.append(tr("书名"));
    horizontalHeader.append(tr("作者"));
    horizontalHeader.append(tr("出版社"));
    ui->BookList->setColumnCount(4);
    ui->BookList->setHorizontalHeaderLabels(horizontalHeader);
    ui->BookList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化界面
    ui->stackedWidget->setCurrentIndex(1); // 屏蔽图片模式

    // 隐藏详细信息
    ui->DetailsBoxWidget->hide();

    // 立即加载图书
    emit showAllBooks();

    // 加载成功
    qDebug() << "Finished.";
}

/* 窗口析构函数 */
BookListWindow::~BookListWindow()
{
    qDebug() << "Destroying Book List Window...";
    delete ui;
    qDebug() << "Finished.";
}

/* 图片模式/列表模式 切换响应函数 */
void BookListWindow::onChangedMode(bool state)
{
    // 检查选中状态；非选中状态不处理
    if(!state) return;

    // 切换列表
    const QRadioButton* rbList[] = {ui->PictureMode, ui->ListMode};
    const QRadioButton* BookListObject = ui->ListMode;

    for(unsigned int i = 0; i < sizeof(rbList)/sizeof(rbList[0]); i++)
    {
        if(rbList[i] == qobject_cast<QRadioButton*>(sender())) // 已找到
        {
            ui->stackedWidget->setCurrentIndex(i);
            if(rbList[i] == BookListObject) // 发送加载列表信号
                emit showAllBooks();
        }
    }
}

/* 槽：装载列表 */
void BookListWindow::showAllBooks(void)
{
    // 清空列表
    ui->BookList->setRowCount(reader.bookman.Size());

    reader.bookman.GetAllBooksInfo([this](int index, BookInfo* bi)->bool
    {
        int currentRow = index-1; // 当前行数

        // 逐项添加内容
        std::string *contents[] = {
            &bi->bookid, &bi->title, &bi->author, &bi->press
        };
        for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
        {
            QTableWidgetItem* item = new QTableWidgetItem(QString(contents[i]->c_str()));

            // 染色
            if(index % 2)
                item->setBackgroundColor(QColor(235, 235, 235));
            else
                item->setBackgroundColor(QColor(255, 255, 255));

            ui->BookList->setItem(currentRow, i, item);
        }
        return true; // 返回true以继续加载
    });

    // 保存状态变量
    isLoaded = true;
}

/* 槽：点击图书列表 */
void BookListWindow::onSelectedBook(int row, int col)
{
    // 获取Bookid
    QTableWidgetItem* item = ui->BookList->item(row, 0);

    // 查找并显示
    BookInfo bi;

    if(reader.bookman.GetBookInfo(item->text().toStdString(), &bi) == ERROR)
        return;

    bookinfo = bi;
    this->showDetails(&bi);

    emit toggleDetailsBox(true);
}

/* 槽：点击图书信息关闭按钮 */
void BookListWindow::onClickedCloseDetailsBox(void)
{
    emit toggleDetailsBox(false);
}

/* 显示图书详情 */
void BookListWindow::showDetails(BookInfo* bi)
{
    // 渲染封面
    QImage coverImage(QString(bi->coverPath.c_str()));
    ui->Cover->setPixmap(QPixmap::fromImage(coverImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    // 显示数据
    std::string* datas[] = {
        &bi->title, &bi->author, &bi->press, &bi->classification,
        new std::string(bi->permissionRequested == teacher_permission ? "教师" : "学生"),
        &bi->ISBN,
        new std::string(TimeMachine::ConvertDate(bi->purchaseTime)),
        &bi->bookid,
        new std::string(std::to_string(bi->price))
    };
    QLabel* labels[] = {
        ui->Title, ui->Author, ui->Press, ui->Classification,
        ui->Permission, ui->ISBN, ui->PurchaseTime, ui->BookID, ui->Price
    };
    for(unsigned int i = 0; i < sizeof(datas)/sizeof(datas[0]); i++)
        labels[i]->setText(QString(datas[i]->c_str()));

    // 隐藏所有操作按钮
    ui->Borrow->hide();
    ui->Return->hide();
    ui->Subscribe->hide();
    ui->Unsubscribe->hide();

    // 查询允许的借阅状态
    BorrowStatus::status currentStatus = BorrowStatus::UnknownError;
    BorrowStatus::status bStatus[] = {
        BorrowStatus::AllowedToBorrow, BorrowStatus::OnBorrowing,
        BorrowStatus::NoPermission, BorrowStatus::QuantityRunsOut
    };
    QPushButton *bButton[] = {
        ui->Borrow, ui->Return, nullptr, nullptr
    };
    QString bString[] = {
        QString("当前可借阅，剩余 %1 本。").arg(bi->bookQuantity - bi->bookBorrowedOut),
        QString("正在借阅。"),
        QString("目前无法借阅，因为您的借阅权限不足。"),
        QString("目前无法借阅，剩余 0 本。")
    };
    currentStatus = reader.checkBorrowingStatus(bi->bookid);
    for(unsigned int i = 0; i < sizeof(bStatus)/sizeof(bStatus[0]); i++)
        if(currentStatus == bStatus[i])
        {
            if(bButton[i] != nullptr) // 显示按钮
                bButton[i]->show();
            ui->CountState->setText(bString[i]); // 设置显示文字
            break;
        }

    // 查询允许的借阅状态
    if(currentStatus != BorrowStatus::OnBorrowing && currentStatus != BorrowStatus::AllowedToBorrow)
    {
        if(reader.checkSubscribingStatus(bi->bookid) == SubscribeStatus::OnSubscribing)
            ui->Unsubscribe->show();
        else
            ui->Subscribe->show();
    }
}

/* 切换列表显示状态 */
void BookListWindow::toggleDetailsBox(bool state)
{
    if(state)
        ui->DetailsBoxWidget->show();
    else
        ui->DetailsBoxWidget->hide();

    emit parent->toggleBanner(this, !state);
}

/* 借书按钮 */
void BookListWindow::onCommandButtonClicked(void)
{
    // 按钮列表
    QPushButton *button[] = {
        ui->Borrow, ui->Return, ui->Subscribe, ui->Unsubscribe
    };
    // 命令列表
    std::function<void(void)> func[] = {
        [this](void) { reader.borrows(bookinfo.bookid, timemac.Now()); },
        [this](void) { reader.returns(bookinfo.bookid, timemac.Now()); },
        [this](void) { reader.subscribes(bookinfo.bookid, timemac.Now()); },
        [this](void) { reader.unsubscribes(bookinfo.bookid); }
    };

    for(unsigned int i = 0; i < sizeof(button)/sizeof(button[0]); i++)
        if(qobject_cast<QPushButton*>(sender()) == button[i])
            (func[i])();

    // 刷新命令按钮
    this->showDetails(&bookinfo);
}

/* 刷新 */
void BookListWindow::onRefreshDetails(void)
{
    this->showDetails(&bookinfo);
}

/* 搜索 */
void BookListWindow::searchBooks(QString type, QString keywords)
{
    typedef int(BookManager::*SearchFunc)(const std::string, std::function<BookCallbackFunc>);

    // 映射表
    QString types[] = {"Title", "Author", "Press"};
    SearchFunc func[] = {
        &BookManager::SearchBooksByName,
        &BookManager::SearchBooksByAuthor,
        &BookManager::SearchBooksByPress
    };

    // 匹配映射表
    for (unsigned int i = 0; i < sizeof(types)/sizeof(types[0]); i++)
    {
        if (types[i] == type)
        {
            // 清除所有行
            ui->BookList->setRowCount(0);

            // 计数器
            int count = 0;

            (reader.bookman.*func[i])(keywords.toStdString(), [&, this](int index, BookInfo* bi)->bool // 调用
            {
                // 添加新行并填充内容
                std::string *contents[] = {
                    &bi->bookid, &bi->title, &bi->author, &bi->press
                };

                // 添加新行
                ui->BookList->insertRow(count);
                for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
                {
                    QTableWidgetItem* item = new QTableWidgetItem(QString(contents[i]->c_str()));

                    // 染色
                    if(count % 2)
                        item->setBackgroundColor(QColor(235, 235, 235));
                    else
                        item->setBackgroundColor(QColor(255, 255, 255));

                    ui->BookList->setItem(count, i, item);
                }
                count ++;

                // 返回true以继续加载
                return true;
            });
        }
    }
}
