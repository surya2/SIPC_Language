# Deliverable 3 Solution Design

## General Procedure
Most feature implementation was generally straightforward. Unary, Binary statements just took additions to the if-else blocks of the existing code, and the for loop with start and end used roughly similar constructions as the while look. The majority of the work was in the array, and array related code gen functions.

## Arrays
Arrays and downstream functions took the bulk of the work in this assignment. We implemented many many many versions of arrays before things started working.The final version uses a struct with the length of the array and a pointer to the array. It also uses calloc to store this on the heap. Returning a struct is helpful because it allows other functions to extract the length from the array. This is now array ref and iter can do bounds checking.

There was some difficulty in referencing this struct in ArrayRef and Iter, because the return value struct ptr is casted into a llvm::value int64. Thus we had to add the int to pointer cast to retrieve the original pointer.

One this structure of storing the arrays was functional, most of the remainder of array functions were generally trivial.

## Testing 

We tested with compiled self testing. This introduced a lot of complexity, so to isolate just our new tests, we both added test runners to run.sh, but also created runsipc.sh, which is ran with ./runtests.sh -n.

The tests separate out each feature we implemented separately 
