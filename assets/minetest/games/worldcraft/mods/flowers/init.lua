-- Minetest 0.4 mod: default
-- See README.txt for licensing and other information.

flower_tmp={}

-- Map Generation
dofile(minetest.get_modpath("flowers").."/mapgen.lua")
dofile(minetest.get_modpath("flowers").."/func.lua")

minetest.register_node("flowers:rose", {
	description = "Rose",
	drawtype = "plantlike",
	tiles = { "flowers_rose.png" },
	inventory_image = "flowers_rose.png",
	wield_image = "flowers_rose.png",
	sunlight_propagates = true,
	paramtype = "light",
	walkable = false,
	groups = {snappy=3,flammable=2,flower=1,flora=1,attached_node=1,color_red=1},
	sounds = default.node_sound_leaves_defaults(),
	selection_box = {
		type = "fixed",
		fixed = { -0.15, -0.5, -0.15, 0.15, 0.2, 0.15 },
	},
})

minetest.register_node("flowers:dandelion_yellow", {
	description = "Yellow Dandelion",
	drawtype = "plantlike",
	tiles = { "flowers_dandelion_yellow.png" },
	inventory_image = "flowers_dandelion_yellow.png",
	wield_image = "flowers_dandelion_yellow.png",
	sunlight_propagates = true,
	paramtype = "light",
	walkable = false,
	groups = {snappy=3,flammable=2,flower=1,flora=1,attached_node=1,color_yellow=1},
	sounds = default.node_sound_leaves_defaults(),
	selection_box = {
		type = "fixed",
		fixed = { -0.15, -0.5, -0.15, 0.15, 0.2, 0.15 },
	},
})

minetest.register_abm({
	nodenames = {"group:flora"},
	neighbors = {"default:dirt_with_grass", "default:sand"},
	interval = 50,
	chance = 25,
	action = function(pos, node)
		pos.y = pos.y - 1
		local under = minetest.get_node(pos)
		pos.y = pos.y + 1
		if under.name == "default:sand" then
			minetest.set_node(pos, {name="default:dry_shrub"})
		elseif under.name ~= "default:sand" then
			return
		end
		
		local light = minetest.get_node_light(pos)
		if not light or light < 13 then
			return
		end
		
		local pos0 = {x=pos.x-4,y=pos.y-4,z=pos.z-4}
		local pos1 = {x=pos.x+4,y=pos.y+4,z=pos.z+4}
		if #minetest.find_nodes_in_area(pos0, pos1, "group:flora_block") > 0 then
			return
		end
		
		local flowers = minetest.find_nodes_in_area(pos0, pos1, "group:flora")
		if #flowers > 3 then
			return
		end
		
		local seedling = minetest.find_nodes_in_area(pos0, pos1, "default:dirt_with_grass")
		if #seedling > 0 then
			seedling = seedling[math.random(#seedling)]
			seedling.y = seedling.y + 1
			light = minetest.get_node_light(seedling)
			if not light or light < 13 then
				return
			end
			if minetest.get_node(seedling).name == "air" then
				minetest.set_node(seedling, {name=node.name})
			end
		end
	end,
})

--
-- Flower Pot
--

-- Note: The flowerpot in minecraft lets you put anything in there.

minetest.register_node("flowers:pot",{
	description = "Flower Pot",
	drawtype = "nodebox",
	node_box = { type = "fixed", fixed = {
		{-0.125000,-0.125000,-0.187500,-0.187500,-0.500000,0.187500}, --Wall 1
		{0.187500,-0.125000,-0.125000,0.125000,-0.500000,0.187500}, --Wall 2
		{-0.187500,-0.125000,-0.125000,0.187500,-0.500000,-0.187500}, --Wall 3
		{0.187500,-0.125000,0.125000,-0.187500,-0.500000,0.187500}, --Wall 4
		{-0.125000,-0.500000,-0.125000,0.125000,-0.250000,0.125000}, --Dirt 5
	}},
	--selection_box = { type = "fixed", fixed = {-7/16, -0.5, -7/16, 7/16, 12/16, 7/16} },
	tiles = {"flowers_pot_top.png", "flowers_pot_bottom.png", "flowers_pot_top.png"},
	inventory_image="flowers_pot_inventory.png",
	paramtype = "light",
	groups = {snappy=3,attached_node=1},
	sounds = default.node_sound_defaults(),
	after_place_node = function(pos, placer, itemstack)
		local meta = minetest.env:get_meta(pos)
		meta:set_string("owner",placer:get_player_name())
		meta:set_string("infotext","Flower Pot (owned by "..placer:get_player_name()..")")
	end,
	on_rightclick = function(pos, node, clicker, itemstack)
		if not itemstack then return end
		local meta = minetest.env:get_meta(pos)
		if clicker:get_player_name() == meta:get_string("owner") then
			flower_pot_drop_item(pos,node)
			local s = itemstack:take_item()
			meta:set_string("item",s:to_string())
			flower_pot_update_item(pos,node)
		end
		return itemstack
	end,
	on_punch = function(pos,node,puncher)
		local meta = minetest.env:get_meta(pos)
		if puncher:get_player_name() == meta:get_string("owner") then
			flower_pot_drop_item(pos,node)
		end
	end,
	can_dig = function(pos,player)
		local meta = minetest.env:get_meta(pos)
		return player:get_player_name() == meta:get_string("owner")
	end,
})