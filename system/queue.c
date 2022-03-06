/* queue.c - enqueue, dequeue, isempty, nonempty, et al. */

#include <xinu.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Prints out contents of a queue
 * @param q	pointer to a queue
 */
void	printqueue(struct queue *q)
{
	// print all contents from head to tail
	// format should be [(pid,key), (pid,key), ...]

	kprintf("[");
	struct qentry *curr = q->head;
	while (curr != NULL){
		if (curr->next == NULL){
			kprintf("(pid=%d)]\n", curr->pid);
		}
		else{
			kprintf("(pid=%d), ", curr->pid);
		}
		curr = curr->next;

	}
}

/**
 * Checks whether queue is empty
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	isempty(struct queue *q)
{
	if (q->size == 0){
		return true;
	}
	return false;
}

/**
 * Checks whether queue is nonempty
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	nonempty(struct queue *q)
{
	return !isempty(q);
}


/**
 * Checks whether queue is full
 * @param q	Pointer to a queue
 * @return TRUE if true, FALSE otherwise
 */
bool8	isfull(struct queue *q)
{
	// check if there are at least NPROC processes in the queue
	if (q->size >= NPROC){
		return true;
	}
	return false;
}


/**
 * Insert a process at the tail of a queue
 * @param pid	ID process to insert
 * @param q	Pointer to the queue to use
 *
 * @return pid on success, SYSERR otherwise
 */
pid32 enqueue(pid32 pid, struct queue *q)
{
        // check if queue is full and if pid is illegal, and return SYSERR if either is true
		if (isfull(q) || isbadpid(pid)){
			return SYSERR;
		}

        // allocate space on heap for a new QEntry
		struct qentry *entry = (struct qentry *)malloc(sizeof(struct qentry));

        // initialize the new QEntry
		entry->pid = pid;
		entry->prev = q->tail;
		entry->next = NULL;

        // insert into tail of queue
		if (isempty(q)){
			q->head = entry;
		}
		else{
			q->tail->next = entry;
		}
		q->tail = entry;
		q->size++;

        // return the pid on success
		return pid;
}


/**
 * Remove and return the first process on a list
 * @param q	Pointer to the queue to use
 * @return pid of the process removed, or EMPTY if queue is empty
 */
pid32 dequeue(struct queue *q)
{
        // return EMPTY if queue is empty
		if (isempty(q)){
			return EMPTY;
		}

        // get the head entry of the queue
		struct qentry *h = q->head;

        // unlink the head entry from the rest
		q->size--;
		if (isempty(q)){
			q->head = NULL;
			q->tail = NULL;
		}
		else{
			struct qentry *temp = h->next;
			temp->prev = NULL;
			q->head = temp;
		}
        // free up the space on the heap
		pid32 pid = h->pid;
		free(h, sizeof(h));

        // return the pid on success
		return pid;
}


/**
 * Finds a queue entry given pid
 * @param pid	a process ID
 * @param q	a pointer to a queue
 * @return pointer to the entry if found, NULL otherwise
 */
struct qentry *getbypid(pid32 pid, struct queue *q)
{
	// return NULL if queue is empty or if an illegal pid is given
	if (isempty(q) || isbadpid(pid)){
		return NULL;
	}

	// find the qentry with the given pid
	struct qentry *curr = q->head;
	while (curr != NULL){
		if (curr->pid == pid){ // return a pointer to it
			return curr;
		}
		curr = curr->next;
	}
	return NULL;

}

/**
 * Remove a process from the front of a queue (pid assumed valid with no check)
 * @param q	pointer to a queue
 * @return pid on success, EMPTY if queue is empty
 */
pid32	getfirst(struct queue *q)
{
	// return EMPTY if queue is empty
	if (isempty(q)){
		return EMPTY;
	}

	// remove process from head of queue and return its pid
	return dequeue(q);
}

/**
 * Remove a process from the end of a queue (pid assumed valid with no check)
 * @param q	Pointer to the queue
 * @return pid on success, EMPTY otherwise
 */
pid32	getlast(struct queue *q)
{
	// return EMPTY if queue is empty
	if (isempty(q)){
		return EMPTY;
	}

	// get the tail entry of the queue
	struct qentry *t = q->tail;

    // unlink the tail entry from the rest
	q->size--;
	if (isempty(q)){
		q->head = NULL;
		q->tail = NULL;
	}
	else{
		struct qentry *temp = t->prev;
		temp->next = NULL;
		q->tail = temp;
	}
    // free up the space on the heap
	pid32 pid = t->pid;
	free(t, sizeof(t));

    // return the pid on success
	return pid;
}



/**
 * Remove a process from an arbitrary point in a queue
 * @param pid	ID of process to remove
 * @param q	Pointer to the queue
 * @return pid on success, SYSERR if pid is not found
 */
pid32	remove(pid32 pid, struct queue *q)
{
	// return EMPTY if queue is empty
	if (isempty(q)){
		return EMPTY;
	}

	// return SYSERR if pid is illegal
	if (isbadpid(pid)){
		return SYSERR;
	}

	// remove process identified by pid parameter from the queue and return its pid
	if (pid == q->head->pid){ // if process is at head of queue
		getfirst(q);
		return pid;
	}
	else if (pid == q->tail->pid){ // if process is at tail of queue
		getlast(q);
		return pid;
	}
	else{ // if process is in middle of queue
		struct qentry *curr = q->head;
		struct qentry *temp;
		while (curr != NULL){
			if (curr->pid == pid){
				q->size--;
				temp = curr->prev;
				temp->next = curr->next;
				temp = curr->next;
				temp->prev = curr->prev;
				free(curr, sizeof(curr));
				return pid;
			}
			curr = curr->next;
		}
	}

	// if pid does not exist in the queue, return SYSERR
	return SYSERR;
}
