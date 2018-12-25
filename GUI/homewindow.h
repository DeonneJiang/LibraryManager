#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QWidget>
#include <QHeaderView>

#include "Core/timemac.h"
#include "Core/reader.h"

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HomeWindow(TimeMachine &timemac, Reader &reader, QWidget *parent = 0);
    ~HomeWindow();

private slots:
    void initNewBookRecommanded(void);
    void initBorrowList(void);
    void initSubscribeList(void);
    void refreshBorrowList(void);

private:
    /* User interface */
    Ui::HomeWindow *ui;
    /* Database */
    Reader &reader;
    TimeMachine &timemac;

private:
    /* callback function for query */
    bool onSearchingBorrowRecordByUser(int index, BorrowingRecordInfo* bri);
};

#endif // HOMEWINDOW_H
