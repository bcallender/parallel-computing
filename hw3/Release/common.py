from collections import namedtuple

Result = namedtuple('Result', ['time', 'fingerprint', 'experiment', 'type', 'strategy', 'n', 'w', 'lockType', 'exp_unf'])

CounterResult = namedtuple('CounterResult', ['parallel', 'time', 'value', 'n', 'LockType', 'experiment'])

def generate_subprocess_array(p, n, N, w, d=32, e=0, u=0, L=0, S=0):
    if p:
        return ["./ParallelHW3", '-p', "-n", str(n), "-N", str(N), "-D", str(d), "-W", str(w), "-E", str(e), "-u", str(u),
                '-L', str(L), '-S', str(S)]
    return ["./ParallelHW3", "-n", str(n), "-N", str(N), "-D", str(d), "-W", str(w), "-E", str(e), "-u", str(u),
                '-L', str(L), '-S', str(S)]


def generate_counter_subprocess_array(p=False, N=1, L=0, T=100000):
    if p:
        return ["./ParallelHW3Counter", "-p", "-N", str(N), "-L", str(L), "-T", str(T) ]
    return ["./ParallelHW3Counter", "-T", str(T)]

def expToString(exp):
    if exp == 0: return 'UNIFORM'
    else: return 'EXPONENTIAL'

def lockTypeToString(lockType):
    if lockType == 0: return 'TAS'
    elif lockType == 1: return 'BACKOFF'
    elif lockType == 2: return 'ALOCK'
    elif lockType == 3: return 'MUTEX'
    elif lockType == 4: return 'CLH'
    else: return 'NONE'


def strategyToString(strategy):
     if strategy is 0: return 'LOCK_FREE'
     elif strategy is 1: return "HOME_QUEUE"
     elif strategy is 2: return "RANDOM_QUEUE"
     elif strategy is 3: return 'AWESOME'
     else: return "BAD_STRATEGY"


def parse_results(res, e, type, numThreads, work, strategy, lockType, exp_unf):
    arr = res.split('\n')
    r = Result(time=arr[0], fingerprint=arr[1], experiment=e, type=type, n=numThreads, w=work, strategy=strategy, lockType=lockType, exp_unf=exp_unf)
    return r

def parse_counter_results(res, parallel, locktype, n, e):
    arr = res.split('\n')
    r = CounterResult(time=arr[1], parallel=parallel, value=arr[0], LockType=locktype, n=n, experiment=e)
    return r
