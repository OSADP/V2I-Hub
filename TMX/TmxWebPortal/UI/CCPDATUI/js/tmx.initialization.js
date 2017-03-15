// Connection specifics
// 	devices: 	Must be 1 dimentional arra with "TEST" only
// 	op: 		Address of Websocket Server to connect
// 	port: 		Port on which to connect to the Web Socket
// 	led: 		id of the element that contains the status indicator. 
var connections = 	[
				{devices:["TEST"], ip:"127.0.0.1", port:"24603", led:"Logo"}
			];

// Messages to subscribe to on the tmx core
var subscriptions = [{type: "Decoded", subtype: "Location"}, {type: "Vehicle", subtype: "Basic"}, {type: APPLICATIONMESSAGE_TYPE, subtype: "Basic"}];

var optgroups = [
/*                 
                 {
                     label: 'J2735', children: [
                         {label: BSMMESSAGE_SUBTYPE, value: 'J2735,' + BSMMESSAGE_SUBTYPE},
                         {label: CSRMESSAGE_SUBTYPE, value: 'J2735,' + CSRMESSAGE_SUBTYPE},
                         {label: SSMMESSAGE_SUBTYPE, value: 'J2735,' + SSMMESSAGE_SUBTYPE},
                         {label: INTERSECTIONCOLLISIONMESSAGE_SUBTYPE, value: 'J2735,' + INTERSECTIONCOLLISIONMESSAGE_SUBTYPE},
                         {label: NMEAMESSAGE_SUBTYPE, value: 'J2735,' + NMEAMESSAGE_SUBTYPE},
                         {label: MAPDATAMESSAGE_SUBTYPE, value: 'J2735,' + MAPDATAMESSAGE_SUBTYPE},
                         {label: SPATMESSAGE_SUBTYPE, value: 'J2735,' + SPATMESSAGE_SUBTYPE},
                         {label: TIMMESSAGE_SUBTYPE, value: 'J2735,' + SPATMESSAGE_SUBTYPE},
                         {label: BSMVMESSAGE_SUBTYPE, value: 'J2735,' + BSMVMESSAGE_SUBTYPE},
                         {label: PVDMESSAGE_SUBTYPE, value: 'J2735,' + PVDMESSAGE_SUBTYPE},
                         {label: EVAMESSAGE_SUBTYPE, value: 'J2735,' + EVAMESSAGE_SUBTYPE},
                         {label: RTCMMESSAGE_SUBTYPE, value: 'J2735,' + RTCMMESSAGE_SUBTYPE},
                         {label: PDMMESSAGE_SUBTYPE, value: 'J2735,' + PDMMESSAGE_SUBTYPE},
                         {label: SRMMESSAGE_SUBTYPE, value: 'J2735,' + SRMMESSAGE_SUBTYPE},
                         {label: RSAMESSAGE_SUBTYPE, value: 'J2735,' + RSAMESSAGE_SUBTYPE},
                         {label: 'PSM', value: 'J2735,PSM'}
                     ]
                 },
*/
                 {
                     label: APPLICATIONDATAMESSAGE_TYPE, children: [
                         {label: APPLICATIONDATAMESSAGE_SUBTYPE, value: APPLICATIONDATAMESSAGE_TYPE + ',' + APPLICATIONDATAMESSAGE_SUBTYPE},
                         {label: PEDESTRIANMESSAGE_SUBTYPE, value: APPLICATIONDATAMESSAGE_TYPE + ',' + PEDESTRIANMESSAGE_SUBTYPE},
                         {label: VEHICLESTATECHANGE_SUBTYPE, value: APPLICATIONDATAMESSAGE_TYPE + ',' + VEHICLESTATECHANGE_SUBTYPE},
                         {label: APPLICATIONMESSAGE_SUBTYPE, value: APPLICATIONDATAMESSAGE_TYPE + ',' + APPLICATIONMESSAGE_SUBTYPE},
                         {label: APPLICATIONEVENTMESSAGE_SUBTYPE, value: APPLICATIONDATAMESSAGE_TYPE + ',' + APPLICATIONEVENTMESSAGE_SUBTYPE}
                     ]
                 },
                 {
                     label: "Decoded", children: [
                         {label: LOCATIONMESSAGE_SUBTYPE, value: 'Decoded,' + LOCATIONMESSAGE_SUBTYPE},
                         {label: DECODEDBSMMESSAGE_SUBTYPE, value: 'Decoded,' + DECODEDBSMMESSAGE_SUBTYPE},
                         {label: VEHICLEBASICMESSAGE_SUBTYPE, value: 'Decoded,' +  VEHICLEBASICMESSAGE_SUBTYPE},
                         {label: REMOTEVEHICLEMESSAGE_SUBTYPE, value: 'Decoded,' +  REMOTEVEHICLEMESSAGE_SUBTYPE},
                         {label: EVENTLOGMESSAGE_SUBTYPE, value: 'Decoded,' +  EVENTLOGMESSAGE_SUBTYPE},
                         {label: 'Location', value: 'Decoded,Location'},
                         {label: 'Vehicle', value: 'Vehicle,Basic'},
                         {label:'Pmm', value: 'Decoded,NextRequest'}
                     ]
                 },                            
             ];

// Name that is registered when the page registers with the tmx core
var uiString = "CCPDiagnosticsUI";

// Settings for unit. This will come from the database. Set here for default values and testing
var radioSettings = 
{
	Type:"Bus", 
	ID:"112", 
	Mode:"Operational"
};