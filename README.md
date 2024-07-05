# Capstone Project Goals
•	Learn ESP-IDF <br>
•	Prototype an automated plant growing system
# Development Environment
•	VS Code <br>
•	PlatformIO extension
# Testing Framework
•	Unity 
# System Overview
Growing Buddy consists of three hubs that handle different needs of the system: the communication hub, the power hub, and the sensor hub. <br> <br>
•	The communication hub handles the computation of sensor data, hosting of the UI on a HTTPS server, and sending commands to the power hub. <br>
•	The power hub switches relays that can handle high Wattage applications. <br>
•	The sensor hub collects data to send to the communication hub. <br> <br>
![system_overview_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/9f382680-4367-4af6-82fd-07e7d9d3d024)
<br>*Figure 1: High Level Block Diagram* <br><br>
Each of these hubs interact with one another to autonomously maintain an ideal environment for a plant based on a profile collected by the user. Each hub is controlled by an Espressif ESP-32 development board.
# Communication Hub
The communication hub is configured as a soft access point. It hosts a HTTP server for user in
terface. It collects sensor data from the sensor hub and outputs commands to power hub. <br><br>
![ComHubTaskUMLFinal_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/951249aa-6de5-48f7-b070-91ad6e9a1840)
<br>*Figure 2: Communication Hub Flow Chart*<br><br>
![comHub_pinout_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/735b75cb-b5fe-4209-b37f-b5fbb18ef91f)
<br>*Figure 3: Communication Hub Pinout*
# Power Hub
The power hub switches high powered peripheral using relays. It also updates the communication hub on the light status. <br> <br>
![powerHub_UML_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/3be84ffe-2e7c-4f0a-9536-ee9d9d82a00d)
<br>*Figure 4: Power Hub Flow Chart* <br> <br>
![PowerHub_pinout_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/8597f773-141f-4d66-a60f-694ec53b86cd)
<br>*Figure 5: Power Hub Pinout*
# Sensor Hub
The sensor hub collects environmental information from sensors and sends this to the communication hub.<br><br>
![sensorHub_UML_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/ecd14247-03e0-436a-b906-472161281fc6)
<br>*Figure 6: Sensor Hub Flow Chart* <br> <br>
![sensorHub_pinout_resize](https://github.com/SpenceJohnstonProjects/Wireless-Automated-Plant-Growing-System/assets/118693339/1cc6147c-627a-4456-a888-423d1d05aea0)
<br>*Figure 7: Sensor Hub Pinout*
# Next Steps
•	Refactor code <br>
•	Remove HTTP server, add app for mobile devices <br>
•	Reduce cost <br>
•	Build custom PCBs for each hub <br>
•	Add additional features for quality-of-life improvements <br>
•	Allow for multiple hubs of the same type to connect to a single communication hub <br>
•	Add waterproofing for outdoor and greenhouse use <br>
•	Add SD card for logging <br>
