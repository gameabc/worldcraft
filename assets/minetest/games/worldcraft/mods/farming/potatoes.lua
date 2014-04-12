minetest.register_node("farming:potato_1", {
	paramtype = "light",
	walkable = false,
	drawtype = "plantlike",
	drop = "farming:potato_item",
	tiles = {"farming_potato_1.png"},
	selection_box = {
		type = "fixed",
		fixed = {
			{-0.5, -0.5, -0.5, 0.5, -0.5+6/16, 0.5}
		},
	},
	groups = {snappy=3, flammable=2, not_in_creative_inventory=1},
	sounds = default.node_sound_leaves_defaults(),
})

minetest.register_node("farming:potato_2", {
	paramtype = "light",
	walkable = false,
	drawtype = "plantlike",
	drop = "farming:potato_item",
	tiles = {"farming_potato_2.png"},
	selection_box = {
		type = "fixed",
		fixed = {
			{-0.5, -0.5, -0.5, 0.5, -0.5+9/16, 0.5}
		},
	},
	groups = {snappy=3, flammable=2, not_in_creative_inventory=1},
	sounds = default.node_sound_leaves_defaults(),
})

minetest.register_node("farming:potato", {
	paramtype = "light",
	walkable = false,
	drawtype = "plantlike",
	tiles = {"farming_potato_3.png"},
	drop = {
		max_items = 1,
		items = {
			{ items = {'farming:potato_item 2'} },
			{ items = {'farming:potato_item 3'}, rarity = 2 },
			{ items = {'farming:potato_item 4'}, rarity = 5 }
		}
	},
	groups = {snappy=3, flammable=2, not_in_creative_inventory=1},
	sounds = default.node_sound_leaves_defaults(),
})

minetest.register_craftitem("farming:potato_item", {
	description = "Potato",
	inventory_image = "farming_potato.png",
	on_use = minetest.item_eat(1),
	on_place = function(itemstack, placer, pointed_thing)
		local above = minetest.env:get_node(pointed_thing.above)
		if above.name == "air" then
			above.name = "farming:potato_1"
			minetest.env:set_node(pointed_thing.above, above)
			itemstack:take_item(1)
			return itemstack
		end
	end,
})

minetest.register_craftitem("farming:potato_item_baked", {
	description = "Baked Potato",
	inventory_image = "farming_potato_baked.png",
	on_use = minetest.item_eat(6),
})

minetest.register_craftitem("farming:potato_item_poison", {
	description = "Poisonous Potato",
	inventory_image = "farming_potato_poison.png",
	on_use = minetest.item_eat(2),
})

minetest.register_craft({
	type = "cooking",
	output = "farming:potato_item_baked",
	recipe = "farming:potato_item",
})

farming:add_plant("farming:potato", {"farming:potato_1", "farming:potato_2"}, 50, 20)
