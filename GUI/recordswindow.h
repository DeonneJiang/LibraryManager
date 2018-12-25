#ifndef RECORDSWINDOW_H
#define RECORDSWINDOW_H

#include "Core/borrowingrecordman.h"
#include "Core/common.h"
#include <QWidget>

namespace Ui {
class RecordsWindow;
}

class RecordsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RecordsWindow(const ReaderInfo *currentReader, BorrowingRecordManager &brm, QWidget *parent = 0);
    ~RecordsWindow();

private:
    void appendNewItem(int currentRow, BorrowingRecordInfo* bri);

private slots:
    void initSearchTypeMenu(void);
    void initRecordListHeader(void);
    void listAllRecords(void);
    void onSearchTypeChanged(QAction *action);
    void onSwitchModeClicked(void);
    void SearchEvent(void);

private:
    Ui::RecordsWindow *ui;
    BorrowingRecordManager &recordman;
    const ReaderInfo *currentReader;

    int searchType;
    bool isRootMode;
};

#endif // RECORDSWINDOW_H
