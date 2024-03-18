import http.client, urllib
from time import localtime, strftime
# download from http://code.google.com/p/psutil/
import psutil
import time

channel=1538804
wkey= "YOX31M0EDKO0JATK";


def doit():
	delta = 10      # 10 means - 0.1%, 100 - 1%, ..
	cycle = 10000	# 10000 in milliseconds 
	pkey = 100	# every 100 cycles 
	print("Give the parameters: delta, cycle, pkey")
	delta=input("delta: ")
	cycle=input("cycle: ")
	pkey=input("pkey: ")
	params = urllib.parse.urlencode({'field1': delta, 'field2': cycle, 'field3': pkey, 'key':'YOX31M0EDKO0JATK'})
	headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}
	conn = http.client.HTTPConnection("api.thingspeak.com:80")
	
	try:
		conn.request("POST", "/update", params, headers)
		response = conn.getresponse()
		print(delta)
		print(cycle)
		print(pkey)
		print( strftime("%a, %d %b %Y %H:%M:%S", localtime()))
		print(response.status, response.reason)
		data = response.read()
		conn.close()
	except:
		print("connection failed")

#sleep for 16 seconds (api limit of 15 secs)
if __name__ == "__main__":
	while True:
		doit()
		time.sleep(16) 

