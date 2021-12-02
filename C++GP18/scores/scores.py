import codecs
from datetime import datetime
import logging
import requests
from requests.auth import HTTPBasicAuth
from selenium import webdriver

access_token = 'a3818a61f8617c0b0163cde8d09cb4d60da53e59e99802a422a7e6eaa16a2906'
logging.basicConfig(filename='scores.log', level=logging.INFO, format='[%(asctime)s %(levelname)s] %(message)s', datefmt='%Y-%m-%d %H:%M:%S')

def wrap(str):
    for (a,b) in [('"', '""'), (u'õ', u'ő'), (u'û', u'ű')]:
        str = str.replace(a, b).replace(a.upper(), b.upper())
    return '"' + str + '"' # ('"' if str[0].isalnum() else '="')

def email_norm(email):
    email = email.lower()
    if email.endswith('gmail.com'):
        email = email.split('@')
        email[0] = email[0].replace('.', '')
        email = '@'.join(email)
    return email

try:
    # read team names
    teams = {} # email to team name mapping
    logging.info('Parsing team names...')
    try:
        with codecs.open('csapatok.csv', 'r', encoding='utf_8') as csv:
            for idx, line in enumerate(csv):
                if idx == 0: continue
                line = line.strip().split(',')
                teams[email_norm(line[2])] = line[0]
    except:
        pass

    # fetch candidates
    candidates = []
    url = 'https://www.hackerrank.com/x/api/v3/tests/276763/candidates?limit=100'
    logging.info('Fetching list of candidates...')
    while url:
        req = requests.get(url, auth=HTTPBasicAuth('', access_token))
        if req.status_code != requests.codes.ok:
            break
        candidates.extend(req.json()['data'])
        url = req.json()['next']

    candidates = [cand for cand in candidates if email_norm(cand['email'])
                not in ('dhanak@gmail.com', 'bela.schaum@nng.com', 'vadam88@gmail.com')]
    for cand in candidates:
        email = email_norm(cand['email'])
        cand['nng'] = email.endswith('nng.com') or email == 'martonantoni@gmail.com'
        cand['team_name'] = teams[email] if email in teams else \
            cand['candidate_details'][0]['value'] if cand['candidate_details'] else ''
    candidates.sort(key=lambda cand: str('A' if cand['nng'] else 'B') + cand['full_name'])

    # prepare report
    logging.info('Fetching details...')
    csvdata = [[u'Csapatkapitány',u'Csapatnév','NNG-s?',u'Közbringák',u'Bonbonok',u'Kiállítás',u'Logó',u'Kapuk',u'Összpontszám']] # header
    options = webdriver.ChromeOptions()
    options.set_headless()
    options.add_argument('log-level=3')
    with webdriver.Chrome(chrome_options=options) as driver:
        driver.get('https://www.hackerrank.com/work/tests/276763')
        driver.add_cookie({'name': 'access_token', 'value': access_token})
        driver.implicitly_wait(30)  # seconds

        for cand in candidates:
            logging.debug(cand['email'])
            row = [wrap(cand['full_name']), wrap(cand['team_name']), '1' if cand['nng'] else '0']
            if 'report_url' in cand:
                driver.get(cand['report_url'])
                scores = [float(score.text.split('/')[0]) for score in driver.find_elements_by_css_selector('table.mjB tr > td:nth-child(3)')]
                row.extend([str(score) for score in scores])
                row.append(str(sum(scores)))
            else:
                row.extend(['0']*6)
            csvdata.append(row)
    csv = ''.join(['\t'.join(row) + '\n' for row in csvdata])

    csvfile = 'scores_{0:%m%dT%H%M}.csv'.format(datetime.now())
    with codecs.open(csvfile, 'w', encoding='utf_8', errors='replace') as csvf:
        csvf.write(csv)

    # publish report
    logging.info('Publishing report...')
    req = requests.post('https://www.nng.com/wp-admin/admin-ajax.php', #'http://nng.fps2.hu/wp-admin/admin-ajax.php', auth=HTTPBasicAuth('client', 'view'),
                headers={'token': '$2y$10$dqPD30Dy3f77dtM2NU8t1uLronal/pVQpKFY2qT93gTTD9V9Ra/CK'},
                files={'csv': (csvfile, open(csvfile, 'rb'))},
                data={'action': 'competitionResultUpload'})
    req.raise_for_status()
    logging.info(req.json())

except Exception as e:
    logging.error(repr(e))
