-- We can include files using filein 
filein "mvd_components.ms"
filein "mvd_utility_managers.ms"
filein "mvd_scene_exporter.ms"
filein "mvd_scene_load.ms"
filein "mvd_utils.ms"

-- Declare global variables needed to load and save objects
col_manager = ColliderManager() -- Collider tool manager
man_selection = SelectionManager() -- Selection utility tool manager

gc()
clearListener()
--------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------

-- Instructions rollouts
rollout SBInstructions "Instructions" width:351 height:99
(
	label 'lbl1' "- Selection tools: Select nodes by name, layer and category." pos:[17,9] width:302 height:24 align:#left
	label 'lbl3' "- Export tools: Export a mesh or the whole scene into the engine." pos:[17,28] width:321 height:30 align:#left
	label 'lbl4' "- Animation tools: Export and tweak your animations. WIP" pos:[17,46] width:321 height:30 align:#left
	label 'lbl5' "- Util tools: Utility tools used by ." pos:[17,65] width:321 height:30 align:#left
	label 'lbl6' "- Asset library: Visor to watch your assets on live." pos:[17,65] width:321 height:30 align:#left
)

-- Selection tools
rollout SBSelection "Selection Tools" width:477 height:252
(
	GroupBox 'grp1' "Selection" pos:[11,12] width:450 height:222 align:#left
	edittext 'mesh_filter' "Name:" pos:[37,47] width:184 height:21 fieldwidth:150 labelOnTop:false align:#left
	button 'btn_select' "Select" pos:[34,175] width:188 height:33 toolTip:"This is a tooltip" align:#left

	dropdownList 'sel_layers' "" pos:[73,85] width:149 height:21 align:#left 
 
	listbox 'lbx1' "Selection Hierarchy" pos:[274,31] width:161 height:12 align:#left
	label 'lbl1' "Layers:" pos:[35,87] width:39 height:14 align:#left
	button 'btn13' "Button" pos:[248,36] width:2 height:181 align:#left
	dropdownList 'sel_cat' "" pos:[73,118] width:149 height:21 align:#left
	label 'lbl4' "Tag:" pos:[35,121] width:39 height:14 align:#left
	
	local var1 = ""
	local var2 = sel_layers.selected 
	local var3 = sel_cat.selected
	
	-- On window open
	-- Get current strings and set them
	on SBSelection open do (
		
		man_selection.updateManager() 
		sel_layers.items = man_selection.layers
		sel_cat.items = man_selection.categories
		var2 = sel_layers.selected 
		var3 = sel_cat.selected
		lbx1.items = man_selection.updateHierarchy var1 var2 var3 
	)
	
	on sel_layers selected var2 do (
		
		-- Update the manager, with new layers
		-- Update the hierarchy with the current selected layer and prefix
		man_selection.updateManager()
		var2 = sel_layers.selected 
		var3 = sel_cat.selected
		lbx1.items = man_selection.updateHierarchy var1 var2 var3 
	)
	
	on sel_cat selected  var3 do ( 
		
		-- Update the manager with new layers
		-- Update the hierarchy with the current selected category
		man_selection.updateManager() 
		var2 = sel_layers.selected 
		var3 = sel_cat.selected
		lbx1.items = man_selection.updateHierarchy var1 var2 var3 
	)
	
	on mesh_filter changed var1 do (
		
		man_selection.updateManager() 
		var2 = sel_layers.selected 
		var3 = sel_cat.selected
		lbx1.items = man_selection.updateHierarchy var1 var2 var3 
	)
	
	-- Here we catch button actions
	on btn_select pressed do (
		
		-- Update the results with new selected nodes.
		man_selection.triggerSelection();
	)
)

-- Export Tools
--------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------

rollout SBExport "Export Tools" width:474 height:266
(
	edittext 'asset_dir' "Asset dir:" pos:[18,17] width:389 height:20 fieldwidth:340 labelOnTop:false align:#left
	button 'btn_test' "..." pos:[420,17] width:25 height:21 toolTip:"This is a tooltip" align:#left
	
	GroupBox 'grp2' "Export Scene" pos:[10,65] width:450 height:153 align:#left
	progressBar 'pb6' "ProgressBar" pos:[11,229] width:449 height:21 color:(color 0 255 0) align:#left
	button 'btn36' "Button" pos:[158,89] width:2 height:116 align:#left
	button 'btn39' "Export Selection" pos:[191,176] width:123 height:22 align:#left
	button 'btn40' "E. All" pos:[318,176] width:51 height:22 align:#left
	button 'btn41' "Load" pos:[373,176] width:51 height:22 align:#left
	edittext 'scene_name' "Name" pos:[191,95] width:234 height:19 align:#left
	checkbox 'chk5' "Generate Colliders" pos:[28,175] width:108 height:14 align:#left
	checkbox 'chk7' "Generate Textures" pos:[28,155] width:107 height:14 checked:true align:#left
	checkbox 'chk8' "Generate Materials" pos:[28,136] width:111 height:14 checked:true align:#left
	checkbox 'chk9' "Rewrite existing files" pos:[307,126] width:120 height:14 checked:true align:#left
	dropdownList 'ddl1' "Type" pos:[28,86] width:110 height:40 items:#("Scene", "Prefab", "Mesh", "Curve") align:#left
	
	-- Inital names and path setters
	on SBExport open do
	(
		scene_name.text = getFilenameFile maxFileName
		asset_dir.text = getCurrentAssetDir() 
	)
	
	-- Button to load the folder path
	on btn_test pressed do
	(			
		dir = getSavePath initialDir:asset_dir.text caption:"Select Folder"
		if  dir != undefined do 
			asset_dir.text = dir
	)
	
	-- Export selected items
	on btn39 pressed do
	(
		pb6.value = 0
		local objs_selected = selection as array
		local module_exporter = MVDExporter item_name:scene_name.text gen_mat:chk8.state gen_tex:chk7.state  gen_col:chk5.state 
		module_exporter.exportItem objs_selected ddl1.selection asset_dir.text pb6 		 
	)
	
	-- Export all items from the scene
	on btn40 pressed do
	(
		pb6.value = 0
		local module_exporter = MVDExporter item_name:scene_name.text gen_mat:chk8.state gen_tex:chk7.state  gen_col:chk5.state 
		module_exporter.exportItem $* ddl1.selection asset_dir.text pb6 		
	)
	
	on btn41 pressed do
	(
		pb6.value = 0
		local module_loader = MVDLoader()
		module_loader.loadScene asset_dir.text scene_name.text		
	)
)

--------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------

-- Utils Tools
rollout SBAnimations "Animation Tools" width:469 height:263
(
	
)

--------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------

-- Utils Tools
rollout SBUtils "Utils Tools" width:469 height:263
(
	
	dropdownList 'ddl3' "" pos:[91,39] width:93 height:21 items:#("basic", "convex", "mesh") align:#left
	label 'lbl3' "Type" pos:[63,42] width:29 height:17 align:#left
	GroupBox 'grp11' "Collider Generator" pos:[9,9] width:453 height:108 align:#left
	button 'btn30' "Generate Selection" pos:[249,40] width:155 height:20 align:#left
	button 'btn31' "Generate All" pos:[249,73] width:155 height:20 align:#left
	
	GroupBox 'grp5' "Component Generator" pos:[8,130] width:453 height:107 align:#left
	dropdownList 'ddl2' "" pos:[93,162] width:93 height:21 items:#("tag", "collider", "prefab", "technique", "audio", "particles", "rotator", "moving", "neon") align:#left
	label 'lbl4' "Type" pos:[63,164] width:29 height:17 align:#left
	button 'btn3' "Generate Selection" pos:[247,163] width:156 height:20 align:#left	
	button 'btn7' "Remove Selection" pos:[247,197] width:156 height:20 align:#left
	
	-- Generate collider on selection
	on btn30 pressed do
	(
		for obj in selection do (
			col_manager.generateType obj ddl3.selection
		)
	)
	
	-- Generate collider on all objects
	on btn31 pressed do
	(
		for obj in rootScene.world.children do (
			col_manager.generateType obj ddl3.selected
		)
	)
	
	-- Add component
	on btn3 pressed do
	(
		addComponent selection ddl2.selected
	)
	
	-- Remove component
	on btn7 pressed do
	(
		removeComponent selection ddl2.selected
	)
)

-- Build a rollout that can be floated
function createRollouts = 
(
	-- Create a rollout floater and load it.
	try(cui.UnRegisterDialogBar FirstDialog)catch()
	try(destroyDialog FirstDialog)catch()
	FirstDialog = newRolloutFloater "MVD Export Tools" 485 600
	
	addrollout SBInstructions FirstDialog rolledUp:on
	addrollout SBSelection FirstDialog rolledUp:on
	addrollout SBExport FirstDialog rolledUp:on
	addrollout SBAnimations FirstDialog rolledUp:on
	addrollout SBUtils FirstDialog rolledUp:on
)

createRollouts()