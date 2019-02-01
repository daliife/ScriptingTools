filein "mvd_json_exporter.ms"
filein "mvd_components.ms"
filein "mvd_utils.ms"

-- Determine geometry layer names and other definitions in this file header
global layer_colliders = "colliders"

struct TMeshSubGroup (
	first_idx,
	num_indices,
	material_index,
	user_material_id
)

-- Class used to export the meshes as .obj or binary files
-- https://learnopengl.com/Advanced-Lighting/Normal-Mapping
struct MeshManager (
	
	fs = MVDJson(),           
	vtxs = #(),
	idxs = #(),
	subgroups = #(),
	type = "PosNUvUvT",
	
	-- Method to export mesh in .obj format
	fn exportItem obj epath = (
		
		local prev_pos = obj.pos
		local prev_rot = obj.rotation 
		
		obj.pos = [0,0,0]
		obj.rotation = eulerangles 0 0 0
		
		select obj
		local full_path = (mesh_path + obj.name + ".obj")
		exportFile epath #noprompt selectedOnly:true #noprompt
		
		obj.rotation = prev_rot
		obj.pos = prev_pos
		
		return full_path
	),
	
	-- Retrieve mesh info and save in memory
	fn exportBinaryFetch obj = (
	
		-- Submaterial and vertices duplicate data structures
		local subgroups = #()
		local ht = dotNetObject "System.Collections.Hashtable"
		
		-- Multidimensional array, for each material a list of faces that use it
		local faces_by_material= getMaterialsUsedByMesh obj
		
		-- Some transforms needed to change coordinate systems.
		local MaxToMVD = rotateXMatrix  -90
		local MaxToLocal = inverse obj.transform
		local MaxToLocalMVD = MaxToLocal * MaxToMVD
		
		-- Primary and secondary map support
		-- Secondary map should be in channel number 3 (our standard)
		local mapA = 1
		local mapB = mapA
		local mapB_status = meshop.getMapSupport obj 3
		if mapB_status == true then ( mapB = 3 )
			
		-- For each material used by the mesh
		for mat_idx = 1 to faces_by_material.count do (
			
			local faces = faces_by_material[mat_idx]
			if faces == undefined then continue

			-- We are starting a new group. 
			local new_subgroup = TMeshSubGroup()
			new_subgroup.first_idx = idxs.count
			
			-- Each face uses 3 indices..
			new_subgroup.num_indices = faces.count * 3
			new_subgroup.material_index = subgroups.count
			new_subgroup.user_material_id = mat_idx
			
			-- For each face using that material
			for face_id in faces do (
			
				-- Retrieve the 3 indexes in the list of vertex positions
				local face = getFace obj face_id
				local faceA = meshop.getMapFace obj mapA face_id
				local faceB = meshop.getMapFace obj mapB face_id
				local normals = meshop.getFaceRNormals obj face_id
				
				-- For each vertex of each face
				for vtx_id in #(1,3,2) do (
					
					-- Get vertices in MVD system
					local posW = getVert obj face[ vtx_id ]
					local pos = posW * MaxToLocalMVD
					
					-- Get normals in MVD system
					local NW = normals[ vtx_id ]
					local N = NW * MaxToLocalMVD.rotationpart
					
					-- Get UV's
					local uvA = meshop.getMapVert obj mapA faceA[vtx_id]
					local uvB = meshop.getMapVert obj mapB faceB[vtx_id]
					
					local new_vtx = #( pos.x, pos.y, pos.z, N.x, N.y, N.z, uvA.x, uvA.y, uvB.x, 1-uvB.y ) 

					-- Check if the vtx is unique, get the final index then
					local new_idx = findIndexOrRegister ht vtxs new_vtx
					
					-- Register the index
					append idxs new_idx
				)
			)
			
			append subgroups new_subgroup
		)

		-- Set vertices and ids in tangent space
		computeTangentSpace vtxs idxs
	),
	
	--https://en.wikipedia.org/wiki/List_of_file_signatures
	fn exportBinaryFile obj epath = (

		-- Chunk = MagicID(4bytes) + Size(4bytes)
		local magicHeader = 0x44444444	-- Chunk HeaderData
		local magicVtxs = 0x55554433		-- Chunk Vtxs
		local magicIdxs = 0x55556677		-- Chunk Idxs
		local magicSubGroups = 0x55556688
		local magicEoF = 0x55558888		-- .. Chunk CollisionData
		local triangle_list = 4
		
		-- Create file
		local fs = fopen epath "wb"
		local bytes_per_vtx = vtxs[1].count * 4
		local bytes_per_idx = 4 -- up to 65536
		
		-- Total Bytes, NumIdxs, 
		local headerSize = 4*(4+4)+32
		writeChunk fs magicHeader headerSize	-- 8 bytes
		
		WriteLong fs vtxs.count			-- NumVertexs
		WriteLong fs idxs.count				-- NumIndices
		WriteLong fs triangle_list  			-- PrimitiveType
		WriteLong fs bytes_per_vtx  		-- bytes_per_vtx
		
		WriteLong fs bytes_per_idx  		-- bytes_per_idx
		WriteLong fs subgroups.count	-- Num SubGroups
		WriteLong fs 0              			-- dummy2, AABB?
		WriteLong fs 0              			-- dummy3

		-- Write the vertex type as 32 bytes string with zero paddings
		WriteString fs type
		for i = type.count+1 to 31 do WriteByte fs 0
		
		-- Save vtxs
		local bytes_vtxs = vtxs.count * bytes_per_vtx
		writeChunk fs magicVtxs bytes_vtxs
		for v in vtxs do (	-- Each vtx...
			for vd in v do (	-- Each component in the vtxs
				writeFloat fs vd
			)
		)
		
		-- Save idxs, header + data
		local bytes_idxs = idxs.count * bytes_per_idx
		writeChunk fs magicIdxs bytes_idxs
		for i = idxs.count to 1 by -1 do (
			writeLong fs (idxs[i] - 1 ) 
		)
		
		-- Save subgroups, header + data
		local bytes_per_subgroup = 4 * 4
		local bytes_subgroups = subgroups.count * bytes_per_subgroup
		writeChunk fs magicSubGroups bytes_subgroups
		for sg in subgroups do (	-- Each subgroup
			writeLong fs sg.first_idx #unsigned
			writeLong fs sg.num_indices #unsigned
			writeLong fs sg.material_index #unsigned
			writeLong fs sg.user_material_id #unsigned
		)
		
		-- end of file and close
		writeChunk fs magicEoF 0		
		fclose fs
	),
	
	-- Method to export a mesh in binary format
	fn exportItemBinary obj epath = (
		
		-- Convert the given object to triangular mesh
		-- Make a deep copy of it to prevent modifiyng the original one
		local given_obj = obj
		if classof obj != TriMesh then (
			given_obj = copy obj
			given_obj.name = obj.name
			convertToMesh given_obj
		)
		
		-- First read vertices, ids and other data from the mesh
		exportBinaryFetch given_obj
		
		-- Export the given data into a binary stream
		exportBinaryFile given_obj epath
		
		-- Remove copied item if necessary
		if given_obj != obj then delete given_obj

		return epath
	)
)

-- Class used to export materials and textures
struct MaterialManager (
	
	fs = MVDJson(),    
	fs_tex = MVDJson(), 	
	tech = "pbr.tech",
	shadows = true,
	gen_mat = true,
	gen_tex = true,
	
	-- Export the texture of the given map
	fn exportMap emap ename = (
		
		-- Build the json and export the texture
		local f_name = filenameFromPath emap.filename
		local full_path = (project_path + tex_path + f_name)
		local short_path = (tex_path + f_name)
		fs_tex.add ename short_path
		
		-- Check if full_path exists, if not, copy texture there
		if not doesFileExist full_path and gen_tex == true then (
			format "Copying file from % to %\n" emap.filename full_path
			copyFile emap.filename full_path
		)
	),
	
	-- Export the material given, parameters and texture definition
	fn exportItem emat = (
		
		fs.add "tech" tech
		fs.add "shadows" (shadows as integer)
		local short_path = (mats_path + emat.name + ".mtl")
		local full_path = (project_path + short_path)
		
		-- Atleast default diffuse needed
		if emat.diffusemap != undefined then (
			exportMap emat.diffusemap "diffuse"
		) else (
			fs_tex.add "diffuse" (tex_path + "default.dds")
		)
		
		-- Atleast default normal needed
		if emat.bumpMap != undefined then (
			exportMap emat.bumpMap "normal"
		) else (
			fs_tex.add "normal" (tex_path + "null_normal.dds")
		)
		
		if emat.ambientMap != undefined then (
			exportMap emat.ambientMap "lightmap"
		)		
		
		if emat.specularMap != undefined then (
			exportMap emat.specularMap "metallic"
		)	
		
		if emat.glossinessMap != undefined then (
			exportMap emat.glossinessMap "roughness"
		)	
		
		if emat.displacementMap != undefined then (
			exportMap emat.displacementMap "height"
		)
		
		if emat.selfIllumMap != undefined then (
			exportMap emat.selfIllumMap "emissive"
		)
		
		if emat.refractionMap != undefined then (
			exportMap emat.refractionMap "aocclusion"
		)
		
		fs.add "textures" fs_tex
		if gen_mat == true then (fs.writeDisk full_path)
		
		return short_path
	)
)

-- Collider manager
-- Used to generate collider meshes and types
struct ColliderManager (
	
	col_layer,
	
	-- Method to export the collider as a mesh
	fn exportItem obj epath = (
		
		mesh_exporter = MeshManager()
		mesh_exporter.exportItemBinary obj epath
	),
	
	-- Generate basic collider
	fn generateBasic obj = (
		
		addComponent obj "collider"
	),
	
	-- Generate complex mesh collider
	fn generateTriangle obj = (

		col_layer = getLayerByName layer_colliders
		
		cloned_mesh = copy obj
		cloned_mesh.rotation = obj.rotation
		cloned_mesh.pos = obj.pos
		cloned_mesh.name = "collider_" + obj.name
		cloned_mesh.parent = obj
		
		col_layer.addnode cloned_mesh 		
		addComponent obj "collider"
	),
	
	-- Generate convex mesh collideer
	fn generateConvex obj = (
		
		col_layer = getLayerByName layer_colliders
		
		local cvx_hull = nvpx.CreateConvexHull obj.mesh 100 0
		local cloned_mesh = box name:"cloned_collider"
		local conv_obj = convertToMesh cloned_mesh
		cloned_mesh.mesh = cvx_hull
		cloned_mesh.rotation = obj.rotation
		cloned_mesh.pos = obj.pos
		cloned_mesh.name = "collider_" + obj.name
		cloned_mesh.parent = obj
		
		col_layer.addnode cloned_mesh
		addComponent obj "collider"
	),
	
		
	-- Generate the colliding mesh depending on the type
	fn generateType obj type = (
		
		case type of
		(
			1: generateBasic obj
			2: generateConvex obj
			3: generateTriangle obj
			default: generateBasic obj
		)
	)
)