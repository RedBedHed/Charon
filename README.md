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

### These are Charon's initial position perft results !!!
###### *(single-threaded, 1.6 ghz i5, strictly-legal moves only.)*
 <pre>
  <code>

     @@@    @@
   @@   @@  @@
  @@        @@ @@@      @@@@    @@ @@@      @@@@    @@ @@@
  @@        @@@   @@  @@   @@@  @@@   @@  @@    @@  @@@   @@
  @@        @@    @@  @@    @@  @@        @@    @@  @@    @@
   @@   @@  @@    @@  @@   @@@  @@        @@    @@  @@    @@
     @@@    @@    @@   @@@@ @@  @@          @@@@    @@    @@

~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~.


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
        perft(5) -  0.016 seconds -    4865609 nodes visited.
        perft(6) -  0.406 seconds -  119060324 nodes visited.
        
~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~.
 </code>
</pre> 
### For comparison, QPerft by H.G. Muller does *this* on my computer:
###### *(single-threaded, 1.6 ghz i5, strictly-legal moves only.)*
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
         perft( 6)=    119060324 ( 0.625 sec)
 </code>
</pre>
# Extra results (for science) !
### Charon starting from [KiwiPete](https://www.chessprogramming.org/Perft_Results#Position_2) !
<pre>
 <code>
        perft(1) -  0.000 seconds -         48 nodes visited.
        perft(2) -  0.000 seconds -       2039 nodes visited.
        perft(3) -  0.000 seconds -      97862 nodes visited.
        perft(4) -  0.031 seconds -    4085603 nodes visited.
        perft(5) -  0.766 seconds -  193690690 nodes visited.
        perft(6) - 31.766 seconds - 8031647685 nodes visited.
 </code>
</pre>
### Charon starting from [position 3](https://www.chessprogramming.org/Perft_Results#Position_3) !
<pre>
 <code>
        perft(1) -  0.000 seconds -         14 nodes visited.
        perft(2) -  0.000 seconds -        191 nodes visited.
        perft(3) -  0.000 seconds -       2812 nodes visited.
        perft(4) -  0.000 seconds -      43238 nodes visited.
        perft(5) -  0.000 seconds -     674624 nodes visited.
        perft(6) -  0.078 seconds -   11030083 nodes visited.
        perft(7) -  1.297 seconds -  178633661 nodes visited.
        perft(8) - 21.859 seconds - 3009794393 nodes visited.
 </code>
</pre>
### Charon starting from [position 4](https://www.chessprogramming.org/Perft_Results#Position_4) !
<pre>
 <code>
        perft(1) -  0.000 seconds -          6 nodes visited.
        perft(2) -  0.000 seconds -        264 nodes visited.
        perft(3) -  0.000 seconds -       9467 nodes visited.
        perft(4) -  0.000 seconds -     422333 nodes visited.
        perft(5) -  0.078 seconds -   15833292 nodes visited.
        perft(6) -  2.812 seconds -  706045033 nodes visited.
 </code>
</pre>
### Charon starting from [position 5](https://www.chessprogramming.org/Perft_Results#Position_5) !
<pre>
 <code>
        perft(1) -  0.000 seconds -         44 nodes visited.
        perft(2) -  0.000 seconds -       1486 nodes visited.
        perft(3) -  0.000 seconds -      62379 nodes visited.
        perft(4) -  0.000 seconds -    2103487 nodes visited.
        perft(5) -  0.375 seconds -   89941194 nodes visited.
 </code>
</pre>
### Charon starting from [position 6](https://www.chessprogramming.org/Perft_Results#Position_6) !
<pre>
 <code>
        perft(1) -  0.000 seconds -         46 nodes visited.
        perft(2) -  0.000 seconds -       2079 nodes visited.
        perft(3) -  0.000 seconds -      89890 nodes visited.
        perft(4) -  0.016 seconds -    3894594 nodes visited.
        perft(5) -  0.641 seconds -  164075551 nodes visited.
        perft(6) - 26.172 seconds - 6923051137 nodes visited.
 </code>
</pre>

