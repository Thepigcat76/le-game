-- properties
Name = "ballz"
Props = {
    std = "c23",
    version = "0.1",
    type = "bin",
    compiler = "clang",
}

-- external dependenciess
Dependencies = {
    --{ "https://github.com/Surtur-Team/surtils", 0.1 }
}

Libraries = {
    "raylib",
    "GL",
    "m",
    "pthread",
    "dl",
    "rt",
    "X11",
    "cjson"
}

WinLibraries = {
    "winraylib",
    "opengl32",
    "gdi32",
    "winmm",
    "wincjson"
}
