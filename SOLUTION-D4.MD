# Deliverable 4 Solution Design

## General Procedure
Most feature implementation was generally straightforward. Unary, Binary statements just took additions to the if-else blocks of the existing code, and the for loop with start and end used roughly similar constructions as the while look. The majority of the work was in the array, and array-related code gen functions. We followed the normal conventions from other parts of the code base as a convention to make the new features. 

## Arrays
Arrays and downstream functions took the bulk of the work in this assignment. We implemented many many many versions of arrays before things started working. The biggest issue across  The final version uses a struct with the length of the array and a pointer to the array. It also uses calloc to store this on the heap. Returning a struct is helpful because it allows other functions to extract the length from the array. This is now array ref and iter can do bounds checking.

There was some difficulty in referencing this struct in ArrayRef and Iter, because the return value struct ptr is casted into a llvm::value int64. Thus we had to add the int to pointer cast to retrieve the original pointer. Once this was implemented, using the struct to retrieve the array worked fine. The other difficulty was getting array assignment to work, because it was often causing a segfault, but we ultimately realized we need to factor in the lvalue taken from the assignment statement to determine if we are supposed to return the value at that position, or a pointer to that value. Once we got the correct pointer returning, we no longer had issues with segfaults from the wrong pointers.

Once this structure of storing the arrays was functional, most of the remainder of array functions were generally trivial. We had to implement iter statements, which followed the structure of for loops using the same code to index the array that we used in array ref, and array length followed the same structure used to get the length in array ref for bounds checking. Lastly, we had to implement the ofArrays, with the main constraint that they had to follow the same array struct at Array.

## Testing 

We tested with compiled self-testing. This introduced a lot of complexity, so to isolate just our new tests, we both added test runners to run.sh, but also created runsipc.sh, which is ran with ./runtests.sh -n. This script compiles and runs all of the tests, and keeps a record of what passed and failed. Additionally, we added a section to run.sh that procedurally generated and saved the pretty printer files as a means of comparison. This made testing much faster because we could iteratively test just the new tests that we wanted to target.

The tests separate out each feature we implemented separately. They also implement relevant edge cases to make sure the code still works. By testing many different cases we were able to check many edge cases that we otherwise would have missed, which helped us ensure our code was functional. One debugging trick that was particularly helpful was using error and output to get information in the terminal.

The hardest tests to debug were those returning segfaults, as often happened with array-related tests. With these, we used output to find the lastest portions of the test working and identify where the segfault (or other system errors) happened, and from there we could backtrack and find the root of the segfault, although this often proved somewhat difficult. 



