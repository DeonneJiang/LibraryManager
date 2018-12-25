#ifndef BOOKMAN_H
#define BOOKMAN_H

#include <functional>
#include <map>
#include "common.h"

/* 回调函数类型 */
typedef bool(BookCallbackFunc)(int, BookInfo*);

/* BookManager class: 图书管理器 提供对所有图书的管理，如批量录入/统计 */
class BookManager
{
public:
    BookManager(void);  // 构造函数
    ~BookManager(void); // 析构函数

                          /*--搜索操作集合，不改变图书数据库。--*/
public:
    // 根据编号获取图书信息
    Status GetBookInfo(const std::string bookid, BookInfo* outinfo);
    // 导出所有图书
    int GetAllBooksInfo(std::function<BookCallbackFunc> callbackFunc);
    // 根据书名搜索图书
    int SearchBooksByName(const std::string keywords , std::function<BookCallbackFunc> callbackFunc);
    // 根据作者搜索图书
    int SearchBooksByAuthor(const std::string keywords , std::function<BookCallbackFunc> callbackFunc);
    // 根据出版社搜索图书
    int SearchBooksByPress(const std::string keywords , std::function<BookCallbackFunc> callbackFunc);
    //
    unsigned int Size();


                            /*修改操作集合，改变图书数据库*/
public:
    // 图书购入
    Status AddBook( BookInfo* bookinfo);
    // 根据编号进行图书修改（不修改图书编号）
    Status ModifyBook(const std::string bookid , const BookInfo* info);
    // 根据编号进行图书删除
    Status RemoveBook(const std::string bookid);

                               /*数据及基本函数*/
private:
    // 从文件读取读者数据进入内存
    Status GetInfoFromDatabase();
    // 从内存写入读者数据进入文件
    Status WriteInfoToDatabase();
    std::map<std::string,BookInfo> book_database;

    const char* database_path = "Database\\book_database.bin";
};

#endif // BOOKMAN_H