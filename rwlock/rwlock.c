#include "rwlock.h"
#include "utility.h"

int rwlock_init(rwlock_t* lock) {
    int ret = pthread_mutex_init(&lock->mutex, NULL);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_init fail", -1);
    }

    ret = pthread_cond_init(&lock->read_cv, NULL);
    if (ret != 0) {
        pthread_mutex_destroy(&lock->mutex);
        ERROR_RETURN("pthread_cond_init fail", -1);
    }

    ret = pthread_cond_init(&lock->write_cv, NULL);
    if (ret != 0) {
        pthread_mutex_destroy(&lock->mutex);
        pthread_cond_destroy(&lock->read_cv);
        ERROR_RETURN("pthread_cond_init fail", -1);
    }

    lock->readers = lock->writers = 0;
    lock->read_waiters = lock->write_waiters = 0;
    lock->valid = LOCK_VALID;

    return 0;
}

int rwlock_destroy(rwlock_t* lock) {
    if (lock->valid != LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthred_mutex_lock fail", -1);
    }

    if (lock->readers > 0 || lock->read_waiters > 0) {
        pthread_mutex_unlock(&lock->mutex);
        return EBUSY;
    }

    if (lock->writers > 0 || lock->write_waiters > 0) {
        pthread_mutex_unlock(&lock->mutex);
        return EBUSY;
    }

    lock->valid = LOCK_INVALID;

    ret = pthread_mutex_unlock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_unlock fail", -1);
    }

    ret = pthread_mutex_destroy(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_destroy fail", -1);
    }

    ret = pthread_cond_destroy(&lock->read_cv);
    if (ret != 0) {
        ERROR_RETURN("pthread_cond_destroy fail on read_cv", -1);
    }

    ret = pthread_cond_destroy(&lock->write_cv);
    if (ret != 0) {
        ERROR_RETURN("pthread_cond_destroy fail on write_cv", -1);
    }

    return 0;
}

/*
 * rwlock_readcleanup
 * Handle cleanup when the read lock condition wait is cancelled.
 */
static void rwlock_readcleanup(void* arg) {
    rwlock_t* lock = (rwlock_t *)(arg);

    lock->read_waiters--;
    pthread_mutex_unlock(&lock->mutex);
}

/*
 * rwlock_writecleanup
 * Handle cleanup when the write lock condition wait is cancelled
 */
static void rwlock_writecleanup(void* arg) {
    rwlock_t* lock = (rwlock_t *)(arg);

    lock->write_waiters--;
    pthread_mutex_unlock(&lock->mutex);
}

int rwlock_readlock(rwlock_t* lock) {
    if (lock->valid!= LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_lock fail", -1);
    }

    if (lock->writers > 0) {
        lock->read_waiters++;

        pthread_cleanup_push(rwlock_readcleanup, (void *)lock);

        while (lock->writers > 0) {
            ret = pthread_cond_wait(&lock->read_cv, &lock->mutex);
            if (ret != 0) {
                break;
            }
        }

        pthread_cleanup_pop(0);
        lock->read_waiters--;
    }

    if (ret == 0) {
        lock->readers++;
    }

    pthread_mutex_unlock(&lock->mutex);
    return ret;
}

int rwlock_readtrylock(rwlock_t* lock) {
    if (lock->valid != LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthrad_mutex_lock fail", -1);
    }

    if (lock->writers > 0) {
        pthread_mutex_unlock(&lock->mutex);
        return EBUSY;
    }

    lock->readers++;
    pthread_mutex_unlock(&lock->mutex);
    return 0;
}

int rwlock_readunlock(rwlock_t* lock) {
    if (lock->valid != LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_lock fail", -1);
    }

    lock->readers--;

    if (lock->readers == 0 && lock->write_waiters > 0) {
        ret = pthread_cond_signal(&lock->write_cv);
        if (ret != 0) {
            pthread_mutex_unlock(&lock->mutex);
            ERROR_RETURN("pthread_cond_signal on write_cv fail", -1);
        }
    }
    ret = pthread_mutex_unlock(&lock->mutex);
    return 0;
}

int rwlock_writelock(rwlock_t* lock) {
    if (lock->valid!= LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_lock fail", -1);
    }

    if (lock->readers > 0 || lock->writers > 0) {
        lock->write_waiters++;

        pthread_cleanup_push(rwlock_writecleanup, (void *)lock);

        while (lock->readers > 0 || lock->read_waiters > 0) {
            ret = pthread_cond_wait(&lock->write_cv, &lock->mutex);
            if (ret != 0) {
                break;
            }
        }

        pthread_cleanup_pop(0);
        lock->write_waiters--;
    }

    if (ret == 0) {
        lock->writers++;
    }

    pthread_mutex_unlock(&lock->mutex);
    return ret;
}

int rwlock_writetrylock(rwlock_t* lock) {
    if (lock->valid != LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthrad_mutex_lock fail", -1);
    }

    if (lock->writers > 0 || lock->readers > 0) {
        pthread_mutex_unlock(&lock->mutex);
        return EBUSY;
    }

    lock->writers++;
    pthread_mutex_unlock(&lock->mutex);
    return 0;
}

int rwlock_writeunlock(rwlock_t* lock) {
    if (lock->valid != LOCK_VALID) {
        return EINVAL;
    }

    int ret = pthread_mutex_lock(&lock->mutex);
    if (ret != 0) {
        ERROR_RETURN("pthread_mutex_lock fail", -1);
    }

    lock->writers--;

    if (lock->read_waiters > 0) {
        ret = pthread_cond_broadcast(&lock->read_cv);
        if (ret != 0) {
            pthread_mutex_unlock(&lock->mutex);
            ERROR_RETURN("pthread_cond_signal on read_cv fail", -1);
        }
    } else if (lock->write_waiters > 0) {
        ret = pthread_cond_signal(&lock->write_cv);
        if (ret != 0) {
            pthread_mutex_unlock(&lock->mutex);
            ERROR_RETURN("pthread_cond_signal on write_cv fail", -1);
        }
    }

    ret = pthread_mutex_unlock(&lock->mutex);
    return 0;
}
