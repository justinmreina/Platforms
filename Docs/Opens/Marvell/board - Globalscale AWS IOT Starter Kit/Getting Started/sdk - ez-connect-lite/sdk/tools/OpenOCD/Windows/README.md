### Installing OpenOCD
* Get OpenOCD 0.9.0 for Windows from http://www.freddiechopin.info/en/download/category/4-openocd
* Unzip OpenOCD tarball and copy the contents of bin/ folder (for 32-bit Windows machines) or bin-x64/ folder (for 64-bit Windows machines) to this folder (wmsdk/tools/OpenOCD/Windows).
````json
copy \path\to\OpenOCD-0.9.0\bin\* \path\to\aws_starter_sdk\wmsdk\tools\OpenOCD\Windows
OR
copy \path\to\OpenOCD-0.9.0\bin-x64\* \path\to\aws_starter_sdk\wmsdk\tools\OpenOCD\Windows
````
### Installing WinUSB Driver
* Download WinUSB driver installer from http://zadig.akeo.ie/.
* Connect the starter kit to the PC, let default Windows driver install first.
* After this, launch Zadig WinUSB driver installer, click on options->List all Devices, select Dual RS232 (Interface 0) in the dropdown list, select driver WinUSB for this device.
* Install the driver.
