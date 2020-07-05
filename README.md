# LapMonitor

This is a 'front end' for ubertooth-rx built using Eclipse. It is fed through FIFO /tmp/ubt and provides an aggregate view of lap frames that are received and allows for sorting laps based on RSSI, rx time, or lap address.

## Requirements
Eclipse<br />Ubertooth installed and working on system<br />  root privliges<br />  sudo apt-get install libncurses5-dev libncursesw5-dev<br />  sudo apt install libboost-program-options-dev<br />

## Usage
mkfifo /tmp/ubt<br />ubertooth-rx > /tmp/ubt<br />
./LapManager --sort [rssi] [lap] [rxts] --purge [minutes]<br />e.g. ./LapManager --sort rssi --purge 30
