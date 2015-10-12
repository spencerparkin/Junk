return
{
	["alpha"] = 1,
	["blue"] = 0,
	["children"] = 
	{
		[1] = 
		{
			["coneAngle"] = 0.471239,
			["explanation0"] = "",
			["explanation1"] = "",
			["maxLength"] = 20,
			["meta_data"] = 
			{
				["expanded"] = false,
				["paneInfo"] = "name=;caption=;state=2099196;dir=4;layer=0;row=0;pos=0;prop=0;bestw=-1;besth=-1;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1",
				["visible"] = true,
			},
			["name"] = "SprayDirectionCone",
			["type"] = "ConeNode",
			["vector"] = 
			{
				["x"] = 6.062e-016,
				["y"] = 6.062e-016,
				["z"] = 9.9,
			},
		},
		[2] = 
		{
			["booleanValue"] = true,
			["explanation0"] = "The particle should jitter along its trajectory.",
			["explanation1"] = "Lots of farts.",
			["meta_data"] = 
			{
				["expanded"] = false,
				["paneInfo"] = "name=;caption=;state=2099196;dir=4;layer=0;row=0;pos=0;prop=0;bestw=-1;besth=-1;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1",
				["visible"] = true,
			},
			["name"] = "JitterFly",
			["type"] = "BooleanNode",
		},
		[3] = 
		{
			["children"] = 
			{
				[1] = 
				{
					["explanation0"] = "",
					["explanation1"] = "",
					["max"] = 1,
					["meta_data"] = 
					{
						["expanded"] = true,
						["visible"] = true,
					},
					["min"] = 0,
					["minValue"] = 0.7221,
					["name"] = "MinMaxOfJoy",
					["type"] = "BoundedFloatPairNode",
				},
				[2] = 
				{
					["explanation0"] = "",
					["explanation1"] = "",
					["fileRef"] = "D:/CornucopiaEditor/TestData/TestFile6.lua",
					["meta_data"] = 
					{
						["expanded"] = true,
						["visible"] = true,
					},
					["name"] = "FileOfDoom",
					["type"] = "FileReferenceNode",
				},
				[3] = 
				{
					["explanation0"] = "",
					["explanation1"] = "",
					["meta_data"] = 
					{
						["expanded"] = true,
						["visible"] = true,
					},
					["name"] = "StringCheese",
					["stringValue"] = "",
					["type"] = "StringNode",
				},
				[4] = 
				{
					["bivector"] = 
					{
						["xy"] = -1.38336,
						["yz"] = -3.60378,
						["zx"] = -5.51289,
					},
					["explanation0"] = "",
					["explanation1"] = "",
					["maxArea"] = 10,
					["meta_data"] = 
					{
						["expanded"] = true,
						["visible"] = true,
					},
					["name"] = "AngularMomentum",
					["type"] = "BivectorE3GANode",
				},
				[5] = 
				{
					["explanation0"] = "",
					["explanation1"] = "",
					["max"] = 1,
					["meta_data"] = 
					{
						["expanded"] = false,
						["visible"] = true,
					},
					["min"] = 0,
					["name"] = "TotalAwesomeness",
					["type"] = "BoundedFloatNode",
					["value"] = 0.7246,
				},
				[6] = 
				{
					["explanation0"] = "",
					["explanation1"] = "",
					["maxLength"] = 10,
					["meta_data"] = 
					{
						["expanded"] = true,
						["visible"] = false,
					},
					["name"] = "WayToGlory",
					["type"] = "VectorE3GANode",
					["vector"] = 
					{
						["x"] = 7.71751e-017,
						["y"] = 1.09562,
						["z"] = 1.26036,
					},
				},
			},
			["explanation0"] = "",
			["explanation1"] = "",
			["max"] = 1,
			["meta_data"] = 
			{
				["expanded"] = true,
				["paneInfo"] = "name=004291e051a65d56000baec700000002;caption=Floater;state=2099196;dir=2;layer=0;row=0;pos=0;prop=100000;bestw=200;besth=40;minw=200;minh=40;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1",
				["visible"] = true,
			},
			["min"] = 0,
			["name"] = "Floater",
			["type"] = "BoundedFloatNode",
			["value"] = 0.5,
		},
	},
	["explanation0"] = "",
	["explanation1"] = "",
	["green"] = 1,
	["meta_data"] = 
	{
		["expanded"] = true,
		["paneInfo"] = "name=;caption=;state=2099196;dir=4;layer=0;row=0;pos=0;prop=0;bestw=-1;besth=-1;minw=-1;minh=-1;maxw=-1;maxh=-1;floatx=-1;floaty=-1;floatw=-1;floath=-1",
		["visible"] = true,
	},
	["name"] = "ColorRGBANode",
	["red"] = 0.501961,
	["type"] = "ColorRGBANode",
}
