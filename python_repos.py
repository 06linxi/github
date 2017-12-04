import requests

url='https://api.github.com/search/rapositories?q=language:python&sort=stars'
r=requests.get(url)
print("Start code:",r.status_code)
response_dict = r.json()
print(response_dict.keys())