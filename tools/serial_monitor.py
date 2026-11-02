"""Read the board's actual UTF-8 output and save a timestamped log. Ctrl+C stops."""
import argparse
from datetime import datetime
from pathlib import Path
import sys
import codecs
import serial

p=argparse.ArgumentParser()
p.add_argument('--port', default='COM7')
args=p.parse_args()
decoder=codecs.getincrementaldecoder('utf-8')(errors='replace')
log=Path(__file__).parent / ('serial_'+datetime.now().strftime('%Y%m%d_%H%M%S')+'.txt')
sys.stdout.reconfigure(encoding='utf-8')
print(f'{args.port}: 9600 8N1; UTF-8; logging to {log}; Ctrl+C exits.')
try:
    with serial.Serial(args.port,9600,timeout=0.2) as s, log.open('w',encoding='utf-8') as f:
        while True:
            b=s.read(max(1,s.in_waiting))
            if b:
                text=decoder.decode(b)
                print(text,end='',flush=True)
                f.write(text);f.flush()
except KeyboardInterrupt:
    print('\nStopped.')
