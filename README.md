# Console First-Person Shooter
A First-person Shooter game in the Windows Console.

# About

This is a first-person shooter that runs in the Windows Console. The game comes complete with a the ability to walk around a 3D space,
shoot at "ghost" enemies who appear on your tactical map, and get a score. You can also draw your own map and place it into the game with
ease (you will have to modify a single line of code). 


To run simply make sure your console size is set to 120x40, as that is
the only resolution that is currently supported.

# How to Play

W, A, S, D - move forward, left, backward, right
Q, E - strafe left, strafe right
R - reload equipped weapon
F - switch weapons (there are two inventory slots)
Space - fire weapon

Simply use your tactical map to identify the ghosts (the X's you see). If you reach 50 kills you will acquire the Super Pistol which kills the ghosts in a single shot. 

# Draw your own map

If you want to draw your own map you can! The game map is only a string and the code will parse through a text file to draw it for you.
Just change the line "wifstream file("Map.txt");" to read from the text file where you drew your map. 
When drawing the map follow this key:

'#' - wall
'.' - floor
'W' - Weapon (currently, this will automatically create a 9mm pistol)
'H' - Healthpack

![Gameplay Demo](General_Demo.gif)

