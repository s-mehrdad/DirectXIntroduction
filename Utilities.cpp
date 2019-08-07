﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,08.08.2019</changed>
// ********************************************************************************

#include "Utilities.h"
#include "Shared.h"


theException::theException () : expected ( "null" ) {};
void theException::set ( const char* prm )
{
  expected = prm;
};
const char* theException::what () const throw( )
{
  return expected;
};


toFile::toFile () : ready ( false )
{
  try
  {
    fileStream.open ( "dump.log", std::ofstream::binary );
    if ( fileStream.is_open () )
      ready = true;
    else
    {
      PointerProvider::getException ()->set ( "stream" );
      throw* PointerProvider::getException ();
    }

  }
  catch ( const std::exception& ex )
  {
    if ( ex.what () == "stream" )
      MessageBoxA ( NULL, "The log file could not be opened for writing.", "Error", MB_OK | MB_ICONERROR );
    else
      MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );
  }
};


toFile::~toFile ()
{
  ready = false;
  fileStream.close ();
};


const bool& toFile::state ()
{
  return ready;
}


bool toFile::write ( const Log& entity )
{
  try
  {
    std::wstringstream line;
    if ( ( running == false ) && ( gameState == L"shutting down" ) )
    {
      line << "\r\n\n";
      gameState = L"uninitialized";
    } else
      line << "\r\n";

    if ( entity.id < 1000 )
      line << entity.id << "\t\t";
    else
      line << entity.id << "\t";

    line << entity.cMoment << '\t';

    switch ( entity.type )
    {
      case 0:
        line << "INFO:    ";
        break;
      case 1:
        line << "DEBUG:   ";
        break;
      case 2:
        line << "WARNING: ";
        break;
      case 3:
        line << "ERROR:   ";
        break;
    }
    line << entity.threadId << '\t' << entity.threadName << '\t' << entity.message;

    if ( ( running == true ) && ( gameState == L"initialized" ) )
    {
      line << '\n';
      gameState = L"gaming";
    }

    // Todo add the flush functionality
    if ( ready )
      fileStream << Converter::strConverter ( line.str () );
    else
    {
      PointerProvider::getException ()->set ( "logW" );
      throw* PointerProvider::getException ();
    }
    return true;
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    if ( ex.what () == "logW" )
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Outputting to file stream failed!" );
    else
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    return false;
  }
};


template<class tType>
unsigned int Logger<tType>::counter { 0 };


template<class tType>
void loggerEngine ( Logger<tType>* engine );
template<class tType>
Logger<tType>::Logger () : policy (), writeGuard ()
{
  try
  {
    logEntity.id = 0;
    logEntity.type = logType::info;
    logEntity.threadId = std::this_thread::get_id ();
    logEntity.threadName = L"";
    logEntity.message = L"";

    if ( policy.state () )
    {
      operating.test_and_set (); // mark the write engine as running
      commit = std::move ( std::thread { loggerEngine<tType>, this } );
    }
  }
  catch ( const std::exception& ex )
  {

    MessageBoxA ( NULL, ex.what (), "Error", MB_OK | MB_ICONERROR );

  }
};


template<class tType>
Logger<tType>::~Logger ()
{

#ifndef _NOT_DEBUGGING
  PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread", L"The logging engine is going to successfully shut down..." );
  std::this_thread::sleep_for ( std::chrono::milliseconds { 100 } );
#endif // !_NOT_DEBUGGING

  operating.clear ();
  commit.join ();
  buffer.clear ();
};


template<class tType>
void Logger<tType>::push ( const logType& t,
                           const std::thread::id& tId,
                           const std::wstring& tName,
                           const std::wstring& msg )
{
  try
  {
    Log temp; // temporary buffer list container
    temp.id = 0;
    counter++;
    if ( ( counter > 0 ) && ( logEntity.id == 0 ) )
    {
      logEntity.id = counter;
      logEntity.type = t;
      logEntity.threadId = tId;
      logEntity.threadName = tName;
      logEntity.message = msg;
    } else
    {
      temp.id = counter;
      temp.type = t;
      temp.threadId = tId;
      temp.threadName = tName;
      temp.message = msg;
    }

    std::wstringstream current;
    SYSTEMTIME cDateT;
    GetLocalTime ( &cDateT );
    // date and time format: xx/xx/xx xx:xx:xx
    current << cDateT.wDay << '/' << cDateT.wMonth << '/' << cDateT.wYear << ' ';
    if ( cDateT.wHour > 9 )
      current << cDateT.wHour << ':';
    else
      current << "0" << cDateT.wHour << ':';
    if ( cDateT.wMinute > 9 )
      current << cDateT.wMinute << ':';
    else
      current << "0" << cDateT.wMinute << ':';
    if ( cDateT.wSecond > 9 )
      current << cDateT.wSecond;
    else
      current << "0" << cDateT.wSecond;

    if ( temp.id == 0 )
    {
      logEntity.cMoment = current.str ();
      std::lock_guard<std::timed_mutex> lock ( writeGuard );
      buffer.push_back ( logEntity );
      logEntity.id = 0;
    } else
    {
      temp.cMoment = current.str ();
      std::lock_guard<std::timed_mutex> lock ( writeGuard );
      buffer.push_back ( temp );
      temp.id = 0;
    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


template<class tType>
void loggerEngine ( Logger<tType>* engine )
{
  try
  {
    // dump engine: write the present logs' data
#ifndef _NOT_DEBUGGING
    std::this_thread::sleep_for ( std::chrono::milliseconds { 20 } );
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"logThread", L"Logging engine is successfully started:\n\nFull-featured surveillance is the utter most goal in a digital world, and frankly put, it is well justified! ^,^\n" );
#endif // !_NOT_DEBUGGING

    // Todo robust lock
    // initializing and not locking the mutex object (mark as not owing a lock)
    std::unique_lock<std::timed_mutex> lock ( engine->writeGuard, std::defer_lock );

    do
    {
      if ( engine->buffer.size () )
      {
        if ( !lock.try_lock_for ( std::chrono::milliseconds { 30 } ) )
          continue;
        for ( auto& element : engine->buffer )
          if ( !engine->policy.write ( element ) )
          {

#ifndef _NOT_DEBUGGING
            PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"logThread", L"Dumping wasn't possible." );
#endif // !_NOT_DEBUGGING

          }
        engine->buffer.clear ();
        lock.unlock ();
      }
    } while ( engine->operating.test_and_set () || engine->buffer.size () );
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"logThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


// don't call this function: solution for linker error, when using templates.
void problemSolver ()
{
  Logger<toFile> tempObj;
  tempObj.push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The problem solver... :)" );
}


Configurations::Configurations ()
{
  try
  {
    valid = false;
    std::this_thread::sleep_for ( std::chrono::milliseconds { 30 } );

    // defaults initialization:
    defaults.Width = 640;
    defaults.Height = 480;
    defaults.fullscreen = false;

    // currents initialization:
    currents.Width = 0;
    currents.Height = 0;
    currents.fullscreen = false;

    PWSTR docPath { NULL };
    HRESULT hResult = SHGetKnownFolderPath ( FOLDERID_Documents, NULL, NULL, &docPath );
    std::wstring path { L"" };
    if ( FAILED ( hResult ) )
    {
      MessageBoxA ( NULL, "The path to document directory is unknown! Please contact your OS support.", "Critical-Error", MB_OK | MB_ICONERROR );

#ifndef _NOT_DEBUGGING
      PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Retrieving the document directory path failed!" );
#endif // !_NOT_DEBUGGING

      pathToMyDocuments = L"";
      path = L"C:\\TheGame";
      hResult = SHCreateDirectory ( NULL, path.c_str () );
      if ( FAILED ( hResult ) )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"The creation of directory failed!" );
#endif // !_NOT_DEBUGGING

        path = L"C:\\settings.lua";
      }
      else
        path += L"\\settings.lua";
    } else
      pathToMyDocuments = docPath;
    path = pathToMyDocuments + L"\\settings.lua";


    //!? temporary statement: development time path
    path = { L"C:\\Users\\Mehrdad\\Source\\Repos\\LearningDirectX\\settings.lua" };


    pathToSettings = path;
    // Lua accepts a string type as path
    std::string pathStr { "" };
    pathStr = Converter::strConverter ( pathToSettings );

    for ( char i = 0; i < 2; i++ )
    {
      // read the configuration
      sol::state configs;
      try
      {
        configs.safe_script_file ( pathStr );
        // opening the configuration file
        // read or use the application defaults:
        currents.Width = configs ["configurations"]["resolution"]["width"].get_or ( defaults.Width );
        // the sol state class is constructed like a table, thus nested variables are accessible like multidimensional arrays.
        currents.Height = configs ["configurations"]["resolution"]["height"].get_or ( defaults.Height );
        currents.fullscreen = configs ["fullscreen"].get_or ( defaults.fullscreen );
      }
      catch ( const std::exception& ex )
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

      }

      // validation
      if ( ( currents.Width != 0 ) && ( currents.Height != 0 ) && ( !valid ) )
      {
        valid = true;

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                                  L"The configuration file is successfully read:\n\tResolution: (" +
                                                  std::to_wstring ( currents.Width ) + L" x "
                                                  + std::to_wstring ( currents.Height ) + L" )\t\t" +
                                                  L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );
#endif // !_NOT_DEBUGGING

        break;
      } else
      {

#ifndef _NOT_DEBUGGING
        PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Retrieving the configuration file failed (Non-existent or invalid)!" );
#endif // !_NOT_DEBUGGING

        // rewrite the configuration file with defaults
        std::ofstream writeStream ( path.c_str () );
        if ( writeStream.good () )
        {
          std::stringstream settingsLine;
          settingsLine << "configurations =\n\t{\n" <<
            "\t\tresolution = { width = " << std::to_string ( defaults.Width ) <<
            " , height = " << std::to_string ( defaults.Height ) << " },\n" <<
            "\t\tfullscreen = " << std::to_string ( defaults.fullscreen ) << "\n\t}";
          writeStream << settingsLine.str ();
          writeStream.close ();

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::warning, std::this_thread::get_id (), L"mainThread", L"Configuration file is now rewritten with default settings." );
#endif // !_NOT_DEBUGGING

        } else
        {

#ifndef _NOT_DEBUGGING
          PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", L"Rewriting the Configuration file with default settings failed." );
#endif // !_NOT_DEBUGGING

        }

      }
    }
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


const bool& Configurations::isValid ()
{
  return valid;
};


const ConfigsContainer& Configurations::getDefaults ( void )
{
  return defaults;
};


const ConfigsContainer& Configurations::getSettings ( void )
{
  return currents;
};


void Configurations::apply ()
{
  try
  {
    //xx Todo new settings to configuration file
    // Todo add user settings
    std::ofstream writeStream ( pathToSettings.c_str () );
    if ( writeStream.good () )
    {
      std::stringstream settingsLine;
      settingsLine << "configurations =\n\t{\n" <<
        "\t\tresolution = { width = 800 , height = 600 },\n" <<
        "\t\tfullscreen = 0\n\t}";
      writeStream << settingsLine.str ();
      writeStream.close ();
    }

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::info, std::this_thread::get_id (), L"mainThread",
                                              L"The configuration file is successfully written:\n\tResolution: (" +
                                              std::to_wstring ( currents.Width ) + L" x "
                                              + std::to_wstring ( currents.Height ) + L" )\t\t" +
                                              L"fullscreen: " + std::to_wstring ( currents.fullscreen ) );
#endif // !_NOT_DEBUGGING
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


void Configurations::apply ( const ConfigsContainer& )
{
  try
  {

  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

  }
};


std::wstring Converter::strConverter ( const std::string& str )
{
  try
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
    return entity.from_bytes ( str );
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    return L"";

  }
};


std::string Converter::strConverter ( const std::wstring& wstr )
{
  try
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> entity;
    return entity.to_bytes ( wstr );
  }
  catch ( const std::exception& ex )
  {

#ifndef _NOT_DEBUGGING
    PointerProvider::getFileLogger ()->push ( logType::error, std::this_thread::get_id (), L"mainThread", Converter::strConverter ( ex.what () ) );
#endif // !_NOT_DEBUGGING

    return "";

  }
};
