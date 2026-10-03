
-- include subprojects
includes("lib/CommonLibSSE-NG")
includes("@builtin/xpack")

-- dependencies
add_requires("nlohmann_json")

-- set project constants
set_project("AutomaticRenamer")
set_version("1.4.2")
set_license("GPL-3.0")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

set_defaultmode("releasedbg")

-- define targets
target("AutomaticRenamer")
    set_kind("binary")
    add_rules("commonlibsse-ng.plugin", {
        name = "AutomaticRenamer",
        author = "kjifs",
        description = ""
    })

    add_packages("nlohmann_json")
    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
    set_installdir("D:/MO2-SkyrimTest/mods/AutomaticRenamer")
    add_installfiles("SKSE/Plugins/AutomaticRenamer.json", {prefixdir="SKSE/Plugins"})

    after_build(function(target)
        import("utils.archive")
        local name = target:name()
        print("%s", name)
        os.mkdir("dist")
        os.trycp("$(projectdir)/SKSE", "dist")
        os.trycp(target:targetfile(), "dist/SKSE/Plugins")
        local zipname = format("%s.zip", name)
        local options = {}
        options.recurse = true
        options.excludes = {"build/*", "lib/*", "dist"}
        options.curdir = "dist"
        archive.archive(zipname, "SKSE", options)
    end)
