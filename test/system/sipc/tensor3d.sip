hcube3d(d,v) {
  var t;
  t = [d of [d of [d of v]]];
  return t;
}

sum3d(hcube) {
  var s, i, j, k;
  s = 0;
  for (i : 0 .. #hcube){
    output 1;
    for (j : 0 .. #(hcube[i])){
      output 2;
      for (k : 0 .. #(hcube[i][j])){
        output 3;
        s = s + hcube[i][j][k];
      }
    }
  }
  return s;  
}

main() {
  var t1, t2;
  t1 = hcube3d(3, 1);
  t2 = hcube3d(4, 2); 
  //error t2[0][0][0];
  //error sum3d(t1);
  if (sum3d(t1) != 27) error sum3d(t1); 
  if (sum3d(t2) != 128) error sum3d(t2); 
  return 0;
}
