import subprocess
import pandas as pd
import sys
from subprocess import PIPE
import signal


def pf():
    signal.signal(signal.SIGINT, signal.SIG_IGN)


matmultc = './matmultc'
matmultcpp = './matmultcpp'
start_size = 1000
end_size = 10000
size_block = 1000
thread_nums = [1, 2, 4, 8, 16, 32]
test_num = 10
xls_name = './benchmark_results.xlsx'
log_file = './benchmark.log'

datac = {}
datacpp = {}
xls_datac = {'threads': thread_nums}
xls_datacpp = {'threads': thread_nums}

log = open(log_file, mode='a')

for size in range(start_size, end_size + size_block, size_block):
    log.write(f'Size {size}:\n')
    log.flush()
    datac[f'{size}'] = {}
    datacpp[f'{size}'] = {}
    for thread_num in thread_nums:
        log.write(f'\tThreads {thread_num}: ')
        log.flush()
        datac[f'{size}'][f'{thread_num}'] = []
        datacpp[f'{size}'][f'{thread_num}'] = []
        for _ in range(0, test_num):
            if sys.version_info.minor > 6:
                retc = subprocess.run(
                    ['nohup', matmultc, f'{size}', f'{thread_num}'], capture_output=True, preexec_fn=pf)
                retcpp = subprocess.run(
                    ['nohup', matmultcpp, f'{size}', f'{thread_num}'], capture_output=True, preexec_fn=pf)
            else:
                retc = subprocess.run(
                    ['nohup', matmultc, f'{size}', f'{thread_num}'], stdout=PIPE, stderr=PIPE, preexec_fn=pf)
                retcpp = subprocess.run(
                    ['nohup', matmultcpp, f'{size}', f'{thread_num}'], stdout=PIPE, stderr=PIPE, preexec_fn=pf)
            timec = float(retc.stdout.decode().replace('\n', ''))
            timecpp = float(retcpp.stdout.decode().replace('\n', ''))
            datac[f'{size}'][f'{thread_num}'].append(timec)
            datacpp[f'{size}'][f'{thread_num}'].append(timecpp)
            log.write(f'{timec}/{timecpp}   ')
            log.flush()
        log.write('\n')
        log.flush()
log.write('Get all data.\n')
log.flush()
log.write('Compute results.\n')
log.flush()
for size in range(start_size, end_size + size_block, size_block):
    xls_datac[f'{size}'] = []
    xls_datacpp[f'{size}'] = []
    for thread_num in thread_nums:
        timec = 0
        timecpp = 0
        datac[f'{size}'][f'{thread_num}'].remove(min(datac[f'{size}'][f'{thread_num}']))
        datac[f'{size}'][f'{thread_num}'].remove(max(datac[f'{size}'][f'{thread_num}']))
        datacpp[f'{size}'][f'{thread_num}'].remove(min(datacpp[f'{size}'][f'{thread_num}']))
        datacpp[f'{size}'][f'{thread_num}'].remove(max(datacpp[f'{size}'][f'{thread_num}']))
        for time in datac[f'{size}'][f'{thread_num}']:
            timec += time
        for time in datacpp[f'{size}'][f'{thread_num}']:
            timecpp += time
        timec /= len(datac[f'{size}'][f'{thread_num}'])
        timecpp /= len(datacpp[f'{size}'][f'{thread_num}'])
        xls_datac[f'{size}'].append(timec)
        xls_datacpp[f'{size}'].append(timecpp)
log.write('Computed.\n')
log.flush()
log.write('Save results.\n')
log.flush()
xls_datac = pd.DataFrame(xls_datac)
xls_datacpp = pd.DataFrame(xls_datacpp)
writer = pd.ExcelWriter(xls_name)
xls_datac.to_excel(writer, sheet_name='C')
xls_datacpp.to_excel(writer, sheet_name='CPP')
writer.save()
log.write('Excel file saved.\n')
log.close()
