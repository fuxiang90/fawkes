
/***************************************************************************
 *  exception.cpp - basic exception
 *
 *  Generated: Thu Feb 09 13:04:45 2006 (from FireVision)
 *  Copyright  2005-2006  Tim Niemueller [www.niemueller.de]
 *
 *  $Id$
 *
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1307, USA.
 */

#include <core/exception.h>
#include <core/threading/mutex.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/** @class Exception core/exception.h
 * Base class for exceptions in Fawkes.
 * Exceptions are a good way to handle errors. If you choose to use
 * exceptions use derivates of this class so that there is a unified way of
 * handling errors in Fawkes. Do <i>not</i> throw an arbitrary class such as
 * a string or integer as this is hard to handle.
 *
 * For your exceptions in general you only need to override the constructor
 * and call the Exception constructor with the appropriate message. In
 * cases where more information is needed about the error add appropriate
 * handlers.
 *
 * In most cases it is bad to just throw an Exception like this:
 *
 * @code
 * if ( error_condition ) {
 *   throw Exception("Out of memory");
 * }
 * @endcode
 *
 * Rather you should explicitly subclass Exception appropriately. For the
 * above example you could have something like this as exception class:
 *
 * @code
 * class OutOfMemException : public Exception
 * {
 *  public:
 *   OutOfMemoryException() : Exception("Out of memory") {}
 * }
 * @endcode
 *
 * And in your handling code you throw a OutOfMemoryException. This is
 * especially useful if it is possible to throw several different exceptions.
 * If the message was different you would have to parse the string for
 * the exact error. This can be avoided if you just catch different
 * exceptions. This is also useful if the Exception is not catched explicitly
 * as this will printout the name of the exception class thrown just before
 * exiting the program. And reading something like
 * "terminate called after throwing an instance of 'OutOfMemoryException'"
 * makes it a lot easier to spot the problem.
 *
 * Exceptions should be catched by reference like this:
 * @code
 * try {
 *   some_operation();
 * } catch (OutOfMemoryException &e) {
 *   std::cout << e.c_str() << std::endl;
 *   error_handling();
 * }
 * @endcode
 *
 * Messages are stored as list. You can utilize this by adding appropriate
 * information through appropriate try/catch statements. This way you can
 * build information path ways that will help to debug your software. Use
 * block like this to append information:
 * @code
 * try {
 *   potentially_failing();
 * } catch {MyException &e) {
 *   e.append("info where exception happened");
 *   throw; // re-throw exception
 * }
 * @endcode
 * This is especially useful if the exception may occur at several different
 * places and it cannot be fixed where it happens.
 *
 *
 * @see example_exception.cpp
 * @ingroup FCL
 * @ingroup Exceptions
 *
 * @author Tim Niemueller
 */
/** @var Exception::messages
 * List of messages. Should not be NULL. Messages are append with append().
 * Using a custom list to avoid including STL stuff in this core file.
 * @see append()
 */
/** @var Exception::messages_iterator
 * Iterator to iterate over messages
 */
/** @var Exception::messages_end
 * Pointer that points to the very last message. Used for fast appending.
 */
/** @var Exception::messages_mutex
 * Mutex to protect operations on messages list.
 */
/** @var Exception::_errno
 * Error number, should be used if the error was caused by a method that supplies
 * errno.
 */


/** Constructor.
 * Constructs a new exception with the given message.
 * @param msg The message that this exception should carry. The message is
 * copied and not just referenced. Thus the memory can be freed if it is a
 * string on the heap.
 */
Exception::Exception(const char *msg)
{
  messages_mutex = new Mutex();
  messages_mutex->lock();

  _errno = 0;

  messages = NULL;
  messages_end = NULL;
  messages_iterator = NULL;

  if ( msg != NULL ) {
    append_nolock(msg);
  } else {
    append_nolock("Basic Exception");
  }

  messages_mutex->unlock();
}


/** Constructor.
 * Constructs a new exception with the given message and errno value. This
 * is particularly handy when throwing the exception after a function failed
 * that returns an error code in errno. 
 * @param msg The message that this exception should carry. The message is
 * copied and not just referenced. Thus the memory can be freed if it is a
 * @param errno error number
 * string on the heap.
 */
Exception::Exception(const char *msg, int errno)
{
  messages_mutex = new Mutex();
  messages_mutex->lock();

  _errno = errno;

  messages = NULL;
  messages_end = NULL;
  messages_iterator = NULL;

  if ( msg != NULL ) {
    append_nolock(msg);
  } else {
    append_nolock("Basic Exception");
  }

  char *errno_msg;
  char err[200];
  char *err_s = strerror_r(errno, err, 200);
  asprintf(&errno_msg, "errno: %i   msg: %s", errno, err_s);
  append_nolock_nocopy(errno_msg);

  messages_mutex->unlock();
}


/** Copy constructor.
 * The copy constructor is worth some extra discussion. If you do an exception
 * by value (which you shouldn't in the first place since this will generate a
 * copy, only do this if you can't avoid it for some reason. Not if you only
 * THINK that you can't avoid it) the copy constructor is called. If your catch
 * statements reads like
 * @code
 *   try {
 *     ...
 *   } catch (Exception e) {
 *     ...
 *   }
 * @endcode
 * then a copy will be created for the catch block. You throw the exception with
 * something like
 * @code
 *   throw Exception("Boom");
 * @endcode
 * This will create an Exception which is valid in the block where you throw the
 * exception. Now for the catch block a copy is created. Since the exception
 * holds a pointer on the heap the implicit copy constructor would just copy
 * the pointer, not the data. So both exceptions point to the same data (to the
 * message for the base exception). If now both destructors for the exception
 * are called they both try to free the very same memory. Of course the second
 * destructor will cause a disaster. If you are lucky your glibc detectes the
 * problem an kills the application. If you are not that fortunate you will
 * cause very strange behaviour of your application.
 *
 * In general you should not have to worry about this. But if you choose to have
 * own storage on the heap using either new, malloc or a method that returns
 * memory on the heap (like strdup()) you have to write your own copy contructor
 * and copy the memory area or take care that only one exception frees the memory.
 * @param exc Exception to copy
 */
Exception::Exception(const Exception &exc)
{
  messages_mutex = new Mutex();

  _errno = exc._errno;
  copy_messages(exc);
}


/** Constructor for subclasses.
 * This constructor can be used in subclasses is some processing code is
 * needed (like sprintf) to assign the message. At least assign the empty
 * string to the message.
 */
Exception::Exception()
{
  messages_mutex = new Mutex();
  _errno = 0;
  messages = NULL;
  messages_end = NULL;
  messages_iterator = NULL;
}


/** Destructor. */
Exception::~Exception()
{
  message_list_t *msg_this;
  messages_iterator = messages;
  while ( messages_iterator ) {
    free(messages_iterator->msg);
    msg_this = messages_iterator;
    messages_iterator = messages_iterator->next;
    free(msg_this);
  }
  messages = NULL;
  messages_end = NULL;
  delete messages_mutex;
}


/** Append messages to the message list.
 * @param format format of the message to append, see printf(3) for details about formatting
 * options.
 */
void
Exception::append(const char *format, ...)
{
  // do not append empty messages
  if (format == NULL)  return;

  va_list arg;
  va_start(arg, format);
  messages_mutex->lock();
  append_nolock(format, arg);
  messages_mutex->unlock();
  va_end(arg);
}


/** Append messages without lock.
 * this can be used to append messages without locking the mutex if the mutex
 * has been locked already to append many messages and keep their order intact
 * and thus to prevent messages to be appended inbetween.
 * Used for example in copy constructor.
 * @param msg message to append
 */
void
Exception::append_nolock(const char *msg)
{
  if ( messages == NULL ) {
    // This is our first message
    messages = (message_list_t *)malloc(sizeof(message_list_t));
    messages->next = NULL;
    messages->msg  = strdup(msg);
    messages_end = messages;
  } else {
    message_list_t *ml = (message_list_t *)malloc(sizeof(message_list_t));
    ml->next = NULL;
    ml->msg = strdup(msg);
    messages_end->next = ml;
    messages_end = ml;
  }
}


/** Append messages without lock by formatted string.
 * this can be used to append messages without locking the mutex if the mutex
 * has been locked already to append many messages and keep their order intact
 * and thus to prevent messages to be appended inbetween.
 * Used for example in copy constructor.
 * @param format format of the message to be appended
 * @param ap argument va_list for format
 */
void
Exception::append_nolock(const char *format, va_list ap)
{
  char *msg;
  vasprintf(&msg, format, ap);

  if ( messages == NULL ) {
    // This is our first message
    messages = (message_list_t *)malloc(sizeof(message_list_t));
    messages->next = NULL;
    messages->msg  = msg;
    messages_end = messages;
  } else {
    message_list_t *ml = (message_list_t *)malloc(sizeof(message_list_t));
    ml->next = NULL;
    ml->msg = msg;
    messages_end->next = ml;
    messages_end = ml;
  }
}


/** Append message without copying.
 * Can be used in subclasses to append messages that have been allocated
 * on the heap. Use with extreme care. Do not add constant strings! This would
 * cause your application to crash since the destructor will try to free all
 * messages. The message list is not locked.
 * @param msg Message to append.
 */
void
Exception::append_nolock_nocopy(char *msg)
{
  if ( messages == NULL ) {
    // This is our first message
    messages = (message_list_t *)malloc(sizeof(message_list_t));
    messages->next = NULL;
    messages->msg  = msg;
    messages_end = messages;
  } else {
    message_list_t *ml = (message_list_t *)malloc(sizeof(message_list_t));
    ml->next = NULL;
    ml->msg = msg;
    messages_end->next = ml;
    messages_end = ml;
  }  
}


/** Assign an Exception.
 * As this is one of the Big Three (see C++ FAQ at
 * http://www.parashift.com/c++-faq-lite/coding-standards.html#faq-27.10) this
 * is needed because we already need a copy constructor. Read about the
 * copy constructor why this is the case.
 * @see Exception(const Exception &exc)
 * @param exc The exception with the values to assign to this exception.
 * @return reference to this object. Allows assignment chaining.
 */
Exception &
Exception::operator=(const Exception &exc)
{
  messages_mutex = new Mutex();
  copy_messages(exc);

  return *this;
}


/** Copy messages from given exception.
 * Copies the messages from exc to this exception.
 * @param exc Exception to copy messages from.
 */
void
Exception::copy_messages(const Exception &exc)
{
  messages_mutex->lock();
  exc.messages_mutex->lock();

  // copy messages
  messages_iterator = exc.messages;
  while ( messages_iterator ) {
    append_nolock(messages_iterator->msg);
    messages_iterator = messages_iterator->next;
  }

  exc.messages_mutex->unlock();
  messages_mutex->unlock();
}


/** Get the first message as C string.
 * Messages are stored in a list. To retrieve the first message and thus
 * the original message which was given when the exception was created it
 * should be the most informative message.
 * @return Returns a constant char array with the message. The message is
 * private to the exception and may not be modified or freed (hence const)
 */
const char *
Exception::c_str()
{
  if ( messages != NULL ) {
    return messages->msg;
  } else {
    return "No exception message available";
  }
}


/** This can be used to throw this exception.
 * This can be used to throw this exception instance. This is a precaution if
 * it is needed. See C++ FAQ 17.10.
 */
void
Exception::raise()
{
  throw *this;
}


/** Prints trace to stderr.
 * This prints out a message trace of all messages appended to the exception
 * in chronological order starting with the oldest (first message appended
 * via constructor or append(). Output will be sent to stderr.
 */
void
Exception::printTrace()
{
  messages_mutex->lock();
  fprintf(stderr, "Exception trace\n"
	  "============================================================================\n");
  if ( messages == NULL ) {
    fprintf(stderr, "No messages recorded.\n");
  } else {
    messages_iterator = messages;
    while ( messages_iterator ) {
      fprintf(stderr, "%s\n", messages_iterator->msg);
      messages_iterator = messages_iterator->next;
    }
  }
  fprintf(stderr,
	  "============================================================================\n");
  messages_mutex->unlock();
}


/** Get errno.
 * @return error number, may be 0 if not set
 */
int
Exception::errno() const
{
  return _errno;
}