-- FACTIONS
factions = {}
factions.intervale_between_save_in_seconds = 300 -- 5minutes
factions.faction_file_path = minetest.get_modpath("factions").."/factions.txt";

-- localisation
dofile(minetest.get_modpath("factions").."/localisation.lua")

-- datas
factions.factions_list = {}
factions.player_to_faction = {}

-- Prototype definition

function factions.factory_faction(name_val,fondateur_name_val,date_creation_val,description_val)
	return {
		name = name_val,
		fondateur = fondateur_name_val,
		date_creation = date_creation_val,
		description = description_val,
        free_to_join = false,
		players = {},
	}
end

-- DATA STORAGE

function factions.save_datas()
	local file,err = io.open(factions.faction_file_path,"w+")
	local separateur = "|"
	for k,faction in pairs(factions.factions_list) do
		file:write(faction["name"],separateur,faction["fondateur"],separateur,faction["date_creation"],separateur,faction["description"],separateur,tostring(faction["free_to_join"]),separateur);
		for k,player_name in ipairs(faction["players"]) do
			file:write(player_name,separateur)
		end
	end
    io.close(file)
end

function factions.read_datas()
    local nb_param_before_player_list = 6
	local file,err = io.open(factions.faction_file_path,"r")
	if file ~= nil then -- File exists
		for line in file:lines() do
			local players = {}
			local faction_info = {}
			local k = 1
			for info in string.gmatch(line, "(.-)|") do
				if k < nb_param_before_player_list then
					table.insert(faction_info,info)
				else
					table.insert(players,info)
				end
				k = k+1
			end
			factions.factions_list[faction_info[1]] = factions.factory_faction(faction_info[1],faction_info[2],faction_info[3],faction_info[4])
			factions.factions_list[faction_info[1]]["players"] = players
			for k,p in pairs(players) do
				factions.player_to_faction[p] = faction_info[1]
			end
		end
	else -- File do not exists
		io.close(io.open(factions.faction_file_path,"w+"))
		factions.read_datas()
	end
end

function factions.auto_save()
    minetest.log("action","Factions mod : sauvegarde des informations de factions dans "..factions.faction_file_path)
	factions.save_datas()
	minetest.after(factions.intervale_between_save_in_seconds, factions.auto_save)
end

factions.read_datas()
factions.auto_save()

-- Access to data

function factions.get_faction(faction_name)
	return factions.factions_list[faction_name]
end

function factions.is_faction_exist(faction_name)
	return (factions.get_faction(faction_name) ~= nil)
end

function factions.is_free_to_join(faction_name)
    if factions.is_faction_exist(faction_name) then
        return factions.factions_list[faction_name]["free_to_join"]
    else
        return false
    end
end

function factions.get_fondateur_of_faction(faction_name)
    if factions.is_faction_exist(faction_name) then
        return factions.factions_list[faction_name]["fondateur"]
    else
        return nil
    end
end

function factions.get_description_of_faction(faction_name)
    if factions.is_faction_exist(faction_name) then
        return factions.factions_list[faction_name]["description"]
    else
        return nil
    end
end

function factions.get_faction_name_of_player(player_name)
	return factions.player_to_faction[player_name]
end

function factions.is_player_in_faction(player_name)
	return (factions.get_faction_name_of_player(player_name) ~= nil)
end

-- PRIVS

-- FUNCTIONS

function factions.chat_send_faction_message(faction_name,msg)
	if not factions.is_faction_exist(faction_name) then -- Inexistant faction
		minetest.chat_send_player(player_name,localisation.translate("factions:inexistant_faction",faction_name))
		return false
	else
		for key,player_name in ipairs(factions.factions_list[faction_name]["players"]) do
			minetest.chat_send_player(player_name,"[/F] "..msg)
		end
	end
end

function factions.register_player_joining_faction(faction_name,player_name,operateur)
	if not factions.is_faction_exist(faction_name) then
		minetest.chat_send_player(operateur,localisation.translate("factions:inexistant_faction",faction_name))
		return false
	end
	if not factions.is_player_in_faction(player_name) then
		factions.player_to_faction[player_name] = faction_name
		table.insert(factions.factions_list[faction_name]["players"],player_name)
		factions.chat_send_faction_message(faction_name,localisation.translate("factions:player_join_faction",player_name,faction_name))
	else
		if factions.get_faction_name_of_player(player_name) == faction_name then
			if player_name == operateur then
				minetest.chat_send_player(operateur, localisation.translate("factions:self_already_in_faction"))
			else
				minetest.chat_send_player(operateur, localisation.translate("factions:already_in_faction",player_name))
			end
		else
			minetest.chat_send_player(operateur, localisation.translate("factions:already_in_another_faction",factions.player_to_faction[player_name]))
		end
	end
end

function factions.register_player_leaving_faction(faction_name,player_name,operateur)
	if not factions.is_faction_exist(faction_name) then
		minetest.chat_send_player(operateur,localisation.translate("factions:inexistant_faction",faction_name))
		return false
	end
	if factions.is_player_in_faction(player_name) and factions.get_faction_name_of_player(player_name) == faction_name then
		for k,p in pairs(factions.player_to_faction) do
			if k == player_name then
                factions.player_to_faction[player_name] = nil
                factions.chat_send_faction_message(faction_name,localisation.translate("factions:player_quit_faction",player_name,faction_name))
				minetest.chat_send_player(player_name,localisation.translate("factions:player_quit_faction",player_name,faction_name))
				break
			end
		end
		for k,p in ipairs(factions.factions_list[faction_name]["players"]) do
			if p == player_name then
				table.remove(factions.factions_list[faction_name]["players"],k)
				break
			end
		end
	else
		if not factions.is_player_in_faction(player_name) then
            if player_name == operateur then
                minetest.chat_send_player(operateur, localisation.translate("factions:self_no_faction"))
            else
                minetest.chat_send_player(operateur, localisation.translate("factions:no_faction",player_name))
            end
		else
			minetest.chat_send_player(operateur, localisation.translate("factions:already_in_another_faction",factions.player_to_faction[player_name]))
		end
	end
end

function factions.register_faction(name,fondateur_name)
	if not factions.is_faction_exist(name) and not factions.is_player_in_faction(fondateur_name) then
		factions.factions_list[name] = factions.factory_faction(name,fondateur_name,os.date("!%c"),"Created by "..fondateur_name)
		minetest.chat_send_all(localisation.translate("factions:create_faction",name,fondateur_name))
		factions.register_player_joining_faction(name,fondateur_name)
	else
		if factions.is_faction_exist(name) then
			minetest.chat_send_player(fondateur_name,localisation.translate("factions:existant_faction",name))
		else -- Creator already in faction
			minetest.chat_send_player(fondateur_name, localisation.translate("factions:already_in_another_faction",factions.player_to_faction[fondateur_name]))
		end
	end
end

function factions.disband_faction(name,fondateur_name)
    if factions.is_faction_exist(name) and factions.is_player_in_faction(fondateur_name) then
        minetest.chat_send_all(localisation.translate("factions:delete_faction",name,fondateur_name)) 
        --local f_index = 1
        for f_name,f in pairs(factions.factions_list) do
            if f["name"] == name then
                for i,p in ipairs(factions.factions_list[name]["players"]) do
                    factions.register_player_leaving_faction(name,p,fondateur_name)
                end
                factions.factions_list[f_name] = nil
                break
            end
            --f_index = f_index + 1
        end
    end
end

-- CHAT COMMANDS

minetest.register_chatcommand("faction", {
	params = "create <factionname>"..localisation.translate("factions:help_info_create").."\n"
    .."info <factionname> : "..localisation.translate("factions:help_info_info").."\n"
    .."list <factionname> : "..localisation.translate("factions:help_info_list").."\n"
    .."join <factionname> : "..localisation.translate("factions:help_info_join").."\n"
    .."leave: "..localisation.translate("factions:help_info_leave").."\n"
    .."Faction Admin\n"
    .."kick <playername> : "..localisation.translate("factions:help_info_kick").."\n"
    .."invite <playername> : "..localisation.translate("factions:help_info_invite").."\n"
    .."disband : "..localisation.translate("factions:help_info_disband").."\n"
    .."set_free : "..localisation.translate("factions:help_info_set_free").."\n"
    .."description <text>: "..localisation.translate("factions:help_info_description").."\n"
    ,
	description = localisation.translate("factions:help_description"),
	privs = {},
	func = function(name, param)
		local parameters = {string.match(param,"^([^ ]+)%s?(.*)")}
		if parameters == nil then
			minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Action name like create, info"))
		else
			local action = parameters[1]
			if action == "create" then
				local faction_name = parameters[2]
				if faction_name ~= nil then
					factions.register_faction(faction_name,name,name)
				else
					minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Faction name"))
				end
				return true
			end
            if action == "disband" then
                local faction_name = factions.get_faction_name_of_player(name)
				if faction_name ~= nil then
                    if factions.get_fondateur_of_faction(faction_name) == name then
                        factions.disband_faction(faction_name,name,name)
                    else
                        minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
                    end
				else
					minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Faction name"))
				end
				return true
			end
            if action == "list" then
                local faction_list = {}
                for k,f in pairs(factions.factions_list) do
                    table.insert(faction_list,k)
                end
                if #faction_list ~= 0 then
                    minetest.chat_send_player(name,"Factions("..#faction_list..") : "..table.concat(faction_list,","))
                else
                    minetest.chat_send_player(name,localisation.translate("factions:no_faction_registered"))
                end
                return true
            end
			if action == "info" then
				local faction_name = parameters[2]
				if faction_name ~= "" then
					if factions.is_faction_exist(faction_name) then
                        local str = ""
						for key,par in pairs(factions.factions_list[faction_name]) do
                            if key == "players" then
                                str = str..key.." = "..table.concat(par,",")
                            else
                                str = str..key.." = "..tostring(par)
                            end
                            str = str.."\n"
						end
                        minetest.chat_send_player(name,str)
					else
						minetest.chat_send_player(name,localisation.translate("factions:inexistant_faction",faction_name))
					end
				else
                    minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Faction name"))
                end
                return true
			end
			if action == "join" then
				local faction_name = parameters[2]
				if faction_name ~= "" then
                    if factions.is_free_to_join(faction_name) then
                        factions.register_player_joining_faction(faction_name,name,name)
                    else
                        minetest.chat_send_player(name,localisation.translate("factions:not_free_to_join",faction_name))
                    end
				else
					minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Faction name"))
				end
                return true
			end
			if action == "leave" then
				local faction_name = factions.get_faction_name_of_player(name)
				if faction_name ~= nil then
					factions.register_player_leaving_faction(faction_name,name,name)
				else
					minetest.chat_send_player(name, localisation.translate("factions:self_no_faction"))
				end
                return true
			end
            if action == "invite" then
				local faction_name = factions.get_faction_name_of_player(name)
				local player_to_invite = parameters[2]
				if faction_name ~= nil and faction_name == factions.get_faction_name_of_player(name) then
					if player_to_invite ~= "" and factions.get_fondateur_of_faction(faction_name) == name then
                        factions.register_player_joining_faction(faction_name,player_to_invite,name)
                        factions.chat_send_faction_message(faction_name,localisation.translate("factions:player_invited",player_to_invite,name))
					else
                        if factions.get_fondateur_of_faction(faction_name) == name then
                            minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Player name"))
                        else
                            minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
                        end
					end
				else
					minetest.chat_send_player(name, localisation.translate("factions:self_no_faction"))
				end
                return true
			end
			if action == "kick" then
				local faction_name = factions.get_faction_name_of_player(name)
				local player_to_kick = parameters[2]
				if faction_name ~= nil and faction_name == factions.get_faction_name_of_player(name) then
					if player_to_kick ~= "" and factions.get_fondateur_of_faction(faction_name) == name then
                        factions.chat_send_faction_message(faction_name,localisation.translate("factions:player_kicked",player_to_kick,name))
						factions.register_player_leaving_faction(faction_name,player_to_kick,name)
					else
                        if factions.get_fondateur_of_faction(faction_name) == name then
                            minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Player name"))
                        else
                            minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
                        end
					end
				else
					minetest.chat_send_player(name, localisation.translate("factions:self_no_faction"))
				end
                return true
			end
            if action == "description" then
                local faction_name = factions.get_faction_name_of_player(name)
                local desc = parameters[2]
                if factions.is_faction_exist(faction_name) and factions.get_fondateur_of_faction(faction_name) == name then
                    factions.factions_list[faction_name]["description"] = desc
                    minetest.chat_send_all(localisation.translate("factions:new_faction_description",desc))
                else
                    minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
                end
                return true
            end
            if action == "set_free" then
                local faction_name = factions.get_faction_name_of_player(name)
                if factions.is_faction_exist(faction_name) and factions.get_fondateur_of_faction(faction_name) == name then
                    factions.factions_list[faction_name]["free_to_join"] = not factions.factions_list[faction_name]["free_to_join"]
                    if factions.is_free_to_join(faction_name) then
                        minetest.chat_send_all(localisation.translate("factions:allow_free_join_true",faction_name))
                    else
                        minetest.chat_send_all(localisation.translate("factions:allow_free_join_false",faction_name))
                    end
                else
                    minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
                end
                return true
            end
			if action == "save" then
				if minetest.check_player_privs(name, {server=true}) then
					factions.save_datas()
				else
					minetest.chat_send_player(name, localisation.translate("factions:insufficient_privilege"))
				end
                return true
			end
            if action == nil then
                if factions.is_player_in_faction(name) then
                    local faction_name = factions.get_faction_name_of_player(name)
                    if faction_name ~= "" then
                        if factions.is_faction_exist(faction_name) then
                            local str = ""
                            for key,par in pairs(factions.factions_list[faction_name]) do
                                if key == "players" then
                                    str = str..key.." = "..table.concat(par,",")
                                else
                                    str = str..key.." = "..tostring(par)
                                end
                                str = str.."\n"
                            end
                            minetest.chat_send_player(name, localisation.translate("factions:current_faction",faction_name))
                            minetest.chat_send_player(name,str)
                        else
                            minetest.chat_send_player(name,localisation.translate("factions:inexistant_faction",faction_name))
                        end
                    else
                        minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Faction name"))
                    end
                    return true
                else
                    minetest.chat_send_player(name, localisation.translate("factions:self_no_faction"))
                    return true
                end
            end
            if action ~= "" and action ~= nil then
                minetest.chat_send_player(name,localisation.translate("factions:missing_argument","Action name"))
                return true
            end
		end
	end,
})

minetest.register_chatcommand("f", {
	params = "<msg>",
	description = "Faction channel",
	privs = {},
	func = function(name,param)
		local faction_name = factions.player_to_faction[name]
		if faction_name ~= nil then
			factions.chat_send_faction_message(faction_name,param)
		else
			minetest.chat_send_player(name,localisation.translate("factions:self_no_faction"))
		end
	end,
})