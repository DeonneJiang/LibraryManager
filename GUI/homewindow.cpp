#include "homewindow.h"
#include "ui_homewindow.h"
#include <functional>
#include <ctime>
#include <random>
#include <QTableWidget>
#include <QColor>

using std::string;

HomeWindow::HomeWindow(TimeMachine &timemac, Reader &reader, QWidget *parent) :
    QWidget(parent), ui(new Ui::HomeWindow), reader(reader), timemac(timemac)
{
    qDebug() << "Start loading Home Window...";

    ui->setupUi(this);

    // 初始化借阅列表
    emit initBorrowList();

    // 初始化预约列表
    emit initSubscribeList();

    // 初始化新书推荐
    emit initNewBookRecommanded();

    // 显示图书馆图书总数
    ui->BooksTotal->setText(QString("%1").arg(reader.bookman.Size()));

    // 刷新列表
    emit refreshBorrowList();

    // 加载成功
    qDebug() << "Finished.";
}

HomeWindow::~HomeWindow()
{
    qDebug() << "Destroying Home Window...";
    delete ui;
    qDebug() << "Finished.";
}

/* 初始化新书推荐 */
void HomeWindow::initNewBookRecommanded(void)
{
    // 加载随机数生成器
    std::default_random_engine generator;
    generator.seed(time(NULL));
    std::uniform_int_distribution<int> distribution(1, reader.bookman.Size());
    const int selectedBook = distribution(generator); // 1 ~ Size 的随机数

    // 获取给定图书
    reader.bookman.GetAllBooksInfo([=, this](int index, BookInfo* bi)->bool
    {
        if(index == selectedBook)
        {
            // 创建封面
            QImage coverImage(bi->coverPath.c_str());
            ui->Cover->setPixmap(QPixmap::fromImage(coverImage.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

            // 设置信息
            ui->Title->setText(QString(bi->title.c_str()));
            ui->Author->setText(QString(bi->author.c_str()));
            ui->Press->setText(QString(bi->press.c_str()));

            // 返回 false 停止加载
            return false;
        }
        else
            return true; // 继续加载
    });
}

/* 初始化借阅列表 */
void HomeWindow::initBorrowList(void)
{
    QStringList horizontalHeaderBor;
    horizontalHeaderBor.append(QString("图书编号"));
    horizontalHeaderBor.append(QString("书名"));
    horizontalHeaderBor.append(QString("状态"));
    horizontalHeaderBor.append(QString("借阅日期"));
    horizontalHeaderBor.append(QString("最长借阅日期"));
    ui->BorrowingList->setColumnCount(5);
    ui->BorrowingList->setHorizontalHeaderLabels(horizontalHeaderBor);
    ui->BorrowingList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/* 初始化借阅列表 */
void HomeWindow::initSubscribeList(void)
{
    QStringList horizontalHeaderSub;
    horizontalHeaderSub.append(QString("图书编号"));
    horizontalHeaderSub.append(QString("书名"));
    horizontalHeaderSub.append(QString("状态"));
    horizontalHeaderSub.append(QString("预约日期"));
    horizontalHeaderSub.append(QString("库存"));
    ui->SubscribeList->setColumnCount(5);
    ui->SubscribeList->setHorizontalHeaderLabels(horizontalHeaderSub);
    ui->SubscribeList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

/* 刷新借书列表 */
void HomeWindow::refreshBorrowList(void)
{
    int ExpiredBooks = 0;
    int AvaliableBooks = 0;
    int BorrowingBooks = 0;

    // 清空列表
    ui->SubscribeList->setRowCount(0);
    ui->BorrowingList->setRowCount(0);

    reader.getRecordList([&](int index, BorrowingRecordInfo* bri)->bool
    {
        QTableWidget *SubscribeList = ui->SubscribeList;
        QTableWidget *BorrowingList = ui->BorrowingList;

        // 判断借阅类型
        switch(bri->recordType)
        {
        case 0: // 预约
        {
            int count = SubscribeList->rowCount();

            // 查询图书
            BookInfo bi;
            reader.bookman.GetBookInfo(bri->bookid, &bi);

            // 检查可借阅状态
            bool status = reader.checkBorrowingStatus(bi.bookid) == BorrowStatus::AllowedToBorrow;
            if(status) AvaliableBooks ++;

            // 准备导入数据
            string statusStr(status ? "可以借阅" : "预约中");
            string *contents[] =
            {
                &bi.bookid, &bi.title, &statusStr,
                new string(timemac.ConvertDate(bri->dateofBorrowing)),
                new string(std::to_string(bi.bookQuantity - bi.bookBorrowedOut))
            };
            SubscribeList->insertRow(count);
            for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
            {
                QTableWidgetItem *item = new QTableWidgetItem(QString(contents[i]->c_str()));
                if(status)
                    item->setTextColor(QColor(0, 255, 0));
                SubscribeList->setItem(count, i, item);
            }
            delete contents[3];
            delete contents[4];

            break;
        }
        case 1: // 已借阅未归还
        {
            int count = BorrowingList->rowCount();

            // 增加计数器
            BorrowingBooks ++;

            // 检查过期状态
            bool status = bri->ExpectedDateofReturning >= timemac.Now();
            if(!status) ExpiredBooks ++;

            // 查询图书信息
            BookInfo bi;
            reader.bookman.GetBookInfo(bri->bookid, &bi);

            // 准备导入数据
            std::string statusStr(status ? "正在借阅" : "已过期");
            std::string *contents[] =
            {
                &bi.bookid, &bi.title, &statusStr,
                new std::string(timemac.ConvertDate(bri->dateofBorrowing)),
                new std::string(timemac.ConvertDate(bri->ExpectedDateofReturning))
            };
            BorrowingList->insertRow(count);
            for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
            {
                QTableWidgetItem *item = new QTableWidgetItem(QString(contents[i]->c_str()));
                if(!status)
                    item->setTextColor(QColor(255, 0, 0));
                BorrowingList->setItem(count, i, item);
            }
            delete contents[3];
            delete contents[4];

            break;
        }
        }

        return true;
    });

    // 设置显示数据
    ui->BooksAvaliable->setText(QString("%1").arg(AvaliableBooks));
    ui->BooksBorrowing->setText(QString("%1").arg(BorrowingBooks));
    ui->BooksExpired->setText(QString("%1").arg(ExpiredBooks));
}
