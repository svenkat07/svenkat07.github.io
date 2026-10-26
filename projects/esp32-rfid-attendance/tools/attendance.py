"""Replay JSONL access decisions to a local mock or explicitly enabled Firebase sink."""
import argparse
import hashlib
import json
import os
import sys
import urllib.request
import urllib.parse
from pathlib import Path

def records(lines, session):
    if not session or any(x not in 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-' for x in session):
        raise ValueError('session must contain only letters, digits, underscores or hyphens')
    seen=set()
    for line in lines:
        e=json.loads(line)
        if e['decision']!='granted':continue
        if not isinstance(e['uptime_ms'],int) or e['uptime_ms']<0:raise ValueError('invalid uptime')
        if e['subject'] not in ('demo_user_01','demo_user_02'):raise ValueError('unknown demo subject')
        if not isinstance(e['simulated'],bool):raise ValueError('missing simulation label')
        # Same event/session produces the same key, allowing idempotent PUT retries.
        key=hashlib.sha256(f"{session}:{e['uptime_ms']}:{e['subject']}".encode()).hexdigest()[:24]
        if key in seen:continue
        seen.add(key)
        yield key,dict(e,session=session,record_type='access_grant_not_confirmed_entry',server_received_at={'.sv':'timestamp'})

def main():
    p=argparse.ArgumentParser();p.add_argument('input',type=Path);p.add_argument('--session',required=True)
    p.add_argument('--send',action='store_true',help='Actually send to your configured Firebase database')
    p.add_argument('--output',type=Path,default=Path('examples/mock_firebase.json'));a=p.parse_args()
    events=dict(records(a.input.read_text().splitlines(),a.session))
    if not a.send:
        a.output.parent.mkdir(parents=True,exist_ok=True)
        a.output.write_text(json.dumps({'mock_only':True,'attendance_demo':events},indent=2)+'\n')
        print(f'LOCAL MOCK ONLY: {len(events)} grant records; no network requests')
        return
    base=os.environ['FIREBASE_DATABASE_URL'].rstrip('/')
    token=os.environ['FIREBASE_ID_TOKEN']
    u=urllib.parse.urlparse(base)
    if u.scheme!='https' or not u.hostname or not (u.hostname.endswith('.firebaseio.com') or u.hostname.endswith('.firebasedatabase.app')) or u.path not in ('','/') or u.query or u.fragment:
        raise ValueError('Expected HTTPS Firebase Realtime Database root URL')
    for key,value in events.items():
        url=f'{base}/attendance_demo/{key}.json?auth={urllib.parse.quote(token,safe="")}'
        req=urllib.request.Request(url,data=json.dumps(value).encode(),method='PUT',headers={'Content-Type':'application/json'})
        try:
            with urllib.request.urlopen(req,timeout=10) as response:
                if response.status!=200:raise RuntimeError('Unexpected Firebase status')
        except Exception:
            # Do not print exception URLs, because the query contains a token.
            print('Firebase write failed; stopped. Check token/rules/network. Re-run same session to retry.',file=sys.stderr)
            sys.exit(1)
    print(f'Firebase acknowledged {len(events)} writes')

if __name__=='__main__':main()
