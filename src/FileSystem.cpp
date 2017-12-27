#include<iostream>
#include<string.h>
#include<string>
#include<iomanip>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<iomanip>
#include"FileSystem.h"


#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

using namespace std;
int disk_empty = 10000 ; //Global variable, the size of virtual disk


string error[] = {"/","\\", ":","<",">","|","*","&"} ;  //Illegal character

FileSystem::FileSystem()
{
    size = 0;
    currentDir = NULL;
    copytempfile = NULL;
    copytempdir = NULL;
}

FileSystem::~FileSystem() {
    disk_empty += size;		//Release the mem that user occupy
    size = 0;

    MyDir *d = root;
    MyFile *f = currentDir->filePtr;
    while( f != 0 )
    {
        if( f->nextFile == 0 )
        {
            this->dele_file(f);
            f = 0;
            break;
        }
        while( f->nextFile->nextFile != 0 )
            f = f->nextFile ;
        this->dele_file(f->nextFile);
        f->nextFile = 0;
        f = currentDir->filePtr;
    }
    while (d != 0) {
        if (d->nextDir == 0) {
            this->dele_dir(d);
            d = 0;
            break;
        }
        while (d->nextDir->nextDir != 0)
            d = d->nextDir;
        this->dele_dir(d->nextDir);
        d->nextDir = 0;
        d = root;
    }
}

int FileSystem::newFile() {

    MyFile *p = NULL;
    p = new MyFile( this->getFileNumber() ) ;
    this->setFileNumber( this->getFileNumber() + 1 ) ;
    if( p == 0 )
    {
        cout << "CREATE           -FALSE";
        return 0;
    }
    cin>>p->name ;
    /*Check the naming rule*/
    string tempname(p->name) ;
    for(int i = 0 ;i< 8 ;++i)
    {
        if(tempname.find(error[i],0)!=string::npos)//Check whether there exists illegal char in the character stream
        {
            cout << "RENAME            -FALSE"<<endl;
            return 0 ;
        }
    }

    /* Following situation may apper, right after the registration:
     * 1. There exists NO file in the directory.
     * 2. There exist any file, but it violate the naming rule.
     * 3. There exist any file, and it obey the naming rule.
     * */
    
    /* Check whether there exist another function with same name */
    if( currentDir->filePtr == NULL )
    {
        p->nextFile = currentDir->filePtr;
        currentDir->filePtr = p;
    }
    else
    {
        MyFile *q = currentDir->filePtr;
        while( q != NULL )
        {
            if( strcmp(p->name.c_str(), q->name.c_str())==0 )
            {
                cout << "FILE EXISTS             -FALSE" << endl;
                return 0;
            }
            q = q->nextFile;
        }

        /*Rebuild the Linked List*/
        p->nextFile = currentDir->filePtr;
        currentDir->filePtr = p;
        p->preDir = currentDir ;
        /*put the file into vector*/
        this->getFileVector().push_back( p ) ;

        /*Resize the size of upper layer direcotory*/
        MyDir *h = currentDir;
        while (h != NULL)
        {
            h->size += p->size;
            h = h->preDir;
        }
       
    }
    this->currentDir->filePtr->size = 0;
    cout <<"CREATE             -OK" << endl;
    disk_empty = disk_empty - p->size;
    this->size += p->size;
    return 1;
}

int FileSystem::newDir( )
{
    MyDir *p, *h;
    p = new MyDir( NULL, NULL, NULL, NULL,0 );
    cin >> p->name;

    /*Check naming rule*/
    string tempname(p->name) ;
    for( int i = 0 ;i< 8 ;++i )
    {
        if( tempname.find(error[i],0)!=string::npos )
        {
            cout << "RENAME              -FALSE"<<endl;
            return 0 ;
        }
    }
    if( this->currentDir->dirPtr == NULL)
        h = NULL;
    else
        h = this->currentDir->dirPtr;//First dir on next layer

    /* Following situation may apper, right after the registration:
     * 1. There exists NO file in the directory.
     * 2. There exist any file, but it violate the naming rule.
     * 3. There exist any file, and it obey the naming rule.
     * */
    
    /*Check naming rule*/
    //Iterate dirs on next years
    //Check whether the dir has been existing
    while( h != NULL )
    {
        if( strcmp(h->name.c_str(), p->name.c_str()) == 0 )
        {
            cout << "DIR EXISTS           -FALSE" << endl;
            return 0;
        }
        h = h->nextDir;
    }

    /*Rebuild the Linked List*/
    p->preDir = this->currentDir;
    p->nextDir = this->currentDir->dirPtr;
    this->currentDir->dirPtr = p;

    cout << "CREATE                -OK" << endl;
    return 1;
}
int FileSystem::dele_file(MyFile *f) {
    delete f;
    f = NULL;
    return 1;
}

int FileSystem::deleteFile() {
    char temp[MAX_NAME];

    cin >> temp;
    MyFile *f;
    MyFile *above = NULL;
    f = currentDir->filePtr;

    /*
     * Check whether there exist the files that need to remove.
     * */

    while( f != NULL )
    {
        if( !strcmp(f->name.c_str(), temp) )
            break;
        above = f;
        f = f->nextFile;
    }
    if( f == NULL )
    {
        cout << "NO FILE              -FALSE" << endl;
        return 0;
    }
    disk_empty += f->size;
    MyDir *d = currentDir;
    while( d != 0 ) //The sizes of individual directory, after removing the file
    {
        d->size -= f->size;
        d = d->preDir;
    }

    /* The considerations while removing the file.
     * 1. The file, which needs to be deleted, is the header point (node) of the linked list of directories
     * 2. The file, which needs to be deleted, is the middle point (node) of the linked list of directories
     * */

    if( f == currentDir->filePtr )//Remove the point (node) of the file
        currentDir->filePtr = currentDir->filePtr->nextFile;
    else
        above->nextFile = f->nextFile;
    size -= f->size;
    delete f;
    f = NULL;
    cout << "DELETE             -OK" << endl;
    return 1;
}
int FileSystem::dele_dir(MyDir *d) {
    delete d;
    d = NULL;
    return 1;
}

int FileSystem::deleteDir() {
    char n[MAX_NAME];

    MyDir *p, *pre = NULL;
    p = root;
    p = currentDir->dirPtr;
    cin >> n; //The filename that needs to be deleted

    /*Search for the directory, that need to delete*/
    while( p != NULL )
    {
        if( strcmp(p->name.c_str(), n) == 0 )
            { pre = p; break; }
        p = p->nextDir;
    }

    if( p == NULL )
    {
        cout << "DELETE            -FALSE" << endl;
        return 0;
    }

    /* Consideraions while deleting the directory
     * 1. The location of the deleted dir in the linked list of root dir
     * 2. Other files/directories in the currrent directory
     * */
    disk_empty += p->size;
    if (p == currentDir->dirPtr)
        currentDir->dirPtr = currentDir->dirPtr->nextDir;
    else
        p->preDir->nextDir = p->nextDir;

    pre = currentDir;
    while (pre != NULL)//Resize the size of individual directory, after the deletion of dir
    {
        pre->size -= p->size;
        pre = pre->preDir;
    }
    size -= p->size;
    MyDir *d = p->dirPtr;
    MyFile *f = p->filePtr;
    if( f != 0 )
    {
        while( p->filePtr->nextFile != NULL )//Delete the files under this dir
        {
            f = p->filePtr;
            while (f->nextFile->nextFile != NULL)//Search the last(end) file node
                f = f->nextFile;
            this->dele_file(f->nextFile);
            f->nextFile = NULL;
        }
        if( p->filePtr->nextFile == NULL )
        {
            this->dele_file(p->filePtr);
            p->filePtr = NULL;
        }
    }
    if( d != NULL )
    {
        while( p->dirPtr->nextDir != NULL )//Delete the dirs under this dir
        {
            d = p->dirPtr;
            while (d->nextDir->nextDir != NULL)//Search the last(end) dir node
                d = d->nextDir;
            this->dele_dir(d->nextDir);//Recursion
            d->nextDir = NULL;
        }
        if (p->dirPtr->nextDir == NULL) {
            this->dele_dir(p->dirPtr);
            p->dirPtr = NULL;
        }
    }
    delete p,d,f;

    cout << "DELETE           -OK" << endl;
    return 1;

}
//****************************************************************************************
//* Fun Name:readDir                                                                     *
//* Work: cd (change directory)                                                          *
//* Where to be called: fsOperate()                                                      *
//* Editor: WEI-CHENG, JHUANG                                                            *
//* **************************************************************************************
int FileSystem::readDir (int number, char** possible_file_path) {
    char name[MAX_NAME];
    char **split_path = 0;
    int path_count = 0;
    int loop_count = 0;
    bool change_dir = false;
    bool end_dir = false;
    MyDir *p;
    
    split_path = (char**) malloc (sizeof (char*) * MAX_NAME);
    for (int i = 0; i < MAX_NAME; i++) {
        split_path [i] = (char*) malloc (sizeof (char) * MAX_NAME);
    }
    for (int i = 0; i < MAX_NAME; i++) {
        for (int j = 0; j < MAX_NAME;j++) {
            split_path [i][j] = 0;
        }
    }
    
    p = currentDir->dirPtr;
    
    if (strchr (possible_file_path [number], '~') != NULL) {
        p = currentDir;
        while (1) {
            if (p->preDir == NULL) {
                break;
            }
            p = p->preDir;
        }
        for (int i = 0; possible_file_path [number][i] != 0; i++) {
            possible_file_path [number][i] = possible_file_path [number][i + 2];
        }
    }
    path_count = splitPath (possible_file_path [number], split_path);
    if ((split_path [0][0] == 'r' && split_path [0][1] == 0) && path_count == 1) {
        p = currentDir;
        while (1) {
            if (p->preDir == NULL) {
                break;
            }
            p = p->preDir;
        }
        loop_count = path_count;
        change_dir = true;
    }
    while (loop_count < path_count) {
        if (dirExist (p, split_path [loop_count])) {
            change_dir = true;
            while (p != NULL) {
                if (strcmp(p->name.c_str(), split_path [loop_count]) == 0) {
                    if (p->dirPtr != NULL) {
                        if (loop_count + 1 != path_count) {
                            p = p->dirPtr;
                        }
                    }
                    break;
                }
                p = p->nextDir;
            }
        }
        else {
            change_dir = false;
            break;
        }
        loop_count++;
    }
    
    if (end_dir && path_count != 1) {
        while (p != NULL) {
            if (strcmp(p->name.c_str(), split_path [loop_count - 1]) == 0) {
                break;
            }
            p = p->nextDir;
        }
    }
    for (int i = 0; i < path_count; i++) {
        if (path_count != 1) {
            free (split_path [i]);
        }
    }
    
    if (path_count != 1) {
        free (split_path);
    }
    
    if (change_dir) {
        currentDir = p;
        return 1;
    }
    else {
        cout << "NO DIR             -FALSE" << endl;
    }
    
    return 0;
}
//****************************************************************************************
//* Fun Name:dirExist()                                                                  *
//* Work: Check whether the dir exist                                                    *
//* Where to be called: readDir()                                                        *
//* Editor: WEI-CHENG, JHUANG                                                            *
//* **************************************************************************************
int FileSystem::dirExist (MyDir *p, char* path) {
    while (p != NULL) {
        if (strcmp(p->name.c_str(), path) == 0) {
            return 1;
        }
        p = p->nextDir;
    }
    return 0;
}
//****************************************************************************************
//* Fun Name:dirExist()                                                                  *
//* Work: split the string of path                                                       *
//* Where to be called: readDir()                                                        *
//* Editor: WEI-CHENG, JHUANG                                                            *
//* **************************************************************************************
int FileSystem::splitPath (char* path, char** split_path) {
    int path_count = 0;
    int str_count = 0;
    if (strchr (path, '/') == NULL) {
        for (int i = 0; path [i] != 0; i++) {
            split_path [path_count][i] = path [i];
        }
        path_count++;
    }
    else {
        for (int i = 0; path [i] != 0; i++) {
            if (path [i] == '/') {
                split_path [path_count][str_count] = 0;
                path_count++;
                str_count = 0;
            }
            else {
                split_path [path_count][str_count++] = path [i];
            }
        }
        path_count++;
    }
    return path_count;
}

int FileSystem::readFile() {
    char n[MAX_NAME];
    cin >> n;
    MyFile *f = currentDir->filePtr;
    while (f != 0)
    {
        if( strcmp(f->name.c_str(), n)==0)
        {
            cout << f->content << endl;
            f->count = f->count + 1 ;
            return 1;
        }
        f = f->nextFile;
    }
    cout << "NO FILE              -FALSE" << endl;
    return 0;
}

int FileSystem::renameDir() {
    MyDir *h;
    char n1[MAX_NAME], n2[MAX_NAME];
    cin >> n1;
    cin >> n2;
    h = currentDir->dirPtr;
    while (h != NULL) {
        if (strcmp(h->name.c_str(), n2)==0) {
            cout << "DIR EXIST        -FALSE" << endl;
            return 0;
        }
        h = h->nextDir;
    }
    h = currentDir->dirPtr;
    while (h != NULL) {
        if (strcmp(h->name.c_str(), n1)==0)
            break;
        h = h->nextDir;
    }
    if (h == NULL) {
        cout << "NO DIR         -FALSE" << endl;
        return 0;
    }
    //strcpy(h->name, n2);
    h->name = n2 ;
    cout << "RENAME            -OK" << endl;
    return 1;
}

int FileSystem::renameFile() {
    MyFile *h;
    char n1[MAX_NAME], n2[MAX_NAME];
    cin >> n1;
    cin >> n2;
    h = currentDir->filePtr;
    while (h != NULL) {
        if (!strcmp(h->name.c_str(), n2)) {
            cout << "FILE EXISTS           -FALSE" << endl;
            return 0;
        }
        h = h->nextFile;
    }
    h = currentDir->filePtr;
    while (h != NULL) {
        if (!strcmp(h->name.c_str(), n1))
            break;
        h = h->nextFile;
    }
    if (h == NULL) {
        cout << "NO FILE            -FALSE" << endl;
        return 0;
    }
    h->name = n2 ;
    cout << "RENAME             -OK" << endl;
    return 1;
}

MyDir *FileSystem::copy_dir(MyDir *d) {
    MyFile *fh;
    MyDir *dh, *dir;
    dir = new MyDir( NULL, NULL, NULL, NULL, 0 );
    fh = d->filePtr;
    dh = d->dirPtr;

    while (fh != NULL) {
        MyFile *f;
        f = copy_file(fh);
        f->nextFile = dir->filePtr;
        dir->filePtr = f;
        fh = fh->nextFile;
    }

    while (dh != NULL) {
        MyDir *d1, *d2;
        d1 = copy_dir(dh);
        d1->preDir = dir;
        dir->dirPtr = d1;
        d2 = dh->nextDir;
        while (d2 != NULL) {
            d1->nextDir = copy_dir(d2);

            d2 = d2->nextDir;
            d1 = d1->nextDir;
            d1->preDir = dir;
        }

        dh = dh->dirPtr;
    }

    //strcpy(dir->name, d->name);
    dir->name = d->name ;
    dir->size = d->size;
    return dir;
}

MyDir *FileSystem::copyDir() {
    MyDir *h;
    char n[MAX_NAME];
    cin >> n;
    h = currentDir->dirPtr;
    while (h != NULL) {
        if (!strcmp(h->name.c_str(), n))
            break;
        h = h->nextDir;
    }
    if (h == NULL) {
        cout << "NO DIR            -FALSE" << endl;
        return 0;
    }
    copytempdir = copy_dir(h);
    cout << "COPY           -OK" << endl;
    return copytempdir;
}

MyFile *FileSystem::copy_file( MyFile *h )
{
    MyFile *f;
    f = new MyFile( this->getFileNumber() + 1  );
    this->setFileNumber( this->getFileNumber() + 1 ) ;
    
    f->size = h->size;
    f->name = h->name ;
    f->content = h->content;
    return f;
}

MyFile *FileSystem::copyFile() {
    MyFile *h;
    char n[MAX_NAME];
    cin >> n;
    h = currentDir->filePtr;

    while (h != NULL) {
        if (!strcmp(h->name.c_str(), n))
            break;
        h = h->nextFile;
    }
    if (h == NULL) {
        cout << "NO FILE             -FALSE" << endl;
        return NULL;
    }
    copytempfile = copy_file(h);
    cout << "COPY             -OK" << endl;
    return copytempfile;
}

int FileSystem::pasteDir() {
    MyDir *h, *d1;
    h = currentDir->dirPtr;

    if (copytempdir == NULL) {
        cout << "NO SOURCE            -FALSE" << endl;
        return 0;
    }
    if (h == NULL) {
        if (disk_empty < copytempdir->size) {
            cout << "NO ENOUGH SPACE             -FALSE";
            return 0;
        }
        disk_empty -= copytempdir->size;
        d1 = copy_dir(copytempdir);
        currentDir->dirPtr = d1;
        d1->preDir = currentDir;
        currentDir->size += copytempdir->size;

    } else {
        while (h != NULL) {
            if (!strcmp(h->name.c_str(), copytempdir->name.c_str()))
                break;
            h = h->nextDir;
        }
        if (h != NULL) {
            cout << "DIR EXISTS          -FALSE" << endl;
            return 0;
        } else {
            if (disk_empty < copytempdir->size) {
                cout << "NO ENOUGH SPACE            -FALSE";
                return 0;
            }
            d1 = copy_dir(copytempdir);
            d1->nextDir = currentDir->dirPtr;
            currentDir->dirPtr = d1;
            d1->preDir = currentDir;
            currentDir->size += copytempdir->size;
        }
    }
    cout << "PASTE              -OK" << endl;
    return 1;
}

int FileSystem::pasteFile() {
    MyFile *h = currentDir->filePtr;
	MyFile *pTemp = h;
    if (copytempfile == NULL) {
        cout << "NO SOURCE              -FALSE" << endl;
        return 0;
    }
    if (h == NULL) {
        if (disk_empty < copytempfile->size) {
            cout << "NO ENOUGH SPACE            -FALSE";
            return 0;
        }
        currentDir->filePtr = copy_file(copytempfile);
        currentDir->size += copytempfile->size;
    }
	else {
        while (h != NULL) {
            if (!strcmp(h->name.c_str(), copytempfile->name.c_str())) {
                cout << "FILE EXISTS                -FALSE" << endl;
                return 0;
            }
            //break;
            h = h->nextFile;
        }

        if (disk_empty < copytempfile->size) {
            cout << "NO ENOUGH SPACE              -FALSE";
            return 0;
        }
        currentDir->filePtr = copy_file(copytempfile);
		currentDir->filePtr->nextFile = pTemp;
        //currentDir->filePtr->nextFile = h;
        currentDir->size += copytempfile->size;
		 cout << "PASTE                -OK" << endl;
    return 1;
    }
return 0;
   
}

int FileSystem::edit() {
    string filename ;
    string content  ;
    cin >> filename ;
    MyFile *f = currentDir->filePtr;
    while( f != 0 )
    {
        if( f->name == filename  )
        {
            cin >> content ;//The content of the file.
            f->content = content ;
            f->count = f->count + 1 ;
            f->size = (int)(content.length());
            disk_empty -= f->size;
            MyDir *d = currentDir;
            while( d != 0 )//Resize the size of dirs of each layer
            {
                d->size += f->size;
                d = d->preDir;
            }
            cout << "EDIT              -OK" << endl;
            size += f->size;
            return 1;
        }
        f = f->nextFile;
    }

    cout << "NO FILE                -FALSE" << endl;
    return 0;
}

int FileSystem::show_path(MyDir *d)//show the current path
{
    if( d->preDir == NULL )
        cout <<  CYAN << root->name;
    if( d->preDir != NULL ){
        this->show_path(d->preDir);//recursion
        cout <<  CYAN << d->name;
    }
    cout << "/" << RESET ;
    return 1;
}
int FileSystem::showCurrentDir()
{
    MyDir *d = currentDir->dirPtr;
    MyFile *f = currentDir->filePtr;
    if (d == NULL && f == NULL) {
        cout << "EMPTY" << endl;
        return 0;
    }
    cout << "CONTENT:" << endl;

    if (d != NULL) {
        cout << "\t" << "D_NAME" << "\t" << "D_SIZE" << endl;
        while (d != NULL) {
            cout << "\t" << d->name << "\t" << d->size << endl;
            d = d->nextDir;
        }
        cout << endl;
    }

    if (f != NULL) {
        cout << "\t" << "F_NAME" << "\t" << "F_SIZE" << "\t" << endl;
        while (f != NULL) {
            cout << "\t" << f->name << "\t" << f->size << "\t" << endl;
            f = f->nextFile;
        }
    }
    return 1;
}

int FileSystem::showPath()//Show the current path
{
    show_path( this->currentDir );
    return 1;
}
int FileSystem::goback() {
    if (currentDir->preDir == NULL) {
        cout << "ROOT DIR          -FALSE" << endl;
        return 0;
    }
    currentDir = currentDir->preDir;
    return 1;
}

int FileSystem::setUser( string user, string passwd )
{
    strcpy( this->name, user.c_str() );
    strcpy( this->password, passwd.c_str());

    MyDir *itr = this->root->dirPtr ;
    
    while( itr != NULL )
    {
        if( itr->name == user )
        {
            this->currentDir = itr ;
        }
        itr = itr->nextDir ;
    }
    return 1;
}

int FileSystem::getSize() {
    return size;
}

MyDir *FileSystem::getCurrentdir() {
    return currentDir;
}

//****************************************************************************************
//* Fun Name: FindFilePath()                                                             *
//* Work: split the string name of filepath                                              *
//* Where to be called: fsoperate()                                                      *
//* Editor: Zong Bin, Chen                                                               *
//****************************************************************************************
const char* FindFilePath ( const char* str, char* path) { //parse file path
    string str11(str);
    int idx=0;
    int res=0;
    while(str11[idx]!=NULL){
        if(str11[idx]=='/')
            res=idx;
        idx++;
    }
    if(res==0)
        return 0;
    
    return str11.substr(0,res).c_str(); //filepath
}
//****************************************************************************************
//* Fun Name: FindFileName()                                                             *
//* Work: split the string name of filepath                                              *
//* Where to be called: fsoperate()                                                      *
//* Editor: Zong Bin, Chen                                                               *
//****************************************************************************************
const char* FindFileName ( const char* str, char* file_name) { //parse file name
    string str11(str);
    int idx=0;
    int res=0;
    while(str11[idx]!=NULL){
        if(str11[idx]=='/')
            res=idx;
        idx++;
    }
    if(res==0)
        return 0;
    
    return str11.substr(res+1,str11.length()).c_str(); //filename
}
//****************************************************************************************
//* Fun Name: find()                                                                     *
//* Work: find the specefied files in the whole system                                   *
//* Where to be called: fsoperate()                                                      *
//* Editor: CHENG SHIANG, SHIU                                                           *
//****************************************************************************************
void FileSystem::find(char* fileName)
{
    int numberOfDirectory = 0, shortcutChoice ;
    
    MyDir  *currentProcessedDir  = root ;//set initial MyDir to root
    MyFile *currentProcessedFile = root->filePtr ;//set initial MyFile to the first file of directory root
    vector<MyDir*> nextDirVector, matchingDirVector;
    
    //continue compare file in directory if currentProcessedDir isn't NULL
    while (currentProcessedDir != NULL) {
        //enqueue dirPtr to vector if not NULL
        if (currentProcessedDir->dirPtr != NULL)
            nextDirVector.push_back(currentProcessedDir->dirPtr);
        //process directory for files that matches
        while (currentProcessedFile != NULL) {
            //if file matching is found, record and show the current processed directory and break the while loop
            if (strcmp(currentProcessedFile->name.c_str(), fileName) == 0) {
                cout << "(" << ++numberOfDirectory << ") found in directory: " << currentProcessedDir->name << endl;
                matchingDirVector.push_back(currentProcessedDir);
                break;
            }
            //point to next MyFile if exists and no file matching is found
            currentProcessedFile = currentProcessedFile->nextFile;
        }
        //point to nextDir if exists
        
        if (currentProcessedDir->nextDir != NULL) {
            currentProcessedDir = currentProcessedDir->nextDir;
            currentProcessedFile = currentProcessedDir->filePtr;
        }
        //if nextDir is NULL, dequeue nextDirVector to process child directory of some directory if exists
        else if (nextDirVector.size() != 0) {
            
            currentProcessedDir = nextDirVector[0];
            nextDirVector.erase(nextDirVector.begin());
            currentProcessedFile = currentProcessedDir->filePtr;
        }
        //set currentProcessedDir to NULL if both conditions previously are false
        else
            currentProcessedDir = NULL;
    }
    //neglect the string previously inputed
    //cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //if nothing is found, show the error message
    
    if (matchingDirVector.size() == 0)
        cout << "!!! no corresponding file is found !!!" << endl;
    //if at least one file is found, ask for choice of shortcut
    else {
        //loop until the integer with correct range is inputed
        while (true) {
            cout << "please input choice for directory shortcut (1 ~ " << numberOfDirectory << "), input (0) for cancel: ";
            cin >> shortcutChoice;
            //if "0" is entered, simply terminate current function
            
            if (shortcutChoice == 0)
                return;
            //the correct range is inputed, set the currentDir to chosen shortcut before terminate
            else if (1 <= shortcutChoice && shortcutChoice <= numberOfDirectory) {
                currentDir = matchingDirVector[shortcutChoice - 1];
                return;
            }
            //integer with incorrect range is inputed
            else
                cout << "!!! input error, the integer inputed isn't in correct range !!!" << endl;
        }
    }
}



