-- Minetest 0.4 mod: localisation
-- See README.txt for licensing and other information.
-- LOCALISATION
if minetest.setting_get("language") == nil then
	minetest.setting_set("language", "EN")
end

localisation = {}
localisation.translations_per_mod = {} -- will received and store the translations

function localisation.register_translations(mod_name,translations)
    local function merge_translation(translations_old,translations_new)
        for field_name,field_value in pairs(translations_new) do
            translations_old[field_name] = field_value
        end
        return translations_old
    end
    if localisation.translations_per_mod[mod_name] ~= nil then
        local result_trans = localisation.translations_per_mod[mod_name]
        for lang,trans in pairs(translations) do
            if result_trans[lang] == nil then
                result_trans[lang] = {}
            end
            result_trans[lang] = merge_translation(result_trans[lang],trans)
        end
        localisation.translations_per_mod[mod_name] = result_trans
    else
        localisation.translations_per_mod[mod_name] = translations
    end
end

-- localisation , need the function previously defined
dofile(minetest.get_modpath("localisation").."/localisation.lua")

-- msg_label should be prefixed by mod_name like this "mod_name:msg_label_value"
function localisation.translate(msg_label,...)
    local fallback = "EN"
    local function translation(label_list,language,msg_label,fallback,...)
        local result = ""
        if label_list[language] == nil then
            -- Fallback on english
            language = fallback
            result = translation(label_list,language,msg_label,fallback,...)
        else
            if label_list[language][msg_label] == nil then
                if language ~= fallback then
                    language = fallback
                    result = translation(label_list,language,msg_label,fallback,...)
                else
                    return nil
                end
            else
                local msg = label_list[language][msg_label]                
                local args = {...}
                for key,arg in ipairs(args) do
                    msg = string.gsub(msg, "%$"..key.."", arg)
                end
                result = msg
            end
        end
        return result
    end
    if msg_label == nil then
        return localisation.translate("localisation:nil_value_for_msg_label")
    end
    local mod_name = string.match(msg_label,"([%w_]+):")
    msg_label = string.match(msg_label,":([%w_]+)")
    if mod_name == nil then
        return localisation.translate("localisation:no_mod_specified_for_label",msg_label)
    end
    local label_list = localisation.translations_per_mod[mod_name]
    if label_list == nil then
        return localisation.translate("localisation:no_translation_registered_for_mod",mod_name)
    end
    local language = minetest.setting_get("language") -- has previously been defined as EN if not set
    local result = translation(label_list,language,msg_label,fallback,...)
    if type(result) == "string" then
        return result
    else
        return localisation.translate("localisation:no_translation_available",language,msg_label,mod_name,fallback)
    end
end