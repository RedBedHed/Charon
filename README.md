# Charon Perft
# Hi! And welcome to the glorious mess that is Charon!

### Few things worth mentioning:

<p>
 <ol>
  <li>
   <p>
Board.h is still in the works... And I could really use advice on how to improve it.
The applyMove and retractMove algorithms are completely unfinished.
   </p> 
  <li>
   <p>
There are multiple files that need to be deleted. Among these are empty cpp files and 
my crude List that I opted not to use.
   </p>
  </li> 
  <li>
   <p>
Charon is vanilla bulk-counted perft. No hashing. Strictly legal move generation.
   </p>
  </li> 
 </ol> 
</p>

### These are Charon's perft scores !!!
###### *(single-threaded, 1.6 ghz i5, strictly-legal count only.)*
 <pre>
  <code>
        .~* Charon Perft *~.

        *. by Ellie Moore .*

        Starting Position:

            H   G   F   E   D   C   B   A
          +---+---+---+---+---+---+---+---+
        1 | R | N | B | K | Q | B | N | R | 1
          +---+---+---+---+---+---+---+---+
        2 | P | P | P | P | P | P | P | P | 2
          +---+---+---+---+---+---+---+---+
        3 |   |   |   |   |   |   |   |   | 3
          +---+---+---+---+---+---+---+---+
        4 |   |   |   |   |   |   |   |   | 4
          +---+---+---+---+---+---+---+---+
        5 |   |   |   |   |   |   |   |   | 5
          +---+---+---+---+---+---+---+---+
        6 |   |   |   |   |   |   |   |   | 6
          +---+---+---+---+---+---+---+---+
        7 | p | p | p | p | p | p | p | p | 7
          +---+---+---+---+---+---+---+---+
        8 | r | n | b | k | q | b | n | r | 8
          +---+---+---+---+---+---+---+---+
            H   G   F   E   D   C   B   A


        perft(1) -  0.000 seconds -         20 nodes visited.
        perft(2) -  0.000 seconds -        400 nodes visited.
        perft(3) -  0.000 seconds -       8902 nodes visited.
        perft(4) -  0.000 seconds -     197281 nodes visited.
        perft(5) -  0.031 seconds -    4865609 nodes visited.
        perft(6) -  0.656 seconds -  119060324 nodes visited.
 </code>
</pre> 
### For comparison, QPerft by H.G. Muller does *this* on my computer:
###### *(single-threaded, 1.6 ghz i5, strictly-legal count only.)*
<pre>
 <code>
         - - - - - - - - - - - -
         - - - - - - - - - - - -
         - - r n b q k b n r - -
         - - p p p p p p p p - -
         - - . . . . . . . . - -
         - - . . . . . . . . - -
         - - . . . . . . . . - -
         - - . . . . . . . . - -
         - - P P P P P P P P - -
         - - R N B Q K B N R - -
         - - - - - - - - - - - -
         - - - - - - - - - - - -

        Quick Perft by H.G. Muller
        Perft mode: No hashing, bulk counting in horizon nodes

        perft( 1)=           20 ( 0.000 sec)
        perft( 2)=          400 ( 0.000 sec)
        perft( 3)=         8902 ( 0.000 sec)
        perft( 4)=       197281 ( 0.000 sec)
        perft( 5)=      4865609 ( 0.031 sec)
        perft( 6)=    119060324 ( 0.844 sec)
 </code>
</pre>
