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
#ifndef __DCDEBUG_H__
#define __DCDEBUG_H__


#if defined(DC_ENABLE_DEBUGGING)

/// Sets the stream to which a the debugger writes information.  The stream class must remain
/// valid until another stream is swapped in, or DEBUG_ATEXIT() is invoked.
#define DEBUG_STREAM_CHANGE(stream)  dc::dcDebug::global(__FILE__, __LINE__)->changeOutputStream(stream)
#define DEBUG_STREAM_LOG_FILENAME() dc::dcDebug::global(__FILE__, __LINE__)->getDefaultFilename();

/// Simply outputs an information message
#define DEBUG_INFO      dc::dcDebug::global( __FILE__, __LINE__ )->info

/// Outputs a warning message
#define DEBUG_WARNING   dc::dcDebug::global( __FILE__, __LINE__ )->warning

/// Outputs an error message
#define DEBUG_ERROR     dc::dcDebug::global( __FILE__, __LINE__ )->error

/// Outputs a fatal error message
#define DEBUG_FATAL     dc::dcDebug::global( __FILE__, __LINE__ )->fatal

/// Allows the user to conditionally specify an information message to display.  Use a syntax
/// like the following:  APP_INFO( true )( "This message will always be displayed!" );
/// The return value of this command will be the same as 'cond', so this can be used inside of
/// another conditional statement.
#define APP_INFO( cond )     ((cond) ? dc::dcDebug::global( __FILE__, __LINE__ ) : dc::dcDebug::nullGlobal())->info

/// Outputs a warning message if the 'cond' parameter resolves to a boolean true.  The return
/// value of this command will be the same as 'cond', so this can be used inside of another
/// conditional statement.
#define APP_WARNING( cond )  ((cond) ? dc::dcDebug::global( __FILE__, __LINE__ ) : dc::dcDebug::nullGlobal())->warning

/// Outputs an error message if the 'cond' parameter resolves to a boolean true.  The return
/// value of this command will be the same as 'cond', so this can be used inside of another
/// conditional statement.
#define APP_ERROR( cond )    ((cond) ? dc::dcDebug::global( __FILE__, __LINE__ ) : dc::dcDebug::nullGlobal())->error

/// Outputs a fatal error message if the 'cond' parameter resolves to a boolean true.  The return
/// value of this command will be the same as 'cond', so this can be used inside of another
/// conditional statement.
#define APP_FATAL( cond )    ((cond) ? dc::dcDebug::global( __FILE__, __LINE__ ) : dc::dcDebug::nullGlobal())->fatal

/// Checks the condition.  If it is not met, the error is logged and the method executes the "else" line
/// This should be used in the following manner:  CONFIRM( pointer ) else return false;
#define CONFIRM( cond )     if( !APP_ERROR( !(cond) )( "CONFIRM( %s )", #cond ) ) {}

/// Same as the CONFIRM macro, but this method produces a fatal error instead of a regular error
#define ASSERT( cond )      if( !APP_FATAL( !(cond) )( "ASSERT( %s )", #cond ) ) {}

/// Displays a warning message to the user, and brings up the debug log if there was a fatal
/// error of some sort.  Otherwise, this method will do nothing.
#define DEBUG_ATEXIT()    dc::dcDebug::global( __FILE__, __LINE__ )->atExit()

#else


inline bool killStringTrue( char* string, ... ) { return true; }
inline bool killStringFalse( char* string, ... ) { return false; }

#define DEBUG_INFO			__noop
#define DEBUG_WARNING		__noop
#define DEBUG_ERROR			__noop
#define DEBUG_FATAL			__noop
#define APP_INFO( cond )	(*((cond) ? &killStringTrue : &killStringFalse))
#define APP_WARNING( cond ) (*((cond) ? &killStringTrue : &killStringFalse))
#define APP_ERROR( cond )	(*((cond) ? &killStringTrue : &killStringFalse))
#define APP_FATAL( cond )	(*((cond) ? &killStringTrue : &killStringFalse))
#define CONFIRM( cond )     if( (cond) ) {}
#define ASSERT( cond )      if( (cond) ) {}
#define DEBUG_ATEXIT()		__noop

#endif



namespace dc {

	/**
	 * This debug class, used to output message to the user and display information relevant to the
	 * program's execution.
	 */
	class dcDebug
	{
		public:

			/**
			 * Obtains a debug interface that will output data from the given
			 * file and line.  This class's output methods will always return true.
			 */
			static dcDebug* global( const char* file, unsigned long line );

			/**
			 * The null-global object is a disabled Debug object.  This class's
			 * functions do nothing with their inputs.  The output functions always
			 * return false.
			 */
			static dcDebug* nullGlobal();


		private:

			/**
			 * Initializes this class
			 */
			dcDebug( bool isNull );


		public:

			/**
			 * Just writes directly to the output
			 */
			bool print( const char* message );

			/**
			 * Logs an information message to the debug output
			 */
			bool info( const char* message, ... );

			/**
			 * Logs a warning message to the debug output
			 */
			bool warning( const char* message, ... );

			/**
			 * Logs an error message to the debug output
			 */
			bool error( const char* message, ... );

			/**
			 * Something has stopped the program from executing.  This message will
			 * be sent to the user via a message box during atExit(), and the debug
			 * log will be brought up.
			 */
			bool fatal( const char* message, ... );

      /**
       * Redirects output from the debugger.  This method must remain valid until
       * another stream is swapped in or DEBUG_ATEXIT is called.
       */
      void changeOutputStream( dc::dcStreamOut* stream );

      const std::string& getLogFilename() const;

		public:

			/**
			 * Call this method when the program is shutting down to prompt the user
			 * with any fatal errors that occurred.  If no fatal errors occurred,
			 * this will not do anything.
			 */
			void atExit();


		protected:

			/// The debug log file name
			std::string myOutputLogFile;

			/// Whether or not this class is the dummy (null) debugger
			bool thisIsNull;

			/// The C++ source file from which debug info is coming
			const char* myFile;

			/// The line for debug output
			unsigned long myLine;

			/// The output stream for this debug log
      dcStreamOut* myOutputStream;

			/// The fatal error that triggered the shutdown
			std::string myFatalError;
	};

}



#endif