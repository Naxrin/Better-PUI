# <cy>Better Platformer & Practice Config UI</c>
Geometry Dash is a cool game but our Platformer & Practice buttons config UI should be better, it's terribly hard to get their logic of settings and they actually have tons of weird designs and bugs like:
- No position inputer for buttons;
- In Platformer buttons UI, if you have save or load a dpad layout slot, the six buttons will turn not available until you close the UI menu and launch it again, also we cannot read the data from the three save slots obviously, let alone getting a brief preview;
- In Practice buttons UI, the opacity inputer and slider does not return to 1.00 on reset button clicked.
- If we transport our savefile (or save load?) between devices of different screen ratio, our carefully designed UI will definitely break or even go outside of the screen so we cannot drag it unless clicking reset.
- In Platformer buttons UI, snap and split options look like nonsense;

So this mod aims to fix all of them above ~~except the last cuz idk wtf does snap and split option mean either~~. I'm not sure it's worthy or not (for it has been 20 months since 2.2 and most of us should seldom access the two menus?)  
But anyway it's already here.  

### <cy>Geometry Dash 2.209 note:</c>
2.209 replaced the old platformer buttons UI (`UIOptionsLayer`) on Windows with the new touch controls editor, so this mod now replaces that editor instead: opening the global touch controls settings gives you this mod's UI, with exact positions, sizes, scale, opacity, deadzone and radius, grid snapping, three save slots with previews, symmetric P2 editing and copy / paste of the whole configuration. Level-specific custom controls keep the vanilla editor. The Practice UI replacement works exactly like before.

### <cy>Contact / Report Bugs</c>
- Github: [Post an Issue](https://github.com/Naxrin/Better-PUI/issues)
- Discord: [Naxrin#6957](https://discordapp.com/users/414986613962309633)
- In-Game ID: [Naxrin](user:4759632)