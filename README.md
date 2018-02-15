# esp32bleSpheroMini
Hacking the Sphero Mini to be driven by the esp32

These are two apps for the ESP32 ble arduino environement, in use to hack the sphero mini (also BB9).

Sphero seems to have given up, on providing open source implementations to control their newer robots working with BLE.

IT IS A WORK IN PROGRESS, IT DOES NOT WORK NOW.

- fakeSpheroBLE_server is the recipient for the commands from the Sphero Apps,
  so we can read what kind of commands are sent.

- esp32spheroBLE_client should become the controler app,
  so we can feed the commands to the real Sphero.


For the moment I get the passphrase and something, I can stay connected to the real Sphero Mini, but fail to stay connected to the apps.

You're welcome to help, and find out how it can work...



