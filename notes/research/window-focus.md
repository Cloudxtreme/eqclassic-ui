# Window Focus

After editing EQUI_TargetWindow.xml to stop the window from producing errors, I realized that this actually started making the window come to focus over other windows (a good example is the inventory) when a new target is activated.

Some windows won't achieve focus as long as certain window objects are not utilized. This of course produces errors visible from within the UIErrors log. The errors will remain as they are to stop this particular window from maintaining focus incorrectly, but this was mainly posted as a means of maybe looking into helping other windows with producing the same results.