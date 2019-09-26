
***************************************************************************


	              ******************************

	              <<<<<<   OTHELLO v1.5   >>>>>>

  	              ******************************

		      <<<<< by François LEIBER >>>>>

		      <<< http://leiber.free.fr/ >>>


***************************************************************************


A famous strategy game for the Ti-89, with AI, using the *wonderful*
library for the BASIC Flib :)

Version 1.5, 03-13-00.
Copyright 2000-2001 by François LEIBER.

This program is distributed under the GPL license, please refer to the Flib
Readme.html for further information.


Requirements :
--------------
 - A Ti-89, with any ROM or hardware.

 - The launcher othello() and the compressed var othdata, 7.1 KB.

 - Libraries Flib() and Flib2(), Anniversary release or more.

 - A few bytes free to start the game :)

Both files put themselves by default in foler 'flib', don't change this
place. Flib and Flib2 have to be in 'main' folder. I suggest you archive
them all.

Features :
----------
 - A game between two humans, or human-89, or 89-89.
	But, due to BASIC's slowness, the reflection will not resist against
	a good player. If you want a STRONG opponent, play against Othello
	II, my C Othello game...

 - Three different thinking levels :
	- Good : usual level
	- Medium : thinking limited
	- Looser : the calculator tries to loose, this level is interesting
	  when you're in a bad mood and you want to have a easy victory...
	  Another possibility : you consider the player who has the least
	  discs at the end wins, so you have a second game !

 - Automatic game saving when quitting.

 - A "teacher key".

 - Nice graphics, with gray levels for the intro.

In fact, the hardest thing when doing this new version was to resist the 
temptation to make a small C library to speed up the reflection and the
game in general, but I decided I wanted to continue to take up the 
challenge of making a playable game in BASIC.


Keys :
------
At the beginning :
 - Horizontal arrows to go from one player to the other.
 - Vertical arrows to change the player's level.
 - ESC to quit.
 - ENTER to start a new game.
 - F1 to load saved game ; if nothing appears at the bottom of the screen, 
	othello hasn't found any saved game, it will begin a new one.

During the game :
 - Arrows to move the cursor.
 - ENTER to place a disc at the cursor's place.
 - ESC to quit, you will have a confirmation menu.
 - CLEAR to turn the calculator off, very useful ;-)


Rules :
-------
Othello is stategy game for two players (blacks and whites), that place 
alternatively a disc of their color on a 8x8 board. Blacks begin.

The goal is to have, at the end of the game (when the board is full), 
more discs of your color than your opponent. If both players have the 
same nomber of discs at the end, they are declared tie.

To capture discs, you must place your disc so opponent discs are placed 
between this one and other of your discs (it can be done on a row, a 
column or diagonal). This discs will be captured and will become yours 
(you see they change color).

You can place only a disc at each turn, and this disc has to capture at 
least one of your opponent's.
A little rectangle inside the cursor tells you if you can play in this case.
If you don't have a valid move to play, then you must pass your turn.

At the beginning of a human's turn, the cursor will seem slow because the 
89 determines background all the valid moves : thus it detects 
automatically if the player has to pass.

When the 89 is thinking, a horizontal line at the bottom of a case
indicates what was the last move.


History :
---------
Version	 Date	  Bytes 	Description
-------	 ----	  ----- 	-----------

1.5	03-13-01  7803		Adaptation to Flib Anniversary release :
				all files are grouped in a compressed 
				variable, with a little launcher.

1.4	09-24-00  6957		Adaptation to Flib 2.1, with gray levels.

1.2-1.3	07-25-00  6500		Adaptation of Othello to the fantastic
				library Flib, I seize the opportunity to 
				thank its programmer, Francois Leiber ;)

1.1	11-28-99  7180		A few optimisations.
				The 89 detects in the background when a 
				human player has to pass, and when the 
				game is finished before the board is full.
				Added a light version.
				Thinking progression displayed, possibility
				to quit the game when the 89 is thinking.
				A few other new things, especially graphics.

1.0b	11-02-99  5522		Thinking slightly improved, translation
				and distribution in english.

1.0	10-23-99  5596		First real release.

0.1	10-20-99  5800		Eric and Benoit Charpentier put it on 
				their web page (www.multimania.com/ti8x)
				but it's not finished...

Infos :
-------
I know my translation of this text is far from good, but I've already made
quite an effort to do it, so be comprehensiv, I'm french :-)

For any suggestion, commentary or bug (that would be quite surprising ;-) ),
you can join me at :

	Francois.leiber@laposte.net
	---------------------------

All the ideas are welcome !

All my program updates are on my (french) webpage :

	http://leiber.free.fr
	---------------------
