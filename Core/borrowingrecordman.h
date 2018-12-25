#ifndef BORROWINGRECORDMAN_H
#define BORROWINGRECORDMAN_H

#include "common.h"
#include <functional>
#include <map>

// 回调函数类型
typedef bool(BorrowingRecordCallbackFunc)(int, BorrowingRecordInfo*);

/* BorrowingRecordManager class: 提供对所有日志的管理 */
class BorrowingRecordManager
{
public:
    //构造函数
     BorrowingRecordManager();
    //析构函数
    ~BorrowingRecordManager();

public:
    //添加日志信息
    Status AddRecord(BorrowingRecordInfo* info);
    //删除日志信息
    Status RemoveRecord(const long int recid);
    //导出所有日志信息
    int GetAllRecordInfo(std::function<BorrowingRecordCallbackFunc> callbackFunc);


    //根据借阅日期搜索日志信息
    int SearchRecordBydateofBorrowing( const __time64_t dateofBorrowing ,
                                       std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据预计归还日期搜索日志信息
    int SearchRecordByExpectedDateofReturning( const __time64_t ExpectedDateofReturning ,
                                               std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据归还日期搜索日志信息
    int SearchRecordBydateofReturning( const __time64_t dateofReturning ,
                                       std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据图书编号搜索日志信息
    int SearchRecordBybookid( const std::string bookid ,
                              std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据读者编号搜索日志信息
    int SearchRecordByuserid( const std::string userid ,
                              std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据借阅种类搜索日志信息
    int SearchRecordByrecordType( const int recordType ,
                                  std::function<BorrowingRecordCallbackFunc> callbackFunc );
    //根据记录编号搜索日志信息
    Status SrearchRecordByrecid( const long int recid , BorrowingRecordInfo* info );


    //根据传入的info和编号修改日志信息
    Status ModifyRecordInfo( const long int recid , const BorrowingRecordInfo* info);
    //
    unsigned int Size();


private:
    // 从文件读取日志数据进入内存
    Status GetInfoFromDatabase();
    // 从内存写入日志数据进入文件
    Status WriteInfoToDatabase();

    std::map<long int,BorrowingRecordInfo> borrowingRecord_database;

    const char* database_path = "Database\\borrowingRecord_database.bin";
};

#endif // BORROWINGRECORDMAN_H
