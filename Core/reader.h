#ifndef READER_H
#define READER_H

#include <functional>
#include <string>

#include "common.h"
#include "bookman.h"
#include "borrowingrecordman.h"
#include "readerman.h"

class BorrowStatus
{
public:
    typedef enum _status {
        UnknownError = 0, Success, BookNotExist, AllowedToBorrow, QuantityRunsOut, NoPermission, OnBorrowing
    } status;
};

class ReturnStatus
{
public:
    typedef enum _stauts {
        UnknownError = 0, Success, BookNotExist, SuccessButExpired
    } status;
};

class SubscribeStatus
{
public:
    typedef enum _status {
        UnknownError = 0, Success, BookNotExist, OnSubscribing, NotSubscribing, OnBorrowing
    } status;
};

class Reader
{
public:
    BookManager&            bookman;
    BorrowingRecordManager& recman;
    ReaderManager&          readman;
    const ReaderInfo&       ri;

public:
    const int AllowedBorrowTime = 604800; // 允许借阅的时长：一周

public:
    Reader(BookManager& bm, BorrowingRecordManager& brm, 
        ReaderManager& rm, ReaderInfo& ri);
    ~Reader();

public:
    BorrowStatus::status    borrows(std::string bookid, __time64_t now);   // 借书
    ReturnStatus::status    returns(std::string bookid, __time64_t now);   // 还书
    SubscribeStatus::status subscribes(std::string bookid, __time64_t now);
    SubscribeStatus::status unsubscribes(std::string bookid);
    int                     getRecordList(std::function<BorrowingRecordCallbackFunc> func);
    BorrowStatus::status    checkBorrowingStatus(std::string bookid);
    SubscribeStatus::status checkSubscribingStatus(std::string bookid);

};

#endif  
