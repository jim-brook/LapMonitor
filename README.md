# LapMonitor

This is a 'front end' for ubertooth-rx built using Eclipse. It provides an aggregate view of lap frames that are received and allows for sorting laps based on RSSI, rx time, or lap address.

## Requirements
Eclipse
Ubertooth installed and working on system<br />  root privliges<br />  sudo apt-get install libncurses5-dev libncursesw5-dev<br />  sudo apt install libboost-program-options-dev<br />

## Usage
./LapManager --sort <rssi> <lap> <rxts> --purge <minutes>
