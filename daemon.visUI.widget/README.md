This Widget presents the structure of TuyaDAEMON, in a dynamic 'tree' arrangement and with popups to read the status and updated values of all devices.
Zoom, Pan, and alterations can be done with just the mouse or, optionally. with buttons.

The data update interval, in the tooltips of each device, is given by <br>
                    `num_device x refresh_time` (default 4s) e.g. 1m20s for 20 devices

![User interface using VIS](https://github.com/msillano/tuyaDAEMON/blob/main/pics/daemonUIvis.png?raw=true)

The choice of the visualization library fell on Vis Network (https://visjs.github.io/vis-network/docs/network/) for its flexibility combined with good ease of use.
The nodes and links are two arrays dynamically built by the code by reading the data from TuyaDAEMON via REST.
The main file `visUI.html` is an APP standalone, example of the widget use. The widget file is `daemonUIVis.js` and requires also `daemonREST.js`. 


### Specifications
  - Special process for TuyaDAEMON modules: CORE - including CORE_devices, core_TRIGGER, core_SYSTEM, core_MQTT, core_OPENAPI. If new TuyaDAEMON extension modules are created in the future, these must be added (`visUI.html` line 28).
- The code automatically adapts to each TuyaDAEMON implementation and requires very little intervention.

### Configuration

- You need to update the TuyaDAEMON URL in the main file (`visUI.html` line 16). Typically it is local, but this is not necessarily the case: remote monitoring is also possible, especially if used as a standalone APP and not as a widget.
- The `modules` array contains names 'user defined' in the `global.alldevices`: you must update the array to match your names. If new TuyaDAEMON extension modules are created in the future, these must be added (`visUI.html` line 28).


### Options
1) The list of monitored devices is obtained when the program is started and is NOT updated.
The program uses 2 methods to get the list, which the user can choose from (`visUI.html` line 19):

  - _'live' mode_: ALL devices in `tuyastatus` are monitored, i.e. the devices that have sent data. Since a Deploy or Restart of node-red resets all data, the number of devices varies: some devices can disappear from the UI until they send new data (even hours/days) A reload of the widget updates the list. More suitable for occasional use. 
Note: requires CORE only to run.

  - _'all' mode_: ALL devices in `global.alldevices` are monitored. This method can lead to many disconnected devices, especially during the testing phase. Always present ALL devices in TuyaDEAMON. 
Note: requires at least CORE + SYSTEM to run.

2) The basic data loop time is set to 4s by default. The polling frequency depends on the number of devices (`visUI.html` line 23).

### Customization
There are a few sectors of optional customizations to improve the look & feel of the widget:

1) To have specialized icons for some devices, you must provide a criterion and an indication of the special icon to use (see https://fontawesome.com/v4/cheatsheet/). 
By default, they have special icons: Thermometers (a device with the name 'Temp...'), Thermostatic valves (a device with the name 'Termo...'), and Gateways (a device with 'Gateway' in the name). (`daemonUIVis.js`, line 45)

2) The tooltip that opens under the mouse contains by default ALL the latest values of the device's DPs (as per SCHEMA), and normally no processing is necessary.
In case of very long information, the browser can truncate the tooltip. A filter function can be customized to select the most important information by device type. (`daemonUIVis.js`, line 70)

3) Typically the mouse + well is sufficient to do all commands, but if desired you can add control buttons for pan/zoom - see [figure](https://github.com/msillano/tuyaDEAMON-applications/tree/main). (`daemonUIVis.js`, line 156)

### Use
Standalone as an APP or as a widget within a custom WEB page (`visUI.html` is an example of use). Being HTML/js it works in any OS, with a recent browser.
You just need to load the main file `visUI.html` in your browser. If you want you can add an icon to the desktop.
<hr>

### Installation
Download and unzip the files to a directory. A WEB server is NOT necessary, as the code is all in JavaScript and executed by the browser.
At least update the URL, as indicated in Configuration.

### Versions
 . 1.0   initial version 11/05!2024
