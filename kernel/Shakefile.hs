import           Development.Shake
import           Development.Shake.Command
import           Development.Shake.FilePath
import           Development.Shake.Util

arch = "i686-naos"
gcc = arch ++ "-gcc"
flags = "-Wall -Wextra -std=c99 -mno-red-zone -mno-80387"
sysroot = "../sysroot"
includes = ["include", sysroot </> "include", sysroot </> "usr" </> "include"]

main :: IO ()
main =
    shakeArgs shakeOptions { shakeFiles = "_build" } $ do
        want ["ker.bin"]

        phony "clean" $ do
            putNormal "Cleaning build files"
            removeFilesAfter "_build" ["//*.o"]

        "ker.bin" %> \out -> do
            cs <- getDirectoryFiles "src" ["//*.c"]
            let os = ["_build" </> c -<.> "o" | c <- cs]
            let libdirs = "-L ../libc"
            need os
            cmd_ gcc flags ["-o", out] ["-T", "linker.ld"] libdirs "-nostdlib -Llibc" os "-lgcc -lk"

        "_build//*.o" %> \out -> do
            let c = ("src" </>) $ dropDirectory1 $ out -<.> "c"
            let m = out -<.> "m"
            let incs = map ("-I" ++) includes
            cmd_ gcc ["-c", c] ["-o", out] "-MMD -MF" [m] incs flags "-nostdlib -D__is_libk"
            neededMakefileDependencies m
