/*
 *   Copyright (C) 2021-2023 by C.H. Huang
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

#include <errno.h>
#include <stdlib.h>
#include <SqThread.h>

#if defined(_WIN32) || defined(_WIN64)
/* ------ Windows ------ */

#include <windows.h>
#include <process.h>    // _beginthreadex

int  sq_thread_create(SqThread *thread, SqThreadFunc func, void *data)
{
	thread->data = _beginthreadex(NULL, 0,    // stack size
			(_beginthreadex_proc_type) func, data,
			0, NULL);
	if (thread->data)
		return SQ_THREAD_OK;
	return 1;
}

int   sq_thread_join(SqThread *thread)
{
	DWORD result;

	result = WaitForSingleObject((HANDLE) thread->data, INFINITE);
	if (result == WAIT_FAILED || result == WAIT_TIMEOUT)
		return 1;
	return SQ_THREAD_OK;
}

void  sq_thread_unjoin(SqThread *thread)
{
	// free resource after thread exit
	CloseHandle((HANDLE) thread->data);
}

SqThreadData  sq_thread_self(void)
{
	return GetCurrentThreadId();
}

/* ------ mutex ------ */
void  sq_mutex_init(SqMutex *mutex)
{
	mutex->data = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(mutex->data);
}

void  sq_mutex_clear (SqMutex *mutex)
{
	DeleteCriticalSection(mutex->data);
	free(mutex->data);
}

int  sq_mutex_trylock(SqMutex *mutex)
{
	BOOL result = TryEnterCriticalSection(mutex->data);
	if (result == FALSE)
		return EBUSY;
	return 0;
}

void  sq_mutex_lock(SqMutex *mutex)
{
	EnterCriticalSection(mutex->data);
}

void  sq_mutex_unlock(SqMutex *mutex)
{
	LeaveCriticalSection(mutex->data);
}

/* ------ readers-writer lock ------ */
void  sq_rwlock_init(SqRwlock *rwlock)
{
	rwlock->data = malloc(sizeof(SRWLOCK));
	InitializeSRWLock(rwlock->data);
}

void  sq_rwlock_clear(SqRwlock *rwlock)
{
//	DeleteSRWLock(mutex->data);
	free(rwlock->data);
}

int   sq_rwlock_reader_trylock(SqRwlock *rwlock)
{
	BOOLEAN result = TryAcquireSRWLockShared(rwlock->data);
	if (result == FALSE)
		return EBUSY;
	return 0;
}

void  sq_rwlock_reader_lock(SqRwlock *rwlock)
{
	AcquireSRWLockShared(rwlock->data);
}

void  sq_rwlock_reader_unlock(SqRwlock *rwlock)
{
	ReleaseSRWLockShared(rwlock->data);
}

int   sq_rwlock_writer_trylock(SqRwlock *rwlock)
{
	BOOLEAN result = TryAcquireSRWLockExclusive(rwlock->data);
	if (result == FALSE)
		return EBUSY;
	return 0;
}

void  sq_rwlock_writer_lock(SqRwlock *rwlock)
{
	AcquireSRWLockExclusive(rwlock->data);
}

void  sq_rwlock_writer_unlock(SqRwlock *rwlock)
{
	ReleaseSRWLockExclusive(rwlock->data);
}

#endif  // _WIN32 || _WIN64
