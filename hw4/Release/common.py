from collections import namedtuple

Result = namedtuple('Result', ['packets', 'fingerprint', 'experiment', 'load', 'contains', 'n', 'w', 'tableType'])

def generate_subprocess_array(v=0, n=1, M=5000, W=4000, i=16, H=0.9, a=0.5, r=0.1, T=0):
    return ["./ParallelHW4", "-v", str(v), "-n", str(n), "-M", str(M), "-W", str(W), "-i", str(i),
                '-H', str(H), '-a', str(a), '-r', str(r), '-T', str(T)]


def loadConfigToString(loadConfig):
    if loadConfig == 0: return 'MOSTLY_READS'
    if loadConfig == 1: return 'MOSTLY_WRITES'
    else: return 'NO_LOAD'


def tableTypeToString(tableType):
     if tableType is 0: return 'COARSE_TABLE'
     if tableType == 1: return 'CONCURRENT_TABLE'
     else: return "NO TABLE"


def parse_results(res, e, load, numThreads, work, contains, tableType):
    arr = res.split('\n')
    r = Result(packets=int(arr[0]), fingerprint=arr[1], experiment=e, load=load, n=numThreads, w=work, contains=contains, tableType=tableType)
    return r
