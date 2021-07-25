# Charon
# Hi! And welcome to the glorious mess that is Charon!

### Few things worth mentioning:

<p>
 <ol>
  <li>
   <p>
Board.cpp is still in the works... And I could really use advice on how to improve it.
The applyMove and undoMove algorithms are unfinished. I wrote them in one shot and I 
haven't had time to look at them again.
   </p> 
  <li>
   <p>
There are multiple files that need to be deleted. Among these are empty cpp files and 
my crude List that I opted not to use.
   </p>
  </li> 
 </ol> 
</p>

### These are Charon's perft scores !!!
###### (Keep in mind that Charon is currently single-threaded and generates strictly-legal moves only.)


 <pre>
  <code>
        (1) 1.32e-05 seconds - 20 nodes visited.
        (2) 2.71e-05 seconds - 400 nodes visited.
        (3) 0.0002933 seconds - 8902 nodes visited.
        (4) 0.0059451 seconds - 197277 nodes visited.
        (5) 0.138123 seconds - 4865308 nodes visited.
        (6) 3.30326 seconds - 119038876 nodes visited.
 </code>
</pre> 

###### *Warning: There is a bug in MoveMake... Several moves are not being generated from depth 4 onward.*
