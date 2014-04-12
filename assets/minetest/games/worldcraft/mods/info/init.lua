dofile(minetest.get_modpath("info").."/info.lua")

minetest.register_chatcommand("info", {
	params = "(blank) | update | version | creative | suprise",
	description = "To get info on stuff.",
	func = function(name, param)
		if param == "" then
			minetest.chat_send_player(name, INFO_BLANK)
		end
		if param == "update" then
			minetest.chat_send_player(name, INFO_UPDATE)
		end
		if param == "version" then
			minetest.chat_send_player(name, INFO_VERSION)
		end
		if param == "creative" then
			minetest.chat_send_player(name, INFO_CREATIVE)
		end
		if param == "random" then
			minetest.chat_send_player(name, INFO_RANDOM)
		end
	end
})
