This home automation application offers a seamless and intuitive user experience, empowering you to take control of your home's energy management, and photovoltaic system. Explore the possibilities and make your home smarter and more efficient with an HTML front-end to TuyaDEAMON.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/app003.png?raw=true)

_This HTML page  (index.php) uses the data present in the `historical.energy1h` table obtained from the `tuyathome.energy view` with the techniques illustrated in [Archiving db tuyathome.messages](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-2:-Archiving-db-tuyathome.messages). Only a few specific aspects of this project are highlighted here._

This is a work in progress: the last version is 'wwwdaemon.zip'.

### step 1: fine-tuning tuyaDEAMON

To tailor the flow and **tuyaDEAMON** to achieve a well-populated 5-minute `'energy' view` while dealing with lazy devices and verbose devices, I used the following approaches:

#### 1.a Verbose Devices (Too Much Data)
 A [BreakerDIN](https://github.com/msillano/tuyaDAEMON/blob/main/devices/BreakerDIN/device_BreakerDIN.pdf) is used as a differential breaker on the house electrical switchboard and as the grid power meter. The problem is that it sends data every second, and I haven't found a method to reduce the throughput of the device. 

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC003.png?raw=true)

The better solution found requires the use of a `node-red-contrib-tuya-smart-device` ver. 5.2.0 with two nodes that control the data processed by the `tuya-smart-device` node, exploiting the fact that redundant messages are `event-refresh`:

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC002.png?raw=true)

- The first node, `inject`, produces a message with payload 'now' every 97 seconds.
- The second node, `trigger`, produces two messages for the device:
      
when a message arrives, it produces:
````       
     "payload": {
            "operation":"CONTROL",
            "action":"SET_EVENT_MODE",
            "value":"event-both"
            }
````            
and, 5 seconds later:
````            
        "payload": {
            "operation":"CONTROL",
            "action":"SET_EVENT_MODE",
            "value":"event-data"
            }   
````            
            
This gives 5 seconds of data every 97 seconds, enough to ensure some values every 5 minutes.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC004.png?raw=true)

An alternative is an aggregation mechanism to reduce the amount of unnecessary data stored in the database but not the processing load. Here is used the `RT/AVG subflow`.

![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/mainAC001.png?raw=true)



#### 1.b Lazy Devices (Too Little Data)
 Other devices transmit data too infrequently. This is the case, for example, of [meter-plugs](https://github.com/msillano/tuyaDAEMON/blob/main/devices/Smart_socket/device_Smart_socket.pdf) used to evaluate the consumption of some household appliances.
One solution is to poll the device at regular intervals, e.g. 120 s, to have enough data in view intervals, 5 m (300 s).
Options:
 - send a `REFRESH`: as a standard response, the device sends changed dPs.
 - send a `GET`: as a standard response, the device sends the asked dP.
 - send a `SCHEMA`: as a standard response, the device sends all dPs.
  
The more efficient strategy (if `SCHEMA` and `REFRESH` are allowed):
  - At startup (or as soon as possible) send a `SCHEMA` request to the device.
  - Then do `REFRESH` polling
  - Applications can READ data from `global.tuyastatus` (as REST does) that stores the last results.
   
Since you don't have to use new functions, you don't need to change the flows, and the implementation is all done with 'share', updating `_system._laststart`, and adding a new `dp` (method) `_system._refreshforever`. 

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
                                "tuyastatus.meterB  && tuyastatus.meterB._connected"
                            ],
                            "action": [
                                {
                                    "__comment": "This is a GET",
                                    "device": "meterB",
                                    "property": "19",
                                    "value": null
                                } ] },
                                
                       "__comment":" more devices here, if required",
                       
                            {
                             "action": [
                                {
                                    "property": "_timerON",
                                    "value": {
                                        "timeout": "99929",
                                        "id": "refreshforever",
                                        "alarmPayload": {
                                            "device": "_system",
                                            "property": "_refreshforever",
                                            "value": "loop"
                               } } } ]
                     }]
              }
     
````

 ![](https://github.com/msillano/tuyaDEAMON-applications/blob/main/pics/meterA001.png?raw=true)
 
 As can be seen in the figure, two devices (`meterB`, `meterC`) respond to periodical `GET` with the requested `dp`., while the device `meterZ` responds to a `SCHEMA` request.
 
 note: _For the repeated intervals (like 'Interval for retry connection' in tuya-smart-device nodes, timeout in loops, etc.) use different values, better if prime numbers (https://www.walter-fendt.de/html5/mit/primenumbers_it.htm), to spread the tuyaDAEMON activity, and NOT 2 sec, 3000 ms etc._
 
note: _To send an initial `SCHEMA` to a device, just add a `share` to the device dp `_connected`,
with a test on the value, which must be 'true'._

### step 2: dataBase management
The `historic.energy1h` table is updated automatically every 24h (or 12h) by TuyaDAEMON, to guarantee its completeness. An extra update when loading this page allows an updated graph of the last 24 hours (requires the reload of the page).

````
 -- This stored procedure updates to 'now' the 'energy1h' table
DELIMITER //
CREATE PROCEDURE do_refreshEnergy()
BEGIN
      CALL `tuyathome`.`energyViewToEnergyTable`();
      DO SLEEP(2);
      CALL `storico`.`consolidate_toHour`('storico','energy');
	  END //
DELIMITER ;
````
And the PHP code  in `index.php` is:
````
  $query1  = "CALL  `storico`.`do_refreshEnergy`();";
  $db = new mysqli($servername, $username, $password, "storico");       // connection stuff defined in interface.php
  if ($db->connect_errno) {
      die('Error DB connection: '. $db->connect_error);
      }
  $db->query($query1);
  $db->close;
````

For details on DB management, see [note 2: Archiving db `tuyathome.messages`](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-2:-Archiving-db-tuyathome.messages).




