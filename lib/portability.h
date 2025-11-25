
#pragma once

typedef int pid_t;


#define _SIGSET_NWORDS (1024 / (8 * sizeof (unsigned long int)))
typedef struct
{
  unsigned long int __val[_SIGSET_NWORDS];
} __sigset_t;

typedef void (*sighandler_t) (int);

typedef struct s_sigact {
	union
	  {
	/* Used if SA_SIGINFO is not set.  */
	sighandler_t _sa_handler;
	/* Used if SA_SIGINFO is set.  */ // we can ignore it as long as we don't use SA_SIGINFO duh
	// void (*sa_sigaction) (int, siginfo_t *, void *);
	  }
	__sigaction_handler;
	# define sa_handler	__sigaction_handler._sa_handler
	# define sa_sigaction	__sigaction_handler._sa_sigaction

	/* Additional set of signals to be blocked.  */
	__sigset_t sa_mask;

	/* Special flags.  */
	int sa_flags;

	/* Restore handler.  */
	/* NOTE: Deprecated. Don't use as it will be ignored */
	void (*sa_restorer) (void);
} sigaction_t;
