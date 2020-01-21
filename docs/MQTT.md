MQTT Client for OpenZWave
=========================

Docker Repo: https://hub.docker.com/r/openzwave/ozwdaemon

Copy the "config" folder from OZW to `/tmp/ozw/`.

Start a container with the following command line:
```
docker run -it --security-opt seccomp=unconfined --device=/dev/ttyUSB0 -v /tmp/ozw:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USBPATH=/dev/ttyUSB0 openzwave/ozwdaemon:latest
```

(replace `MQTT_SERVER` with the IP address of the MQTT Server and all `/dev/ttyUSB0` entries with the path to your USB Stick.

## MQTT Topic Structure
The MQTT Client for OZW will send messages to a MQTT Broker with a predifined topic structure. The structure of the messages are detailed in the sections below.

All topics live under the `OpenZWave/` topic structure. 

The MQTT Client allows for several different instances of OZW (and controllers) to run in parallel (thus manage several different networks). Different instances are indicated by a number and must be specified when starting the MQTT Client. Each instance will then publish to `OpenZWave/<instance/>`

The Status of the MQTT Client is published to the Status Topic as detailed in [MQTT Client Status](#mqtt-client-status)

Individual Nodes and the related values are published to the Nodes Topic

Statistics are published to either the Nodes Topic, or to the Instance Topic for OZW (if its Network wide Statistics)

Applications can communicate with the Network or control the MQTT Client via the [MQTT Commands](#mqtt-commands) and indvidual commands are documented in the [Commands List](#commands-list)

Network Related events or status changes to nodes (not relating to Values) are sent as [Notifications](#notifiations)

# MQTT Client Status
The MQTT Client status relays the status of the OZW Daemon and overall network status when starting up.

The Status is published to `OpenZWave/<instance>/status/` and payload is set to be retained by the MQTT Broker, so the payload should always indicate the most recent status of OZW. 

The Payload is:

"Status" - Enum: Indicates what state the library is in as far as if its ready, active or starting up.
* *Offline* - The MQTT Daemon is not running. This is set as Will Message, and will be published when the MQTT Client disconnects from the Broker.
* *starting* - the MQTT Daemon is starting up. Its reading config files and initilizing. This does not indicate it is bringing the network online.
* *started* - The MQTT Daemon has started and will transition to the Ready state shortly.
* *stoped* - OZW has been stopped and the network is offline. 
* *Ready* - OZW is up and running and ready to Start. The Client Application should send a "open" command to attach OZW to a serial port that is connected to a ZWave USB Dongle.
* *driverReady* - OZW has opened the Serial Port and is initilzing the USB Controller.
* *driverFailed* - OZW failed to start the Serial Port Driver or Initilize the USB Controller. 
* *driverReset* - The Driver has been reset and OZW is offline.
* *driverRemoved* - The Driver has been removed. Usually emitted when shuting down, or executing the close command.
* *driverAllNodesQueriedSomeDead* - OZW has Initilized the Network and completed querying Devices on the Network. Some Nodes were detected as Dead and should be either removed or resurected after performing diagnostics on the affected nodes.
* *driverAllNodesQueried* - OZW has Initilized the Network and completed querying all devices on the Network. 
* *driverAwakeNodesQueried* - OZW has Initilized the Network, and completed querying all "awake" or mains powered devices. Battery or Sleeping Devices may still be not fully initilized and need to be woken up before they are fully operational
 
"TimeStamp" - uint64: Timestamp when the Message was sent. Applications can use this to track when the status was updated

"homeID" - uint: Decimal Representation of the Network ID or Home ID when the ZWave Network is operational.

"ManufacturerSpecificDBReady" - Bool: Indicates that the Config Files have been loaded and are valid. A false reading here indicates a problem with the config files and OZW may not be able to function fully.

# Node and Value Topics
The MQTT Client publishes details about Nodes and their related Values in a hierarchical manner. The Node and Value topics are set to be retained by the MQTT Broker, so as long as the [Status message](#mqtt-client-status) is "driverAllNodesQueriedSomeDead", "driverAllNodesQueried" or "driverAwakeNodesQueried", then you can be assured it is the latest data. 

When the MQTT Client initially connects to the Broker, it will clean up any previous Nodes/Values payloads that are stored on the Broker. Client Applications should not interpret this "cleanup" as Nodes or Values being deleted. (Applications should only take action on a Node/Value topic being removed when the Status Message indicates OZW is operational)

Node related information is published to `OpenZWave/<instance>/node/<nodeid>/`. See below for details on the most important parts of the Payload.

## Node Payload

The following fields are present in the Node Payload:

### NodeID
Type: Integer

Description: The Node ID of the device on the Network

### NodeQueryStage
Type: - Enum

Description: The Stage of the Interview process of a Node. The Target Stage would be Complete. 

Possible Values:
* *None* - Interview Process has not started for this node
* *ProtocolInfo* - Obtaining basic Z-Wave Capabilities of this node from the controller
* *Probe* - Check if the Node is Alive/Awake
* *WakeUp* - Setup Support for Wakeup Queues and Messages
* *ManufacturerSpecific1* - Obtain Manufacturer/Product ID and Type Codes from device
* *NodeInfo* - Obtain supported Command Classes from the Node
* *NodePlusInfo* - Obtain Z-Wave+ Relvent information if supported by the Node 
* *ManufacturerSpecific2* - Obtain Manufacturer/Product ID and Type Codes from device.
* *Versions* - Get information about Firmware and CommandClass Versions
* *Instances* - Get Details about what instances/channels a device supports
* *Static* - Obtain Values from the device that are static/don't change.
* *CacheLoad* - Load information from the Cache File. If its a sleeping device, it will stay at this stage until the device wakes up
* *Associations* - Refresh Association Groups and Memberships
* *Neighbors* - Get the Neighbors List
* *Session* - Get infrequently changing values from device
* *Dynamic* - Get Frequently changing values from device
* *Configuration* - Obtain Configuration Values from the device
* *Complete* - Interview Process is Complete.

### isListening
Type: Bool

Description: If the Node is in constant listening mode. This usually indicates mains powered devices

### isFlirs
Type: Bool

Description: If the Device Supports FLiRS (Frequent Listening) - The device periodically checks for messages for it. Requires the sending node/neighbors to support Beaming. Usually found on Locks

### isBeaming
Type: bool

Description: If the node has the capability to "wake up" a FLiRS device.

### isRouting
Type: bool

Description: If the Node is participating in routing messages on the Mesh Network. 

### isSecurityv1
Type: bool

Description: If the Device Supports the original version of encryption on Z-Wave. This is **NOT** S0 or S2 Security

### isZWavePlus
Type: bool

Description: If the Device is a Z-Wave+ Device

### isNIFRecieved
Type: bool

Description: If the NIF (Node Information Frame) has been recieved. This is part of the Interview process.

### isAwake
Type: bool

Description: If the Node is Awake and Able to recieve messages

### isFailed
Type: book

Description: If the Z-Wave Controller believes the node has failed. (eg, dead battery etc)

### MetaData
Type: Structure

Description: MetaData about the Device, such as inclusion/exclusion data, product pictures, manuals etc

The MetaData Fields are:

#### OZWInfoURL
Type: String

Description: URL to the OZW Database Entry for this device.

#### ZWAProductURL
Type: String

Description: URL to the Z-Wave Alliance Product Page

#### ProductPIC
Type: String

Description: Filesystem/HTTP Path to the Product Picture.

#### Description
Type: String

Description: Description of the device and its features.

#### ProductManualURL
Type: String

Description: URL to the Product Manual

#### ProcuctPageURL
Type: String

Description: URL the vendors website for the Device

#### InclusionHelp
Type: String

Description: Instructions for performing inclusion in the Network

#### ExclusionHelp
Type: String

Description: Instructions for performing exclusion of the device from a Network

#### ResetHelp
Type: String

Description: Instructions for performing a factory reset of the device

#### WakeupHelp
Type: String

Description: Instructions for Waking up the Device

#### ProductSupportURL:
Type: String

Description: URL to the vendors support pages for the device

#### Frequency:
Type: String

Description: Frequency/Region this device operates in.

#### Name:
Type: String

Description: Long Version of the Product Name

#### ProductPicBase64:
Type: String

Description: The Product Picture (200x200 resolution) encoded with base64. 

### Event
Type: Enum

Description: The Last Event that triggered a update to this payload.
Please see the Notifications Topic for possible values

### TimeStamp
Type: Integer

Description: The timestamp when this message was published

### NodeManufacturerName
Type: String

Description: The Name of the Manufacturer of this device

### NodeProductName
Type: String

Description: The Short Name of this product

### NodeBasicString
Type: String

Description: The Basic Type of Node

### NodeBasic
Type: Integer

Description: The Basic Type of the Node (as a Integer)

### NodeGenericString
Type: String

Description: The Generic Function of this device

### NodeGeneric
Type: Integer

Description: The Generic Function of this device (as a Integer)

### NodeSpecificString
Type: String

Description: The Specific Function of this device

### NodeSpecific
Type: Integer

Description: The Specific Function of this device

### NodeManufactuerID
Type: String

Description: The Manufacturer ID of the device, in HEX

### NodeProductType
Type: String

Description: The Product Type of the device, in HEX

### NodeProductID
Type: String

Description: The Product ID of the device, in HEX

### NodeBaudRate
Type: Integer

Description: THe Maximum Speed the device communicates on the network at

### NodeVersion
Type: Integer

Description: The SDK Version of the Node. 

### NodeGroups
Type: Integer

Description: The Number of Association Groups the Node Supports

### NodeName
Type: String

Description: The User Defined name of the Node. Only supported if the device has NodeNaming CommandClass

### NodeLocation
Type: String

Description: The User Defined location of the Node. Only supported if the device has the NodeNaming CommandClass

### NodeDeviceTypeString
Type: String

Description: The DeviceType of the Node. Only Supported if the Node is Z-Wave+ 

### NodeDeviceType
Type: integer

Description: The DeviceType of the Node as a Integer. Only Supported if the Node is Z-Wave+ 

### NodeRoleString
Type: String

Description: The Role of the Node in the Z-Wave Network. Only available if the device is Z-Wave+

### NodeRole
Type: integer

Description: The Role of the Node as a integer in the Z-Wave Network. Only available if the device is Z-Wave+

### NodePlusTypeString
Type: String

Description: The Type of Z-Wave+ Device this node is. Only available if the device is Z-Wave+

### NodePlusType
Type: String

Description: The Type of Z-Wave+ Device as a integer this node is. Only available if the device is Z-Wave+

### Neighbors
Type: Array (Integers)

Description: The Neighbors the Node can see. This is not the full routing table from a node and only a partial representation of how a node participates in the Mesh Network. You should not draw any conclusions about the effectiveness of your Mesh with this information alone without understaning how Z-Wave Mesh Networks and routing are performed.

## Instance Payload
The Instance Topic represents the different instances/channels/endpoints available on a Node.

## CommandClass Payload
The CommandClass Topic represents the different commandclasses a endpoint supports on the device.

## Value Payload
The Value Payload represent the individual values that a application can monitor/alter on a device. This is the Primary topic that Applications would interact with in order to get the status of different paramaters a device might expose, such as the state of a light, or power consumption

# MQTT Commands

Most OZW "Commands" run asynchronously meaning that the results of the command may not be immediately available and require talking to the Device on your network. 

The MQTT Client for OZW will usually report the status of each command it process in at least one stage, although some commands will return the progress of the command:

1) Command Accepted by OZW - This means any parameters required are passed and valid and that the OZW library has accepted the command
2) For some commands, the subsequent updates are reported via Notifications, which will be reported to the `/OpenZWave/<instance>/event/<command>/` (where command may be a specific command, or in some cases, "ControllerCommand" where the notification may be generic in nature)
    

## Command Status and Feedback

All Commands are located under the `OpenZWave/<instance>/command/<command>/` topics. Clients will send a JSON payload with the required parameters to the relevant topic to execute the command. 

As OZW and Z-Wave in general operates in a asynchronous manner, feedback about most commands will be sent in stages. More often that not, the first feedback will be to indicate if OZW has accepted the command and will process it. This usually just involves checking any parameters are valid, and the Network is in a state that can accept the command.

In the Command List Section below, these immediate updates are indicated as "Returns"  and are sent to the  `OpenZWave/<instance>/event/<command>/` topic

For Commands that send subsequent updates, these are sent as "Notifications" to the `OpenZWave/<instance>/event/<topic>/` topic with the payload described below. Please note, that some "Notifications" maybe unsolicited, such as Timeout messages, or when commands are executed by another client application, or changes in the Z-Wave Network. Please see the Notifications Section for details.

## Return Payload

Return Payloads usually indicate if OZW has accepted the command or not - It does not mean that the command has been executed, and does not indicate if the command is successful or not. In most cases, it is to indicate that the command has been received and will be processed. These types of "Returns" are indicated as in the command list with the description "if OZW has accepted the command"

The Payload for these returns are below. For commands where the result of a command can be returned immediately, the payload will be documented in the relevant command list entry below

**Param**:

"status" - Text - Value of "ok" indicates the command was accepted. value of "failed" indicates command was not accepted and the "error" Param should describe the error

"error" - Text - If present, a short description of why the command failed. Could be things like malformed JSON payload, missing Parameters, incorrect parameter types (a string instead of integer JSON value for example), or in some cases, invalid NodeID's passed to the command. 

# Notifications

Notifications are issued to update the application on the success/failure or action required by a user/application in response to a command or something happening on the ZWave Network. Some Notifications may be unsolicited (due to a event on the ZWave Network) while others are only sent in response to commands. 

Some commands will send further updates as the are executed on the Network. These are indicated as Notifications in the individual [Command Lists](#commands-lists) section.

Notifications are sent to a specific topic that relates to the type of Notification, or Command that triggered it. The Topic that a command may reply on are indicated in the CommmandList. Note, some Notifications may be sent even without a application executing a command. This may be due to another client application, or activity on the network.
The topics are located at `OpenZWave/<instance>/event/<topic>/`

There are 3 General Types of Notifications:
1.  Controller Commands - These are Notifications that are generated by Commands Executed by the USB Controller
2. Generic Notifications - Usually Related to the state of individual nodes on the Network (such as Awake/Sleeping/Alive/Dead)
3. User Notifications - These are Generated by OZW and usually should be conveyed to the user in some mannoer. 

## Controller Notifications
These commands related to some activity or action performed by a controller. Either Executed by a Application or a event on the Network.

"Node" - uint8: If applicable, the Node that this Notification is for.

"State" - Enum: Reports the progress, or state of a command being executed on the network. Not all States are applicable for all commands. The Possible Options are:

* *Ctrl_State_Normal* - OZW is in normal operations mode
* *Ctrl_State_Starting* - The Action is starting. OZW is now in Controller Mode.
* *Ctrl_State_Cancel* - The Action is canceled. OZW is returning to Network Mode.
* *Ctrl_State_Error* - A error occurred. Consult the Error field. 
* *Ctrl_State_Waiting* - The Action is pending a User Activity, such as setting the device into inclusion/exclusion mode etc.
* *Ctrl_State_Sleeping* - The Device is Sleeping.
* *Ctrl_State_InProgress* - The Action is in progress.
* *Ctrl_State_Completed* - The Action completed. OZW is returning to Network Mode. 
* *Ctrl_State_Failed* - The Action Failed. OZW is returning to Network Mode.
* *Ctrl_State_NodeOk* - The Node is marked as healthy. 
* *Ctrl_State_NodeFailed* - The Node is marked as dead.

"Error" - Enum: When the Z-Wave Network returns a error for a command, This field will contain details about the error detected:
* *Ctrl_Error_None* - No Error Occurred.
* *Ctrl_Error_ButtonNotFound* - The Button was not found.
* *Ctrl_Error_NodeNotFound* - The Node was not found.
* *Ctrl_Error_NotBridge* - The Controller is not of a Bridge Type Controller.
* *Ctrl_Error_NotSUC* - The Node is not a SUC (Static Update Controller).
* *Ctrl_Error_NotSecondary* - The Controller is not a Secondary Controller.
* *Ctrl_Error_NotPrimary* - The Controller is not a Primary Controller. 
* *Ctrl_Error_IsPrimary* - The Controller is a Primary Controller.
* *Ctrl_Error_NotFound* - The Node/Action etc was not Found.
* *Ctrl_Error_Busy* - The Network or Controller are busy and cannot perform the action requested. 
* *Ctrl_Error_Failed* - The Command/Action Failed.
* *Ctrl_Error_Disabled* - The Command/Action is disabled by OZW or the controller.
* *Ctrl_Error_OverFlow* - The Controller has overflowed and cannot continue.

"Command" - Enum: For some cases, where a notification is related to a Command, this field will be present:
* *Ctrl_Cmd_None* - No Specific Command is related to this Notification.
* *Ctrl_Cmd_AddNode* - This notification is related to a AddNode Command.
* *Ctrl_Cmd_CreateNewPrimary* - This notification is related to a CreateNewPrimary Command.
* *Ctrl_Cmd_ReceiveConfiguration* - This notification is related to a ReceiveConfiguration Command (usually related to changing a Primary/Secondary Controller)
* *Ctrl_Cmd_RemoveNode* - This Notification is related to a Remove Node Command.
* *Ctrl_Cmd_RemoveFailedNode* - This Notification is related to a RemoveFailedNode Command.
* *Ctrl_Cmd_HasNodeFailed* - This Notification is related to a HasNodeFailed Command.
* *Ctrl_Cmd_ReplaceFailedNode* - This Notification is related to a ReplaceFailedNode Command.
* *Ctrl_Cmd_TransferPrimaryRole* - This Notifiation is related to a TransferPrimaryRole Command.
* *Ctrl_Cmd_RequestNetworkUpdate* - This Notification is related to a RequestNetworkUpdate Command.
* *Ctrl_Cmd_RequestNodeNeighborUpdate* - This Notification is related to a RequestNodeNeighborUpdate Command.
* *Ctrl_Cmd_AssignReturnRoute* - This Notification is related to a AssignReturnRoute Command.
* *Ctrl_Cmd_DeleteAllReturnRoute* - This Notification is related to a DeleteAllReturnRoute Notification.
* *Ctrl_Cmd_SendNodeInformation* - This Notification is related to a SendNodeInformation Command
* *Ctrl_Cmd_ReplicationSend* - This Notification is Related to Controller Replication Commands.
* *Ctrl_Cmd_CreateButton* - This Notification is related to CreateButton Commands.
* *Ctrl_Cmd_DeleteButton* - This Notification is related to DeleteButton Commands.

## Generic Notifications
These Notifications are sent to the `OpenZWave/<instance>/event/Notification/` topic and contain the following payload.

"Node" - uin8: The Node that this notification relates to

"Event" - Enum: The type of event that occurred:
* Notification_Code_MsgComplete - The Message Transaction is completed. This will only be sent if Enabled via the Options Class/Config File.
* Notification_Code_MsgTimeout - A Message to the Node Timed out. Could indicate a Network issue, or unsupported command on the Node
* Notification_Code_NoOperation - The Node Replied to a No Operation Packet.
* Notification_Code_NodeAwake - The Node is now awake.
* Notification_Code_NodeAsleep - The Node is now asleep
* Notification_Code_NodeDead - The Node has been marked as Dead.
* Notification_Code_NodeAlive - The Node has been marked as alive.

## User Notifications
These Notifications Related to events that should be conveyed to the user somehow to indicate activity or action that needs to be taken. These Notifications are sent to the `OpenZWave/<instance>/event/UserAlert/` topic and contain the following payload:

"Node" - uint8: The Node that this notification relates to

"Event" - Enum: The Type of Event that occurred:
* Notification_User_None - There is no Event.
* Notification_User_ConfigOutOfDate - A Config File is out of date and the user should consider executing the [downloadLatestConfigFileRevision](#downloadLatestConfigFileRevision) commannd to update it.
* Notification_User_MFSOutOfDate - The manufacturer_specific.xml file is out of date, and a user should consider executing the [downloadLatestMFSRevision](#downloadLatestMFSRevision) command to update it.
* Notification_User_ConfigFileDownloadFailed - A Config file failed to download.
* Notification_User_DNSError - A error occurred looking up the latest config file revisions
* Notification_User_NodeReloadRequired - The User should reload a node. This is often emmited after a new config file was downloaded. The user should consider executing [refreshNodeInfo](#refreshNodeInfo)
* Notification_User_UnsupportedController - The Controller is not supported by OZW.
* Notification_User_ApplicationStatus_Retry - The Node did not accept a update/packet and wants the application to retry. The delay the application should retry in is indicated in a "Retry" field present in the payload.
* Notification_User_ApplicationStatus_Queued - The Node has queued a update/packet for execution at a later time.
* Notification_User_ApplicationStatus_Rejected - The Node has rejected a update/packet.


# Commands Lists
This is a list of Commands that the MQTT Client will accept and process. As mentioned above, the Commands often return a status, and then subsequent notifications to indicate the progress or success/failure of the command. 

## addNode

**Params**:

​    "secure" - bool: If we should attempt to add the node to a network securely.

**Returns**:

​    Event "addNode" - if OZW has accepted the command or not. 

**Notifications**:

​    topic "addNode" - Status if the Controller has successfully entered into Inclusion Mode or not. 

**Notes**:

​    If a Node is successfully added, its details will be progressively added to the Nodes topic with all applicable ValueID's

**See Also**:
    [cancelControllerCommand](#cancelControllerCommand)

## assignReturnRoute

**Params**:

​    "node" - uint8: The NodeID to execute this command on

**Returns**:

​    Event "assignReturnRoute" - If OZW accepted the command or not.

**Notification**:

​    topic "assignReturnRoute" - Status of the Controller after issuing the command to the Node. 

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

​    "node" - uint8: Instructs OZW to check if it has the latest version of the config file for a particular node

**Returns**:

​    Event "checkLatestConfigFileRevision" - if OZW accepted the command.

**Notification**:

​    Topic "Notification" with the field "Event" set to Alert_ConfigOutOfDate if the Config file is out of date. Payload of the Message should include the Node in question

​    Topic "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 

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

​    Topic "Notification" with the field "Event" set to Alert_MFSOutOfDate if the Manufacturer_specific.xml file is out of date. 

​    Topic "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 

**Note**:
    OZW will check if the installation has the latest manufacturer_specific.xml file. Use this if you have a device that is not recognized but present in the latest release of OZW.

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

​    As OZW shuts down, it will progressively delete values/notes and association information from the MQTT Topics. Applications should not treat the disappearance of these values as NodeDeleted or ValueDeleted once a "close" event is received 

**See Also**:

​    [open](#open)

## deleteAllReturnRoute

**Params**:

​    "node" - uint8: The NodeID to perform the action on

**Returns**:

​    Event "deletAllReturnRoute" - if OZW accepted the command

**Notification**:

​    Topic "deleteAllReturnRoute" - Status of the command after OZW has processed it.

**Note**:

​    This is a advanced command and should not be required on a ZWave+ Device.

## downloadLatestConfigFileRevision

**Params**:

​    "node" - uint8: The NodeID to Update the Config File for

**Returns**:

​    Event "downloadLatestConfigFileRevision" - if OZW accepted the command

**Notification**:
    topic "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason

​   topic "Notification" with Event field set to "Alert_NodeReloadRequired" - The Node Information needs to be reloaded, via the [refreshNodeInfo](#refreshNodeInfo) Command. 

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

​    topic "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason.

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

​    This will factory reset the Controller. All Nodes will need to be re-included after this command. Applications should completely reset the state of Z-Wave.

**See Also**:
    [softResetController](#softResetController)

## hasNodeFailed

**Params**:

​    "node" - uint8: The NodeID of the device you wish to test.

**Returns**:

​    Event "hasNodeFailed" - if OZW accepted the command

**Notification**:

​    topic "hasNodeFailed" - Sent after the Controller as tested the Node.

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
    This asks the Z-Wave Network to rediscover nodes, neighbors and routes on the Network. This command should only be used under the following circumstances:
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

​    Asks a Node to recalculate its neighbors and routes to other devices. 
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

​    "ping" - Field "pong" set to the value of the ping variable in the Command

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

​    topic "RemoveFailedNode" - The status of the Controller removing the failed node.

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

​    topic "RemoveNode" - When the Controller has entered the Excluding Mode.

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

​    topic "replaceFailedNode" - When the Controller has entered the replacefailednode state.

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

​    "node" - uint8: the NodeID of the device you wish to get a Config Param

​    "param" - uint8: The index of the Config Param you wish to get.

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

​    "requestNetworkUpdate" - if OZW accepted the command

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

​    topic "requestNodeNeighborUpdate" - Once the Node has a updated Neighbor list.

**Notes**:

​    Synchronizes the list of Neighbors a node has.
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

​	topic "SendNodeInformation" - The status after sending a NIF to the device.

**Notes**:

​	This command sends a NIF (Not Information Frame) to the target device. 

## setValue

This allows a MQTT Client to set a value on a Device. As OZW supports many different types of Values, you must ensure that the payload that you send matches the type of Value you are attempting to change (String, Integer etc)

**Params**:

    "ValueIDKey" - the ValueID Key Number of the Value You want ot change

    "Value" - The new Value to Set - Encoded as below:

        BitSet - A Array of Bits you wish to change:
        ```"Value": { [
            {"Label":"<string label of the BitSet you wish to change>",
             "Value":<bool>
            },
            {"Position":<position of the BitSet you wish to change>,
             "Value":<bool>
            }
        ]}```
        You can specify either Label or Position

        Bool - true/false

        Button - true/false 
        sending "True" simulates a button push, sending false simulates a button release

        Byte/Short/Integer - Decimal value that falls within the range of the type you are changing

        Decimal - Value encoded as a JSON Double

        List - Value is the Position of the entry you wish to select

        Raw - Value is encoded as a Hex String

        String - Value is encoded as a JSON String

**Returns:**
  ​	"setValue" - If OZW accepted the command

**Notification**:

​	The Affected ValueID Topic will be updated with the new value if the Device accepted the change. 

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

​topic "Notification" with Event Type set to *Code_NoOperation* - A Notification for each packet the Nodes Respond to, or a Notification with Event Type set to *Code_Timeout* if a Node failed to respond

**Notes**:

​	This sends a No Operation Message (NOP) to each node on the network. This can be used to test and measure the response times of each node on your network. You will receive a Notification indicating if the node responded or not 

**See Also**:

​	[testNetworkNode](#testNetworkNode)

## testNetworkNode

**Params**:

​	"node" - uint8: The NodeID to test

​	"count" - int: The number of packets to send to the Node

**Returns**:

​"testNetworkNode" - if OZW accepted the command

**Notification**:

​topic "Notification" with a Event Type set to *Code_NoOperation* - a Notification for each packet the Node Responds to. 

​topic "Notification" with Event Type set to *Code_Timeout* if the Node Failed to respond. 

**Notes**:

​	This sends a No Operations Message (NOP) to the specified Node. This can be used to test and measure the response times of each node on your network.  You will receive a Notification indicating if the node responded or not 

**See Also**:

​	[testNetwork](#testNetwork)

## addAssociation

**Params**:

​	"node" - uint8: The NodeID to test

​	"group" - uint8: The Association Group you wish to add a member to

   "target" - string: The Target Node you wish to send notifications to for this group. 

**Returns**:

​"addAssociation" - if OZW accepted the command

**Notification**:

​topic "nodeGroupChanged" Indicates the Memberships was successfully refreshed from the device

**Notes**:

    Note - The Target string can be specified as a single NodeID (eg, "1" or "1.0") or a NodeID​ and Instance as "<nodeid>.<instance>". This allows you to configure a device to send a notification to a different instance on Multi Channel Devices. You should ensure that the target device has a valid instance and supports the commands it will recieve. 
    If you specify a instance (other than 0) for a target node, but the group does not support sending MultiChannel messages, the instance will be removed (or set to 0, which indicates the root node). 

**See Also**:

​	[removeAssociation](#removeAssociation)

## removeAssociation

**Params**:

​	"node" - uint8: The NodeID to test

​	"group" - uint8: The Association Group you wish to add a member to

   "target" - string: The Target Node you wish remove from the Group Membership List. 

**Returns**:

​"removeAssociation" - if OZW accepted the command

**Notification**:

​topic "nodeGroupChanged" Indicates the Memberships was successfully refreshed from the device

**Notes**:

    Note - The Target string can be specified as a single NodeID (eg, "1" or "1.0") or a NodeID​ and Instance as "<nodeid>.<instance>". You should first check the membership list via the Association Topics for the device and only attempt to remove existing members. Attempting to remove a member that does exist will result in a error.

**See Also**:

​	[addAssociation](#addAssociation)

## enablePoll

**Params**:

​	"ValueIDKey" - uint64: The ValueID Key

​	"Intensity" - uint8: The Intensity we should poll the value at

**Returns**:

​"enablePoll" - if OZW accepted the command

**Notification**:

The appropriate value key in the MQTT Topic should be updated with a updated IsPolled Value

**See Also**:

​	[disablePoll](#disablePoll)

## disablePoll

**Params**:

​	"ValueIDKey" - uint64: The ValueID Key

**Returns**:

​"disablePoll" - if OZW accepted the command

**Notification**:

The appropriate value key in the MQTT Topic should be updated with a updated IsPolled Value

**See Also**:

​	[enablePoll](#enablePoll)
