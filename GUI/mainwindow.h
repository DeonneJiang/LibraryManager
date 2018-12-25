#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Core/common.h"
#include "Core/timemac.h"
#include "Core/reader.h"

#include <QMainWindow>
#include <QRadioButton>
#include <QWidget>
#include <QAction>
#include <QTimer>
#include <QCursor>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    const int waitForOperator = 10; // 无操作等待时间

public:
    explicit MainWindow(TimeMachine &tm, Reader &reader, QWidget *parent = 0);
    ~MainWindow();

public slots: /* 公有接口 */
    void toggleBanner(const QWidget *object, bool toggle);
    void forceToLogout(QString &reason);
    void refreshTime(void);

private slots: /* 槽 */
    void initWidget(void);
    void changeWidget(int i);
    void initSearchMenu(void);
    void initUserMenu(void);
    void onTimerEvent(void);
    void onChangedWidget(bool);
    void onChangedSehType(QAction*);
    void onSelectedUserMode(QAction*);
    void onSearchButtonClicked(void);
    void onSearchTextChanged(QString);
    void checkMouseMove(void);
    void keyPressEvent(QKeyEvent*);

private: /* 状态变量 */
    int searchType = 0;

private: /* 窗体元素变量 */
    Ui::MainWindow *ui;

    TimeMachine &timemac;
    Reader      &reader;

    QTimer        *timer;
    long long int runningTime;
    long long int lastOperationTime;

    QRadioButton* *rbList;      // Radio Button List on the left
    QWidget*      *wList;             // Widget List; shown in the stackedwidget object
    bool          *bVisible;
    static const int panelCount = 6;
    QWidget*      noPermissionPanel;
};

#endif // MAINWINDOW_H
