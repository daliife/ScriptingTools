filein "mvd_json_exporter.ms"
filein "mvd_geometry_manager.ms"
filein "mvd_utils.ms"

json = dotNet.loadAssembly  "Newtonsoft.Json.dll"

-- Global path definitions for each asset data folder
global base_path = "data/assets/"
global scenes_path = base_path + "scenes/"
global prefabs_path = base_path + "prefabs/"
global mesh_path = base_path + "meshes/"
global mats_path = base_path + "materials/"
global cols_path = base_path + "colliders/"
global tex_path = base_path + "textures/"
global curve_path = base_path + "curves/"   
global parent_path = ""
global project_path = ""

-- Global transformation matrices needed
global MaxToMVD = rotateXMatrix  -90
global MVDToMax = rotateXMatrix  90

-- Exporter class used to export scenes into the engine
struct MVDLoader (
	
	-- Update transform
	fn updateTransform obj = (
		
		local obj_name = (obj.item["name"]).value
		local trans_comp = obj.item["transform"]
		local pos = trans_comp.item["translation"]
		local f_pos [(pos.item(0)).value, (pos.item(0)).value, (pos.item(0)).value]
		local object_find = $(obj_name) -- Get the current object that is being updated
		
		-- Update the object transformations
		if object_find != undefined then (
			
			object_find.position = (f_pos * MVDToMax)
			-- Do the same for the rotation
			-- Do the same for the scaling
		)
	),
	
	fn updateCollider obj = (
		
		-- Check if component exists
		-- Create it otherwise and add it to the object!
	),
	
	-- Check between different type of object generation
	-- https://forums.cgsociety.org/t/json-and-maxscript/1552038/11
	fn loadScene l_path l_name= (

		local f_path = (l_path + scenes_path + l_name+ ".scene")
		print ("Loading file: " + f_path)
		
		local jsonString = ""
		local file = openFile f_path

		while not eof file do 
		(
			r=  readLine file --read the first line and store as a string it in r
			if r != undefined do (
				jsonString += r
			)
		)
		
		-- We parse the json file
		myJObject=dotNetObject "Newtonsoft.Json.Linq.JObject"
		myJObject=myJObject.parse jsonString
		local entities = myJObject.item["entities"]
		
		-- Loop through all the entities and update their components
		for i = 0 to (entities.count-1) do (
			local entity = entities.item(i)
			updateTransform entity
			updateCollider entity
			-- Add the rest of the components here!
		)
		
		seek file 0    -- reset position to beginning
		close file -- closes the file
	)
)
