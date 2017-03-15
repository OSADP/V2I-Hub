// Connection specifics
// 	devices: 	Must be 1 dimentional arra with "TEST" only
// 	op: 		Address of Websocket Server to connect
// 	port: 		Port on which to connect to the Web Socket
// 	led: 		id of the element that contains the status indicator. 
var connections = 	[
				{devices:["TEST"], ip:"127.0.0.1", port:"24603", led:"Logo"}
			];

// Messages to subscribe to on the tmx core
var subscriptions = [{type:"Decoded",subtype:"Location"},
                     {type:"Pmm",subtype:"NextRequest"},
                     {type:APPLICATIONMESSAGE_TYPE,subtype:"*"}
];

// Name that is registered when the page registers with the tmx core
var uiString = "MobileDevicesUI";

// Settings for unit. This will come from the database. Set here for default values and testing
var radioSettings = 
{
	Type:"Bus", 
	ID:"112", 
	Mode:"Operational"
};