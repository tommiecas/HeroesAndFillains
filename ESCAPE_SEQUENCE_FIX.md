# Fix Escape Sequence Error in FillainCharacter.cpp

## Problem
Line 1836 has a literal `\t` string instead of a tab character, causing "illegal escape sequence" error.

## Fix
**Find this line (around line 1836):**
```
\t// FixSelfCameraCollision(); // Removed as part of camera watchdog cleanup
```

**Replace with (use a real TAB character, not spaces):**
```
	// FixSelfCameraCollision(); // Removed as part of camera watchdog cleanup
```

## How to Fix:
1. Open FillainCharacter.cpp
2. Go to line 1836
3. Delete the `\t` characters at the start of the line
4. Press TAB once to add a real tab character
5. Save the file (Ctrl+S)
6. Rebuild

## Alternative: Just delete the entire line
Since this is just a comment, you can also just delete the entire line.
