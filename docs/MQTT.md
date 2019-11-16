MQTT Client for OpenZWave
=========================

Docker Repo: https://cloud.docker.com/u/openzwave/repository/docker/openzwave/ozw-mqtt

Copy "config" folder from OZW to /tmp/ozw/

Start Container with the following command line:
>docker run -it --device=/dev/ttyUSB0 -v /tmp/ozw:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USBPATH=/dev/ttyUSB0 ozw-mqtt:latest

(replace MQTT_SERVER with IP address of the MQTT Server and all /dev/ttyUSB0 entries with path to your USB Stick

## MQTT Commands

Most OZW "Commands" asyncronys meaning that the results of the command may not be immediatly available and require talking to the Device on your network. 

The MQTT Client for OZW will usually report the status of each command it process in at least one stage, although some commands will return the progress of the command:

1) Command Accepted by OZW - This means any parameters required are passed and valid and that the OZW library has accepted the command
2) For some commands, the subsequent updates are reported via Notifications, which will be reported to the `/OpenZWave/<instance>/event/<command>/` (where command may be a specific command, or in some cases, "ControllerCommand" where the notification may be generic in nature)
    

## Command Status and Feedback

All Commands are located under the `OpenZWave/<instance>/command/<command>/` topics. Clients will send a JSON payload with the required parameters to the relevant topic to execute the command. 

As OZW and Z-Wave in general operates in a asynchronous manner, feedback about most commands will be sent in stages. More often that not, the first feedback will be to indicate if OZW has accepted the command and will process it. This usually just involves checking any parameters are valid, and the Network is in a state that can accept the command.

In the Command List Section below, these immediate updates are indicated as "Returns"  and are sent to the  `OpenZWave/<instance>/event/<command>/` topic

For Commands that send subsequent updates, these are sent to the `OpenZWave/<instance>/event/Notification/` topic with the payload described below. Please note, that some "Notifications" maybe unsolicited, such as Timeout messages, or when commands are executed by another client application. 

## Return Payload

Return Payloads usually indicate if OZW has accepted the command or not - It does not mean that the command has been executed, and does not indicate if the command is successful or not. In most cases, it is to indicate that the command has been received and will be processed. These types of "Returns" are indicated as in the command list with the description "if OZW has accepted the command"

The Payload for these returns are below. For commands where the result of a command can be returned immediately, the payload will be documented in the relevant command list entry below

**Param**:

"status" - Text - Value of "ok" indicates the command was accepted. value of "failed" indicates command was not accepted and the "error" Param should describe the error

"error" - Text - If present, a short description of why the command failed. Could be things like malformed JSON payload, missing Parameters, incorrect parameter types (a string instead of integer JSON value for example), or in some cases, invalid NodeID's passed to the command. 

## Notification Payload

Notifications are issued to update the application on the success/failure or action required by a user/application in response to a command. Some Notifications may be unsolicited (due to a event on the ZWave Network) while others are only sent in response to commands. 

# 

Some commands will send further updates as the are executed on the Network. These are indicated as Notifications in the Command List.

Commands Lists
==============

## addNode

**Params**:

​    "secure" - bool: If we should attempt to add the node to a network securely.

**Returns**:

​    Event "addNode" - if OZW has accepted the command or not. 

**Notifications**:

​    Event "addNode" - Status if the Controller has sucessfully entered into Inclusion Mode or not. 

​    Event "ControllerCommand" - When Controller enters and exits from Inclusion Mode

**Notes**:

​    If a Node is succesfully added, its details will be progressively added to the Nodes topic with all applicable ValueID's

**See Also**:
    [cancelControllerCommand](#cancelControllerCommand)

## assignReturnRoute

**Params**:

​    "node" - uint8: The NodeID to execute this command on

**Returns**:

​    Event "assignReturnRoute" - If OZW accepted the command or not.

**Notification**:

​    Event "assignReturnRoute" - Status of the Controller after issuing the command to the Node. 

**Notes**:

​    Asks thee Node to calculate the Route to communicate with the Controller. This is a advanced command and should not be required on ZWave Plus devices. 

## cancelControllerCommand

**Params**:

​    None

**Returns**:

​    Event "cancelControllerCommand" - if OZW accepted the command or not

**Notification**:

​    None

**Note**:

​    Cancels in Controller Commands that are in progress.

## checkLatestConfigFileRevision

**Params**:

​    "node" - uint8: Instructs OZW to check if it has hte latest version of the confile file for a particular node

**Returns**:

​    Event "checkLatestConfigFileRevision" - if OZW accepted the command.

**Notification**:

​    "Notification" with the field "Event" set to Alert_ConfigOutOfDate if the Config file is out of date. Payload of the Message should include the Node in question

​    "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 

**Note**:
    OZW will check if the installation has the latest config file for a particular node.

**See Also**:

​    [checkLatestMFSRevision](#checkLatestMFSRevision), [downloadLatestConfigFileRevision](#downloadLatestConfigFileRevision), [downloadLatestMFSRevision](#downloadLatestMFSRevision)

## checkLatestMFSRevision

**Params**:

​    None

**Returns**:

​    Event "checkLatestMFSRevision" - if OZW accepted the command

**Notification**:

​    "Notification" with the field "Event" set to Alert_MFSOutOfDate if the Manufacturer_specific.xml file is out of date. 

​    "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 

**Note**:
    OZW will check if the installation has the latest manufacturer_specific.xml file. Use this if you have a device that is not recongised but present in the latest release of OZW.

**See Also**:
[checkLastestConfigFileRevision](#checkLastestConfigFileRevision), [downloadLatestConfigFileRevision](#downloadLatestConfigFileRevision), [downloadLatestMSFRevision](#downloadLatestMSFRevision)

## close

**Params**:

​    None

**Returns**:

​    Event "close" - if OZW accepted the command

**Notification**:

​    A Series of Events and Notifications will be issued as OZW shuts down. Eventually the OZW status will be set to stopped when OZW has fully shutdown.

**Note**:

​    As OZW shuts down, it will progressively delete values/notes and association information from the MQTT Topics. Applications should not treat the disappearance of these values as NodeDeleted or ValueDeleted once a "close" event is recieved 

**See Also**:

​    [open](#open)

## deleteAllReturnRoute

**Params**:

​    "node" - uint8: The NodeID to perform the action on

**Returns**:

​    Event "deletAllReturnRoute" - if OZW accepted the command

**Notification**:

​    deleteAllReturnRoute - Status of the command after OZW has processed it.

**Note**:

​    This is a advanced command and should not be required on a ZWave+ Device.

## downloadLatestConfigFileRevision

**Params**:

​    "node" - uint8: The NodeID to Update the Config File for

**Returns**:

​    Event "downloadLatestConfigFileRevision" - if OZW accepted the command

**Notification**:
    "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason

​    "Notification" with Event field set to "Alert_NodeReloadRequired" - The Node Information needs to be reloaded, via the [refreshNodeInfo](#refreshNodeInfo) Command. 

**Note**:

​    Depending upon the OZW configuration (set in Options.xml) a node may automatically reload 

**See Also**:

[checkLatestConfigRevision](#checkLatestConfigRevision), [checkLatestMFSRevision](#checkLatestMFSRevision), [downloadLatestMFSRevision](#downloadLatestMFSRevision)

## downloadLatestMFSRevision

**Params**:

​    None

**Returns**:

​    Event "downloadLatestMFSRevision" - if OZW accepted the command

**Notification**:

​    "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason.

**Note**:

​    A successful download of manufacturer_specific.xml will also trigger the download of any "new" device config files. This will happen transparently in the background.

**See Also**:

[checkLatestConfigRevision](#checkLatestConfigRevision), [checkLatestMFSRevision](#checkLatestMFSRevision), [downloadLatestConfigFileRevision](#downloadLatestConfigFileRevision)



## hardResetController

**Params**:

​    None

**Returns**:

​    Event "hardResetController" - if OZW accepted the command

**Notification**:

​    Various. OZW will remove all existing Nodes/Values from the MQTT Topic. Applications should also remove these Nodes/Values.

**Note**:

​    This will factory reset the Controller. All Nodes will need to be reincluded after this command. Applications should completely reset the state of Z-Wave.

**See Also**:
    [softResetController](#softResetController)

## hasNodeFailed

**Params**:

​    "node" - uint8: The NodeID of the device you wish to test.

**Returns**:

​    Event "hasNodeFailed" - if OZW accepted the command

**Notification**:

​    "hasNodeFailed" - Sent after the Controller as tested the Node.

**Note**:

​    This command forces the Controller to attempt to communicate with a Node, Regardless of the state of the nodes IsFailed status. This can be used to "revive" a dead node once the issues have been addressed (such as replacing a dead battery)

**See Also**:

​    [isNodeFailed](#isNodeFailed)

## healNetwork

**Params**:

​    None

**Returns**:

​    Event "healNetwork" - if OZW accepted the command

**Notification**:

​    None

**Note**:
    This asks the Z-Wave Network to rediscover nodes, neighbours and routes on the Network. This command should only be used under the following circumstances:
    1) Adding or Deleting Nodes
    2) Physically Moving a Device
    If you have a pure Z-Wave+ (or ZWave+2) network, this command *should* not be used.
    The actual networking healing process may take several hours, and is likely to consume the majority of the bandwidth on your network. You should expect Z-Wave to be sluggish as this command runs.

There is no Notification when the Network has completed the Healing Process.

Unnecessarily running this command on a Network with ZWave+ (and higher) devices can result in worse stability rather than better. Be Warned!

See Also:

​    [healNetworkNode](#healNetworkNode)

## healNetworkNode

**Params**:

​    "node" - uint8: The Node to execute this command on.

**Returns**:

​    "healNetworkNode" - If OZW accepted this command

**Notification**:

​    None

**Notes**:

​    Asks a Node to recalculate its neighbours and routes to other devices. 
​    See warning for [healNetwork](#healNetwork).

**See Also**:

​	[healNetwork](#healNetwork)

## isNodeFailed

**Params**:

​    "node" - uint8: The NodeID to check

**Returns**:

​    "isNodeFailed" - if OZW believes the node is dead. 

**Notification**:

​    None

**Notes**:

​    This does not confirm with the controller. Use [hasNodeFailed](#hasNodeFailed ) to have the controller check if the Node is alive or not.

**See Also**:

​    [hasNodeFailed](#hasNodeFailed)

## open

**Params**:

​    "serialport" - String: The Path to the Serial Port to open

**Returns**:

​    "open" - if OZW accepted the command

**Notification**:

​    Various - Sent as OZW starts the Network.

**Notes**:

​    Starts OZW by opening the Serial Port

**See Also**:
    [close](#close)

## ping

**Params**:

​    "ping" - String: A variable to return.

**Returns**:

​    "ping" - Field "pong" set to the value of the ping variabvle in the Command

**Notification**:

​    None

**Notes**:

​    Can be used to check if the OZW MQTT Client is alive

## refreshNodeInfo

**Params**:

​    "node" - uint8: The NodeID of the device you wish to refresh

**Returns**:

​    "refreshNodeInfo" - if OZW accepted the command

**Notification**:

​    Various - OZW will interview the devices capabilities and may delete or add new values or update the various properties of a device. 

**Notes**:

​    This essentially forces OZW to re-interview a device like it would when a device has just been included. You should use this when the configuration of the device changes its operation (consult the Device Manual for such scenarios) or after OZW has downloaded a new configuration file and you receive a notification indicating it needs to be reloaded.

**See Also**:

​    [downloadLatestConfigFileRevision](#downloadLatestConfigFileRevision)

## removeFailedNode

**Params**:

​    "node" - uint8: NodeID of the device that has failed.

**Returns**:

​    "removeFailedNode" - if OZW has accepted the command

**Notification**:

​    "RemoveFailedNode" - The status of the Controller removing the failed node.

**Notes**:

​    You can use this command if a Node is failed and it can't be excluded via the normal [removeNode](#removeNode ) command. The Controller needs to believe the node has failed, and you can force the controller to check the node via the [hasNodeFailed](#hasNodeFailed) command. 

**See Also**:

​    [hasNodeFailed](#hasNodeFailed)

## removeNode

**Params**:

​    None

**Returns**:

​    "removeNode" - if OZW has accepted the command

**Notification**:

​    "RemoveNode" - When the Controller has entered the Excluding Mode.

**Notes**:

​    Once you receive the RemoveNode Notification, you need to perform the network exclusion steps as detailed in the device manual to actually exclude the Node. 

**See Also**:

​	[removeFailedNode](#removeFailedNode)

## replaceFailedNode

**Params**:

​    "node" - uint8: The NodeID of the device you wish to replace.

**Returns**:

​    "replaceFailedNode" - if OZW has accepted the command

**Notification**:

​    "replaceFailedNode" - When the Controller has entered the replacefailednode state.

**Notes**:

​    You should use this command to include a device that has may have been factory reset or lost its configuration into the Network Again. It will reuse its old NodeID. 
​    This command may not always be successful, in which cases, you may have to include it as per [addNode](#addNode), and remove the old "ghost" node via the [removeFailedNode](#removeFailedNode) command

**See Also**:

​	[removeFailedNode](#removeFailedNode)


## requestAllConfigParam

**Params**:

​    "node" - uint8: The NodeID of the device you wish to get all Config Parameters from

**Returns**:

​    "requestAllConfigParam" - if OZW has accepted the command

**Notification**:

​    None

**Notes**:

​    OZW will get the latest configuration settings from a device. You should issue this after initially including a device or if the Device Settings were modified while OZW was offline.

**See Also**:

​	[requestConfigParam](#requestConfigParam)

## requestConfigParam

**Params**:

​    "node" - uint8: the NodeID of the device yu wish to get a Config Param

​    "param" - uint8: THe index of the Config Param you wish to get.

**Returns**:

​    "requestConfigParam" - if OZW accepted the command

**Notification**:

​    None

**Notes**:

​    OZW will refresh the particular config param 

**See Also**:

​	[requestAllConfigParam](#requestAllConfigParam)

## requestNetworkUpdate

**Params**:

​    "node" - uint8: The NodeID to execute this command against

**Returns**:

​    "requestNetworkUpdate" - if OZW accpeted the command

**Notification**:

​    None

**Notes**:

​    Only required for Networks that have older (pre-ZWave+ roughly) controllers. Requests the SUC to update the Network Routing Tables. 

## requestNodeDynamic

**Params**:

​    "node" - uint8: The NodeID to execute this command against

**Returns**:

​    "requestNodeDynamic" - if OZW accepted the command

**Notification**:

​    None

**Notes**:

​    OZW will refresh all ValueID's that are considered "Dynamic" - such as the State of a light etc

## requestNodeNeighborUpdate

**Params**:

​    "node" - uint8: The NodeID to execute this command against

**Returns**:

​    "requestNodeNeighborUpdate" - if OZW accepted the command

**Notification**:

​    "requestNodeNeighborUpdate" - Once the Node has a updated Neighbour list.

**Notes**:

​    Synchronises the list of Neighbours a node has.
​    

## requestNodeState

**Params**:

​	"node" - uint8: The NodeID to execute this command against

**Returns**:

​	"requestNodeState" - if OZW accepted the command

**Notification**:

​	None

**Notes**: 

​	This command refreshes all values that are considered both Static and Dynamic

## sendNodeInformation

**Params**:

​	"node" - uint8: the NodeID to send the NIF frame to

**Returns**:

​	"sendNodeInformation" - If OZW accepted the command

**Notification**:

​	"SendNodeInformation" - The status after sending a NIF to the device.

**Notes**:

​	This command sends a NIF (Not Information Frame) to the target device. 

## setValue

**Params**:

**Returns:**

**Notes**:

​	This command will set a new Value on a particular device. 

## softResetController

**Params**:

​	None

**Returns**:

​	"softResetController" - if OZW accepted the command

**Notification**:

​	Various - Several Notifications are sent as the Controller Restarts. 

**Notes**:

​	Resets the Controller. Configuration and Network Details are not reset. This is essentially like restarting 	OZW 

**See Also**:

​	[hardResetController](#hardResetController)

## testNetwork

**Params**:

​	"count" - int: The number of packets to send to the network

**Returns**:

​	"testNetwork" - if OZW Accepted the command

**Notification**:

​	"Notification" with Event Type set to "Code_NoOperation" - A Notification for each packet the Nodes Respond to, or a Notification with Event Type set to "Code_Timeout" if a Node failed to respond

**Notes**:

​	This sends a No Operation Message (NOP) to each node on the network. This can be used to test and measure the response times of each node on your network. You will receive a Notification indicating if the node responded or not 

**See Also**:

​	[testNetworkNode](#testNetworkNode)

## testNetworkNode

**Params**:

​	"node" - uint8: The NodeID to test

​	"count" - int: The number of packets to send to the Node

**Returns**:

​	"testNetworkNode" - if OZW accepted the command

**Notification**:

​	"Notification" with a Event Type set to "Code_NoOperation" - a Notification for each packet the Node Responds to. 

​	"Notification" with Event Type set to "Code_Timeout" if the Node Failed to respond. 

**Notes**:

​	This sends a No Operations Message (NOP) to the specified Node. This can be used to test and measure the response times of each node on your network.  You will receive a Notification indicating if the node responded or not 

**See Also**:

​	[testNetwork](#testNetwork)

