#ifndef BOOKSWINDOW_H
#define BOOKSWINDOW_H

#include "Core/bookman.h"

#include <QWidget>

namespace Ui {
class BooksWindow;
}

class BooksWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BooksWindow(BookManager &bm, QWidget *parent = 0);
    ~BooksWindow();

private:
    void appendNewItem(int currentRow, BookInfo* bi);

private slots:
    void initSearchTypeMenu(void);
    void initBookListHeaders(void);
    void listAllBooks(void);
    void onAddClicked(void);
    void onSaveClicked(void);
    void onDeleteClicked(void);
    void onChangeCoverClicked(void);
    void onCloseDetailsClicked(void);
    void onSearchTypeChanged(QAction *action);
    void SearchEvent(void);
    void onBookSelected(int row, int col);

private:
    Ui::BooksWindow *ui;
    BookManager     &bookman;

    int searchType; // 搜索类型
    bool isEditMode;
};

#endif // BOOKSWINDOW_H
