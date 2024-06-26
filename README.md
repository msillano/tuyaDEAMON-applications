# tuyaDAEMON - An IoT Framework Empowering Project Development

[TuyaDAEMON is an IoT framework](https://github.com/msillano/tuyaDAEMON) that provides users with a comprehensive set of resources to leverage in their projects. While each project is unique, tuyaDAEMON offers reusable solutions that facilitate the development process. 

### Enhanced IoT Project Development
tuyaDAEMON serves as a powerful toolset for creating IoT projects, offering a wide range of features and resources. Whether you are developing a smart home system, industrial automation solution, or a wearable device, tuyaDAEMON provides a solid foundation to build upon. Its open and extensible nature allows developers to integrate various devices, sensors, and protocols seamlessly, ensuring compatibility and interoperability.

<img src= 'https://github.com/msillano/tuyaDAEMON/blob/main/pics/TuyaDAEMON_UI.png?raw=true' width="850px"   />

### Key Features and Capabilities

1. Device Management: tuyaDAEMON simplifies device management by providing a unified interface for controlling and monitoring connected devices. This includes device registration, authentication, and status monitoring, enabling efficient management of a diverse range of IoT devices.

1. Data Collection and Analysis: With tuyaDAEMON, collecting data from connected devices becomes effortless. The framework offers powerful data collection mechanisms, enabling users to gather valuable insights for decision-making, optimization, and predictive maintenance.

1. Interoperability: tuyaDAEMON supports a wide range of communication protocols, ensuring interoperability with different devices and platforms. This allows seamless integration of devices from various manufacturers, making it easier to create interconnected IoT systems.


1. Scalability and Flexibility: tuyaDAEMON is designed to support projects of all scales, from small prototypes to large-scale deployments. Its flexible architecture allows users to expand and customize their solutions as per their specific requirements, accommodating future growth and innovation.

1. Reuse of Solutions: While every IoT project is unique, there are often common challenges and requirements across different use cases. tuyaDAEMON acknowledges this and offers reusable solutions that can be leveraged to accelerate development. These solutions encompass pre-built modules, libraries, and code snippets that address common functionalities, such as data management, data visualization, user interfaces, and tasks management. By reusing these solutions, developers can save time, reduce development costs, and focus on adding value to their projects.

### Conclusion
tuyaDAEMON empowers IoT project development by providing a comprehensive framework with versatile features and resources. Its capabilities span device management, data collection, interoperability, security, and scalability. Furthermore, tuyaDAEMON promotes the reuse of solutions, enabling developers to leverage pre-built modules and libraries to expedite development while maintaining flexibility. With tuyaDAEMON, IoT enthusiasts and professionals can create innovative and robust solutions, unlocking the full potential of the Internet of Things.

----------------------------
### tuyaDEAMON model for applications
<table border=0>
<tr>
<td>
<img src='https://github.com/msillano/tuyaDAEMON/blob/main/pics/daemon_0B.png?raw=true' width="850px"   />
<BR CLEAR=”left” /></td><td>
When designing applications, the TuyaDEAMON model simplifies as in the figure (in green the data, in black the commands). This doesn't mean that when developing an application one shouldn't sometimes intervene at a low level, to adapt some properties. Still, the application project is logically 'above' this simplified model of TuyaDAEMON.

The two most essential interfaces for applications are highlighted: **REST** and **database**.

The [REST](https://github.com/msillano/tuyaDAEMON/wiki/tuyaDAEMON-REST) interface allows us to send any command (SET) and to send requests to know the last value of each property (GET). The format is straightforward and above all the same for all devices: `device[, property[, value]]`.
</td></tr></table>

- The device's know-how is crystallized in [documentation datasheets](https://github.com/msillano/tuyaDAEMON/tree/main/devices),  which are generated by [tuyadaemon.toolkit](https://github.com/msillano/tuyaDAEMON/wiki/tuyaDAEMON-toolkit), with information on `DPs` (device's attributes or methods), valid values, any quirks.<br>
Some activities are handled by TuyaDAEMON with the _Tuya-cloud_ collaboration: in this case, the two-way communication takes place via [core_OPENAPI](https://github.com/msillano/tuyaDAEMON/tree/main/Tuya_cloud_API#core_openapi-custom-device) or [core_TRIGGER](https://github.com/msillano/tuyaDAEMON/tree/main/tuyaTRIGGER), but in a completely transparent way for the user, and the documentation is always in the datasheets of the devices, [e.g. Smoke_Detector](https://github.com/msillano/tuyaDAEMON/blob/main/devices/Smoke_Detector/device_Smoke_Detector.pdf).
The characteristics of the _custom devices_ (HW or software-only) are also included in documentation datasheets, the collection of which therefore represents the complete documentation of the system for the applications, maintained by the user with the available tools.

- Applications can also use the [MQTT interface](https://github.com/msillano/tuyaDAEMON/blob/1896ec5a969e51af062c8b91253b779dcbecf3c2/README.md#mqtt-interface), which, unlike the REST interface, is asynchronous: it does not use 'tuyastatus' to speed up responses, but always sends commands to the CORE.
  
<table><tr><td > Of the <a href="https://github.com/msillano/tuyaDAEMON/tree/main#database-interface"> DataBase </a>, only one table affects the applications and contains the historical log (commands and events) of TuyaDAEMON: `tuyathome.messages`. The record comprises complete information.<br> example:<br>
<pre>45276 | 2023-05-28 07:33:32 | MACTEST | RX | 1234569b99a78mkj6 | mainAC | 6 | phaseA |
  CYgAAlgAADMACw== | {"V":244,"Leack":0.002,"A":0.67584,"W":51}</td></pre>.
 </td><td style="width:850px"><img src="https://github.com/msillano/tuyaDAEMON/blob/main/pics/dbtuyathome03.png?raw=true" width="800px" /> </td></tr></table>  

 Specialized or application-oriented tables can be derived from this table and automatically updated. See [Archiving db messages](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-2:-Archiving-db-tuyathome.messages).
  
  
  
  
  

-----------------
## Applications

- [TuyaDAEMON UI standalone](https://github.com/msillano/tuyaDEAMON-applications/tree/main/daemon.visUI.widget)  An APP example of the use of the UI widget (see figure on top).
  
- [daemon.energy](https://github.com/msillano/tuyaDEAMON-applications/tree/main/daemon.energy) A WEB dashboard application for a photovoltaic home system.

- [Watering timer](https://github.com/msillano/YAWT---Yet-Another-Watering-Timer) with a [WEB interface](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-3-‐-dynamic-HTML-pages).
  
- [weather-station](https://github.com/msillano/tuyaDAEMON/wiki/custom-device:-433-MHz-weather-station)

  
-----------------
## Programmer's notes 
### Device design
-  [Gateways and sensors](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-4:-Gateways-and-sensors): use of Zigbee and Bluetooth sensors with tuyaDAEMON.
-  [Watchdog for IOT](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-5:-Watchdog-for-IOT):  watchdog project (3 versions) and use of the ESP-01 (ESP8266) for custom devices.

### Data management
-  [Data tuning for application](https://github.com/msillano/tuyaDEAMON-applications/tree/main/daemon.energy): strategies for optimize the device's data throughput.
-  [Purging 'tuyathome.messages' table](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-1:--Purging--tuyathome.messages-table): strategies for managing db log tables.
-  [Archiving device's data](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-2:-Archiving-db-tuyathome.messages):  algorithms for the creation and management of db time series.
  
### Application building 
-  [WEB user interface](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-6:-WEB-user-interface) Environments and Frameworks: why I choose the [Gentelella](https://github.com/ColorlibHQ/gentelella) template.
 
### Using the Gentelella template 
-  [Widget Customization - Energy/month](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-7:-Widget-customization): step-by-step custom widget for energy [month data](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-2:-Archiving-db-tuyathome.messages#:~:text=improve%20the%20system.-,EXAMPLE%239%20monthly%20energy%20view,-To%20evaluate%20the).   
-  [Dynamic HTML pages - Garden irrigation](https://github.com/msillano/tuyaDEAMON-applications/wiki/note-3-%E2%80%90-dynamic-HTML-pages):  step-by-step simple WEB interface page for tuyaDAEMON. 
