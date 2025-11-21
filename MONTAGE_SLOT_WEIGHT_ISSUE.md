# 🎯 MONTAGE PLAYS BUT MESH DOESN'T MOVE - SLOT WEIGHT ISSUE

## 🔍 The Problem:

- Montage plays in code ✅
- Sound/VFX play ✅
- Mesh stays in idle pose ❌

Even though DefaultSlot exists in both AnimBP and Montages, the animation isn't visible.

## 🎯 Possible Causes:

### 1. **Slot Weight is 0**
The Slot node might have a weight of 0, making it invisible even though it's connected.

### 2. **State Machine has higher priority**
The state machine might be overriding the slot output.

### 3. **Blend settings**
The montage might have a blend-in time that's too long, or the AnimBP might be blending it out immediately.

### 4. **Wrong Slot Name**
The montage might be using a different slot name than "DefaultSlot".

## 🔧 Debug Steps:

### Check Montage Slot Name:
Open `AM_GnarledlingAttack1` and verify:
- Slot name is exactly "DefaultSlot" (case-sensitive)
- Blend In Time is reasonable (0.1-0.25 seconds)
- Blend Out Time is reasonable (0.1-0.25 seconds)

### Check AnimBP Slot:
Open the Gnarledling AnimBP and verify:
- Slot node is named exactly "DefaultSlot"
- Slot is connected AFTER the state machine
- Slot weight/alpha is 1.0 (not 0)

### Test with Simple Montage:
Try playing the montage manually in the editor:
1. Select the Gnarledling in viewport
2. Open the montage asset
3. Click "Play" in the montage preview
4. Does it play in the preview window?

If it plays in preview but not in-game, the issue is the AnimBP configuration.
If it doesn't play in preview, the montage asset itself is broken.

## 🎯 Most Likely Issue:

Since Gnarleds worked yesterday, and they use the same system, something changed in the Gnarledling Blueprint specifically. Check if:
- The Gnarledling Blueprint is using a different AnimBP than Gnarleds
- The AnimBP was recently modified
- There's a Blueprint override that's blocking montages
