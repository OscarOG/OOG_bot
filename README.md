<b>BOTs for HL1 MOD Contra-Force 1.4b. Based on Jeffrey´s HPB Bots v3.0 .</b>

I´m a Half-Life fan and one of the multiple MODs i loved to play is Contra-Force.

Contra-Force was a mix between Sven Co-op and Counter-Strike, a realistic 
and dificult MOD where players cooperate to fight monsters in a simulated real gameplay.

It was not a very famous MOD. The last version was 1.4b, as far as i know
so it had some bugs to fix and features to add but looked very promising
from my point of view.

The weakest point of this MOD was the horrible maps because it had not much
support from the community.

When i feel nostalgic and want to revive old times playing this MOD, i found
(as expected) that none plays this MOD at this times, so i wanted to try adding Bots
to it, but my main problem is that i´m not a programmer.

First i tried to make bots based on RCBot 1.5 beta mm release with no luck,
always crashed the MOD when trying to add bots so i look for a more basic version
of the bots, like the HBP Bots v3.0 (non metamod).

After some copy & paste and various code modifications, it seems i got the most
approaching to a bot i can have, at least no game crash!

I want to share this to those posible interested and those that may help me make
a decent bots to this MOD, either this version or a metamod one.


Commands are:

<b>addbot</b>    -   add a bot to the game


<b>waypoint on</b>   -   show waypoints (if any)

<b>waypoint add</b>    -   add a waypoint in current user position

<b>waypoint del</b>    -   delete current nearest to player waypoint

<b>waypoint menu</b>   -   show a menu for current nearest to player waypoint for special waypoint flags


<b>pathwaypoint on</b>   -   show paths from one waypoint to other

<b>pathwaypoint add1</b> / <b>add2</b>    -   manually add a path from waypoint A (add1) to waypoint B (add2)

<b>pathwaypoint del1</b> / <b>del2</b>    -   manually delete a path from waypoint A (del1) to waypoint B (del2)


Waypointing is a very important part for a good bot handling on maps.

It´s a very good idea to bind the commands listed above to simplify the work of waypointing maps.

Ex: bind c "waypoint add"
