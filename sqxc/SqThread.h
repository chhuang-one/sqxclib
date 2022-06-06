/*
 *   Copyright (C) 2021-2022 by C.H. Huang
 *   plushuang.tw@gmail.com
 *
 * sqxclib is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#ifndef SQ_THREAD_H
#define SQ_THREAD_H

#include <SqConfig.h>
#if SQ_CONFIG_HAVE_THREAD || defined(_WIN32) || defined(_WIN64)

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

// ----------------------------------------------------------------------------
// C/C++ common declarations: declare type, structue, macro, enumeration.

typedef struct SqThread    SqThread;
typedef struct SqMutex     SqMutex;
typedef struct SqRwlock    SqRwlock;

// ----------------------------------------------------------------------------
// C declarations: declare C data, function, and others.

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
/* ------ Windows ------ */

typedef unsigned              SqThreadResult;
typedef uintptr_t             SqThreadData;
typedef LPCRITICAL_SECTION    SqMutexData;
typedef PSRWLOCK              SqRwlockData;

// This function must return SQ_THREAD_RESULT
typedef SqThreadResult (*SqThreadFunc)(void*);

#define SQ_THREAD_RESULT   0
#define SQ_THREAD_OK       0

/* ------ thread ------ */
// sq_thread_create() and sq_thread_join() return SQ_THREAD_OK if successful
int   sq_thread_create(SqThread *thread, SqThreadFunc func, void *data);
int   sq_thread_join(SqThread *thread);
void  sq_thread_unjoin(SqThread *thread);
SqThreadData  sq_thread_self(void);

/* ------ mutex ------ */
void  sq_mutex_init(SqMutex *mutex);
void  sq_mutex_clear(SqMutex *mutex);
int   sq_mutex_trylock(SqMutex *mutex);
void  sq_mutex_lock(SqMutex *mutex);
void  sq_mutex_unlock(SqMutex *mutex);

/* ------ readers-writer lock ------ */
void  sq_rwlock_init(SqRwlock *rwlock);
void  sq_rwlock_clear(SqRwlock *rwlock);
int   sq_rwlock_reader_trylock(SqRwlock *rwlock);
void  sq_rwlock_reader_lock(SqRwlock *rwlock);
void  sq_rwlock_reader_unlock(SqRwlock *rwlock);
int   sq_rwlock_writer_trylock(SqRwlock *rwlock);
void  sq_rwlock_writer_lock(SqRwlock *rwlock);
void  sq_rwlock_writer_unlock(SqRwlock *rwlock);

#else
/* ------ pthread ------ */

typedef void*               SqThreadResult;
typedef pthread_t           SqThreadData;
typedef pthread_mutex_t     SqMutexData;
typedef pthread_rwlock_t    SqRwlockData;

// This function must return SQ_THREAD_RESULT
typedef SqThreadResult (*SqThreadFunc)(void*);

#define SQ_THREAD_RESULT   NULL
#define SQ_THREAD_OK       0

/* ------ thread ------ */
// sq_thread_create() and sq_thread_join() return SQ_THREAD_OK if successful
// int  sq_thread_create(SqThread *thread, SqThreadFunc func, void *data);
#define sq_thread_create(thread, func, data)    \
		pthread_create(&(thread)->data, NULL, func, data)

// int  sq_thread_join(SqThread *thread);
#define sq_thread_join(thread)    \
		pthread_join((thread)->data, NULL)

// void sq_thread_unjoin(SqThread *thread);
#define sq_thread_unjoin(thread)    \
		pthread_detach((thread)->data)

// SqThreadData  sq_thread_self(void);
#define sq_thread_self          pthread_self

/* ------ mutex ------ */
// void sq_mutex_init(SqMutex *mutex);
#define sq_mutex_init(mutex)      pthread_mutex_init(&(mutex)->data, NULL)

// void sq_mutex_clear(SqMutex *mutex);
#define sq_mutex_clear(mutex)     pthread_mutex_destroy(&(mutex)->data)

// int  sq_mutex_trylock(SqMutex *mutex);
#define sq_mutex_trylock(mutex)   pthread_mutex_trylock(&(mutex)->data)

// void sq_mutex_lock(SqMutex *mutex);
#define sq_mutex_lock(mutex)      pthread_mutex_lock(&(mutex)->data)

// void sq_mutex_unlock(SqMutex *mutex);
#define sq_mutex_unlock(mutex)    pthread_mutex_unlock(&(mutex)->data)

/* ------ readers-writer lock ------ */
// void sq_rwlock_init(SqRwlock *rwlock);
#define sq_rwlock_init(rwlock)             pthread_rwlock_init(&(rwlock)->data, NULL)

// void sq_rwlock_clear(SqRwlock *rwlock);
#define sq_rwlock_clear(rwlock)            pthread_rwlock_destroy(&(rwlock)->data)

// int  sq_rwlock_reader_trylock(SqRwlock *rwlock);
#define sq_rwlock_reader_trylock(rwlock)   pthread_rwlock_tryrdlock(&(rwlock)->data)

// void sq_rwlock_reader_lock(SqRwlock *rwlock);
#define sq_rwlock_reader_lock(rwlock)      pthread_rwlock_rdlock(&(rwlock)->data)

// void sq_rwlock_reader_unlock(SqRwlock *rwlock);
#define sq_rwlock_reader_unlock(rwlock)    pthread_rwlock_unlock(&(rwlock)->data)

// int  sq_rwlock_writer_trylock(SqRwlock *rwlock);
#define sq_rwlock_writer_trylock(rwlock)   pthread_rwlock_trywrlock(&(rwlock)->data)

// void sq_rwlock_writer_lock(SqRwlock *rwlock);
#define sq_rwlock_writer_lock(rwlock)      pthread_rwlock_wrlock(&(rwlock)->data)

// void sq_rwlock_writer_unlock(SqRwlock *rwlock);
#define sq_rwlock_writer_unlock(rwlock)    pthread_rwlock_unlock(&(rwlock)->data)

#endif   // _WIN32 || _WIN64


#ifdef __cplusplus
}  // extern "C"
#endif

// ----------------------------------------------------------------------------
// C/C++ common definitions: define structue

/* ------ SqThread ------ */
struct SqThread
{
	SqThreadData  data;

#ifdef __cplusplus
	// C++11 standard-layout

	int  create(SqThreadFunc func, void *data) {
		return sq_thread_create(this, func, data);
	}
	int  join(void) {
		return sq_thread_join(this);
	}
	void unjoin(void) {
		sq_thread_unjoin(this);
	}

	SqThread &operator=(SqThreadData rvalue) {
		this->data = rvalue;
		return *this;
	}
	bool operator==(SqThreadData rvalue) {
		return (this->data == rvalue);
	}

	static inline SqThreadData current(void) {
		return sq_thread_self();
	}
	static inline SqThreadData self(void) {
		return sq_thread_self();
	}
#endif  // __cplusplus
};

/* ------ SqMutex ------ */
struct SqMutex
{
	SqMutexData   data;

#ifdef __cplusplus
	// C++11 standard-layout

	SqMutex() {
		sq_mutex_init(this);
	}
	~SqMutex() {
		sq_mutex_clear(this);
	}

	int   trylock(void) {
		return sq_mutex_trylock(this);
	}
	void  lock(void) {
		sq_mutex_lock(this);
	}
	void  unlock(void) {
		sq_mutex_unlock(this);
	}

	SqMutex &operator=(SqMutexData rvalue) {
		this->data = rvalue;
		return *this;
	}
	bool operator==(SqMutexData rvalue) {
		return (this->data == rvalue);
	}
#endif  // __cplusplus
};

/* ------ SqRwlock ------ */
struct SqRwlock
{
	SqRwlockData  data;

#ifdef __cplusplus
	// C++11 standard-layout

	SqRwlock() {
		sq_rwlock_init(this);
	}
	~SqRwlock() {
		sq_rwlock_clear(this);
	}

	int   readerTrylock(void) {
		return sq_rwlock_reader_trylock(this);
	}
	void  readerLock(void) {
		sq_rwlock_reader_lock(this);
	}
	void  readerUnlock(void) {
		sq_rwlock_reader_unlock(this);
	}
	int   writerTrylock(void) {
		return sq_rwlock_writer_trylock(this);
	}
	void  writerLock(void) {
		sq_rwlock_writer_lock(this);
	}
	void writerUnlock(void) {
		sq_rwlock_writer_unlock(this);
	}

	SqRwlock &operator=(SqRwlockData rvalue) {
		this->data = rvalue;
		return *this;
	}
	bool operator==(SqRwlockData rvalue) {
		return (this->data == rvalue);
	}
#endif  // __cplusplus
};

// ----------------------------------------------------------------------------
// C++ definitions: define C++ data, function, method, and others.

#ifdef __cplusplus

namespace Sq {

/* --- define C++11 standard-layout structures --- */
typedef struct SqThread    Thread;
typedef struct SqMutex     Mutex;
typedef struct SqRwlock    Rwlock;

}  // namespace Sq

#endif  // __cplusplus


#endif  // SQ_CONFIG_HAVE_THREAD || _WIN32 || _WIN64

#endif  // SQ_THREAD_H
