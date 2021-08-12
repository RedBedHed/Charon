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


        perft(1) -  0.000 seconds   -          20 nodes visited.
        perft(2) -  0.000 seconds   -         400 nodes visited.
        perft(3) -  0.000 seconds   -        8902 nodes visited.
        perft(4) -  0.000 seconds   -      197281 nodes visited.
        perft(5) -  0.016 seconds   -     4865609 nodes visited.
        perft(6) -  0.375 seconds   -   119060324 nodes visited.
        perft(7) -  9.469 seconds   -  3195901860 nodes visited.
        perft(8) -  272.094 seconds - 84998978956 nodes visited.
        
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
         perft( 7)=   3195901860 (16.576 sec)
         perft( 8)=  84998978956 (441.816 sec)
 </code>
</pre>
# Extra results (for science) !
### Charon starting from [KiwiPete](https://www.chessprogramming.org/Perft_Results#Position_2) !
<pre>
 <code>
 Charon:
        perft(1) -  0.000 seconds -         48 nodes visited.
        perft(2) -  0.000 seconds -       2039 nodes visited.
        perft(3) -  0.000 seconds -      97862 nodes visited.
        perft(4) -  0.000 seconds -    4085603 nodes visited.
        perft(5) -  0.469 seconds -  193690690 nodes visited.
        perft(6) - 23.641 seconds - 8031647685 nodes visited.
        
Qperft:
        perft( 1)=           48 ( 0.000 sec)
        perft( 2)=         2039 ( 0.000 sec)
        perft( 3)=        97862 ( 0.001 sec)
        perft( 4)=      4085603 ( 0.022 sec)
        perft( 5)=    193690690 ( 0.902 sec)
        perft( 6)=   8031647685 (41.636 sec)
 </code>
</pre>
### Charon starting from [position 3](https://www.chessprogramming.org/Perft_Results#Position_3) ! 
<pre>
 <code>
Charon:
        perft(1) -  0.000 seconds -         14 nodes visited.
        perft(2) -  0.000 seconds -        191 nodes visited.
        perft(3) -  0.000 seconds -       2812 nodes visited.
        perft(4) -  0.000 seconds -      43238 nodes visited.
        perft(5) -  0.000 seconds -     674624 nodes visited.
        perft(6) -  0.078 seconds -   11030083 nodes visited.
        perft(7) -  0.953 seconds -  178633661 nodes visited.
        perft(8) - 16.312 seconds - 3009794393 nodes visited.
        
Qperft:
        perft( 1)=           14 ( 0.000 sec)
        perft( 2)=          191 ( 0.000 sec)
        perft( 3)=         2812 ( 0.000 sec)
        perft( 4)=        43238 ( 0.001 sec)
        perft( 5)=       674624 ( 0.007 sec)
        perft( 6)=     11030083 ( 0.108 sec)
        perft( 7)=    178633661 ( 1.524 sec)
        perft( 8)=   3009794393 (28.467 sec)
 </code>
</pre>
### Charon starting from [position 4](https://www.chessprogramming.org/Perft_Results#Position_4) !
<pre>
 <code>
Charon:
        perft(1) -  0.000 seconds -          6 nodes visited.
        perft(2) -  0.000 seconds -        264 nodes visited.
        perft(3) -  0.000 seconds -       9467 nodes visited.
        perft(4) -  0.000 seconds -     422333 nodes visited.
        perft(5) -  0.047 seconds -   15833292 nodes visited.
        perft(6) -  2.094 seconds -  706045033 nodes visited.
        
Qperft:
        perft( 1)=            6 ( 0.000 sec)
        perft( 2)=          264 ( 0.000 sec)
        perft( 3)=         9467 ( 0.000 sec)
        perft( 4)=       422333 ( 0.003 sec)
        perft( 5)=     15833292 ( 0.090 sec)
        perft( 6)=    706045033 ( 5.406 sec)
 </code>
</pre>
### Charon starting from [position 5](https://www.chessprogramming.org/Perft_Results#Position_5) !
<pre>
 <code>
Charon:
        perft(1) -  0.000 seconds -         44 nodes visited.
        perft(2) -  0.000 seconds -       1486 nodes visited.
        perft(3) -  0.000 seconds -      62379 nodes visited.
        perft(4) -  0.000 seconds -    2103487 nodes visited.
        perft(5) -  0.250 seconds -   89941194 nodes visited.
        
Qperft:
        perft( 1)=           44 ( 0.000 sec)
        perft( 2)=         1486 ( 0.000 sec)
        perft( 3)=        62379 ( 0.001 sec)
        perft( 4)=      2103487 ( 0.010 sec)
        perft( 5)=     89941194 ( 0.635 sec)
 </code>
</pre>
### Charon starting from [position 6](https://www.chessprogramming.org/Perft_Results#Position_6) !
<pre>
 <code>
Charon:
        perft(1) -  0.000 seconds -         46 nodes visited.
        perft(2) -  0.000 seconds -       2079 nodes visited.
        perft(3) -  0.000 seconds -      89890 nodes visited.
        perft(4) -  0.016 seconds -    3894594 nodes visited.
        perft(5) -  0.406 seconds -  164075551 nodes visited.
        perft(6) - 18.766 seconds - 6923051137 nodes visited.
        
Qperft:
        perft( 1)=           46 ( 0.000 sec)
        perft( 2)=         2079 ( 0.000 sec)
        perft( 3)=        89890 ( 0.000 sec)
        perft( 4)=      3894594 ( 0.013 sec)
        perft( 5)=    164075551 ( 0.578 sec)
        perft( 6)=   6923051137 (24.300 sec)
 </code>
</pre>
<table style="width:100%">
 <tr>
  <th>Position</th>
  <th>Node Count</th>
  <th>Charon (seconds)</th>
  <th>Qpeft  (seconds)</th>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_2">KiwiPete]</a> - Depth 6</td>
  <td>8031647685</td>
  <td>23.641</td>
  <td>41.636</td>
 </tr>
</table>

