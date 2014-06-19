multiple shairport instances (each room)
multiple mpd instances ? (each room)
one mpd instance multiple rooms ?


use libpulse to enable/disable speakers ?
http://freedesktop.org/software/pulseaudio/doxygen/subscribe.html --> subscribe to callback
and enable audio output (start raspi ?) on each rasperry pi ?

select player to dispatch to raspi on master

## use a remote source

load-module module-tunnel-source server=x40 source=alsa_output.pci-0000_00_1f.5.analog-stereo.monitor
load-module module-loopback source=tunnel-source.x40


## create a combined sink (local + tunnel)

load-module module-tunnel-sink server=erdbeere
load-module module-combine-sink slaves=Wohnzimmer,Schlafzimmer sink_name=Wohnzimmer_Schlafzimmer
set-default-sink combined
 
 
## reattach sink 
 
 list-sink-inputs 
 >>> move-sink-input 16 combined
 >>> move-sink-input 16 0


## change sink input volume

>>> set-sink-input-volume 1 65536


set default sink input for session 

move-sink-input 0 Schlafzimmer


associate an input with a single / multiple sinks 


** who is playing ? (--> profile with input volume / equalizer settings )




pulseaudio to JSON RPC / over websockets API ?

`man pulse-cli-syntax`


RTP
——————————
* RTP transport does not sync over network (if one host is localhost)
* http://www.hackerposse.com/~rozzin/journal/whole-home-pulseaudio.html
* https://www.ibm.com/developerworks/community/blogs/fe313521-2e95-46f2-817d-44a4f27eba32/entry/configuring_iptables_for_ip_multicast1?lang=en


```
load-module module-loopback latency_msec=450 source=rtp.monitor
```

mpd.conf
```
audio_output {
    type            "pulse"
    name            "MPD Stream"
    sink            "rtp"
    description     "what's playing on the stereo"
    mixer_type      "software"
}
```

```
[ruben@blaubeere ~]$ tail /etc/pulse/default.pa

load-module module-null-sink sink_name=rtp format=s16be channels=2 rate=44100
load-module module-rtp-send source=rtp.monitor source_ip=172.66.66.3 destination_ip=224.0.0.1 loop=1
```


block multicast from jamming the network  (requires ebtables) /etc/firewall.user
```
# block multicast traffic from wlan
# requires package 'ebtables'
ebtables -A FORWARD -o wlan0 -d Multicast -j DROP
```

```
load-module module-rtp-recv sap_address=224.0.0.1
```

Configuration
=================

## erdbeere start daemon and enable remote access



```
 /usr/bin/pulseaudio --start --high-priority=1 --realtime=1 --log-target=file:/tmp/pulse.log --resample-method=src-sinc-fastest
```

```
pulseaudio --disallow-exit=1 --exit-idle-time=-1
```

/etc/pulse/default.pa

```
load-module module-cli-protocol-tcp listen=0.0.0.0
# required by module-cli-protocol-tcp
load-module module-native-protocol-tcp auth-anonymous=1 auth-ip-acl=127.0.0.1;172.66.66.0/24
```

## blaubeere

/etc/pulse/default.pa

```
# we allow a maximum of 3 mpd instances
# the null sink is connected to output via loopback
load-module null-sink sink_name=mpd1
load-module null-sink sink_name=mpd<pid>
load-module null-sink sink_name=mpd<pid>

# permit access from remote daemons via tunnel sink/source
load-module module-native-protocol-tcp auth-ip-acl=127.0.0.1;172.66.66.0/24 auth-anonymous=1
```

## connect null sink monitor to default sink

sudo -u mpd pactl load-module module-loopback source=mpd1.monitor


## connect remote to null sink (erdbeere)

# erdbeere from remote
pactl -s erdbeere load-module module-tunnel-source server=blaubeere source=mpd1.monitor
# connect remote source to default sink
pactl -s erdbeere load-module module-loopback source=tunnel-source.blaubeere


pactl -s erdbeere l

Events
===================
* http://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/Developer/
* http://freedesktop.org/software/pulseaudio/doxygen/subscribe.html



## start system daemon

* add to group audio, otherwise alsa is not accessible
* configure system.pa

pulseaudio --system=1 --daemonize --high-priority=1 --exit-idle-time=-1 --disallow-exit -v

## connect to system daemon 

sudo -u pulse PULSE_RUNTIME_PATH=/var/run/pulse pacmd

load-module module-cli-protocol-unix

pulseaudio --system=1 --daemonize --high-priority=1 --exit-idle-time=-1 --disallow-exit -v --disallow-module-loading




2014-06-13T21:20:34+02:00 warning pulseaudio [pulseaudio] module-tunnel.c: Stream died.

--> automatically reconnect ?


### connect to mpd pulse

sudo -u mpd pacmd



### zeroconf


$ systemctl start avahi-daemon


pulseaudio --system=1 --high-priority=1 --exit-idle-time=-1 --disallow-exit -v --disallow-module-loading




### mpd instances

sudo mpd --no-daemon --stdout -v /home/ruben/code/mpd/mpd-1.conf
sudo mpd --no-daemon --stdout -v /home/ruben/code/mpd/mpd-2.conf







### remember to copy the kernel images / initrd to airstation 
after an kernel upgrade

/mnt/storage/pxe/172.30.66.2/boot


http://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/User/Network/




## player with mplayer slave protocol ?

http://www.mplayerhq.hu/DOCS/tech/slave.txt

https://github.com/ctcherry/mplayerweb.git


mkfifo mplayer.fifo
mplayer -slave -quiet -idle -input file=mplayer.fifo
 
 
 [ruben@x40 ~]$ echo "loadfile http://x40:3000/ES/C-41985-1101-28403" > .config/systemd/user/mplayer.fifo
 [ruben@x40 ~]$ echo "pause" > .config/systemd/user/mplayer.fifo


sudo -u pulse PULSE_RUNTIME_PATH=/var/run/pulse pacmd



status commands are logged to stdout

[ruben@x40 ~]$ echo "get_property stream_pos" > mplayer.fifo
[ruben@x40 ~]$ echo "get_property filename" > mplayer.fifo
[ruben@x40 ~]$ echo "get_property path" > mplayer.fifo





## systemd init script

* realiablity: restart server on failure !!!
* http://0pointer.de/blog/projects/socket-activation.html


## user concept

* multiuser
* each user has it's own: favorites, stars, history

* map to unix user accounts ? 
* generate mpd configuration ? (zero conf name, audio output name)
* how to start servers ? (on demand, spawn process ?)


### systemd user control


systemctl --user start mpd

one master, multiple slaves

## convert playlists

* relative to playlist directory of MPD


## bind mpd to unix domain socket 
bind_to_address "/tmp/mpd.sock"

see man mpd.conf



## volume control

* limit maximum volume !!!
* normalize volume between sinks 


setvol

## linger session

* https://wiki.archlinux.org/index.php/Systemd/User#Automatic_start-up_of_systemd_user_instances
* http://lists.freedesktop.org/archives/systemd-devel/2014-March/018151.html
* https://bugzilla.redhat.com/show_bug.cgi?id=753882



[root@x40 ruben]# export XDG_RUNTIME_DIR=/run/user/1338
[root@x40 ruben]# sudo -u christina systemctl --user stop mpd
Failed to get D-Bus connection: Verbindungsaufbau abgelehnt
[root@x40 ruben]# sudo -E -u christina systemctl --user stop mpd
[root@x40 ruben]# sudo -E -u christina systemctl --user stop start
Failed to stop start.service: Unit start.service not loaded.
[root@x40 ruben]# sudo -E -u christina systemctl --user start mpd
[root@x40 ruben]# sudo -E -u christina systemctl --user stop mpd
[root@x40 ruben]# ls /run/user/
1000  1338




https://bugzilla.redhat.com/show_bug.cgi?id=753882


sudo -u christina XDG_RUNTIME_DIR=/run/user/`id -u christina` systemctl --user start mpd


    [ruben@x40 ~]$ cat /usr/local/sbin/sudo-systemctl
    #!/bin/sh
    USER=$1
    shift
    CMD=$@
    sudo -u $USER XDG_RUNTIME_DIR=/run/user/`id -u $USER` systemctl --user $CMD


## mpd socket activation

http://www.musicpd.org/doc/user/ch02s03.html

* socket path from configuration is ignored



## debugging pulseaudio

* pkgbuild enable debug !strip options
* recompile
* load into debugger, add breakpoints, load source (relative) with 'dir' command

* access permissions problems --> chmod 777 helps to diagnose the problem fast 


## rc.local service

* apply powertop settings

https://thomas-leister.de/allgemein/arch-linux-rc-local-ausfuehren-mit-systemd/