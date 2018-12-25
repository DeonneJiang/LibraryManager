#ifndef READERMAN_H
#define READERMAN_H

#include "common.h"
#include <functional>
#include <map>

// 回调函数类型
typedef bool(ReaderCallbackFunc)(int, ReaderInfo*);

/* ReaderManager class: 提供对所有读者的管理 */
class ReaderManager
{
public:
    ReaderManager();  // 构造函数
    ~ReaderManager(); // 析构函数
public:
    // 添加用户
    // 参数：将要添加的用户的全部信息
    Status AddReader(const ReaderInfo* info);
    // 删除用户
    // 参数：将要删除的用户的用户编号
    Status RemoveReader(const std::string userid);
    // 导出所有读者
    // 参数：一个回调函数
    int GetAllReadersInfo(std::function<ReaderCallbackFunc> callbackFunc);
    // 根据读者编号搜索读者
    // 参数：读者编号，已经分配了内存的读者信息结构体指针
    Status SearchReadersByuserid(const std::string userid , ReaderInfo* info);
    // 根据学院搜索读者
    // 参数：学院名，一个回调函数
    int SearchReadersBycollege(const std::string college , std::function<ReaderCallbackFunc> callbackFunc);
    // 根据名字搜索读者
    // 参数：读者名，一个回调函数
    int SearchReadersByname( const std::string , std::function<ReaderCallbackFunc> callbackFunc );
    // 修改读者信息
    // 参数：读者编号，已经分配了内存的新的读者信息结构体指针
    Status ModifyReadersInfo( const std::string userid , const ReaderInfo* info);
    //
    unsigned int Size();
private:
    // 从文件读取读者数据进入内存
    Status GetInfoFromDatabase();
    // 从内存写入读者数据进入文件
    Status WriteInfoToDatabase();
    std::map<std::string,ReaderInfo> reader_database;
    const char* databsae_path = "Database\\reader_database.bin";
};

#endif // READERMAN_H
