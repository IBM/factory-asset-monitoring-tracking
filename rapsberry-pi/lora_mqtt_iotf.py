import json
import paho.mqtt.client as mqtt
import ssl
import serial
import time
import serial.tools.list_ports
import ibmiotf.device
import decimal

client = None

print(serial.tools.list_ports.comports()[0].device)
ser = serial.Serial(serial.tools.list_ports.comports()[0].device, 9600, timeout=0, parity=serial.PARITY_EVEN, rtscts=1)

org="hz7oe0"
username = "a-hz7oe0-su87h65c94"
password = "hqJC5DhwarJ7XYwD0_"
token="use-token-auth"
secret="+Trx9(7IghIy?VViXd"
deviceType="lg"
deviceID="lora1"
topic = "iot-2/evt/status/fmt/json"
rootCert = "certs/messaging.pem"
clientID = "d:" + org + ":" + deviceType + ":" + deviceID
client = mqtt.Client(clientID)

print("Connecting to broker ",clientID)

client.username_pw_set(token, secret)
client.tls_set(ca_certs=rootCert, certfile=None, keyfile=None, cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_SSLv23)
client.connect(org+".messaging.internetofthings.ibmcloud.com", 8883, 60)
client.loop()

print("loop starts")

while True:
    l=''
    msg={}
    s = ser.readline()
    if len(s) > 0:
      s=s.replace("\r\n","")
      l=s.split(",")
      if len(l):
         if l[1] == "A":
          lat= round(float(l[2][2:])/60+float(l[2][:2]),4)
          lng= round(float(l[4][3:])/60+float(l[4][:3]),4)
          if l[3] == "S":
            lat=lat*-1
          if l[5] == "W":
            lng=lng*-1
          spd=float(l[6])
          dir=float(l[7])
          tmd=l[8][4:]+"-"+l[8][2:-4]+"-"+l[8][:2]+"T"+l[0][:2]+":"+l[0][2:-5]+":"+l[0][4:-3]+"Z"
          msg = json.JSONEncoder().encode({"d":{"lat":lat,"lng":lng,"spd":spd,"tmd":tmd,"id":deviceID,"asset":"car"}})
          print(msg)
          try:
              client.publish(topic, json.dumps(msg))
          except ConnectionException as e:
              print(e)
          print("published")
          time.sleep(0.01)
          pass
client.disconnect()
