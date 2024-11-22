// in-place insertion sort for array of integers
sort(a) {
  var i, j, k, break;
  for (i : 1 .. #a) {
    k = a[i];                    // array indexing
    j = i - 1;

    /* SIP's logical operators do not use short-circuit evaluation
     * so this more complex logic is required.
     */
    break = false;
    while (j >= 0 and not break) {
      if (a[j] > k) { 
        a[j+1] = a[j];
        j--;       
      } else {
        break = true;
      }
    }

    a[j+1] = k;
  } 
  return a;
}

copy(a) {
  var b, i;
  b = [ #a of 0 ];	// computed array initialization
  for (i : 0 .. #a) {   
    b[i] = a[i];
  }
  return b;
}

main() {
  var a, b, x;
  var e;
  a = [ 13, 7, -4, 14, 0 ];
  b = copy(a);
  a[2] = 2;
  a[4] = 3;
  x = sort(b);
  return 0;
}
