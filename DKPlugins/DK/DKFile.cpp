#include "stdafx.h"
#include "DKFile.h"
#include "DKLog.h"
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sys/stat.h>

DKString DKFile::appfilename;
DKString DKFile::local_assets;
DKString DKFile::online_assets;


/////////////////////////////////////////////
bool DKFile::PathExists(const DKString& path)
{
	//DKLog("DKFile::Exists("+path+")\n", DKDEBUG);
	if(boost::filesystem::exists(path)){
		return true;
	}

	//DKLog("Path does not exist: "+path+" \n", DKDEBUG);
	return false;
}

///////////////////////////////////////
bool DKFile::VerifyPath(DKString& path)
{
	if(DKFile::PathExists(path)){
		return true;
	}
	if(DKFile::PathExists(DKFile::local_assets+path)){ 
		path = DKFile::local_assets+path;
		return true;
	}

	DKLog("DKFile::VerifyPath("+path+") Could not find path. \n", DKERROR);
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::Rename(const DKString& input, const DKString& output, const bool overwrite)
{
	//DKLog("DKFile::Rename("+input+","++","+toString(overwrite)+")\n", DKDEBUG);
	if(!PathExists(input)){ return false; }

	if(PathExists(output)){
		if(overwrite == false){
			DKLog("DKFile::Rename() failed! Path already exists \n", DKERROR);
			return false;
		}
		else{
			if(!DKFile::Delete(output)){
				DKLog("DKFile::Rename() failed! Could not delete "+output+" \n", DKERROR);
				return false;
			}
		}
	}

	if(rename(input.c_str(), output.c_str()) != 0){
		DKLog("DKFile::Rename() failed to rename "+input+" \n", DKERROR);
		return false;
	}

	DKLog("Renamed "+input+" to-> "+output+"\n", DKINFO);
	return true;
}

/////////////////////////////////////////
bool DKFile::Delete(const DKString& path)
{
	//DKLog("DKFile::Delete("+path+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }
	if(DKFile::IsDirectory(path)){
		DKFile::RemoveDirectory(path); //first delete all contents
	}
	else{
		if(remove(path.c_str()) != 0){
			DKLog("DKFile::Delete() failed! "+path+" \n", DKERROR);
			return false;
		}
	}
	
	DKLog("Deleted path "+path+"\n", DKWARN);
	return true;
}

//////////////////////////////////////////////////
bool DKFile::RemoveDirectory(const DKString& path)
{
	//DKLog("DKFile::RemoveDirectory("+path+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }
	if(!boost::filesystem::remove_all(path.c_str())){
		DKLog("boost::filesystem::remove_all failed\n",DKERROR);
		return false;
	}
	return true;
}

/////////////////////////////////////////
bool DKFile::MakeDir(const DKString& dir)
{
	//DKLog("DKFile::MakeDir("+dir+")\n", DKDEBUG);
    if(dir.empty()){ return false; }
	if(PathExists(dir)){ return true; }
	if(!boost::filesystem::create_directory(dir)){
		DKLog("DKFile::MakeDir() failed! \n", DKERROR);
		return false;
	}
	return true;
}

///////////////////////////////////////
bool DKFile::ChDir(const DKString& dir)
{
	//DKLog("DKFile::ChDir("+dir+")\n", DKDEBUG);
	if(!PathExists(dir)){ return false; }
	boost::filesystem::current_path(dir);
	DKLog("Path set to "+dir+"\n", DKDEBUG);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::Copy(const DKString& src, const DKString& dst, const bool overwrite, const bool recursive)
{
	//DKLog("DKFile::Copy("+src+","+dst+","+toString(overwrite)+","+toString(recursive)+")\n", DKDEBUG);
	if(!PathExists(src)){ return false; }
	if(boost::filesystem::is_directory(src)){
		if(!CopyDirectory(src,dst,overwrite,recursive)){
			return false;
		}
	}
	else{
		if(PathExists(dst) && overwrite == false){
			DKLog("DKFile::Copy(): destination already exists. \n", DKWARN);
			return false;
		}
		boost::filesystem::copy_file(src,dst,boost::filesystem::copy_option::overwrite_if_exists);
	}

	DKLog("Copied to "+dst+"\n", DKINFO);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::CopyFolder(const DKString& src, const DKString& dst, const bool overwrite, const bool recursive)
{
	//DKLog("DKFile::CopyFolder("+src+","+dst+","+toString(overwrite)+","+toString(recursive)+")\n", DKDEBUG);
	if(!PathExists(src)){ return false; }
	if(!CopyDirectory(src,dst,overwrite,recursive)){
		DKLog("CopyFolder()\n", DKERROR);
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::CopyDirectory(boost::filesystem::path const& source, boost::filesystem::path const& destination, const bool overwrite, const bool recursive)
{
	//DKLog("DKFile::CopyDirectory(source,destination,"+toString(overwrite)+","+toString(recursive)+")\n", DKDEBUG);
	namespace fs = boost::filesystem;
    try{
		// Check whether the function call is valid
        if( !fs::exists(source) || !fs::is_directory(source) ){
            DKLog("Source directory "+source.string()+" does not exist or is not a directory.\n", DKERROR);
            return false;
        }
		if(!fs::exists(destination)){ 
			// Create the destination directory
			if(!fs::create_directory(destination)){
				DKLog("Unable to create destination directory "+destination.string()+"\n", DKERROR);
				return false;
			}
        }
    }
    catch(fs::filesystem_error const & e){
        std::cerr << e.what() << '\n';
        return false;
    }
    // Iterate through the source directory
    for( fs::directory_iterator file(source); file != fs::directory_iterator(); ++file ){
		try{
			fs::path current(file->path());
            if(fs::is_directory(current) && recursive){
				// Found directory: Recursion
                if( !CopyDirectory( current, destination / current.filename(), overwrite, recursive) ){
                    return false;
                }
            }
            else{
                // Found file: Copy
				if(overwrite){
					if(has(current.string(), ".dll")){ continue; }//skip overwriting .dlls in case they are in use
					fs::copy_file( current, destination / current.filename(), boost::filesystem::copy_option::overwrite_if_exists);
				}
				else{
					fs::copy_file( current, destination / current.filename() );
				}
            }
        }
        catch(fs::filesystem_error const & e){
            std:: cerr << e.what() << '\n';
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////
bool DKFile::GetShortName(const DKString& file, DKString& shortname)
{
	//DKLog("DKFile::GetShortName("+file+","+shortname+")\n", DKDEBUG);
	if(!PathExists(file)){ return false; }
#ifdef WIN32
    long length = 0;
    TCHAR* buffer = NULL;
	length = GetShortPathName(file.c_str(), NULL, 0);
	if(length < 1){
		//DKLog("DKFile::GetShortName(): length < 1 \n", DKERROR);
		return false;
	}
	buffer = new TCHAR[length];
	length = GetShortPathName(file.c_str(), buffer, length);
	if(length < 1){
		//DKLog("DKFile::GetShortName(): length < 1 \n", DKERROR);
		return false;
	}
	DKString temp = buffer; 
    delete [] buffer;
	shortname = temp;
#else
	shortname = file;
#endif
	return true;
}

//////////////////////////////////////////////
bool DKFile::GetDrives(DKStringArray& strings)
{
	//DKLog("DKFile::GetDrives(&strings)\n", DKDEBUG);
#ifdef WIN32
	TCHAR szDrive[] = " A:";
	DWORD drives = GetLogicalDrives();
	if(drives == 0){
		DKLog("GetLogicalDrives() failed. \n", DKERROR);
		return false;
	}

	for (int i=0; i<26; i++){
		if((drives & (1 << i ))){
			TCHAR driveName[] = { TEXT('A') + i, TEXT(':'), TEXT('\0') }; 
			strings.push_back(driveName);
		}
	}
	return true;
#endif //(WIN32)
	DKLog("DKFile::GetDrives() not implemented on this OS. \n", DKERROR);
	return false;
}


///////////////////////////////////////////////////////////////////////////////
bool DKFile::GetDirectoryContents(const DKString& path, DKStringArray& strings)
{
	//DKLog("DKFile::GetDirectoryContents("+path+",&strings)\n", DKDEBUG);
	if(!PathExists(path)){ return false; }

	boost::filesystem::directory_iterator end_itr;
	for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr){
		DKString filename;
		DKFile::GetFileName(itr->path().string(),filename);
		strings.push_back(filename);
	}

#ifdef LINUX
	std::sort(strings.begin(), strings.end());
#endif

	//remove . && ..
	for(unsigned int i=0; i < strings.size(); ++i){
		if(same(strings[i],".") || same(strings[i],"..")){
			strings.erase(strings.begin()+i);
			--i;
		}
	}
	return true;
}

//////////////////////////////////////////
bool DKFile::GetExeName(DKString& exename)
{
	//DKLog("DKFile::GetExeName("+exename+")\n", DKDEBUG);
#ifdef WIN32
	if(!DKFile::PathExists(DKFile::appfilename)){
		TCHAR appfilename[MAX_PATH];
		GetModuleFileName(NULL, appfilename, MAX_PATH);
		DKFile::appfilename = appfilename;
	}
#endif 

	if (!DKFile::PathExists(DKFile::appfilename)){
		return false;
	}
	DKString filename = DKFile::appfilename;
    
#ifdef WIN32
	unsigned found = filename.find_last_of("\\");
#else
	unsigned found = filename.find_last_of("/");
#endif

    exename = filename.substr(found+1);
    return true;
}

//////////////////////////////////////////////////
bool DKFile::GetFullExeName(DKString& fullexename)
{
	//DKLog("DKFile::GetFullExeName("+fullexename+")\n", DKDEBUG);
	if(!DKFile::PathExists(DKFile::appfilename)){ return false; }
	fullexename = DKFile::appfilename;
    return true;
}

//////////////////////////////////////////
bool DKFile::GetExePath(DKString& exepath)
{
	//DKLog("DKFile::GetExePath("+exepath+")\n", DKDEBUG);
    unsigned found = 0;
#ifdef WIN32
    exepath = DKFile::appfilename;
	found = exepath.find_last_of("\\");
    exepath.erase (exepath.begin()+found+1, exepath.end()); 
	return true;
#elif defined(ANDROID)
	exepath = "/mnt/sdcard/digitalknob/";
	return true;
#elif defined(MAC) || defined(IOS) || defined(LINUX)
    exepath = DKFile::appfilename;
    found = exepath.find_last_of("/");
    exepath.erase (exepath.begin()+found+1, exepath.end());
	return true;
#endif
	
	DKLog("DKFile::GetExePath() not implemented on this OS \n", DKERROR);
	return false;
}

//////////////////////////////////////////////////////////////////
bool DKFile::GetFileName(const DKString& path, DKString& filename)
{
	//DKLog("DKFile::GetFileName("+path+","+filename+")\n", DKDEBUG);
	unsigned found = path.find_last_of("/\\");
	if(found != std::string::npos && found < path.length()){
		filename = path.substr(found+1);
		return true;
	}
	filename = path;
	return true;
}

//////////////////////////////////////////////////////////////
bool DKFile::GetFilePath(const DKString& file, DKString& path)
{
	//DKLog("DKFile::GetFilePath("+file+","+path+")\n", DKDEBUG);
	unsigned int found = file.find_last_of("/");
	if(found != std::string::npos && found < file.length()){
		path = file.substr(0, found+1);
		return true;
	}
	found = file.find_last_of("\\");
	if(found != std::string::npos && found < file.length()){
		path = file.substr(0, found+1);
		return true;
	}
	return false;
}

/////////////////////////////////////////////////////////////////
bool DKFile::FileToString(const DKString& file, DKString& string)
{
	//DKLog("DKFile::FileToString("+file+","+string+")\n", DKDEBUG);
	if(!PathExists(file)){
		DKLog("DKFile::FileToString failed! \n", DKERROR);
		return false; 
	}

	//TODO: error control
	std::ifstream t(file.c_str());
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	string = str;
	return true;
}

///////////////////////////////////////////////////////////////////////
bool DKFile::StringToFile(const DKString& string, const DKString& file)
{
	//DKLog("DKFile::StringToFile("+string+","+file+")\n", DKDEBUG);
	DKString folder;
	DKFile::GetFilePath(file,folder);
	if(!folder.empty()){
		boost::filesystem::path path(folder);
	    if(!(boost::filesystem::exists(path))){
	        if(!boost::filesystem::create_directory(path)){
				DKLog("DKFile::StringToFile(): could not create path. \n", DKERROR);
				return false;
			}
	    }
	}
	
	std::ofstream t(file.c_str());
	t << string;
	t.close();
	DKLog("Saved file "+file+"\n", DKINFO);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::FindFile(DKString& filename, const DKString& path, const DKString& extension)
{
	//DKLog("DKFile::FindFile("+filename+","+path+","+extension+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }
	DKStringArray files;
	if(!GetDirectoryContents(path, files)){
		DKLog("DKFile::GetFiles() failed. \n", DKERROR);
		return false;
	}

	for(unsigned int i=0; i<files.size(); ++i){
		if(has(files[i],extension)){
			filename = files[i];
			return true;
		}
	}

	DKLog("Could not find "+filename+extension+" file. \n", DKERROR);
	return false;
}

////////////////////////////////////////////////////////////////////
bool DKFile::GetExtention(const DKString& file, DKString& extension)
{
	//DKLog("DKFile::GetExtention("+file+","+extension+")\n", DKDEBUG);
    if(!has(file,".")){return false;}
	unsigned found = file.find_last_of(".");
	extension = file.substr(found,file.size());
	return true;
}

////////////////////////////////////////////
bool DKFile::RemoveExtention(DKString& file)
{
	//DKLog("DKFile::RemoveExtention("+file+")\n", DKDEBUG);
	unsigned found = file.find_last_of(".");
    file = file.substr(0,found);
    return true;
}

//////////////////////////////////////////////
bool DKFile::IsDirectory(const DKString& file)
{
	//DKLog("DKFile::IsDirectory("+file+")\n", DKDEBUG);
	if(!PathExists(file)){ return false; }
	struct stat s;
	if( stat(file.c_str(),&s) == 0 ){
		if( s.st_mode & S_IFDIR ){
			return true;
		}
		else if( s.st_mode & S_IFREG ){
			return false; //it's a file
		}
		else{
			return false; //it's something else
		}
	}
	return false;
}

///////////////////////////////////////////////////
bool DKFile::AppendSystemPath(const DKString& path)
{
	DKLog("DKFile::AppendSystemPath("+path+")\n", DKDEBUG);
	//FIXME: does not work
	DKString command = "set PATH=%PATH%;"+path;
	DKUtil::Execute(command);
	return true;
}

///////////////////////////////////////////////////////////////
bool DKFile::GetAbsolutePath(const DKString& in, DKString& out)
{
	//DKLog("DKFile::GetAbsolutePath("+in+","+out+")\n", DKDEBUG);
	if(!PathExists(in)){ return false; }
#ifdef WIN32
	char *fileExt;
    char resolved_path[256];
	GetFullPathName(in.c_str(), 256, resolved_path, &fileExt);
	out = resolved_path;
	replace(out, "\\", "/");
#else
	char *actualpath;
	actualpath = realpath(in.c_str(), NULL);
	out = actualpath;
#endif
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
bool DKFile::GetRelativePath(const DKString& file, const DKString& path, DKString& out)
{
	if(!PathExists(file)){ return false; }
	DKString file2 = file;
#ifdef WIN32
	replace(file2, "/", "\\");
#endif
	DKString path2 = path;
#ifdef WIN32
	replace(path2, "/", "\\");
#endif
	DKLog("DKFile::GetRelativePath("+file2+","+path2+","+out+")\n", DKDEBUG);

	int MAX_FILENAME_LEN = 512;
#ifdef WIN32
	int ABSOLUTE_NAME_START = 3;
	int SLASH = '\\';
#else
	int ABSOLUTE_NAME_START = 1;
	int SLASH = '/';
#endif

	const char* absoluteFilename = file2.c_str();
	const char* currentDirectory = path2.c_str();
	
	// declarations - put here so this should work in a C compiler
	int afMarker = 0, rfMarker = 0;
	int cdLen = 0, afLen = 0;
	int i = 0;
	int levels = 0;
	static char relativeFilename[512];
	cdLen = strlen(currentDirectory);
	afLen = strlen(absoluteFilename);
	
	// make sure the names are not too long or too short
	if(cdLen > MAX_FILENAME_LEN || cdLen < ABSOLUTE_NAME_START+1 || 
		afLen > MAX_FILENAME_LEN || afLen < ABSOLUTE_NAME_START+1)
	{
		out.clear();
		return false;
	}
	
	// Handle DOS names that are on different drives:
	if(currentDirectory[0] != absoluteFilename[0])
	{
		// not on the same drive, so only absolute filename will do
		strcpy(relativeFilename, absoluteFilename);
		out = relativeFilename;
#ifdef WIN32
		replace(out, "\\", "/");
#endif
		return true;
	}
	// they are on the same drive, find out how much of the current directory
	// is in the absolute filename
	i = ABSOLUTE_NAME_START;
	while(i < afLen && i < cdLen && currentDirectory[i] == absoluteFilename[i])
	{
		i++;
	}
	if(i == cdLen && (absoluteFilename[i] == SLASH || absoluteFilename[i-1] == SLASH))
	{
		// the whole current directory name is in the file name,
		// so we just trim off the current directory name to get the
		// current file name.
		if(absoluteFilename[i] == SLASH)
		{
			// a directory name might have a trailing slash but a relative
			// file name should not have a leading one...
			i++;
		}
		strcpy(relativeFilename, &absoluteFilename[i]);
		out = relativeFilename;
#ifdef WIN32
		replace(out, "\\", "/");
#endif
		return true;
	}
	// The file is not in a child directory of the current directory, so we
	// need to step back the appropriate number of parent directories by
	// using "..\"s.  First find out how many levels deeper we are than the
	// common directory
	afMarker = i;
	levels = 1;
	// count the number of directory levels we have to go up to get to the
	// common directory
	while(i < cdLen)
	{
		i++;
		if(currentDirectory[i] == SLASH)
		{
			// make sure it's not a trailing slash
			i++;
			if(currentDirectory[i] != '\0')
			{
				levels++;
			}
		}
	}
	// move the absolute filename marker back to the start of the directory name
	// that it has stopped in.
	while(afMarker > 0 && absoluteFilename[afMarker-1] != SLASH)
	{
		afMarker--;
	}
	// check that the result will not be too long
	if(levels * 3 + afLen - afMarker > MAX_FILENAME_LEN)
	{
		return NULL;
	}
	
	// add the appropriate number of "..\"s.
	rfMarker = 0;
	for(i = 0; i < levels; i++)
	{
		relativeFilename[rfMarker++] = '.';
		relativeFilename[rfMarker++] = '.';
		relativeFilename[rfMarker++] = SLASH;
	}
	// copy the rest of the filename into the result string
	strcpy(&relativeFilename[rfMarker], &absoluteFilename[afMarker]);
	out = relativeFilename;
#ifdef WIN32
	replace(out, "\\", "/");
#endif
	return true;
}

/////////////////////////////////////;////////////////////////////
bool DKFile::GetModifiedTime(const DKString& path, DKString& time)
{
	//DKLog("DKFile::GetModifiedTime("+path+","+time+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }

#ifdef WIN32 
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
 
	if(GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &fileInfo)){

		FILETIME localFiletime;
		if(FileTimeToLocalFileTime(&fileInfo.ftLastWriteTime, &localFiletime)){

			SYSTEMTIME st={0};
			::FileTimeToSystemTime(&localFiletime,&st);
			//FILETIME ftCreationTime;	// creation time
			//FILETIME ftLastAccessTime;	// last access time	
			//FILETIME ftLastWriteTime;	// last modification time
			//DWORD nFileSizeHigh;	// the high DWORD of the file size (it is zero unless the file is over four gigabytes)
			//DWORD nFileSizeLow;	// the low DWORD of the file size
	
			DKString month = toString((int)st.wMonth);
			DKString day = toString((int)st.wDay);
			DKString year = toString((int)st.wYear);
			int standard = st.wHour % 12;
			DKString hour = toString(standard);
			DKString minute = toString((int)st.wMinute);

			Pad(4, '0', year);
			Pad(2, '0', month);
			Pad(2, '0', day);
			Pad(2, '0', hour);
			Pad(2, '0', minute);
			
			time = month;
			time += "/";
			time += day;
			time += "/";
			time += year;
			time += " ";
			time += hour;
			time += ":";
			time += minute;
			if(st.wHour > 12){
				time += "pm";
			}
			else{
				time += "am";
			}
			return true;
		}
		DKLog("localFiletime; invalid \n", DKERROR);
		return false;
    }
	else{
        DKLog("file not found", DKERROR);
		return false;
	}
#else 
#ifndef ANDROID
	struct tm* local;
	struct stat attrib;
	stat(path.c_str(), &attrib);
	local = localtime(&(attrib.st_mtime));

	int standard = local->tm_hour % 12;
	time = toString(local->tm_mon+1);
	time += "/";
	time += toString(local->tm_mday);
	time += "/";
	time += toString(local->tm_year + 1900);
	time += " ";
	time += toString(standard);
	time += ":";
	time += toString(local->tm_min);
	if(local->tm_hour > 12){
		time += "pm";
	}
	else{
		time += "am";
	}
	return true;
#endif
#endif

	DKLog("DKFile::GetModificationTime() not implemeneted on this OS.\n",DKERROR);
	return false; //TODO
}

/////////////////////////////////////////////
bool DKFile::CreateFile(const DKString& path)
{
	//DKLog("DKFile::CreateFile("+path+")\n", DKDEBUG);
	if(PathExists(path)){ return false; }
	std::ofstream outputFile(path.c_str()); //TODO: return value on success/fail
	return true;
}


///////////////////////////////////////////////////////////////////////////
bool DKFile::GetLocalCreationDate(const DKString& path, DKString& filedate)
{
	//DKLog("DKFile::GetLocalCreationDate("+path+","+filedate+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }

#if defined(WIN32) || defined(MAC) || defined(LINUX)// || defined(ANDROID)	
	struct tm* clock;               // create a time structure
	struct stat attrib;         // create a file attribute structure
	stat(path.c_str(), &attrib);     // get the attributes of afile.txt
	clock = localtime(&(attrib.st_ctime)); 

	DKString month = toString(clock->tm_mon);
	DKString day = toString(clock->tm_mday);
	DKString hour = toString(clock->tm_hour);
	DKString minute = toString(clock->tm_min);
	DKString second = toString(clock->tm_sec);
	DKString year = toString(clock->tm_year + 1900);

	Pad(4, '0', year);
	Pad(2, '0', month);
	Pad(2, '0', day);
	Pad(2, '0', hour);
	Pad(2, '0', minute);
	Pad(2, '0', second);

	//19990102040506
	filedate = year + month + day + hour + minute + second;
	return true;
#endif
	DKLog("DKUpdate::GetLocalCreationDate() is not implemented for this OS. \n", DKERROR);
	return false;
}

///////////////////////////////////////////////////////////////////////////
bool DKFile::GetLocalModifiedDate(const DKString& path, DKString& filedate)
{
	//DKLog("DKFile::GetLocalModifiedDate("+path+","+filedate+")\n", DKDEBUG);
	if(!PathExists(path)){ return false; }

#if defined(WIN32) || defined(MAC) || defined(LINUX)// || defined(ANDROID)
	struct tm* clock;               // create a time structure
	struct stat attrib;         // create a file attribute structure
	stat(path.c_str(), &attrib);     // get the attributes of afile.txt
	clock = localtime(&(attrib.st_mtime)); 

	DKString month = toString(clock->tm_mon);
	DKString day = toString(clock->tm_mday);
	DKString hour = toString(clock->tm_hour);
	DKString minute = toString(clock->tm_min);
	DKString second = toString(clock->tm_sec);
	DKString year = toString(clock->tm_year + 1900);

	Pad(4, '0', year);
	Pad(2, '0', month);
	Pad(2, '0', day);
	Pad(2, '0', hour);
	Pad(2, '0', minute);
	Pad(2, '0', second);

	//19990102040506
	filedate = year + month + day + hour + minute + second;
	return true;
#endif
	DKLog("DKUpdate::GetLocalModifiedDate() is not implemented for this OS. \n", DKERROR);
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////
bool DKFile::GetSetting(const DKString& file, const DKString& setting, DKString& value)
{
	//DKLog("DKFile::GetSetting("+file+","+setting+")\n", DKDEBUG);
	if(!PathExists(file)){ return false; }
	DKString filestring;
	if(!FileToString(file, filestring)){ return false; }
	if(!getSettingFromString(filestring, setting, value)){ return false; }
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::SetSetting(const DKString& file, const DKString& setting, const DKString& value)
{
	//DKLog("DKFile::SetSetting("+file+","+setting+","+value+")\n", DKDEBUG);
	DKString filestring;
	CreateFile(file);
	if(!FileToString(file,filestring)){ return false; }

	//If the variable looks like this: [VARIABLE]
	//then we return everything up to the next [VARIABLE] or to the end of the file.
	if(has(setting,"[") && has(setting,"]")){
		size_t temp = filestring.find(setting,0);
        if(temp == std::string::npos){
			filestring.append("\n" + setting + " " + value); //create entry
			DKFile::StringToFile(filestring,file);
			DKLog("WROTE: "+filestring+" TO: "+file+" \n", DKSUCCESS);
			return true;
		}
		size_t start = filestring.find("]",temp);
		size_t end = filestring.find("[",start);
		if(end == std::string::npos){end = filestring.size();}

		//DKString string = filestring.substr(start+1, end-start-1);
		DKString out = " "+value+"\n";
		filestring.replace(start+1,end-start-1,out.c_str()); 
		DKFile::StringToFile(filestring,file);
		DKLog("WROTE: "+filestring+" TO: "+file+" \n", DKSUCCESS);
		return true;
	}

	//If the variable looks like this:  VARIABLE 
	//then we return the rest of the line
	DKString string = setting + " ";

	size_t temp = filestring.find(string,0);
	if(temp == std::string::npos){
			filestring.append("\n" + setting + " " + value); //create entry
			DKFile::StringToFile(filestring,file);
			DKLog("WROTE: "+filestring+" TO: "+file+" \n", DKSUCCESS);
			return true;
		}
	size_t start = filestring.find(" ",temp);
	size_t end = filestring.find("\n",start);

	//DKString string = filestring.substr(start+1, end-start-1);
	filestring.replace(start+1,end-start-1,value.c_str()); 
	DKFile::StringToFile(filestring,file);
	
	DKLog("WROTE: "+filestring+" TO: "+file+" \n", DKSUCCESS);
	return true;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////
bool DKFile::GetSettings(const DKString& file, const DKString& setting, DKStringArray& arry)
{
	//DKLog("DKFile::GetSettings("+file+","+setting+")\n", DKDEBUG);
	if(!PathExists(file)){ 	return false; }

	DKString filestring;
	if(!FileToString(file, filestring)){
		DKLog("DKFile::FileToString failed! \n", DKERROR);
		return false;
	}

	arry = getSettingsFromString(filestring, setting);
	return true;
}
*/