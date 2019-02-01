struct SelectionManager (
	
	layers = #(),
	selhierarchy = #(), -- list of objects in the hierarchy panel
	categories = #("all", "mesh", "collider", "decal"), --manually adding them by now
	strsearch = "",
	
	-- Function to select everything on the list
	fn triggerSelection = (
		
		local final_selection = #()
		
		for i = 1 to selhierarchy.count do (
			nodetostack = getnodebyname selhierarchy[i]
			append final_selection nodetostack
		)
		
		select final_selection
	),
	
	-- Update the hierarchy panel with the total amount of nodes that should be taken into count
	fn updateHierarchy substr layer category = (
		
		local candidates = #()
		local layer_nodes = for i in $* where i.layer.name == layer collect i
		for x = 1 to layer_nodes.count do (
			local has_substr = findstring layer_nodes[x].Name substr
			if  has_substr != undefined or substr == "" then ( --refactor
				append candidates layer_nodes[x].Name
			)			
		)
				
		local post_candidates = #()
		for i = 1 to candidates.count do (
			-- check for mesh type on given candidates
			-- TO-DO
			append post_candidates candidates[i] 
		)
		
		selhierarchy = post_candidates
		return selhierarchy
	),
	
	-- Function to retrieve the layer names..
	fn updateLayers = (
		
		free layers
				
		for i = 0 to layerManager.count-1 do(
			
			local ilayer = layerManager.getLayer i
			local layerName = ilayer.name
			append layers layerName
		)
	),
	
	-- Function to update everything we need
	fn updateManager = (

		-- First, update the layers, search them in max
		updateLayers()
		
		-- Update the categories?
	)	
)
/*
struct PrefabManager (
	
	
)*/
