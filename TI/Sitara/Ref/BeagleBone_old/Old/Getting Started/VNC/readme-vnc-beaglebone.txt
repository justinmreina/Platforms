0. Powered down board

1. Installed microSD with Linux Image

2. Powered up board

3. Validated cmd ping and firefox web access

1. Installed TightVNC 2.7.10, with dfmirage add-in

2. Plug in Beaglebone Black to USB, confirm it connects (File Explorer opens)

3. Open a PuTTY session (192.168.7.2)

4. logged in as root with an empty password

5. run apt-get update

6. run apt-get tightvncserver

7. closed PuTTY

8. Launced ~#tightvncserver and created a desktop with pswd 'labtec'

9. TightVNC Viewer connect to 192.168.7.2:1 from PC with pswd 'labtec'

