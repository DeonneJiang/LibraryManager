#ifndef COMMON_H
#define COMMON_H

#pragma execution_character_set("utf-8")

#include <string>
#include <QDebug>

// 状态返回值
typedef bool Status;
#define SUCCESS 1
#define ERROR   0
#define CSM 0.14285714285714

//权限枚举类型
enum PERMISSION{
    student_permission,
    teacher_permission,
};

/* 读者信息结构体 */
typedef struct _ReaderInfo
{
    std::string name;              // 姓名
    std::string userid;            // 读者编号
    std::string college;           // 学院
    std::string password;          // 登陆密码（初始化为读者编号）
    PERMISSION readerPermission;   // 借书权限
} ReaderInfo;

/* 图书信息结构体 */
typedef struct _BookInfo
{
    //导入图书时所需要的信息
    std::string title;             // 书名
    std::string author;            // 作者
    std::string press;             // 出版社
    std::string ISBN;              // isbn编码
    __time64_t purchaseTime;       // 购入时间
    std::string coverPath;         // 封面图（路径）
    std::string classification;    // 分类
    //导入时自动生成编号
    std::string bookid;            // 图书编号（和isbn一样）

    PERMISSION permissionRequested;// 权限要求
    int bookQuantity;              // 图书总数量
    int bookBorrowedOut;           // 已借出数量
    float price;                   // 价格
} BookInfo;

/* 借书记录结构体 */
typedef struct _BorrowingRecordInfo
{

    std::string bookid;                  // 图书编号
    std::string userid;                  // 借阅的读者的编号
    int recordType;                      // 记录的种类，0表示预约，1表示已借阅未归还，2表示已借阅已归还
    __time64_t dateofBorrowing;          // 借阅日期
    __time64_t ExpectedDateofReturning;  // 预计归还日期
    __time64_t dateofReturning;          // 归还日期
    long int recid;                      // 记录编号
} BorrowingRecordInfo;

//计算字符串之间的距离之间的递归函数
int CalculateStringDistance_Recursive( std::string strA , int pABegin  , int pAEnd , std::string strB , int pBBegin , int pBEnd );
//计算字符串相似度函数（计算公式：【 1/（距离+1）】），建议匹配距离：1/7
float CalculateStringSimilarity( std::string strA , std::string strB  );

#endif // COMMON_H
