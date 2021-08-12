# Charon Perft 
###### version: *08.11.2021 with support for pext (BMI2), popcnt, and bsf*
###### speed: *180-370 mnps*  
## Stuff worth mentioning :
<p>
 <ul>
  <li>
   <p>
Board.h is still in the works... And I could really use advice on how to improve it.
The applyMove and retractMove algorithms are functional, but unfinished.
   </p> 
  </li> 
  <li>
   <p>
Charon is vanilla bulk-counted perft. No hashing. Strictly legal move generation.
   </p>
  </li> 
 </ul> 
</p>

## These are Charon's initial position perft results !!!
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
<table style="width:100%">
 <tr>
  <th>Position</th>
  <th>Depth</th>
  <th>Node Count</th>
  <th>Charon (seconds)</th>
  <th>Qperft (seconds)</th>
  <th>Charon (mnps)</th>
  <th>Qperft (mnps)</th>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_2">KiwiPete</a></td>
  <td>6</td>
  <td>8031647685</td>
  <td>23.641</td>
  <td>41.636</td>
  <td>339.7</td>
  <td>192.9</td>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_3">Position 3</a></td>
  <td>6</td>
  <td>3009794393</td>
  <td>16.312</td>
  <td>28.467</td>
  <td>184.5</td>
  <td>105.7</td>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_4">Position 4</a></td>
  <td>6</td>
  <td>706045033</td>
  <td>2.094</td>
  <td>5.406</td>
  <td>337.2</td>
  <td>130.6</td>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_5">Position 5</a></td>
  <td>5</td> 
  <td>89941194</td>
  <td>0.250</td>
  <td>0.635</td>
  <td>359.8</td>
  <td>141.6</td>
 </tr>
 <tr>
  <td><a href = "https://www.chessprogramming.org/Perft_Results#Position_6">Position 6</a></td>
  <td>6</td> 
  <td>6923051137</td>
  <td>18.766</td>
  <td>24.300</td>
  <td>368.9</td>
  <td>284.9</td>
 </tr>
</table>

