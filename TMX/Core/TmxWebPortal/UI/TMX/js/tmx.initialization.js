var connections = 	[
				{devices:["TEST"], ip:"127.0.0.1", port:"24603", led:"Logo"}
			];

var subscriptions = [
				{type:APPLICATIONMESSAGE_TYPE,subtype:REMOTEVEHICLEMESSAGE_SUBTYPE},
				{type:REMOTEVEHICLEMESSAGE_TYPE,subtype:BSMMESSAGE_SUBTYPE},
				{type:"Vehicle",subtype:"Remote"},
				{type:"Decoded",subtype:"LocationMessage"},
				{type:APPLICATIONMESSAGE_TYPE,subtype:"*"}

			];

var uiString = "Information";