#include "bookswindow.h"
#include "ui_bookswindow.h"
#include <QMenu>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

BooksWindow::BooksWindow(BookManager &bm, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BooksWindow), bookman(bm), searchType(0), isEditMode(false)
{
    qDebug() << "Start loading Books Window...";

    ui->setupUi(this);

    // 初始化搜索类型菜单
    emit initSearchTypeMenu();

    // 初始化表格表头
    emit initBookListHeaders();

    // 列出所有图书
    emit listAllBooks();

    // 选择第一本书
    ui->BooksList->selectRow(0);
    emit onBookSelected(0, 0);

    // 加载成功
    qDebug() << "Finished.";
}

BooksWindow::~BooksWindow()
{
    qDebug() << "Destroying Books Window...";
    delete ui;
    qDebug() << "Finished.";
}

/* 初始化表格表头 //加权限。。。*/
void BooksWindow::initBookListHeaders(void)
{
    QStringList horizontalHeader;
    horizontalHeader.append(tr("图书编号"));
    horizontalHeader.append(tr("书名"));
    horizontalHeader.append(tr("作者"));
    horizontalHeader.append(tr("出版社"));
    ui->BooksList->setColumnCount(4);
    ui->BooksList->setHorizontalHeaderLabels(horizontalHeader);
    ui->BooksList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStringList qsl;
    qsl.append("学生权限");
    qsl.append("教师权限");
    ui->Permission->addItems(qsl);
}

/* 初始化搜索类型菜单 */
void BooksWindow::initSearchTypeMenu(void)
{
    QMenu* searchMenu = new QMenu();

    QActionGroup *searchSetGroup = new QActionGroup(this);

    const QString typeString[] = {
        QString("ISBN"), QString("书名"), QString("作者"), QString("出版社"), QString("权限"),
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

    // 连接信号槽
    connect(searchMenu, SIGNAL(triggered(QAction*)), this, SLOT(onSearchTypeChanged(QAction*)));

    // 设置初始文字
    ui->SearchType->setText(typeString[0]);
}

/* 添加列表新项目 */
void BooksWindow::appendNewItem(int currentRow, BookInfo* bi)
{
    // 逐项添加内容
    std::string *contents[] = {
        &bi->bookid, &bi->title, &bi->author, &bi->press
    };
    for(unsigned int i = 0; i < sizeof(contents)/sizeof(contents[0]); i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString(contents[i]->c_str()));

        // 染色
        if(currentRow % 2)
            item->setBackgroundColor(QColor(235, 235, 235));
        else
            item->setBackgroundColor(QColor(255, 255, 255));

        ui->BooksList->setItem(currentRow, i, item);
    }

    // 选择已添加的行
    ui->BooksList->selectRow(currentRow);
}

/* 列出所有图书 */
void BooksWindow::listAllBooks(void)
{
    ui->BooksList->setRowCount(bookman.Size());

    bookman.GetAllBooksInfo([this](int index, BookInfo* bi)->bool
    {
        appendNewItem(index-1, bi);

        return true; // 返回true以继续加载
    });
}

/* 添加新图书按钮按下 */
void BooksWindow::onAddClicked(void)
{
    // 取消编辑模式
    isEditMode = false;

    // 设置默认封面
    ui->Path->setText(QString("cover/default-cover.png"));
    QImage coverImage(ui->Path->text());
    ui->Cover->setPixmap(QPixmap::fromImage(
                         coverImage.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    // 解锁图书编号框
    ui->BookID->setEnabled(true);

    // 初始化所有信息
    ui->Title->setText(QString("<无题>"));
    ui->Author->setText(QString("<佚名>"));
    ui->Press->setText(QString("<出版社>"));
    ui->PurchaseTime->setDateTime(QDateTime::currentDateTime());
    ui->ISBN->setText(QString("<ISBN>"));
    ui->Permission->setCurrentIndex(0);
    ui->Classification->setText(QString("<无>"));
    ui->BookID->setText(QString("<自动生成>"));
    ui->Quantity->setValue(1);
    ui->BorrowOut->setValue(0);
    ui->Price->setValue(1.0);

    // 显示信息框
    ui->BookDetailsWidget->show();
}

/* 保存图书信息按钮按下 */
void BooksWindow::onSaveClicked(void)
{
    // 构造图书信息
    BookInfo bi;

    bi.author              = ui->Author->text().toStdString();
    bi.bookBorrowedOut     = ui->BorrowOut->value();
    bi.bookid              = ui->BookID->text().toStdString();
    bi.bookQuantity        = ui->Quantity->value();
    bi.classification      = ui->Classification->text().toStdString();
    bi.coverPath           = ui->Path->text().toStdString();
    bi.ISBN                = ui->ISBN->text().toStdString();
    bi.permissionRequested = ui->Permission->currentIndex() == 1 ? teacher_permission : student_permission;
    bi.press               = ui->Press->text().toStdString();
    bi.price               = ui->Price->value();
    bi.purchaseTime        = ui->PurchaseTime->dateTime().toTime_t();
    bi.title               = ui->Title->text().toStdString();

    // 获取当前选择行
    int currentRow;
    Status result;
    if(isEditMode)
    {
        currentRow = ui->BooksList->currentRow();

        result = bookman.ModifyBook(bi.bookid, &bi);
    }
    else
    {
        ui->BooksList->insertRow(0);
        ui->BooksList->selectRow(0);
        currentRow = 0;

        result = bookman.AddBook(&bi);
    }

    // 检测状态
    if(result == SUCCESS) // 添加图书信息
    {
        ui->Status->setText(QString("%1 图书信息已保存。").arg(QString(bi.bookid.c_str())));
        ui->Status->setStyleSheet("color:green");
    }
    else
    {
        ui->Status->setText(QString("未知错误。未能修改图书信息。"));
        ui->Status->setStyleSheet("color:red;");
    }

    // 更新列表信息
    appendNewItem(currentRow, &bi);
    emit onBookSelected(currentRow, 0);
}

/* 删除图书按钮按下 */
void BooksWindow::onDeleteClicked(void)
{
    // 获取图书编号与名称
    std::string bookid = ui->BookID->text().toStdString();
    std::string booktitle = ui->BooksList->item(ui->BooksList->currentRow(), 1)->text().toStdString();

    // 确认删除
    if(QMessageBox::Ok == QMessageBox::question(this, QString(booktitle.c_str()),
                                                QString("确认删除 %1 吗？").arg(QString(booktitle.c_str())),
                                                QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel))
    {
        if(bookman.RemoveBook(bookid) == SUCCESS) // 删除
        {
            ui->BookDetailsWidget->hide();
            ui->BooksList->removeRow(ui->BooksList->currentRow());
        }
        else
        {
            QMessageBox::critical(this, QString("未知错误"),
                                  QString("发生了未知错误。未能删除图书 %1 。").arg(QString(bookid.c_str()))
                                  );
        }
    }
}

/* 修改封面按钮按下 */
void BooksWindow::onChangeCoverClicked(void)
{
    // 弹出文件选择对话框
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "./", tr("Image Files (*.png *.jpg *.bmp)"));

    if(!fileName.length()) return;

    // 使用相对路径
    fileName.replace(QString("%1%2").arg(QDir::currentPath(), QString("/")), QString(""));

    // 更新显示
    ui->Path->setText(fileName);
    QImage coverImage(fileName);
    ui->Cover->setPixmap(QPixmap::fromImage(coverImage.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

/* 关闭详情页按钮按下 */
void BooksWindow::onCloseDetailsClicked(void)
{
    ui->BookDetailsWidget->hide();
}

/* 搜索按钮按下 */
void BooksWindow::SearchEvent(void)
{
    // 绑定搜索函数
    typedef int(BookManager::*SearchFunc)(const std::string, std::function<BookCallbackFunc>);
    SearchFunc func[] = {
        nullptr,
        &BookManager::SearchBooksByName,
        &BookManager::SearchBooksByAuthor,
        &BookManager::SearchBooksByPress,
        nullptr
    };

    // 添加
    int total = 0;
    if(func[searchType] != nullptr)
    {
        // 清空列表
        ui->BooksList->setRowCount(0);

        (bookman.*func[searchType])(ui->Keywords->text().toStdString(), [&, this](int index, BookInfo* bi) -> bool
        {
            ui->BooksList->insertRow(total);
            appendNewItem(total, bi);
            total++;
            return true;
        });
    }
}

/* 搜索类型改变 */
void BooksWindow::onSearchTypeChanged(QAction* action)
{
    searchType = action->data().toInt();

    // 设置按钮文字
    ui->SearchType->setText(action->text());
}

void BooksWindow::onBookSelected(int row, int col)
{
    // 获取Bookid
    QTableWidgetItem* item = ui->BooksList->item(row, 0);

    // 查找并显示
    BookInfo bi;
    bookman.GetBookInfo(item->text().toStdString(), &bi);
    ui->Title->setText( QString(bi.title.c_str()) );
    ui->Author->setText( QString(bi.author.c_str()) );
    ui->Classification->setText( QString(bi.classification.c_str()) );
    ui->BookID->setText( QString(bi.bookid.c_str()) );
    ui->Press->setText( QString(bi.press.c_str()) );
    ui->ISBN->setText( QString(bi.ISBN.c_str()) );
    ui->BorrowOut->setValue(bi.bookBorrowedOut);
    ui->Permission->setCurrentIndex(bi.permissionRequested);
    ui->Quantity->setValue(bi.bookQuantity);
    ui->PurchaseTime->setDateTime(QDateTime::fromTime_t(bi.purchaseTime));
    ui->Price->setValue( bi.price );
    ui->Path->setText(QString(bi.coverPath.c_str()));

    // 设置封面
    QImage coverImage(QString(bi.coverPath.c_str()));
    ui->Cover->setPixmap(QPixmap::fromImage(coverImage.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation)));

    // 禁止更改图书编号
    ui->BookID->setEnabled(false);

    // 设置编辑状态
    isEditMode = true;

    // 显示信息
    ui->BookDetailsWidget->show();
}
