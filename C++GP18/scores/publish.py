import requests
from requests.auth import HTTPBasicAuth

csvfile = 'scores_1029T0008.csv'

# publish report
req = requests.post('https://www.nng.com/wp-admin/admin-ajax.php', # 'http://nng.fps2.hu/wp-admin/admin-ajax.php', auth=HTTPBasicAuth('client', 'view'),
              headers={'token': '$2y$10$dqPD30Dy3f77dtM2NU8t1uLronal/pVQpKFY2qT93gTTD9V9Ra/CK'},
              files={'csv': (csvfile, open(csvfile, 'rb'))},
              data={'action': 'competitionResultUpload'})
req.raise_for_status()
print req.json()
