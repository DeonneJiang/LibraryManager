#include "bookman.h"
#include <fstream>

// 构造函数
BookManager::BookManager()
{
    this->GetInfoFromDatabase();
}

// 析构函数
BookManager::~BookManager()
{
    this->WriteInfoToDatabase();
}

// 从文件读取图书数据进入内存
// 失败原因：打开文件失败
Status BookManager::GetInfoFromDatabase()
{
    //将文件中的一个记录导入temp
    BookInfo temp;
    unsigned int title_lenth = 0;
    unsigned int author_lenth = 0;
    unsigned int press_lenth = 0;
    unsigned int ISBN_lenth = 0;
    unsigned int coverPath_lenth = 0;
    unsigned int classification_lenth = 0;
    unsigned int bookid_lenth = 0;

    char buffer[256] = {0};

    std::ifstream in_fstream;
    in_fstream.open( database_path , std::ios::binary | std::ios::in );
    if( !in_fstream )
    {
        return ERROR;
    }
    else
    {
        while( !in_fstream.eof() )
        {

            in_fstream.read( (char*)&title_lenth , sizeof(unsigned int) );
            if(in_fstream.eof()) break;//（特殊）判断空文件
            in_fstream.read( buffer, title_lenth );
            buffer[title_lenth] = 0;
            std::string title(buffer);

            in_fstream.read( (char*)&author_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, author_lenth);
            buffer[author_lenth] = 0;
            std::string author(buffer);

            in_fstream.read( (char*)&press_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer , press_lenth );
            buffer[press_lenth] = 0;
            std::string press(buffer);

            in_fstream.read( (char*)&ISBN_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, ISBN_lenth );
            buffer[ISBN_lenth] = 0;
            std::string ISBN(buffer);

            in_fstream.read( (char*)&temp.purchaseTime , sizeof(__time64_t) );

            in_fstream.read( (char*)&coverPath_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, coverPath_lenth );
            buffer[coverPath_lenth] = 0;
            std::string coverPath(buffer);

            in_fstream.read( (char*)&classification_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, classification_lenth );
            buffer[classification_lenth] = 0;
            std::string classification(buffer);

            in_fstream.read( (char*)&bookid_lenth , sizeof(unsigned int) );
            in_fstream.read( buffer, bookid_lenth );
            buffer[bookid_lenth] = 0;
            std::string bookid(buffer);


            in_fstream.read( (char*)&temp.permissionRequested , sizeof(temp.permissionRequested) );
            in_fstream.read( (char*)&temp.bookQuantity , sizeof(temp.bookQuantity) );
            in_fstream.read( (char*)&temp.bookBorrowedOut , sizeof(temp.bookBorrowedOut) );
            in_fstream.read( (char*)&temp.price , sizeof(temp.price) );

            temp.title = title;
            temp.author = author;
            temp.press = press;
            temp.ISBN = ISBN;
            temp.coverPath = coverPath;
            temp.classification = classification;
            temp.bookid = bookid;

            //将temp中的值导入map容器中
            BookManager::book_database.insert( std::map<std::string,BookInfo>::value_type( temp.bookid , temp) );
        }
        in_fstream.close();
    }
    return SUCCESS;
}

// 从内存写入图书数据进入文件
// 失败原因：打开文件失败
Status BookManager::WriteInfoToDatabase()
{
    std::ofstream Info_ostream;
    Info_ostream.open( database_path , std::ios::binary | std::ios::out );
    BookInfo* info;
    if( !Info_ostream )
    {
        throw "打开文件失败";
    }
    else
    {
        for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++)
        {
            info = new BookInfo;
            *info = iter->second;

            unsigned int title_lenth = info->title.length();
            unsigned int author_lenth = info->author.length();
            unsigned int press_lenth =info->press.length();
            unsigned int ISBN_lenth = info->ISBN.length();
            unsigned int coverPath_lenth = info->coverPath.length();
            unsigned int classification_lenth = info->classification.length();
            unsigned int bookid_lenth = info->bookid.length();

            const char* title = info->title.c_str();
            const char* author = info->author.c_str();
            const char* press = info->press.c_str();
            const char* ISBN = info->ISBN.c_str();
            __time64_t purchaseTime = info->purchaseTime;
            const char* coverPath = info->coverPath.c_str();
            const char* classification = info->classification.c_str();
            const char* bookid = info->bookid.c_str();
            PERMISSION permissionRequested_ = info->permissionRequested;
            int bookQuantity_ = info->bookQuantity;
            int bookBorrowedOut_ = info->bookBorrowedOut;
            float price_ = info->price;

            Info_ostream.write( (char*)&title_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)title , title_lenth );

            Info_ostream.write( (char*)&author_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)author , author_lenth );


            Info_ostream.write( (char*)&press_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)press , press_lenth);


            Info_ostream.write( (char*)&ISBN_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)ISBN , ISBN_lenth);

//            Info_ostream.write( (char*)&purchaseTime_lenth , sizeof(unsigned int) );
//            Info_ostream.write( (char*)purchaseTime , purchaseTime_lenth);
            Info_ostream.write( (char*)&(purchaseTime) , sizeof(__time64_t) );


            Info_ostream.write( (char*)&coverPath_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)coverPath , coverPath_lenth);

            Info_ostream.write( (char*)&classification_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)classification , classification_lenth);

            Info_ostream.write( (char*)&bookid_lenth , sizeof(unsigned int) );
            Info_ostream.write( (char*)bookid , bookid_lenth);

            Info_ostream.write( (char*)&(permissionRequested_) , sizeof(PERMISSION) );
            Info_ostream.write( (char*)&(bookQuantity_) , sizeof(int) );
            Info_ostream.write( (char*)&(bookBorrowedOut_) , sizeof(int) );
            Info_ostream.write( (char*)&(price_) , sizeof(float) );

            delete info;
        }
        Info_ostream.close();
    }
    return SUCCESS;
}

// 图书购入
//
Status BookManager::AddBook( BookInfo* bookinfo)
{
    int i = 0;
    for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++ )
    {
        BookInfo * info = new BookInfo;
        *info = iter->second;
        if( info->ISBN == bookinfo->ISBN )
        {
            i++;
        }
    }
    i++;

    std::string s = std::to_string(i);
    bookinfo->bookid = bookinfo->ISBN;
    bookinfo->bookid += s;

    bookinfo->bookBorrowedOut = 0;

    std::pair<std::map<std::string,BookInfo>::iterator,bool> p_pair;
    p_pair = book_database.insert( std::map<std::string,BookInfo>::value_type( bookinfo->bookid , *bookinfo ) );
    if( p_pair.second )
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }

}

// 图书删除
// 失败原因：图书不存在，无法删除
Status BookManager::RemoveBook(const std::string bookid)
{
    std::map<std::string,BookInfo>::iterator iter;
    iter = book_database.find( bookid );
    if( iter !=book_database.end() )
    {
        book_database.erase( iter );
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }

}

//根据编号获取图书信息
Status BookManager::GetBookInfo(const std::string bookid, BookInfo* outinfo)
{
    std::map<std::string,BookInfo>::iterator iter;
    iter = book_database.find( bookid );
    if( iter !=book_database.end() )
    {
        *outinfo = iter->second;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

// 导出所有图书
// 返回值：所有图书的数量
int BookManager::GetAllBooksInfo(std::function<BookCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++ )
    {
        BookInfo * info = new BookInfo;
        *info = iter->second;
        i++;
        callbackFunc(i,info);
        delete info;
    }
    return i;
}

// 根据图书编号和传入的图书信息修改图书信息
// 失败原因：找不到图书
Status BookManager::ModifyBook(const std::string bookid , const BookInfo* info)
{
    std::map<std::string,BookInfo>::iterator iter;
    iter = book_database.find( bookid );
    if( iter !=book_database.end() )
    {
        iter->second = *info;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

// 根据书名搜素图书
// 返回值：所查找书名的图书数量
int BookManager::SearchBooksByName(const std::string keywords , std::function<BookCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++ )
    {
        BookInfo * info = new BookInfo;
        *info = iter->second;
        if( info->title.find(keywords,0) != std::string::npos )
        {
                i++;
                callbackFunc(i,info);
        }

    }
    return i;
}

// 根据作者搜素图书
// 返回值：所查找作者的图书数量
int BookManager::SearchBooksByAuthor(const std::string keywords , std::function<BookCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++ )
    {
        BookInfo * info = new BookInfo;
        *info = iter->second;
        if( info->author.find(keywords,0) != std::string::npos )
        {
                i++;
                callbackFunc(i,info);
        }

    }
    return i;
}

// 根据出版社搜素图书
// 返回值：所查找出版社的图书数量
int BookManager::SearchBooksByPress(const std::string keywords , std::function<BookCallbackFunc> callbackFunc)
{
    int i = 0;
    for( std::map<std::string,BookInfo>::iterator iter = this->book_database.begin() ; iter != this->book_database.end() ; iter++ )
    {
        BookInfo * info = new BookInfo;
        *info = iter->second;
        if( info->press.find(keywords,0) != std::string::npos )
        {
                i++;
                callbackFunc(i,info);
        }

    }
    return i;
}


unsigned int BookManager::Size()
{

    return this->book_database.size();

}
