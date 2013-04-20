//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#pragma once


// todo() is a macro that allows you to mark sections of your code as
// incomplete or in need of revision.  Use a todo when the code compiles and
// runs without error in most situations, but something about it needs to be
// improved.  This block will track the amount of time spent in each todo's
// scope, the number of times each todo is hit in the program's execution and
// other information.  When the program ends, a report is printed that helps
// prioritize bug fixing and features.  In release mode, todo compiles to
// (noop) by default, or it can be forcibly enabled by globally defining
// a macro.
//
// When your program is about to end, invoke todoReport(someFunction)
// to call someFunction with the todo report's text (as a const char* pointer)
// as the only parameter.
//
// There are only 2 things you need to watch out for:
//  - you should not place two todo()s on the same line, since line number
//    is used to make the variables names unique.
//  - timing won't work correctly if the report is printed while a todo()
//    is in scope
// 
// Example Use:
/*

{
  todo("kwg8@cornell.edu", "replace this linear search with a table look-up");
  for (int i = 0; i < 1000; ++i) { todo("kwg8@cornell.edu", "loop example"); sleep(1); }
  sleep(1000);
}
// ...
todoReport(printf);

*/
// This generates a report like:
//
//----[  todo() report by hits  ]----------------------------------
//c:\tests.cpp(188):  "loop example" - kwg8@cornell.edu (hit 1000 times - 1000 ms)
//c:\tests.cpp(188):  "replace this lienar search with a table look-up" - kwg8@cornell.edu (hit 1 time - 2000 ms)
//----[  todo() report by time  ]----------------------------------
//c:\tests.cpp(188):  "replace this lienar search with a table look-up" - kwg8@cornell.edu (hit 1 time - 2000 ms)
//c:\tests.cpp(188):  "loop example" - kwg8@cornell.edu (hit 1000 times - 1000 ms)
//-----------------------------------------------------------------
//
//
// Other Info:
//  - TODO_ENABLE causes todo() and todoReport() to resolve in release mode
//  - TODO_DISABLE forces those symbols to disappear in debug mode


#if (!defined(TODO_DISABLE) && (defined(DEBUG) || defined(_DEBUG))) || defined(TODO_ENABLE)
#define _todo_name_inner(line)  todo_at_line_##line
#define _todo_name(line)  _todo_name_inner(line)
#define _todo_timer_name_inner(line) todo_timer_for_todo_at_line_##line
#define _todo_timer_name(line) _todo_timer_name_inner(line)
#define	todo(who, what)	static TodoStruct _todo_name(__LINE__)(__FILE__,__LINE__,who,what);\
                        TodoTimer _todo_timer_name(__LINE__)(&_todo_name(__LINE__));_todo_name(__LINE__).hit()
#define TODO_IS_ENABLED
#endif

#ifdef TODO_IS_ENABLED

//----[  TodoStruct  ]---------------------------------------------------------
struct TodoStruct {
  TodoStruct(const char* file,
             unsigned int line,
             const char* who,
             const char* what);
  void hit();

  const char* file;
  unsigned int line;
  const char* who;
  const char* what;
  unsigned int hits;
  long local_ms;

  // don't touch this; it's used internally and stuff could explode
  TodoStruct* next;
};


//----[  TodoTimer  ]----------------------------------------------------------
struct TodoTimer {
  TodoTimer(TodoStruct* todo_struct);
  ~TodoTimer();
  long start;
  TodoStruct* todo_struct;
};



//----[  TodoReport  ]---------------------------------------------------------
struct TodoReport {
  const char* text;
  TodoReport();
  TodoReport(TodoReport& rhs);
  ~TodoReport();
  static TodoReport generateReport();
};


//----[  todoReport  ]---------------------------------------------------------
#define todoReport(outputFunction) outputFunction(TodoReport::generateReport().text);

#else
#define todo(who, what)	(0)
#define todoReport(outputFunction)  (0)
#endif