-- Signature.lua

e3ga_table =
{
	e1 = { e1 = 1.0, e2 = 0.0, e3 = 0.0 },
	e2 = { e1 = 0.0, e2 = 1.0, e3 = 0.0 },
	e3 = { e1 = 0.0, e2 = 0.0, e3 = 1.0 },
}

c3ga_table =
{
	no = { no = 0.0, e1 = 0.0, e2 = 0.0, e3 = 0.0, ni = -1.0 },
	e1 = { no = 0.0, e1 = 1.0, e2 = 0.0, e3 = 0.0, ni = 0.0 },
	e2 = { no = 0.0, e1 = 0.0, e2 = 1.0, e3 = 0.0, ni = 0.0 },
	e3 = { no = 0.0, e1 = 0.0, e2 = 0.0, e3 = 1.0, ni = 0.0 },
	ni = { no = -1.0, e1 = 0.0, e2 = 0.0, e3 = 0.0, ni = 0.0 },
}

euclidean_vector_map = {}

signature_table = nil

function signature_func( vecA, vecB )
	
	if euclidean_vector_map[ vecA ] and ( vecB == 'no' or vecB == 'ni' ) then
		return 0.0
	end

	if euclidean_vector_map[ vecB ] and ( vecA == 'no' or vecA == 'ni' ) then
		return 0.0
	end
	
	if not signature_table then
		return nil
	end

	return signature_table[ vecA ][ vecB ]

end

-- Signature.lua