#ifndef READERSWINDOW_H
#define READERSWINDOW_H

#include "Core/readerman.h"

#include <QWidget>

#include "mainwindow.h"

namespace Ui {
class ReadersWindow;
}

class ReadersWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ReadersWindow(const ReaderInfo *currentReader, ReaderManager &rm, MainWindow *parent);
    ~ReadersWindow();

private:
    void setItemContents(int currentRow, ReaderInfo* ri);

private slots:
    void initReaderListHeaders(void);
    void initSearchTypeMenu(void);
    void listAllReaders(void);
    void onAddClicked(void);
    void onSaveClicked(void);
    void onDeleteClicked(void);
    void onCloseDetailsClicked(void);
    void onSearchButtonClicked(void);
    void onSearchTypeChanged(QAction *action);
    void onReaderSelected(int row, int col);

private:
    Ui::ReadersWindow *ui;
    ReaderManager &readerman;
    const ReaderInfo *currentReader;
    MainWindow    *parent;

    int  searchType;
    bool isEditMode;
};

#endif // READERSWINDOW_H
