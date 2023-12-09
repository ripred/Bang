tell application "Music"
    play track "Porcelain"
    repeat until player state is stopped
        delay 1
    end repeat
    stop
end tell
