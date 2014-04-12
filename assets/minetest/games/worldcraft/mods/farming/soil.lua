minetest.register_node("farming:soil", {
	tiles = {"farming_soil.png", "default_dirt.png", "default_dirt.png", "default_dirt.png", "default_dirt.png", "default_dirt.png"},
	drop = "default:dirt",
	groups = {crumbly=3, not_in_creative_inventory=1},
})

minetest.register_node("farming:soil_wet", {
	tiles = {"farming_soil_wet.png", "default_dirt.png", "default_dirt.png", "default_dirt.png", "default_dirt.png", "default_dirt.png"},
	drop = "default:dirt",
	groups = {crumbly=3, not_in_creative_inventory=1},
})

minetest.register_abm({
	nodenames = {"farming:soil"},
	interval = 15,
	chance = 3,
	action = function(pos, node)
		if minetest.env:find_node_near(pos, 9, {"default:water_source", "default:water_flowing"}) then
			node.name = "farming:soil_wet"
			minetest.env:set_node(pos, node)
		end
	end,
})

-- ========= EXPERIMENTAL =========
--[[ This will turn soil to dirt when walking over it
minetest.register_abm({
	nodenames = {"farming:soil", "farming:soil_wet"},
	interval = 20,
	chance = 100,
	action = function(pos, node)
		pos.y = pos.y+1
		if #(minetest.env:get_objects_inside_radius(pos, 0.8)) > 0 then
			pos.y = pos.y-1
			node.name = "default:dirt"
			minetest.env:set_node(pos, node)
		end
	end,
})]]
