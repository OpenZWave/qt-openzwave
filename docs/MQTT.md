MQTT Client for OpenZWave
=========================

Docker Repo: https://cloud.docker.com/u/openzwave/repository/docker/openzwave/ozw-mqtt

Copy "config" folder from OZW to /tmp/ozw/

Start Container with the following command line:
>docker run -it --device=/dev/ttyUSB0 -v /tmp/ozw:/opt/ozw/config -e MQTT_SERVER="10.100.200.102" -e USBPATH=/dev/ttyUSB0 ozw-mqtt:latest

(replace MQTT_SERVER with IP address of the MQTT Server and all /dev/ttyUSB0 entries with path to your USB Stick

MQTT Commands
-------------

Most OZW "Commands" asyncronys meaning that the results of the command may not be immediatly available and require talking to the Device on your network. 

The MQTT Client for OZW will usually report the status of each command it process in at least one stages, although some commands will return the progress of the command:

1) Command Accepted by OZW - This means any paramaters required are passed and valid and that the OZW library has accepted the command
2) For some commands, the subsequent updates are reported via Notifications, which will be reported to the /OpenZWave/<instance>/event/<command>/ (where command may be a specific command, or in some cases, "ControllerCommand" where the notification may be generic in nature)
    Please see the documentation of each command to determine where responses may be sent.

All Commands are located under the "OpenZWave/<instance>/command/<command>/ topics
Success/Failure of commands is sent to the OpenZWave/<instance>/event/<command>/ topic

Commands Lists
--------------
addNode
-------
Params:
    "secure" - bool: If we should attempt to add the node to a network securely.
Returns:
    Event "addNode" - if OZW has accepted the command or not. 
Notifications:
    Event "addNode" - Status if the Controller has sucessfully entered into Inclusion Mode or not. 
    Event "ControllerCommand" - When Controller enters and exits from Inclusion Mode

Notes:
    If a Node is succesfully added, its details will be progressively added to the Nodes topic with all applicable ValueID's

See Also:
    cancleControllerCommand

assignReturnRoute
-----------------
Params:
    "node" - uint8: The NodeID to execute this command on
Returns:
    Event "assignReturnRoute" - If OZW accepted the command or not.
Notification:
    Event "assignReturnRoute" - Status of the Controller after issuinng the command to the Node. 

Notes:
    Asks thee Node to calculate the Route to communicate with the Controller. This is a advanced command and should not be required on ZWave Plus devices. 

cancelControllerCommand
-----------------------
Params:
    None
Returns:
    Event "cancelControllerCommand" - if OZW accepted the command or not
Notification:
    None

Note:
    Cancels in Controller Commands that are in progress.

checkLatestConfigFileRevision
-----------------------------
Params:
    "node" - uint8: Instructs OZW to check if it has hte latest version of the confile file for a particular node
Returns:
    Event "checkLatestConfigFileRevision" - if OZW accepted the command.
Notification:
    "Notification" with the field "Event" set to Alert_ConfigOutOfDate if the Config file is out of date. Payload of the Message should include the Node in question
    "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 


Note:
    OZW will check if the installation has the latest config file for a particular node.

See Also:
    checkLatestMFSRevision, downloadLatestConfigFileRevision, downloadLatestMFSRevision

checkLatestMFSRevision
----------------------
Params:
    None
Returns:
    Event "checkLatestMFSRevision" - if OZW accepted the command
Notification:
    "Notification" with the field "Event" set to Alert_MFSOutOfDate if the Manufacturer_specific.xml file is out of date. 
    "Notification" with Event field set to "Alert_DNSError" - OZW could not get the latest version due to a DNS Failure. 


Note:
    OZW will check if the installation has the latest manufacturer_specific.xml file. Use this if you have a device that is not recongised but present in the latest release of OZW.

See Also:
checkLastestConfigFileRevision, downloadLatestConfigFileRevision, downloadLatestMSFRevision

close
-----
Params:
    None
Returns:
    Event "close" - if OZW accepted the command
Notification:
    A Series of Events and Notificaitons will be issued as OZW shuts down. Eventually the OZW status will be set to stopped when OZW has fully shutdown.

Note:
    As OZW shuts down, it will progressively delete values/notes and association information from the MQTT Topics. Applications should not treat the disappearance of these values as NodeDeleted or ValueDeleted once a "close" event is recieved 

See Also:
    open

deleteAllReturnRoute
--------------------
Params:
    "node" - uint8: The NodeID to perform the action on
Returns:
    Event "deletAllReturnRoute" - if OZW accepted the command
Notification:
    deleteAllReturnRoute - Status of the command after OZW has processed it.

Note:
    This is a advanced command and should not be required on a ZWave+ Device.

downloadLatestConfigFileRevision
--------------------------------
Params:
    "node" - uint8: The NodeID to Update the Config File for
Returns:
    Event "downloadLatestConfigFileRevision" - if OZW accepted the command
Notification:
    "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason
    "Notification" with Event field set to "Alert_NodeReloadRequired" - The Node Information needs to be reloaded, via the refreshNodeInfo Command. 

Note:
    Depending upon the OZW configuration (set in Options.xml) a node may automatically reload 

downloadLatestMFSRevision
-------------------------
Params:
    None
Returns:
    Event "downloadLatestMFSRevision" - if OZW accepted the command
Notification:
    "Notification" with Event field set to "Alert_ConfigFileDownloadFailed" - The Download Failed for whatever reason.

Note:
    A sucessful download of manufacturer_specific.xml will also trigger the download of any "new" device config files. This will happen transparently in the backgound.

hardResetController
-------------------
Params:
    None
Returns:
    Event "hardResetController" - if OZW accepted the command
Notification:
    Various. OZW will remove all existing Nodes/Values from the MQTT Topic. Applications should also remove these Nodes/Values.

Note:
    This will factory reset the Controller. All Nodes will need to be reincluded after this command. Applications should completely reset the state of Z-Wave.

See Also:
    softResetController

hasNodeFailed
-------------
Params:
    "node" - uint8: The NodeID of the device you wish to test.
Returns:
    Event "hasNodeFailed" - if OZW accepted the command
Notification:
    "hasNodeFailed" - Sent after the Controller as tested the Node.

Note:
    This command forces the Controler to attempt to communicate with a Node, Regardless of the state of the nodes IsFailed status. This can be used to "revive" a dead node once the issues have been addressed (such as replacing a dead battery)

See Also:
    isNodeFailed

healNetwork
-----------
Params:
    None
Returns:
    Event "healNetwork" - if OZW accepted the command
Notification:
    None

Note:
    This asks the Z-Wave Network to rediscover nodes, neighbors and routes on the Network. This command should only be used under the following circumstances:
    1) Adding or Deleting Nodes
    2) Physically Moving a Device
    If you have a pure Z-Wave+ (or ZWave+2) network, this command *should* not be used.
    The actual networking healing process may take several hours, and is likely to consume the majority of the bandwidth on your network. You should expect Z-Wave to be slugish as this command runs.

    There is no Notificaiton when the Network has completed the Healing Process.

    Unecessarly running this command on a Network with ZWave+ (and higher) devices can result in worse stability rather than better. Be Warned!

See Also:
    healNetworkNode

healNetworkNode
---------------
Params:
    "node" - uint8: The Node to execute this command on.
Returns:
    "healNetworkNode" - If OZW accepted this command
Notification:
    None

Notes:
    Asks a Node to recalculate its neighbors and routes to other devices. 
    See warning for healNetwork.

isNodeFailed
------------
Params:
    "node" - uint8: The NodeID to check
Returns:
    "isNodeFailed" - if OZW believes the node is dead. 
Notification:
    None

Notes:
    This does not confirm with the controller. Use HasNodeFailed to have the controller check if the Node is alive or not.

See Also:
    hasNodeFailed

open
----
Params:
    "serialport" - String: The Path to the Serial Port to open
Returns:
    "open" - if OZW accepted the command
Notification:
    Various

Notes:
    Starts OZW by opening the Serial Port

See Also:
    close

ping
----
Params:
    "ping" - String: A variable to return.
Returns:
    "ping" - Field "pong" set to the value of the ping variabvle in the Command
Notification:
    None

Notes:
    Can be used to check if the OZW MQTT Client is alive


refreshNodeInfo
---------------
Params:
    "node" - uint8: The NodeID of the device you wish to refresh
Returns:
    "refreshNodeInfo" - if OZW accepted the command
Notification:
    Various - OZW will interview the devices capabilities and may delete or add new values or update the various properties of a device. 

Notes:
    This essentially forces OZW to reinterview a device like it would when a device has just been included. You should use this when the configuration of the device changes its operation (consult the Device Manual for such scenarios) or after OZW has downloaded a new configuration file and you recieve a notification indiciating it needs to be reloaded.

See Also:
    downloadLatestConfigFileRevision

removeFailedNode
----------------
Params:
    "node" - uint8: NodeID of the device that has failed.
Returns:
    "removeFailedNode" - if OZW has accepted the command
Notification:
    "RemoveFailedNode" - The status of the Controller removing the failed node.

Notes:
    You can use this command if a Node is failed and it can't be excluded via the normal removeNode command. The Controller needs to believe the node has failed, and you can force the controller to check the node via the "hasNodeFailed" command. 

See Also:
    hasNodeFailed

removeNode
----------
Params:
    None
Returns:
    "removeNode" - if OZW has accepted the command
Notification:
    "RemoveNode" - When the Controller has entered the Exclusing Mode.

Notes:
    Once you recieve the RemoveNode Notification, you need to perform the network exclusion steps as detailed in the device manual to actually exclude the Node. 

replaceFailedNode
-----------------
Params:
    "node" - uint8: The NodeID of the device you wish to replace.
Returns:
    "replaceFailedNode" - if OZW has accepted the command
Notification:
    "replaceFailedNode" - When the Controller has entered the replacefailednode state.

Notes:
    You should use this command to include a device that has may have been factory reset or lost its configuration into the Network Again. It will reuse its old NodeID. 
    This command may not always be successful, in which cases, you may have to include it as per addNode, and remove the old "ghost" node via the removeFailedNode command


requestAllConfigParam
---------------------
Params:
    "node" - uint8: The NodeID of the device you wish to get all Config Paramaters from
Returns:
    "requestAllConfigParam" - if OZW has accepted the command
Notification:
    None

Notes:
    OZW will get the latest configuration settings from a device. You should issue this after initially including a device or if the Device Settings were modifed while OZW was offline.

requestConfigParam
------------------
Params:
    "node" - uint8: the NodeID of the device yu wish to get a Config Param
    "param" - uint8: THe index of the Config Param you wish to get.
Returns:
    "requestConfigParam" - if OZW accepted the command
Notification:
    None

Notes:
    OZW will refresh the particular config param 

requestNetworkUpdate
--------------------
Params:
    "node" - uint8: The NodeID to execute this command against
Returns:
    "requestNetworkUpdate" - if OZW accpeted the command
Notification:
    None

Notes:
    Only required for Networks that have older (pre-ZWave+ roughly) controllers. Requests the SUC to update the Network Routing Tables. 

requestNodeDynamic
------------------
Params:
    "node" - uint8: The NodeID to execute this command against
Returns:
    "requestNodeDynamic" - if OZW accepted the command
Notification:
    None

Notes:
    OZW will refresh all ValueID's that are considered "Dynamic" - such as the State of a light etc

requestNodeNeighborUpdate
-------------------------
Params:
    "node" - uint8: The NodeID to execute this command against
Returns:
    "requestNodeNeighborUpdate" - if OZW accepted the command
Notification:
    "requestNodeNeighborUpdate" - Once the Node has a updated Neighbor list.

Notes:
    Syncronises the list of Neighbors a node has.
    
requestNodeState
sendNodeInformation
setValue
softResetController
testNetwork
testNetworkNode


