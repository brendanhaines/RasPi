# run to set up raspberry pi
# based on access point setup from adafruit.com

# must run as root (sudo)

apt-get update
apt-get upgrade

#########################
# Wireless Access-Point #
#########################

apt-get install hostapd isc-dhcp-server

# modify /etc/dhcp/dhcpd.conf
sed -i.temp 's/option domain-name "example.org";/#option domain-name "example.org";/' /etc/dhcp/dhcpd.conf
sed -i.temp 's/option domain-name-servers ns1.example.org, ns2.example.org/#option domain-name-servers ns1.example.org, ns2.example.org/' /etc/dhcp/dhcpd.conf
sed -i.temp 's/# If this DHCP server is the official DHCP server for the local\n# network, the authoritative directive should be uncommented.\n#authoritative;/# If this DHCP server is the official DHCP server for the local\n# network, the authoritative directive should be uncommented.\nauthoritative;/' /etc/dhcp/dhcpd.conf
cat "subnet 192.168.42.0 netmask 255.255.255.0 {\nrange 192.168.42.10 192.168.42.50;\noption broadcast-address 192.168.42.255;\noption routers 192.168.42.1;\ndefault-lease-time 600;\nmax-lease-time 7200;\noption domain-name "local";\noption domain-name-servers 8.8.8.8, 8.8.4.4;\n}" >> /etc/dhcp/dhcpd.conf
rm /etc/dhcp/dhcpd.conf.temp

# modify  /etc/default/isc-dhcp-server
sed -i.temp 's/INTERFACES=""/INTERFACES="wlan0"/' /etc/default/isc-dhcp-server
rm /etc/default/isc-dhcp-server.temp

# shutdown wlan0 if it's running
ifdown wlan0

# modify /etc/network/interfaces
echo "auto lo\n\niface lo inet loopback\niface eth0 inet dhcp\n\nallow-hotplug wlan0\n\niface wlan0 inet static\naddress 192.168.42.1\nnetmask 255.255.255.0" > /etc/network/interfaces

# assign IP address to wifi adapter
ifconfig wlan0 192.168.42.1

# create /etc/hostapd/hostapd.conf
echo "interface=wlan0\ndriver=rtl871xdrv\nssid=Pi_AP\nhw_mode=g\nchannel=6\nmacaddr_acl=0\nauth_algs=1\nignore_broadcast_ssid=0\nwpa=2\nwpa_passphrase=Raspberry\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP" > /etc/hostapd/hostapd.conf

# modify /etc/default/hostapd
sed -i.temp 's/#DAEMON_CONF=""/DAEMON_CONF="/etc/hostapd/hostapd.conf"/' /etc/default/hostapd
rm /etc/default/hostapd.temp

# modify /etc/sysctl.conf
cat "net.ipv4.ip_forward=1" >> /etc/sysctl.conf

# activate NAT
sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"

# set up network translation between eth0 and wlan0
iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT

# save these settings
sh -c "iptables-save > /etc/iptables.ipv4.nat"

# modify /etc/network/interfaces (again)
cat "up iptables-restore < /etc/iptables.ipv4.nat" >> /etc/network/interfaces

# update hostapd
wget http://adafruitdownload.s3.amazonaws.com/adafruit_hostapd_14128.zip
unzip adafruit_hostapd_14128.zip
mv /usr/sbin/hostapd /usr/sbin/hostapd.ORIG
mv hostapd /usr/sbin
chmod 755 /usr/sbin/hostapd

# set it up to start when the pi starts
service hostapd start
service isc-dhcp-server start
update-rc.d hostapd enable
update-rc.d isc-dhcp-server enable

# remove WPA-Supplicant
mv /usr/share/dbus-1/system-services/fi.epitest.hostap.WPASupplicant.service ~/

####################
# WiringPi Library #
####################

# install git
apt-get install git-core

# clone the repository
git clone git://git.drogon.net/wiringPi
./wiringPi/build

# test wiringPi
echo 'testing wiringPi...'
gpio -v
gpio readall

##########################
# Disable Serial Console #
##########################

# modify /boot/cmdline.txt
sed -i.temp 's/[^\s]*ttyAMA0[^\s]*/ /g' /boot/cmdline.txt
rm /boot/cmdline.txt.temp

#modify /etc/inittab
sed -i.temp 's/T0:23:respawn:\/sbin\/getty -L ttyAMA0 115200 vt100/#T0:23:respawn:\/sbin\/getty -L ttyAMA0 115200 vt100/' /etc/inittab

##############
# Enable SSH #
##############

update-rc.d ssh enable
invoke-rc.d ssh start

##############
# Enable i2c #
##############

apt-get install i2c-tools

# modify /etc/modules
cat 'i2c-bcm2708\ni2c-dev' >> /etc/modules

# modify /etc/modprobe.d/raspi-blacklist.conf
sed -i.temp 's/blacklist spi-bcm2708/#blacklist spi-bcm2708/' /etc/modprobe.d/raspi-blacklist.conf
sed -i.temp 's/blacklist i2c-bcm2708/#blacklist i2c-bcm2708/' /etc/modprobe.d/raspi-blacklist.conf
rm /etc/modprobe.d/raspi-blacklist.conf.temp

# modify /boot/config.txt
cat 'dtparam=i2c1=on\ndtparam=i2c_arm=on' >> /boot/config.txt

##########
# reboot #
##########

reboot