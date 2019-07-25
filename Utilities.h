﻿// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>ʆϒʅ,22.07.2019</created>
/// <changed>ʆϒʅ,26.07.2019</changed>
// ********************************************************************************

#ifndef UTILITIES_H
#define UTILITIES_H


// the special exception container
class theException : public std::exception
{
private:
  const char* expected;
public:
  theException ();
  void set ( const char* );
  const char* what () const throw( );
};


// the severity of the log entity
enum logType { info = 0, debug, warning, error };


// the log entity
struct Log
{
  unsigned short id;
  unsigned short count;
  logType type;
  std::string cMoment;
  std::thread::id threadId;
  std::string threadName;
  std::string message;

  Log ();
  void set ( const logType&,
             const std::thread::id&,
             const std::string&,
             const std::string& );
};


// file stream policy
class toFile
{
private:
  std::ofstream fileStream;
  bool ready;
public:
  toFile ();
  ~toFile ();
  const bool& state ();
  bool write ( const Log& );
};


// Todo screen stream policy
//class toScreen {};


// log engine
template<class tType>
class Logger
{
private:
  std::list<Log> buffer; // buffer list container 
  tType policy; // output stream policy
  std::timed_mutex writeGuard; // write guard
  std::thread commit; // write engine thread
  std::atomic_flag operating { ATOMIC_FLAG_INIT }; // lock-free atomic flag (checking the running state)
public:
  Logger ();
  ~Logger ();
  void push ( const Log& );

  template<class tType>
  friend void loggerEngine ( Logger<tType>* ); // write engine
};


// don't call this function: solution for linker error, when using templates.
void problemSolver ();


struct Configuration
{
  unsigned int Width;
  unsigned int Height;

  Configuration ();
  const Configuration& set ( void );
};


// Todo file: available and containing valid configurations: file is source
// if first run or the file is not valid then default configurations in the structure
class Configure
{
private:
  std::wstring pathToMyDocuments;
  bool valid;
  Configuration current;
public:
  Configure ();
  ~Configure ();
  void set ( const Configuration& );
  const Configuration& set ( void );

  static const std::wstring& strConverter ( const std::string& );
  static const std::string& strConverter ( const std::wstring& );
};


#endif // !UTILITIES_H
