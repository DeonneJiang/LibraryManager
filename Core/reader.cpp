#include "common.h"
#include "reader.h"

/* Reader类的构造函数 */
Reader::Reader(BookManager & bm, BorrowingRecordManager & brm, 
    ReaderManager & rm, ReaderInfo & ri) :
    bookman(bm), recman(brm), readman(rm), ri(ri)
{
    // Nothing to do
}

/* Reader类的析构函数 */
Reader::~Reader()
{
    // Nothing to do
}

/* 检查一本书的借阅状态 */
BorrowStatus::status Reader::checkBorrowingStatus(std::string bookid)
{
    // 获取被借图书的信息
    BookInfo bi;
    if(bookman.GetBookInfo(bookid, &bi) == ERROR)
        return BorrowStatus::UnknownError;
    
    // 检查借书权限
    if(ri.readerPermission < bi.permissionRequested)
        return BorrowStatus::NoPermission;
    if(bi.bookQuantity - bi.bookBorrowedOut <= 0)
        return BorrowStatus::QuantityRunsOut;

    bool isOnBorrowing = false;
    recman.SearchRecordByuserid(ri.userid,
        [&](int index, BorrowingRecordInfo* bri)->bool
        {
            if(bri->bookid == bookid && bri->recordType == 1) // 正在被借阅
                isOnBorrowing = true;
            return !isOnBorrowing;
        });
    if(isOnBorrowing)
        return BorrowStatus::OnBorrowing;
    else
        return BorrowStatus::AllowedToBorrow;
}

/* 检查一本书的借阅状态 */
SubscribeStatus::status Reader::checkSubscribingStatus(std::string bookid)
{
    bool isOnSubscribing = false;
    recman.SearchRecordByuserid(ri.userid,
        [&](int index, BorrowingRecordInfo* bri)->bool
    {
        if(bri->bookid == bookid && bri->recordType == 0) // 正在预约
            isOnSubscribing = true;
        return !isOnSubscribing;
    });

    if(isOnSubscribing)
        return SubscribeStatus::OnSubscribing;
    else
        return SubscribeStatus::NotSubscribing;
}

/* 用户借书 */
BorrowStatus::status Reader::borrows(std::string bookid, __time64_t now)
{
    // 检查读者是否可以借阅本书
    auto status = this->checkBorrowingStatus(bookid);
    if(status != BorrowStatus::AllowedToBorrow)
        return status;
    
    // 在数据库中查找借阅记录
    BorrowingRecordInfo bri;
    int recid = 0;
    recman.SearchRecordByuserid(ri.userid, 
        [&](int index, BorrowingRecordInfo* br) -> bool
        {
            if(br->bookid == bookid && br->recordType == 0)
                recid = br->recid;
            return recid ? false : true;
        });
    
    // 修改记录以完成借阅
    bri.recordType = 1; // 已借阅
    bri.dateofBorrowing = now;
    bri.ExpectedDateofReturning = now + static_cast<__time64_t>(AllowedBorrowTime);
    bri.dateofReturning = 0;
    // 保存记录
    if(recid)
    {
        if(recman.ModifyRecordInfo(recid, &bri) == ERROR)
            return BorrowStatus::UnknownError;
    }
    else
    {
        bri.bookid = bookid;
        bri.userid = ri.userid;
        if(recman.AddRecord(&bri) == ERROR)
            return BorrowStatus::UnknownError;
    }

    // 修改图书剩余数量
    BookInfo bi;
    if(bookman.GetBookInfo(bookid, &bi) == ERROR)
        return BorrowStatus::UnknownError;
    bi.bookBorrowedOut += 1;
    bookman.ModifyBook(bookid, &bi);

    return BorrowStatus::Success;
}

/* 用户还书 */
ReturnStatus::status Reader::returns(std::string bookid, __time64_t now)
{
    // 查找对应借书记录
    ReturnStatus::status returnStatus = ReturnStatus::Success;

    bool isFound = false;
    recman.SearchRecordByuserid(ri.userid, 
        [&](int index, BorrowingRecordInfo* bri)->bool
        {
            if(bri->bookid == bookid && bri->recordType == 1)
            {
                // 标记该记录已归还
                isFound = true;

                bri->recordType = 2;
                bri->dateofReturning = now;
                if(now > bri->ExpectedDateofReturning)
                    returnStatus = ReturnStatus::SuccessButExpired;
                if(recman.ModifyRecordInfo(bri->recid, bri) == ERROR)
                    returnStatus = ReturnStatus::UnknownError;
            }
            return !isFound;
        });
    
    // 修改图书数据
    if(returnStatus == ReturnStatus::Success || returnStatus == ReturnStatus::SuccessButExpired)
    {
        BookInfo bi;
        if(bookman.GetBookInfo(bookid, &bi) == ERROR)
            return ReturnStatus::UnknownError;

        bi.bookBorrowedOut --;
        if(bookman.ModifyBook(bookid, &bi) == ERROR)
            return ReturnStatus::UnknownError;
    }

    // 检查状态
    if(isFound)
        return returnStatus;
    else
        return ReturnStatus::BookNotExist;
}

/* 用户预约 */
SubscribeStatus::status Reader::subscribes(std::string bookid, __time64_t now)
{
    // 预约不需要检查图书情况；只添加预约记录
    BorrowingRecordInfo bri;
    bri.bookid = bookid;
    bri.userid = ri.userid;
    bri.recordType = 0; // 预约
    bri.dateofBorrowing = now;
    bri.ExpectedDateofReturning = 0;
    bri.dateofReturning = 0;

    // 写入记录
    if(recman.AddRecord(&bri) == ERROR)
        return SubscribeStatus::UnknownError;

    return SubscribeStatus::Success;
}

/* 用户取消预约 */
SubscribeStatus::status Reader::unsubscribes(std::string bookid)
{
    // 搜索
    bool isFound = false;
    bool onError = false;
    recman.SearchRecordByuserid(ri.userid, 
        [&](int index, BorrowingRecordInfo* bri)->bool
        {
            if(bri->bookid == bookid && bri->recordType == 0)
            {
                isFound = true;

                bri->recordType = 4;
                if(recman.ModifyRecordInfo(bri->recid, bri) == ERROR)
                    onError = true;
            }
            return !isFound;
        });
    
    if(isFound)
        if(onError)
            return SubscribeStatus::Success;
        else
            return SubscribeStatus::UnknownError;
    else
        return SubscribeStatus::BookNotExist;
}

/* 查看用户所有借书记录 */
int Reader::getRecordList(std::function<BorrowingRecordCallbackFunc> func)
{
    return recman.SearchRecordByuserid(ri.userid, func);
}
