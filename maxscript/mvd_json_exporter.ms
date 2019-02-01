__author__ = "Ace_Zhang"
__email__ = "394452216@qq.com"
__doc__ = "Simple create json with MaxScript"
__version__ = "1.0.0"
__note__ = "Modified version under MVD"

/*
This is a JSON Exporter template by Ace Zhang
Slightly modified for MVD Course
Some improvements to come.
*/

struct MVDJson
(
	__keys__ = #(),
	__value__ = #(),
	__fclose__ = false,
	
	
	fn format_type value =(
		local f_value = stringstream ""
		
		class_type = classof value

		if superclassof value == StructDef do(
			format "%" ("{" +value.__json__()+"}") to:f_value
			return f_value as string
			)

		if class_type == String do(
			format "\"%\"" value to:f_value
			return f_value as string
		)
		
		if class_type == Integer do(
			format "%" value to:f_value
			return f_value as string
		)
		
		if class_type == BooleanClass do(
			format "%" value to:f_value
			return f_value as string
		)
		
		if class_type == Point3 do(
			format "[%," value.x to:f_value
			format "%," value.y to:f_value
			format "%]" value.z to:f_value
			return f_value as string
		)
		
		if class_type == Point4 do(
			format "[%," value.x to:f_value
			format "%," value.y to:f_value
			format "%," value.z to:f_value
			format "%]" value.w to:f_value
			return f_value as string
		)
		
		if class_type == Quat do(
			format "[%," value.x to:f_value
			format "%," value.y to:f_value
			format "%," value.z to:f_value
			format "%]" value.w to:f_value
			return f_value as string
		)
		
		if class_type == float do(
			format "%" value to:f_value
			return f_value as string
		)

		if class_type == Array do(
			format "["  to:f_value
			for a in value do(
				l_value = this.format_type a
				
				format_s = "%,"
				if a == value[value.count] do(
					format_s = "%"
					)
				format format_s l_value to:f_value
				)

			format "]" to:f_value	
		)

		return f_value as string
	),
	
	fn __json__ = (
		data = ""
		iter = 0
		
		for a in __keys__ do(

			wkeys = filterString a "."
			data += "\""+ wkeys[1] + "\":"
			item = __value__[finditem __keys__ a]

			if superclassof item == StructDef then(
				if iter >= (__keys__.count-1) then (
					data += "{"+item.__json__()+"}"
				) else (
					data += "{"+item.__json__()+"},"	
				)
				
			) else (
				
				end_s = ","
				if finditem __keys__ a >= __keys__.count do(
					end_s = ""
				)
			
				data += this.format_type(item)+end_s
			)
					
			iter = iter + 1
		)
		return data
	),
	
	fn json = (
		return "{"+ this.__json__()+"}"
	),
		
	fn items = (
		return this.__keys__
	),

	fn item key =(
		index = finditem this.__keys__ key 
		if index != 0 do(
			return this.__value__[index]
		)
	),

	fn add key value = (
		index = finditem this.__keys__ key
		if index == 0 then (
			append this.__keys__ key
			index = finditem this.__keys__ key
		) else (
			new_key = (key + "." + (__keys__.count as string))
			append this.__keys__ new_key
			index = finditem this.__keys__ new_key
		)
		this.__value__[index] = value
		
	),
	
	fn addNclose key value = (
		index = finditem this.__keys__ key
		if index == 0 do(
			append this.__keys__ key
			index = finditem this.__keys__ key
		)
		__fclose__ = true
		this.__value__[index] = value
	),
	
	-- Write to disk
	fn writeDisk filename =(
		
		fs = createFile filename
		txt = json()
		format txt to:fs
		close fs
	)
)
