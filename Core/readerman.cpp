#include "readerman.h"
#include <fstream>

// 构造函数
// 读取读者信息数据库到内存
ReaderManager::ReaderManager()
{
    this->GetInfoFromDatabase();
}

// 析构函数
// 将内存中的读者信息数据写到文件中
ReaderManager::~ReaderManager()
{
    this->WriteInfoToDatabase();
}

// 从文件读取读者数据进入内存
// 失败原因：打开文件失败
Status ReaderManager::GetInfoFromDatabase()
{
	
    //将文件中的一个记录导入temp
    ReaderInfo temp;
    unsigned int name_lenth=0;
    unsigned int userid_lenth=0;
    unsigned int college_lenth=0;
    unsigned int password_lenth=0;
    char buffer[256] = {0};

    std::ifstream in_fstream;
    in_fstream.open( databsae_path , std::ios::binary | std::ios::in );
    if( !in_fstream )
    {
        return ERROR;
    }
    else
    {
        while( !in_fstream.eof() )
        {

            in_fstream.read( (char*)&name_lenth , sizeof(unsigned int) );
            if(in_fstream.eof()) break;
            in_fstream.read( buffer, name_lenth );
            buffer[name_lenth] = 0;
            std::string name(buffer);

            in_fstream.read( (char*)&userid_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, userid_lenth);
            buffer[userid_lenth] = 0;
            std::string userid(buffer);

            in_fstream.read( (char*)&college_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer , college_lenth );
            buffer[college_lenth] = 0;
            std::string college(buffer);

            in_fstream.read( (char*)&password_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, password_lenth );
            buffer[password_lenth] = 0;
            std::string password(buffer);


            in_fstream.read( (char*)&temp.readerPermission , sizeof(temp.readerPermission) );
            temp.name = name;
            temp.userid = userid;
            temp.college = college;
            temp.password = password;
            //将temp中的值导入map容器中
            ReaderManager::reader_database.insert( std::map<std::string,ReaderInfo>::value_type( temp.userid , temp) );
        }
        in_fstream.close();
    }
    return SUCCESS;
}

// 从内存写入读者数据进入文件
// 失败原因：打开文件失败
Status ReaderManager::WriteInfoToDatabase()
{
    std::ofstream Info_ostream;
    Info_ostream.open( databsae_path , std::ios::binary | std::ios::out );
    ReaderInfo* info;
    if( !Info_ostream )
    {
        throw "打开文件失败";
    }
    else
    {
        for( std::map<std::string,ReaderInfo>::iterator iter = this->reader_database.begin() ; iter != this->reader_database.end() ; iter++)
        {
            info = new ReaderInfo;
            *info = iter->second;

            unsigned int name_lenth    = info->name.length();
            unsigned int userid_lenth  = info->userid.length();
            unsigned int college_lenth = info->college.length();
            unsigned int password_lenth= info->password.length();
            const char* name = info->name.c_str();
            const char* userid = info->userid.c_str();
            const char* college = info->college.c_str();
            const char* password = info->password.c_str();
            PERMISSION permission_ = info->readerPermission;

            Info_ostream.write( (char*)&name_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)name , name_lenth );
            Info_ostream.write( (char*)&userid_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)userid , userid_lenth );
            Info_ostream.write( (char*)&college_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)college , college_lenth);
            Info_ostream.write( (char*)&password_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)password , password_lenth);
            Info_ostream.write( (char*)&(permission_) , sizeof(PERMISSION) );

            delete info;
        }
        Info_ostream.close();
    }
    return SUCCESS;
}

// 添加用户
// 失败原因：读者已存在，无法添加
Status ReaderManager::AddReader(const ReaderInfo* info)
{
    std::pair<std::map<std::string,ReaderInfo>::iterator,bool> p_pair;
    p_pair = reader_database.insert( std::map<std::string,ReaderInfo>::value_type( info->userid , *info ) );
    if( p_pair.second )
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

// 删除用户
// 失败原因：读者不存在，无法删除
Status ReaderManager::RemoveReader(const std::string userid)
{
    std::map<std::string,ReaderInfo>::iterator iter;
    iter = reader_database.find( userid );
    if( iter !=reader_database.end() )
    {
        reader_database.erase( iter );
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }

}

// 导出所有读者
// 返回值：所有读者的数量
int ReaderManager::GetAllReadersInfo(std::function<ReaderCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<std::string,ReaderInfo>::iterator iter = this->reader_database.begin() ; iter != this->reader_database.end() ; iter++ )
    {
        ReaderInfo * info = new ReaderInfo;
        *info = iter->second;
        i++;
        callbackFunc(i,info);
        delete info;
    }
    return i;
}


// 根据读者编号查找读者信息(保证传入的info指针已分配了内存地址）
// 失败原因：读者不存在，无法定位
Status ReaderManager::SearchReadersByuserid(const std::string userid , ReaderInfo* info)
{
    std::map<std::string,ReaderInfo>::iterator iter;
    iter = reader_database.find( userid );
    if( iter !=reader_database.end() )
    {
        *info = iter->second;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

// 根据读者学院查找读者信息
// 返回值：所查找学院的读者数量
int ReaderManager::SearchReadersBycollege(const std::string college , std::function<ReaderCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<std::string,ReaderInfo>::iterator iter = this->reader_database.begin() ; iter != this->reader_database.end() ; iter++ )
    {
        ReaderInfo * info = new ReaderInfo;
        *info = iter->second;
        if( info->college == college )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

// 根据读者名字查找读者信息
// 返回值：所查找名字的读者数量
int ReaderManager::SearchReadersByname( const std::string name , std::function<ReaderCallbackFunc> callbackFunc )
{
    int i = 0;
    for( std::map<std::string,ReaderInfo>::iterator iter = this->reader_database.begin() ; iter != this->reader_database.end() ; iter++ )
    {
        ReaderInfo * info = new ReaderInfo;
        *info = iter->second;
        if( info->name == name )
        {
            i++;
            callbackFunc(i,info);
        }
        delete info;
    }
    return i;
}

// 根据读者编号和传入的读者信息修改读者信息
// 失败原因：找不到读者
Status ReaderManager::ModifyReadersInfo( const std::string userid , const ReaderInfo* info)
{
    std::map<std::string,ReaderInfo>::iterator iter;
    iter = reader_database.find( userid );
    if( iter !=reader_database.end() )
    {
        iter->second = *info;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

unsigned int ReaderManager::Size()
{
    return this->reader_database.size();
}
