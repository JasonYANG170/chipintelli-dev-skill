

function get_ac_model(file_path)
    local file = io.open(file_path, "rb")
    if not file then
        print("无法打开文件：" .. file_path)
        return nil
    end
    for line in file:lines() do
        -- print(line)
        if string.match(line, "^<AC%-MODEL> V%d%d%d%d%d$") then
            return line
        end
    end
    return nil
end

-- local a = get_ac_model("./asr/[1]asr_chinese_CI1306_V00373.dat")
-- print(a)
-- os.exit()

for asr_file in lfs.dir("./asr") do
    local asr_file_name = string.match(asr_file, "^%[.*%].*")
    if asr_file_name ~= nil then
        local asr_flag = get_ac_model("./asr/" .. asr_file_name)
        for nn_file in lfs.dir("./dnn") do
            local nn_file_name = string.match(nn_file, "^%[.*%].*")
            if nn_file_name ~= nil then
                local nn_flag = get_ac_model("./dnn/" .. nn_file_name)
                print(asr_file_name .. " <--> " .. nn_file_name)
                if asr_flag ~= nil and nn_flag ~= nil then
                    print(asr_flag .. " <--> ".. nn_flag)
                    if asr_flag ~= nn_flag then
                        print("声学模型: " .. nn_file_name .. asr_flag .. " 与语言模型: " .. asr_file_name .. nn_flag .. " 不匹配。")
                        os.exit(1)
                    end
                end
            end
        end
    end
end

os.exit(0)

