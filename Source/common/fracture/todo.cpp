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
#include "todo.h"
#ifdef TODO_IS_ENABLED

#include <vector>     // for holding all todo()s when printing report
#include <algorithm>  // for sorting the report's vector by # of times hit
#include <string>
#include <time.h>     // for clock()

//----[  LinkStaticTodoStructIfValid  ]----------------------------------------
// Adds the TodoStruct ts to a linked list of structures local to this method,
// as long as ts != NULL.
// The head of the list is returned.
static TodoStruct* LinkStaticTodoStructIfValid(TodoStruct* ts) {
  static TodoStruct* first = 0;
  if (ts) {
    ts->next = first;
    first = ts;
  }
  return first;
}


//----[  TodoStruct  ]---------------------------------------------------------
TodoStruct::TodoStruct(const char* file,
                       unsigned int line,
                       const char* who,
                       const char* what)
    : file(file), line(line), who(who), what(what), hits(0), local_ms(0) {
  LinkStaticTodoStructIfValid(this);
}



//----[  hit  ]----------------------------------------------------------------
void TodoStruct::hit() {
  ++hits;
}



//----[  TodoTimer  ]----------------------------------------------------------
TodoTimer::TodoTimer(TodoStruct* todo_struct) : todo_struct(todo_struct) {
  start = (int)clock();
}



//----[  ~TodoTimer  ]---------------------------------------------------------
TodoTimer::~TodoTimer() {
  todo_struct->local_ms += clock() - start;
}

//----[  TodoStructSTLSortPredicateByHits  ]-----------------------------------
//----[  TodoStructSTLSortPredicateByTime  ]-----------------------------------
// sorts TodoStruct pointers in an STL container by descending number of hits
// when used as the predicate for std::sort
bool TodoStructSTLSortPredicateByHits(const TodoStruct* lhs, const TodoStruct* rhs) {
  return lhs->hits > rhs->hits;
}
bool TodoStructSTLSortPredicateByTime(const TodoStruct* lhs, const TodoStruct* rhs) {
  return lhs->local_ms > rhs->local_ms;
}



//----[  TodoReport  ]---------------------------------------------------------
TodoReport::TodoReport() : text(0) {
}



//----[  TodoReport  ]---------------------------------------------------------
TodoReport::TodoReport(TodoReport& rhs) : text(rhs.text) {
  rhs.text = 0;
}


//----[  ~TodoReport  ]--------------------------------------------------------
TodoReport::~TodoReport() {
  delete[] text;
}



//----[  generateReport  ]-----------------------------------------------------
TodoReport TodoReport::generateReport() {
  std::string text;
  static const size_t MAX_CHARS = 1024;
  char line[MAX_CHARS];
  TodoStruct* next_todo = LinkStaticTodoStructIfValid(NULL);
  typedef std::vector<TodoStruct*> TodoStructs;
  TodoStructs all;
  while (next_todo) { // add every TodoStruct to the vector
    all.push_back(next_todo);
    next_todo = next_todo->next;
  }

  for (int report_type = 0; report_type < 2; ++report_type) {
    switch (report_type) {
    case 0:
      text += "----[  todo() report by hits  ]----------------------------------\n";
      std::sort(all.begin(), all.end(), TodoStructSTLSortPredicateByHits);
      break;
    case 1:
      text += "----[  todo() report by time  ]----------------------------------\n";
      std::sort(all.begin(), all.end(), TodoStructSTLSortPredicateByTime);
      break;
    }
    for (TodoStructs::iterator i = all.begin(); i != all.end(); ++i) {
      TodoStruct* ts = *i;
      sprintf_s(line,
                "%s(%lu):  \"%s\" - %s (hit %lu time%s - %i ms)\n",
                ts->file,
                ts->line,
                ts->what,
                ts->who,
                ts->hits,
                ts->hits==1?"":"s",
                ts->local_ms); // pluralize time if hits != 1
      text += line;
    }
  }
  text += "-----------------------------------------------------------------\n";

  // allocate output buffer
  TodoReport tr;
  size_t chars = text.length() + 1;
  tr.text = new char[chars];
  memcpy((void*)tr.text, text.c_str(), chars);
  return tr;
}



#endif