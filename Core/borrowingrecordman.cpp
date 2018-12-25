#include "borrowingrecordman.h"
#include <fstream>

//构造函数
//读取日志信息数据库到内存
BorrowingRecordManager::BorrowingRecordManager()
{
    this->GetInfoFromDatabase();
}

//析构函数
//将内存中的日志信息写到文件中
BorrowingRecordManager::~BorrowingRecordManager()
{
    this->WriteInfoToDatabase();
}

//从文件读取日志信息进入内存
//失败原因：打开文件失败
Status BorrowingRecordManager::GetInfoFromDatabase()
{
    BorrowingRecordInfo temp;
//    unsigned int dateofBorrowing_lenth;
//    unsigned int ExpectedDateofReturning_lenth;
//    unsigned int dateofReturning_lenth;
    unsigned int bookid_lenth;
    unsigned int userid_lenth;
    char buffer[256] = {0};

    std::ifstream in_fstream;
    in_fstream.open( this->database_path , std::ios::binary | std::ios::in );
    if( !in_fstream )
    {
        return ERROR;
    }
    else
    {
        while( !in_fstream.eof() )
        {
//            in_fstream.read( (char*)&dateofBorrowing_lenth , sizeof(unsigned int) );
//            if(in_fstream.eof()) break;
//            in_fstream.read( buffer, dateofBorrowing_lenth );
//            buffer[dateofBorrowing_lenth] = 0;
//            std::string dateofBorrowing(buffer);

//            in_fstream.read( (char*)&ExpectedDateofReturning_lenth , sizeof(unsigned int) );
//            in_fstream.read( buffer, ExpectedDateofReturning_lenth);
//            buffer[ExpectedDateofReturning_lenth] = 0;
//            std::string ExpectedDateofReturning(buffer);

//            in_fstream.read( (char*)&dateofReturning_lenth , sizeof(unsigned int) );
//            in_fstream.read( buffer, dateofReturning_lenth);
//            buffer[dateofReturning_lenth] = 0;
//            std::string dateofReturning(buffer);

            in_fstream.read( (char*)&bookid_lenth , sizeof(unsigned int) );
            if(in_fstream.eof()) break;
            in_fstream.read( buffer, bookid_lenth);
            buffer[bookid_lenth] = 0;
            std::string bookid(buffer);

            in_fstream.read( (char*)&userid_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, userid_lenth);
            buffer[userid_lenth] = 0;
            std::string userid(buffer);


            in_fstream.read( (char*)&temp.recordType, sizeof(temp.recordType) );

            in_fstream.read( (char*)&temp.dateofBorrowing, sizeof(__time64_t) );
            in_fstream.read( (char*)&temp.ExpectedDateofReturning, sizeof(__time64_t) );
            in_fstream.read( (char*)&temp.dateofReturning, sizeof(__time64_t) );

            in_fstream.read( (char*)&temp.recid , sizeof( temp.recid ));
//            temp.dateofBorrowing = dateofBorrowing;
//            temp.ExpectedDateofReturning = ExpectedDateofReturning;
//            temp.dateofReturning = dateofReturning;
            temp.bookid = bookid;
            temp.userid = userid;

            BorrowingRecordManager::borrowingRecord_database.insert( std::map<long int,BorrowingRecordInfo>::value_type( temp.recid, temp ));
        }
        in_fstream.close();
    }
    return SUCCESS;
}

//从内存写入读者数据进入文件
//失败原因：打开文件失败
Status BorrowingRecordManager::WriteInfoToDatabase()
{
    std::ofstream Info_ostream;
    Info_ostream.open( this->database_path , std::ios::binary | std::ios::out );
    BorrowingRecordInfo* info;
    if( !Info_ostream )
    {
        throw "打开文件失败";
    }
    else
    {
        for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++)
        {
            info = new BorrowingRecordInfo;
            *info = iter->second;

//            unsigned int dateofBorrowing_lenth = info->dateofBorrowing.length();
//            unsigned int ExpectedDateofReturning_lenth  = info->ExpectedDateofReturning.length();
//            unsigned int dateofReturning_lenth = info->dateofReturning.length();
            unsigned int bookid_lenth = info->bookid.length();
            unsigned int userid_lenth =info->userid.length();


            const char*  bookid = info->bookid.c_str();
            const char*  userid = info->userid.c_str();
            int recordType_ = info->recordType;
            __time64_t  dateofBorrowing = info->dateofBorrowing;
            __time64_t  ExpectedDateofReturning =info->ExpectedDateofReturning;
            __time64_t  dateofReturning = info->dateofReturning;
            long int recid_ = info->recid;

//            Info_ostream.write( (char*)&dateofBorrowing_lenth , sizeof(unsigned int) );
//            Info_ostream.write( (char*)dateofBorrowing , dateofBorrowing_lenth );

//            Info_ostream.write( (char*)&ExpectedDateofReturning_lenth , sizeof(unsigned int) );
//            Info_ostream.write( (char*)ExpectedDateofReturning , ExpectedDateofReturning_lenth );


//            Info_ostream.write( (char*)&dateofReturning_lenth , sizeof(unsigned int) );
//            Info_ostream.write( (char*)dateofReturning , dateofReturning_lenth );


            Info_ostream.write( (char*)&bookid_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)bookid , bookid_lenth );


            Info_ostream.write( (char*)&userid_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)userid , userid_lenth );

            Info_ostream.write( (char*)&(recordType_) , sizeof(int) );

            Info_ostream.write( (char*)&(dateofBorrowing) , sizeof(__time64_t) );
            Info_ostream.write( (char*)&(ExpectedDateofReturning) , sizeof(__time64_t) );
            Info_ostream.write( (char*)&(dateofReturning) , sizeof(__time64_t) );

            Info_ostream.write( (char*)&(recid_) , sizeof(long int) );

            delete info;
        }
        Info_ostream.close();
    }
    return SUCCESS;
}

//添加日志信息
Status BorrowingRecordManager::AddRecord(BorrowingRecordInfo* info)
{
    unsigned int sum = this->Size();
    info->recid = sum+1;
    std::pair<std::map<long int,BorrowingRecordInfo>::iterator,bool> p_pair;
    p_pair = borrowingRecord_database.insert( std::map<long int,BorrowingRecordInfo>::value_type( info->recid , *info ) );
    if( p_pair.second )
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

//删除日志信息
Status BorrowingRecordManager::RemoveRecord(const long int recid)
{
    /*
    std::map<long int,BorrowingRecordInfo>::iterator iter;
    iter = borrowingRecord_database.find( recid );
    if( iter !=borrowingRecord_database.end() )
    {
        borrowingRecord_database.erase( iter );
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
    */
    return ERROR; // This function is not avaliable
}

//导出所有日志信息
int BorrowingRecordManager::GetAllRecordInfo(std::function<BorrowingRecordCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        i++;
        callbackFunc(i,info);
        delete info;
    }
    return i;
}


//根据借阅日期搜索日志信息
int BorrowingRecordManager::SearchRecordBydateofBorrowing(const __time64_t dateofBorrowing , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->dateofBorrowing == dateofBorrowing )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据预计归还日期搜索日志信息
int BorrowingRecordManager::SearchRecordByExpectedDateofReturning(const __time64_t ExpectedDateofReturning , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->ExpectedDateofReturning == ExpectedDateofReturning )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据归还日期搜索日志信息
int BorrowingRecordManager::SearchRecordBydateofReturning(const __time64_t dateofReturning , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->dateofReturning == dateofReturning )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据图书编号搜索日志信息
int BorrowingRecordManager::SearchRecordBybookid(const std::string bookid , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->bookid == bookid )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据读者编号搜索日志信息
int BorrowingRecordManager::SearchRecordByuserid(const std::string userid , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->userid == userid )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据借阅种类搜索日志信息
int BorrowingRecordManager::SearchRecordByrecordType(const int recordType , std::function<BorrowingRecordCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<long int,BorrowingRecordInfo>::iterator iter = this->borrowingRecord_database.begin() ; iter != this->borrowingRecord_database.end() ; iter++ )
    {
        BorrowingRecordInfo * info = new BorrowingRecordInfo;
        *info = iter->second;
        if( info->recordType == recordType )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

//根据记录编号搜索日志信息
Status BorrowingRecordManager::SrearchRecordByrecid( const long int recid , BorrowingRecordInfo* info )
{
    std::map<long int,BorrowingRecordInfo>::iterator iter;
    iter = borrowingRecord_database.find( recid );
    if( iter !=borrowingRecord_database.end() )
    {
        *info = iter->second;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}


//根据传入的info和编号修改日志信息
Status BorrowingRecordManager::ModifyRecordInfo( const long int recid , const BorrowingRecordInfo* info)
{
    std::map<long int,BorrowingRecordInfo>::iterator iter;
    iter = borrowingRecord_database.find( recid );
    if( iter !=borrowingRecord_database.end() )
    {
        iter->second = *info;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}


unsigned int BorrowingRecordManager::Size()
{
    return this->borrowingRecord_database.size();
}
