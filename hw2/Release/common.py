from collections import namedtuple

Result = namedtuple('Result', ['time', 'fingerprint', 'experiment', 'type', 'n', 'w'])


def generate_subprocess_array(n, t, w, d=32, e=0, v=0, p=0):
        return ["./ParallelHW2", "-n", str(n), "-T", str(t), "-D", str(d), "-W", str(w), "-E", str(e), "-v", str(v),
                '-p', str(p)]


def parse_results(res, e, type, n, w):
    arr = res.split('\n')
    r = Result(time=arr[0], fingerprint=arr[1], experiment=e, type=type, n=n, w=w)
    return r