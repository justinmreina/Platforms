Connected Maraca
====

The following instructions will help you build a Internet connected Maraca using
the [Marvell IoT Starter Kit](http://www.amazon.com/Globalscale-MW302-IoT-Starter-Powered/dp/B0168DLQHI/ref=sr_1_1?ie=UTF8&qid=1459763298&sr=8-1&keywords=marvell+iot+starter+kit) with the [Groove 3-Axis Digital Accelerometer](http://www.seeedstudio.com/depot/twig-3axis-accelerometer-p-765.html?cPath=144_146)


## Step 1 - Wire hardware

The digital accelerometer that we will be using works over the I2C protocol. We are going to connect it to I2C0 bus on the Marvell IoT starter kit

![](https://raw.githubusercontent.com/marvell-iot/aws_starter_sdk_wiki_images/master/PinMap.png)

The sensor has 4 pins -

| Sensor | IoT Starter Kit | Wire color
|:----|:----:|----:|
| GND | GND | Dark Green
| Vcc | +3.3V | Red
| SDA | IO_04 | Light Green
| SCL | IO_05 | Blue

Here's a picture of the connections :
![Connections](./Wires.jpg)

## Step 2 - Get latest blob (includes wifi firmware and bootloader)
Flash the latest blob on the Marvell board.

- Download the latest blob from [releases](https://github.com/marvell-iot/aws_starter_sdk/releases) section of Marvell's SDK on GitHub
```
wget https://github.com/marvell-iot/aws_starter_sdk/releases/download/2015_12_08/aws_starter_sdk-2015_12_08.blob
```
- Use the `flash.py` script found in the GitHub SDK to upload the above blob to
your board
```
python sdk/tools/OpenOCD/flash.py -f aws_starter_sdk-2015_12_08.blob
```

## Step 3 - Compile Maraca sample code
You will need to edit the source code to enter your Device ID. This is the same
ID that you will use while configuring your [CloudFormation Stack](http://iot-hackseries.s3-website-us-west-2.amazonaws.com/aws-launch-cfn.html)
```
#define DEVICE_ID                "<INSERT_YOUR_DEVICE_ID>"
```
Compile the Maraca application to create a binary called connected_maraca.bin
```
make APP=sample_apps/connected_maraca/
```

## Step 4 - Flash application binary to board
Flash the newly created binary to the board using the following command.
```
python sdk/tools/OpenOCD/flash.py --mcufw bin/mw302_rd/connected_maraca.bin
```
## Step 5 - Reset
Reset the board
```
python sdk/tools/OpenOCD/flash.py -r
```
You can also reset the board by pressing the reset button on the top left
of the board.

## Step 6 - Upload keys to board
Upload keys generated from your AWS IoT dashboard to the device by connecting to
the micro AP (Access Point) going to the address 192.168.10.1 using a web
browser. Here you will be asked to provision the device with your AWS IoT thing name, AWS region name and the ceritifcate and private key. Make sure you input the exact name that you used when you created a new thing on your AWS IoT Dashboard.

You can find out more about this on the [Marvell SDK Wiki](https://github.com/marvell-iot/aws_starter_sdk/wiki/Getting%20S
tarted%20Guide#configuring-the-starter-kit)

## Step 7
Try it out !
Shake the Maraca and the changed state should be updated on AWS IoT.

If you run into any issues, you can reach out on [Marvell SDK's Gitter channel](https://gitter.im/marvell-iot/aws_starter_sdk)
