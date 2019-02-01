-- Utility functions declared here
-- Any extra functionality needed to be provided within this context.

-- Check if folder exists
fn doesFolderExist f create:false = (
	
	local val = if (doesFileExist f) and (getfileattribute f #directory) then true else false
	if not val and create then
	(
		val = makeDir f
	)
	
	return val
) 

fn ReverseArray arr =
(
	for i=arr.count to 1 by -1 collect arr[i]
)

-- Get the current directory
fn getCurrentAssetDir = (
	
	local folder = (pathConfig.removePathLeaf (getThisScriptFilename()) )
	return (getfilenamepath (trimRight folder "\\/"))
)

-- Get the layer from layermanager given a name
fn getLayerByName lname = (
	
	for i = 0 to layerManager.count-1 do(
		
		local ilayer = layerManager.getLayer i
		if ilayer.name == lname do ( return ilayer )
	)	
	
	-- create a new layer if it doesn't exist
	layer = LayerManager.newLayer()
	layer.setname lname
	
	return layer
)

-- returns a point3 eulerangle given a quaternion
fn QuatToEulerPoint rot = (
	
	local euler = (rot as eulerAngles)
	return [euler.x, euler.y, euler.z]
)

-- Get all the materials that belong to a mesh
fn getMaterialsUsedByMesh obj = (
	
	-- Get faces by material id
	local num_faces = getNumFaces obj
	local faces_by_mat = #()
	
	for face_id = 1 to num_faces do (
		local mat_id = getFaceMatID obj face_id
		if faces_by_mat[mat_id] == undefined then (
			faces_by_mat[mat_id] = #()
		)			
		append faces_by_mat[mat_id] face_id
	)
	
	return faces_by_mat
)

function findIndexOrRegister ht vtxs new_vtx = (

	-- Key is a string representation of all floats in new_vtx
	local key = with printAllElements off new_vtx as string

	-- Find the string in the hash map
	local idx = ht.Item key
	if idx == undefined then (
		
		-- Register it
		idx = ht.count + 1
		ht.add key idx	
		append vtxs new_vtx	
	)
	return idx
)

-- Write a chunk of bytes into stream
function writeChunk fs id nbytes = (
	
	WriteLong fs id #unsigned
	WriteLong fs nbytes #unsigned
)

----------------------------------------------------------
-- Source from: http://www.terathon.com/code/tangent.html
function computeTangentSpace vertexs idxs = (
	
	local tan1 = #()
	local tan2 = #()
	
	local vertexCount = vertexs.count
	
	-- Init the array with zeros
	for i = 1 to vertexCount do (
		tan1[ i ] = [0,0,0]
		tan2[ i ] = [0,0,0]
	)
	
	local triangleCount = idxs.count / 3
	local a = 0
	while a < triangleCount do (
		local index_base = a * 3 + 1		-- +1 because max scripts starts at 1
		
		local i1 = idxs[index_base+0]
		local i2 = idxs[index_base+1]
		local i3 = idxs[index_base+2]

		-- Our vertexs are arrays of floats... ( position.xyz, normal.xyz, uv.xy, .. )
		local vtx1 = vertexs[i1]
		local vtx2 = vertexs[i2]
		local vtx3 = vertexs[i3]

		-- Extract positions. Pos are in the position 1,2,3 of the array of floats
		local v1 = [ vtx1[1], vtx1[2], vtx1[3] ]
		local v2 = [ vtx2[1], vtx2[2], vtx2[3] ]
		local v3 = [ vtx3[1], vtx3[2], vtx3[3] ]

		-- Extract uv. UV's are in the positions 7 & 8  (posx3, Nx3, <Uv>)
		local w1 =  [ vtx1[7], vtx1[8] ]
		local w2 =  [ vtx2[7], vtx2[8] ]
		local w3 =  [ vtx3[7], vtx3[8] ]

		local x1 = v2.x - v1.x
		local x2 = v3.x - v1.x
		local y1 = v2.y - v1.y
		local y2 = v3.y - v1.y
		local z1 = v2.z - v1.z
		local z2 = v3.z - v1.z

		local s1 = w2.x - w1.x
		local s2 = w3.x - w1.x
		local t1 = w2.y - w1.y
		local t2 = w3.y - w1.y

		local r = 1.0 / (s1 * t2 - s2 * t1)
		local sdir = [(t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r]
		local tdir = [(s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,(s1 * z2 - s2 * z1) * r]

		tan1[i1] = tan1[i1] + sdir
		tan1[i2] = tan1[i2] + sdir
		tan1[i3] = tan1[i3] + sdir

		tan2[i1] = tan2[i1] + tdir
		tan2[i2] = tan2[i2] + tdir
		tan2[i3] = tan2[i3] + tdir
		
		a = a + 1
	)

	for a = 1 to vertexCount do (
		
		-- Extract normal
		local n = [ vertexs[a][4], vertexs[a][5], vertexs[a][6] ]
		local t = tan1[a];
		
		-- Gram-Schmidt orthogonalize
		local dot_nt = dot n t
		local new_tangent = t - n * dot_nt
		new_tangent = normalize new_tangent		-- xyz

		-- Calculate handedness
		local cross_nt = Cross n t
		local dot_cross_tan2 = dot cross_nt tan2[a]
		local new_w = 1.
		if dot_cross_tan2 < 0.0 then new_w = -1.0
		
		-- Append to the end of our 'fat vertex' the new four 'float' components: 
		join vertexs[a] #(new_tangent.x, new_tangent.y, new_tangent.z, new_w)
		
	)
)