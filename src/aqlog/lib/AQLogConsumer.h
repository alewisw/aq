#ifndef AQLOGCONSUMER_H
#define AQLOGCONSUMER_H
//==============================================================================
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0.If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//==============================================================================

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Macros
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Type Definitions
//------------------------------------------------------------------------------

// Forward declarations.
class IAQSharedMemory;
namespace aqlog
{
    class LogDispatcher;
};




//------------------------------------------------------------------------------
// Exported Variable Declarations
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
// Exported Function and Class Declarations
//------------------------------------------------------------------------------

/**
 * A log consumer is used to read log records from an allocating queue and 
 * process those records by dispatching them to the appropriate log handlers
 * with the correctly formatted messages.
 */
class AQLogConsumer
{
public:

    /**
     * Constructs a new log consumer that uses the passed shared memory region
     * to contain the log records and log level hash.  This shared memory 
     * region must be available to all log producers.
     *
     * @param sm The shared memory region containing the log records and log
     * level hash.
     */
    AQLogConsumer(IAQSharedMemory& sm);

    /**
     * Destroys this log consumer.  This does not impact the content of the
     * shared memory region.
     */
    virtual ~AQLogConsumer(void);

private:
    AQLogConsumer(const AQLogConsumer& other);
    AQLogConsumer& operator=(const AQLogConsumer& other);

private:

    // The actual log record dispatcher.
    aqlog::LogDispatcher *m_logDispatcher;

public:

    /**
     * Starts the execution of this log consumer in an asynchronous thread.  
     * This function returns immediatly and log records start to be consumed 
     * in parallel with the normal execution of the program.  Do not call
     * execute() if startAsync() has been called, and likewise never call
     * startAsync() if execute() has been called.
     *
     * Once this function has been called, the stop() function must be called
     * to terminate the thread.
     *
     * @throws domain_error If the consumer is already running either due to
     * a previous call to startAsync() or due to a call to execute().
     */
    void startAsync(void);

    /**
     * Executes the log consumer in the context of the current thread.  This
     * function does not return until stop() is called, which may be called
     * from another thread.  Do not call execute() if startAsync() has been 
     * called, and likewise never call startAsync() if execute() has been called.
     *
     * @throws domain_error If the consumer is already running either due to
     * a previous call to startAsync() or due to a call to execute().
     */
    void execute(void);

};




#endif
//=============================== End of File ==================================
