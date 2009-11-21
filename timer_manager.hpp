/* -*- mode: c++; tab-width: 8; indent-tabs-mode: t; -*-
   vim: ts=8 sw=8 sts=8 noet:
 */
/**
 * @file timer_manager.hpp
 *
 */
#ifndef _TIMER_MANAGER_HPP_
#define _TIMER_MANAGER_HPP_

// STL libraries
#include <ctime>
#include <limits>
#include <map>

// Boost includes
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

/** @struct timer
 * @brief struct used to store timers with actions for timer manager
 */
struct timer;
typedef boost::shared_ptr<timer> timer_ptr;

class timer_manager : boost::noncopyable {
public:
	typedef unsigned long				TimerId;
	typedef time_t					Timeout;
	typedef boost::function<void *(TimerId)>	Action;

	/** @typedef std::multimap<Timeout, timer_ptr>  TimeoutMap
	 * @brief map used to store each timeout mapped to it's action.
	 * @todo probably this should be changed to use multi_index container or two separate containers to simplify searching for TimerId and for timeout.
	 * @todo for now only timeout will be used as index simplifying to search and group of actions to execute.
	 * @todo probably timers should be "rounded" when added to timer_manager to minimize wakeup count (e.g. 0.1 second timer groups)
	 */
	typedef std::multimap<Timeout, timer_ptr>	TimeoutMap;
	typedef TimeoutMap::iterator			TimeoutIterator;
	typedef TimeoutMap::const_iterator		ConstTimeoutIterator;

	static TimerId const empty;
private:
	timer_manager();
	~timer_manager();

public:
//	static timer_manager& get(); //!< timer_manager access function

public:
	TimerId add_timer(Timeout t, Action const& a); //!< add new timer with action to execute
	bool cancel_timer(TimerId id); //!< add new timer with action to execute

	void stop();	//!< stop timer manager thread

public:
	void operator()() const; //!< thread execution function for timer manager

private:
	TimeoutMap		timeouts_;
	TimerId			last_timer_;
	mutable boost::mutex	timeouts_mutex_;
	boost::condition	wait_condition_; //!< condition used by timer_manager thread to wait

	mutable boost::mutex	manager_mutex_;	//!< mutex protecting internal timer_manager state
	bool			is_stopping_;	//!< flag indicating that timer_manager is stopping
};

#endif // include guard
