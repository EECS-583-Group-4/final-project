## Using the MAS

The MAS is structured such that loads are root nodes, and the leaf nodes will be the values from which 
all of the operations are computed to eventually get that load. Right now we don't try to to handle 
loop induction variables, so only constant values are computable at the moment. 

The way I foresee the MAS being useful is in essentially grabbing the base addresses throughout a given
function and then all of the dependencies of that base address. So for instance, if I have several root nodes that all end up with a base address %2 in their set of leaf nodes, I would just insert one check above all of those loads to make sure %2 is valid.

So in the S-Detector Case, probably it looks something like... 
```
BA = set of all base addresses used in the program

for each root node: 
    if BA[i] in leaf nodes --> 
        check for valid BA[i] above this load
```
This works for base addresses, and is pretty straightforward. Less straightforward is for offsets...

Now we need to keep a map of Base Address -> Max Offset, and the idea here is we get the max offset by going through the tree of each load and finding the greatest constant or other value associated with a given base address. Usually this will be another parameter in the getelementptr IR instruction. If this is a constant, great! We can do the comparisons to find the max offset in our static analysis. However, if it is a dynamically loaded variable, then we are going to have to insert a check for that no matter what OR insert logic to calculate the max offset at runtime. Probably there is a heuristic for this we could come up with? 

### What about S-Tracer? 

With S-Tracer it seems like the idea is pretty similar. We want to make a more compact representation of the mess that is the printed MAS by comparing the base address leaf nodes and then using that information to essentially group loads together in the printed representation of memory accesses. 
