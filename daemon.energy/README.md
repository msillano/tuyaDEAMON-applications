This home automation application offers a seamless and intuitive user experience, empowering you to take control of your home's energy management, photovoltaic system, climate control, and garden irrigation. Explore the possibilities and make your home smarter and more efficient with a HTML front-end to TuyaDEAMON.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/app003.png?raw=true)

### step 1: fine tuning tuyaDEAMON

To tailor the flow and tuyaDEAMON to achieve a well-populated 5-minute database 'energy' view while dealing with lazy devices and verbose devices, I used the following approaches:

1. Verbose Devices (Too Much Data):
 A [BreakerDIN](https://github.com/msillano/tuyaDAEMON/blob/main/devices/BreakerDIN/device_BreakerDIN.pdf) is used as differential breacker on the house electrical switchboard, and as grid power meter. The problem is that it send data every second, and I haven't found a method to reduce the throughput of the device. 

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC003.png?raw=true)

The better solution found requires the use of the updated version of the code (see [core installation](https://github.com/msillano/tuyaDAEMON/tree/main/tuyaDAEMON#first-time-installation-core)) with two nodes that control the data processed by the tuya-smart-device node, exploiting the fact that redundant messages are "event-refresh":

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC002.png?raw=true)

- The first node, 'inject', produces a message with payload 'now' every 97 seconds.
- The second node, 'trigger', produces two messages for the device:
      
when a message arrives, it produces:
````       
     "payload": {
            "operation":"CONTROL",
            "action":"SET_DATA_EVENT",
            "value":"both"
            }
````            
and, 5 seconds later:
````            
        "payload": {
            "operation":"CONTROL",
            "action":"SET_DATA_EVENT",
            "value":"event-data"
            }   
````            
            
This gives 5 seconds of data every 97 seconds, enough to ensure enough values every 5 minutes.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC004.png?raw=true)

An alternative is an aggregation mechanism to reduce the amount of unnecessary data stored in the database but not the processing load. Here is used the **RT/AVG** subflow.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC001.png?raw=true)

_This solution has the only advantage of using the original tuya-smart-device node code._




2. Lazy Devices (Too Little Data)
 Other devices transmit data too infrequently. This is the case, for example, of [meter-plugs](https://github.com/msillano/tuyaDAEMON/blob/main/devices/Smart_socket/device_Smart_socket.pdf) used to evaluate the consumption of some household appliances.
With this device you cannot use REFRESH, which produces an output only if the data has changed, but you must make repeated readings (GET) about every 100 seconds. Since you don't have to use new functions, you don't need to change the flows, and the implementation is all done with 'share', updating _system._laststart_, and adding a new dp (method) system._refreshforever. 

````
                {
                    "dp": "_laststart",
                    "capability": "RO",
                    "__comment": "share to implement autostart",
                    "share": [
                        {
                            "test": [
                                "msg.info.value"
                            ],
                            "action": [
                                {
                                    "property": "_refreshforever",
                                    "value": "go"
                                } ] } ] },
                {
                    "dp": "_refreshforever",
                    "capability": "SKIP",
                    "share": [
                        {
                            "test": [
                                "tuyastatus.meterZ  && tuyastatus.meterZ._connected"
                            ],
                            "action": [
                                {
                                    "device": "meterB",
                                    "property": "19",
                                    "value": null
                                } ] ,
                       "__comment":" more, if required",           
                             "action": [
                                {
                                    "property": "_timerON",
                                    "value": {
                                        "timeout": "60037",
                                        "id": "refreshforever",
                                        "alarmPayload": {
                                            "device": "_system",
                                            "property": "_refreshforever",
                                            "value": "more"
                                        } } } ]
                      }
     
````

 
